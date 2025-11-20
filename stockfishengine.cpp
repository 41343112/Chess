#include "stockfishengine.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QThread>
#include <QEventLoop>
#include <QTimer>
#include <QElapsedTimer>

StockfishEngine::StockfishEngine(QObject *parent)
    : QObject(parent)
    , m_process(nullptr)
    , m_skillLevel(10)
    , m_isReady(false)
    , m_isInitialized(false)
{
}

StockfishEngine::~StockfishEngine()
{
    shutdown();
}

QString StockfishEngine::findStockfishPath()
{
    // Try to find Stockfish executable in various locations
    QStringList possiblePaths;
    
    QString appDir = QCoreApplication::applicationDirPath();
    
#ifdef Q_OS_WIN
    possiblePaths << appDir + "/engine/stockfish-windows-x86-64-avx2.exe"
                  << appDir + "/stockfish.exe"
                  << "engine/stockfish-windows-x86-64-avx2.exe"
                  << "stockfish.exe";
#elif defined(Q_OS_LINUX)
    possiblePaths << appDir + "/engine/stockfish"
                  << appDir + "/stockfish"
                  << "engine/stockfish"
                  << "/usr/games/stockfish"
                  << "/usr/local/bin/stockfish"
                  << "stockfish";
#elif defined(Q_OS_MACOS)
    possiblePaths << appDir + "/engine/stockfish"
                  << appDir + "/stockfish"
                  << "engine/stockfish"
                  << "/usr/local/bin/stockfish"
                  << "stockfish";
#endif
    
    for (const QString& path : possiblePaths) {
        QFile file(path);
        if (file.exists()) {
            qDebug() << "Found Stockfish at:" << path;
            return path;
        }
    }
    
    qWarning() << "Stockfish not found in any expected location";
    return QString();
}

bool StockfishEngine::initialize()
{
    if (m_isInitialized) {
        return true;
    }
    
    m_enginePath = findStockfishPath();
    if (m_enginePath.isEmpty()) {
        emit error("Stockfish engine not found");
        return false;
    }
    
    m_process = new QProcess(this);
    connect(m_process, &QProcess::readyRead, this, &StockfishEngine::onReadyRead);
    connect(m_process, &QProcess::errorOccurred, this, &StockfishEngine::onErrorOccurred);
    
    m_process->start(m_enginePath);
    
    if (!m_process->waitForStarted(3000)) {
        emit error("Failed to start Stockfish engine");
        return false;
    }
    
    // Initialize UCI protocol
    sendCommand("uci");
    QString response = waitForResponse("uciok", 5000);
    if (!response.contains("uciok")) {
        emit error("Failed to initialize UCI protocol");
        return false;
    }
    
    // Set initial skill level
    setSkillLevel(m_skillLevel);
    
    // Check if ready
    sendCommand("isready");
    response = waitForResponse("readyok", 5000);
    if (!response.contains("readyok")) {
        emit error("Engine not ready");
        return false;
    }
    
    m_isReady = true;
    m_isInitialized = true;
    emit engineReady();
    
    qDebug() << "Stockfish engine initialized successfully";
    return true;
}

void StockfishEngine::setSkillLevel(int level)
{
    m_skillLevel = qBound(0, level, 20);
    
    if (m_process && m_process->state() == QProcess::Running) {
        // Set UCI options for skill level
        sendCommand(QString("setoption name Skill Level value %1").arg(m_skillLevel));
        
        // For lower skill levels, also limit thinking time
        if (m_skillLevel < 10) {
            sendCommand("setoption name UCI_LimitStrength value true");
            // Map skill level 0-9 to Elo 1000-1900
            int elo = 1000 + (m_skillLevel * 100);
            sendCommand(QString("setoption name UCI_Elo value %1").arg(elo));
        } else {
            sendCommand("setoption name UCI_LimitStrength value false");
        }
        
        qDebug() << "Skill level set to:" << m_skillLevel;
    }
}

