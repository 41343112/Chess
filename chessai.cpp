#include "chessai.h"
#include <QDebug>

StockfishEngine::StockfishEngine(QObject *parent)
    : QObject(parent)
{
    connect(&engine, &QProcess::readyReadStandardOutput,
            this, &StockfishEngine::receiveOutput);
}

bool StockfishEngine::start(const QString &path)
{
    engine.start(path);

    return engine.waitForStarted();
}

void StockfishEngine::stop()
{
    sendCommand("quit");
    engine.close();
}

void StockfishEngine::sendCommand(const QString &cmd)
{
    engine.write((cmd + "\n").toUtf8());
}

void StockfishEngine::setPosition(const QString &fenOrMoves)
{
    sendCommand("position " + fenOrMoves);
}

void StockfishEngine::goDepth(int depth)
{
    sendCommand("go depth " + QString::number(depth));
}

void StockfishEngine::goMovetime(int ms)
{
    sendCommand("go movetime " + QString::number(ms));
}

void StockfishEngine::receiveOutput()
{
    QString out = engine.readAllStandardOutput();
    emit engineOutput(out); // 給你 debug 用

    // 找 bestmove
    if (out.contains("bestmove")) {
        int idx = out.indexOf("bestmove") + 9;
        QString move = out.mid(idx, 10).split(" ").first().trimmed();
        emit bestMoveReady(move);
    }
}
