#include "uciengine.h"
#include <QDebug>
#include <QCoreApplication>
#include <QDir>

UCIEngine::UCIEngine(QObject *parent)
    : QObject(parent),
      m_process(nullptr),
      m_isReady(false),
      m_skillLevel(10)
{
}

UCIEngine::~UCIEngine()
{
    if (m_process) {
        m_process->write("quit\n");
        m_process->waitForFinished(1000);
        delete m_process;
    }
}

bool UCIEngine::initialize(const QString& enginePath)
{
    if (m_process) {
        delete m_process;
    }

    m_process = new QProcess(this);
    
    connect(m_process, &QProcess::readyReadStandardOutput, this, &UCIEngine::onReadyRead);
    connect(m_process, &QProcess::errorOccurred, this, &UCIEngine::onErrorOccurred);

    // 啟動引擎
    m_process->start(enginePath);
    
    if (!m_process->waitForStarted(3000)) {
        qDebug() << "Failed to start engine:" << enginePath;
        emit engineError("Failed to start engine");
        return false;
    }

    // 發送 UCI 初始化命令
    sendCommand("uci");
    
    // 等待 uciok 回應
    m_process->waitForReadyRead(2000);
    
    // 設定選項
    sendCommand(QString("setoption name Skill Level value %1").arg(m_skillLevel));
    sendCommand("isready");
    
    return true;
}

void UCIEngine::setSkillLevel(int level)
{
    // Stockfish 技能等級範圍 0-20
    m_skillLevel = qBound(0, level, 20);
    
    if (m_process && m_process->state() == QProcess::Running) {
        sendCommand(QString("setoption name Skill Level value %1").arg(m_skillLevel));
        sendCommand("isready");
    }
}

void UCIEngine::getBestMove(ChessBoard* board, PieceColor color)
{
    if (!m_process || m_process->state() != QProcess::Running || !m_isReady) {
        emit engineError("Engine not ready");
        return;
    }

    // 建立 FEN 字串
    QString fen = boardToFEN(board, color);
    
    // 發送位置和計算命令
    sendCommand("position fen " + fen);
    sendCommand("go movetime 1000");  // 思考 1 秒
}

void UCIEngine::stop()
{
    if (m_process) {
        sendCommand("stop");
    }
}

void UCIEngine::sendCommand(const QString& command)
{
    if (m_process && m_process->state() == QProcess::Running) {
        qDebug() << "UCI >>" << command;
        m_process->write((command + "\n").toUtf8());
        m_process->waitForBytesWritten();
    }
}

void UCIEngine::onReadyRead()
{
    while (m_process->canReadLine()) {
        QString line = QString::fromUtf8(m_process->readLine()).trimmed();
        processLine(line);
    }
}

void UCIEngine::processLine(const QString& line)
{
    qDebug() << "UCI <<" << line;
    
    if (line == "uciok") {
        qDebug() << "Engine initialized";
    }
    else if (line == "readyok") {
        m_isReady = true;
        qDebug() << "Engine ready";
    }
    else if (line.startsWith("bestmove")) {
        QStringList parts = line.split(' ');
        if (parts.size() >= 2) {
            QString move = parts[1];
            if (move.length() >= 4) {
                QString from = move.mid(0, 2);
                QString to = move.mid(2, 2);
                emit bestMoveFound(from, to);
            }
        }
    }
}

void UCIEngine::onErrorOccurred(QProcess::ProcessError error)
{
    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "Engine failed to start";
            break;
        case QProcess::Crashed:
            errorMsg = "Engine crashed";
            break;
        default:
            errorMsg = "Engine error occurred";
            break;
    }
    
    qDebug() << errorMsg;
    emit engineError(errorMsg);
}

QString UCIEngine::positionToUCI(int row, int col)
{
    // 將棋盤座標 (row, col) 轉換為 UCI 格式 (例如 "e2")
    char file = 'a' + col;
    char rank = '1' + row;
    return QString(file) + QString(rank);
}

QPoint UCIEngine::uciToPosition(const QString& uci)
{
    // 將 UCI 格式 (例如 "e2") 轉換為棋盤座標 (row, col)
    if (uci.length() < 2) return QPoint(-1, -1);
    
    int col = uci[0].toLatin1() - 'a';
    int row = uci[1].toLatin1() - '1';
    
    return QPoint(row, col);
}

QString UCIEngine::boardToFEN(ChessBoard* board, PieceColor turn)
{
    QString fen;
    
    // 棋盤位置（從第8列到第1列）
    for (int row = 7; row >= 0; --row) {
        int emptyCount = 0;
        for (int col = 0; col < 8; ++col) {
            ChessPiece* piece = board->getPieceAt(row, col);
            if (piece) {
                if (emptyCount > 0) {
                    fen += QString::number(emptyCount);
                    emptyCount = 0;
                }
                
                char pieceChar;
                switch (piece->getType()) {
                    case PieceType::PAWN:   pieceChar = 'p'; break;
                    case PieceType::KNIGHT: pieceChar = 'n'; break;
                    case PieceType::BISHOP: pieceChar = 'b'; break;
                    case PieceType::ROOK:   pieceChar = 'r'; break;
                    case PieceType::QUEEN:  pieceChar = 'q'; break;
                    case PieceType::KING:   pieceChar = 'k'; break;
                    default: pieceChar = '?'; break;
                }
                
                if (piece->getColor() == PieceColor::WHITE) {
                    pieceChar = toupper(pieceChar);
                }
                
                fen += pieceChar;
            } else {
                emptyCount++;
            }
        }
        
        if (emptyCount > 0) {
            fen += QString::number(emptyCount);
        }
        
        if (row > 0) {
            fen += '/';
        }
    }
    
    // 輪到誰
    fen += (turn == PieceColor::WHITE) ? " w " : " b ";
    
    // 王車易位權利（簡化版本 - 可以改進）
    QString castling;
    // TODO: 實際檢查王車易位的可用性
    castling = "KQkq";  // 假設全部可用
    fen += castling.isEmpty() ? "-" : castling;
    fen += " ";
    
    // 吃過路兵目標格（簡化）
    fen += "- ";
    
    // 半回合計數器和完整回合數（簡化）
    fen += "0 1";
    
    return fen;
}