QString StockfishEngine::boardToFEN(ChessBoard* board, PieceColor currentTurn)
{
    QString fen;
    
    // Piece placement
    for (int row = 7; row >= 0; --row) {
        int emptyCount = 0;
        for (int col = 0; col < 8; ++col) {
            ChessPiece* piece = board->getPieceAt(row, col);
            if (piece) {
                if (emptyCount > 0) {
                    fen += QString::number(emptyCount);
                    emptyCount = 0;
                }
                
                // Get piece character
                char pieceChar = ' ';
                switch (piece->getType()) {
                    case PieceType::PAWN:   pieceChar = 'p'; break;
                    case PieceType::KNIGHT: pieceChar = 'n'; break;
                    case PieceType::BISHOP: pieceChar = 'b'; break;
                    case PieceType::ROOK:   pieceChar = 'r'; break;
                    case PieceType::QUEEN:  pieceChar = 'q'; break;
                    case PieceType::KING:   pieceChar = 'k'; break;
                }
                
                // Uppercase for white pieces
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
    
    // Active color
    fen += (currentTurn == PieceColor::WHITE) ? " w " : " b ";
    
    // Castling availability
    QString castling;
    // Check if kings and rooks haven't moved
    ChessPiece* whiteKing = board->getPieceAt(0, 4);
    ChessPiece* blackKing = board->getPieceAt(7, 4);
    
    if (whiteKing && whiteKing->getType() == PieceType::KING && !whiteKing->hasMoved()) {
        ChessPiece* whiteKingRook = board->getPieceAt(0, 7);
        if (whiteKingRook && whiteKingRook->getType() == PieceType::ROOK && !whiteKingRook->hasMoved()) {
            castling += 'K';
        }
        ChessPiece* whiteQueenRook = board->getPieceAt(0, 0);
        if (whiteQueenRook && whiteQueenRook->getType() == PieceType::ROOK && !whiteQueenRook->hasMoved()) {
            castling += 'Q';
        }
    }
    
    if (blackKing && blackKing->getType() == PieceType::KING && !blackKing->hasMoved()) {
        ChessPiece* blackKingRook = board->getPieceAt(7, 7);
        if (blackKingRook && blackKingRook->getType() == PieceType::ROOK && !blackKingRook->hasMoved()) {
            castling += 'k';
        }
        ChessPiece* blackQueenRook = board->getPieceAt(7, 0);
        if (blackQueenRook && blackQueenRook->getType() == PieceType::ROOK && !blackQueenRook->hasMoved()) {
            castling += 'q';
        }
    }
    
    fen += castling.isEmpty() ? "- " : (castling + " ");
    
    // En passant target square
    QPoint enPassantTarget = board->getEnPassantTarget();
    if (enPassantTarget.x() >= 0 && enPassantTarget.y() >= 0) {
        char file = 'a' + enPassantTarget.y();
        char rank = '1' + enPassantTarget.x();
        fen += QString("%1%2").arg(file).arg(rank);
    } else {
        fen += "-";
    }
    
    // Halfmove clock and fullmove number (simplified)
    fen += " 0 1";
    
    return fen;
}

QPair<QPoint, QPoint> StockfishEngine::uciMoveToPoints(const QString& uciMove)
{
    // UCI move format: e2e4, e7e5, e1g1 (castling), etc.
    if (uciMove.length() < 4) {
        return QPair<QPoint, QPoint>(QPoint(-1, -1), QPoint(-1, -1));
    }
    
    // Parse from square
    char fromFile = uciMove[0].toLatin1();
    char fromRank = uciMove[1].toLatin1();
    int fromCol = fromFile - 'a';
    int fromRow = fromRank - '1';
    
    // Parse to square
    char toFile = uciMove[2].toLatin1();
    char toRank = uciMove[3].toLatin1();
    int toCol = toFile - 'a';
    int toRow = toRank - '1';
    
    return QPair<QPoint, QPoint>(QPoint(fromRow, fromCol), QPoint(toRow, toCol));
}

QPair<QPoint, QPoint> StockfishEngine::getBestMove(ChessBoard* board, PieceColor color)
{
    if (!m_isReady) {
        if (!initialize()) {
            return QPair<QPoint, QPoint>(QPoint(-1, -1), QPoint(-1, -1));
        }
    }
    
    // Convert board to FEN
    QString fen = boardToFEN(board, color);
    qDebug() << "Position FEN:" << fen;
    
    // Send position to engine
    sendCommand("position fen " + fen);
    
    // Request best move with time limit based on skill level
    int thinkTime = 100 + (m_skillLevel * 50); // 100ms to 1100ms
    sendCommand(QString("go movetime %1").arg(thinkTime));
    
    // Wait for bestmove response
    m_bestMove.clear();
    QString response = waitForResponse("bestmove", 10000);
    
    if (response.isEmpty()) {
        emit error("No response from engine");
        return QPair<QPoint, QPoint>(QPoint(-1, -1), QPoint(-1, -1));
    }
    
    // Parse bestmove from response
    // Format: "bestmove e2e4" or "bestmove e2e4 ponder e7e5"
    QStringList parts = response.split(' ', Qt::SkipEmptyParts);
    for (int i = 0; i < parts.size(); ++i) {
        if (parts[i] == "bestmove" && i + 1 < parts.size()) {
            m_bestMove = parts[i + 1];
            break;
        }
    }
    
    if (m_bestMove.isEmpty() || m_bestMove == "(none)") {
        return QPair<QPoint, QPoint>(QPoint(-1, -1), QPoint(-1, -1));
    }
    
    qDebug() << "Best move from Stockfish:" << m_bestMove;
    emit bestMoveFound(m_bestMove);
    
    return uciMoveToPoints(m_bestMove);
}

void StockfishEngine::sendCommand(const QString& command)
{
    if (m_process && m_process->state() == QProcess::Running) {
        qDebug() << "Sending to Stockfish:" << command;
        m_process->write((command + "\n").toUtf8());
        m_process->waitForBytesWritten();
    }
}

QString StockfishEngine::waitForResponse(const QString& expectedResponse, int timeoutMs)
{
    QString allOutput;
    QElapsedTimer timer;
    timer.start();
    
    while (timer.elapsed() < timeoutMs) {
        if (m_process->waitForReadyRead(100)) {
            QString output = QString::fromUtf8(m_process->readAll());
            allOutput += output;
            
            if (allOutput.contains(expectedResponse)) {
                return allOutput;
            }
        }
        QCoreApplication::processEvents();
    }
    
    return allOutput;
}

void StockfishEngine::onReadyRead()
{
    QString output = QString::fromUtf8(m_process->readAll());
    m_pendingOutput += output;
    qDebug() << "Stockfish output:" << output.trimmed();
}

void StockfishEngine::onErrorOccurred(QProcess::ProcessError error)
{
    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "Failed to start Stockfish engine";
            break;
        case QProcess::Crashed:
            errorMsg = "Stockfish engine crashed";
            break;
        case QProcess::Timedout:
            errorMsg = "Stockfish engine timed out";
            break;
        case QProcess::WriteError:
            errorMsg = "Write error to Stockfish engine";
            break;
        case QProcess::ReadError:
            errorMsg = "Read error from Stockfish engine";
            break;
        default:
            errorMsg = "Unknown error with Stockfish engine";
    }
    
    qWarning() << errorMsg;
    emit error(errorMsg);
}

void StockfishEngine::shutdown()
{
    if (m_process) {
        if (m_process->state() == QProcess::Running) {
            sendCommand("quit");
            m_process->waitForFinished(1000);
            if (m_process->state() == QProcess::Running) {
                m_process->kill();
            }
        }
        delete m_process;
        m_process = nullptr;
    }
    
    m_isReady = false;
    m_isInitialized = false;
}
