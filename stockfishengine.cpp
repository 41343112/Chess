#include "stockfishengine.h"
#include <QDebug>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QElapsedTimer>

StockfishEngine::StockfishEngine(QObject* parent)
    : QObject(parent)
    , m_process(nullptr)
    , m_isReady(false)
    , m_isInitialized(false)
{
    m_process = new QProcess(this);
    connect(m_process, &QProcess::readyRead, this, &StockfishEngine::onReadyRead);
    connect(m_process, &QProcess::errorOccurred, this, &StockfishEngine::onErrorOccurred);
}

StockfishEngine::~StockfishEngine()
{
    quit();
    if (m_process) {
        m_process->waitForFinished(1000);
        delete m_process;
    }
}

bool StockfishEngine::initialize(const QString& enginePath)
{
    if (m_isInitialized) {
        qDebug() << "Stockfish engine already initialized";
        return true;
    }

    // 檢查引擎檔案是否存在
    if (!QFile::exists(enginePath)) {
        qWarning() << "Stockfish engine not found at:" << enginePath;
        emit errorOccurred("Stockfish engine not found: " + enginePath);
        return false;
    }

    qDebug() << "Starting Stockfish engine:" << enginePath;
    m_process->start(enginePath);

    if (!m_process->waitForStarted(5000)) {
        qWarning() << "Failed to start Stockfish engine";
        emit errorOccurred("Failed to start Stockfish engine");
        return false;
    }

    // 發送 UCI 命令初始化引擎
    sendCommand("uci");
    
    if (!waitForResponse("uciok", 5000)) {
        qWarning() << "Stockfish engine did not respond to UCI command";
        emit errorOccurred("Engine initialization failed");
        return false;
    }

    // 設定引擎準備就緒
    sendCommand("isready");
    
    if (!waitForResponse("readyok", 5000)) {
        qWarning() << "Stockfish engine not ready";
        emit errorOccurred("Engine not ready");
        return false;
    }

    m_isInitialized = true;
    m_isReady = true;
    emit engineReady();
    qDebug() << "Stockfish engine initialized successfully";
    
    return true;
}

void StockfishEngine::sendCommand(const QString& command)
{
    if (!m_process || m_process->state() != QProcess::Running) {
        qWarning() << "Cannot send command: engine not running";
        return;
    }

    qDebug() << "Sending to Stockfish:" << command;
    m_process->write((command + "\n").toUtf8());
    m_process->waitForBytesWritten();
}

bool StockfishEngine::waitForResponse(const QString& expectedResponse, int timeoutMs)
{
    QElapsedTimer timer;
    timer.start();

    while (timer.elapsed() < timeoutMs) {
        m_process->waitForReadyRead(100);
        
        if (m_lastResponse.contains(expectedResponse, Qt::CaseInsensitive)) {
            return true;
        }
        
        QCoreApplication::processEvents();
    }

    return false;
}

void StockfishEngine::onReadyRead()
{
    if (!m_process) return;

    QByteArray data = m_process->readAllStandardOutput();
    QString output = QString::fromUtf8(data);
    
    m_lastResponse += output;
    
    qDebug() << "Stockfish output:" << output.trimmed();
    parseOutput(output);
}

void StockfishEngine::parseOutput(const QString& output)
{
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    
    for (const QString& line : lines) {
        QString trimmedLine = line.trimmed();
        
        // 檢查最佳移動
        if (trimmedLine.startsWith("bestmove")) {
            QStringList parts = trimmedLine.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 2) {
                m_pendingBestMove = parts[1];
                emit bestMoveFound(m_pendingBestMove);
            }
        }
    }
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
        break;
    }

    qWarning() << errorMsg;
    emit errorOccurred(errorMsg);
    m_isReady = false;
}

void StockfishEngine::setPosition(const QString& fen)
{
    if (!m_isReady) {
        qWarning() << "Engine not ready";
        return;
    }

    sendCommand("position fen " + fen);
}

void StockfishEngine::setPositionWithMoves(const QVector<QString>& moves)
{
    if (!m_isReady) {
        qWarning() << "Engine not ready";
        return;
    }

    QString command = "position startpos";
    if (!moves.isEmpty()) {
        command += " moves";
        for (const QString& move : moves) {
            command += " " + move;
        }
    }
    
    sendCommand(command);
}

QString StockfishEngine::getBestMove(int thinkingTimeMs)
{
    if (!m_isReady) {
        qWarning() << "Engine not ready";
        return QString();
    }

    m_pendingBestMove.clear();
    m_lastResponse.clear();
    
    // 發送 go 命令，設定思考時間
    QString goCommand = QString("go movetime %1").arg(thinkingTimeMs);
    sendCommand(goCommand);
    
    // 等待最佳移動
    QElapsedTimer timer;
    timer.start();
    int timeout = thinkingTimeMs + 5000; // 額外5秒容錯時間
    
    while (timer.elapsed() < timeout) {
        m_process->waitForReadyRead(100);
        
        if (!m_pendingBestMove.isEmpty()) {
            return m_pendingBestMove;
        }
        
        QCoreApplication::processEvents();
    }

    qWarning() << "Timeout waiting for best move";
    return QString();
}

void StockfishEngine::setSkillLevel(int level)
{
    if (!m_isReady) {
        qWarning() << "Engine not ready";
        return;
    }

    // 限制技能等級在 0-20 之間
    level = qBound(0, level, 20);
    
    sendCommand(QString("setoption name Skill Level value %1").arg(level));
    
    // 等待引擎準備就緒
    sendCommand("isready");
    waitForResponse("readyok", 2000);
}

void StockfishEngine::stop()
{
    if (m_process && m_process->state() == QProcess::Running) {
        sendCommand("stop");
    }
}

void StockfishEngine::quit()
{
    if (m_process && m_process->state() == QProcess::Running) {
        sendCommand("quit");
        m_process->waitForFinished(1000);
    }
    m_isReady = false;
    m_isInitialized = false;
}
