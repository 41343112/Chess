#ifndef STOCKFISHENGINE_HPP
#define STOCKFISHENGINE_HPP

#include <QObject>
#include <QProcess>

class StockfishEngine : public QObject
{
    Q_OBJECT

public:
    explicit StockfishEngine(QObject *parent = nullptr);

    bool start(const QString &path = "stockfish.exe");
    void stop();

    void sendCommand(const QString &cmd);
    void setPosition(const QString &fenOrMoves);
    void goDepth(int depth);
    void goMovetime(int ms);

signals:
    void bestMoveReady(const QString &move);
    void engineOutput(const QString &text);

private slots:
    void receiveOutput();

private:
    QProcess engine;
};

#endif // STOCKFISHENGINE_HPP
