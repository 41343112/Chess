#ifndef UCIENGINE_H
#define UCIENGINE_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QQueue>
#include "chessboard.h"
#include "chesspiece.h"

class UCIEngine : public QObject
{
    Q_OBJECT

public:
    explicit UCIEngine(QObject *parent = nullptr);
    ~UCIEngine();

    // 初始化引擎
    bool initialize(const QString& enginePath);
    
    // 設定技能等級 (0-20, Stockfish 支援)
    void setSkillLevel(int level);
    
    // 取得最佳移動
    void getBestMove(ChessBoard* board, PieceColor color);
    
    // 停止引擎
    void stop();
    
    // 檢查引擎是否準備就緒
    bool isReady() const { return m_isReady; }

signals:
    void bestMoveFound(QString from, QString to);
    void engineError(QString error);

private slots:
    void onReadyRead();
    void onErrorOccurred(QProcess::ProcessError error);

private:
    QProcess* m_process;
    bool m_isReady;
    int m_skillLevel;
    QString m_pendingCommand;
    
    void sendCommand(const QString& command);
    void processLine(const QString& line);
    QString positionToUCI(int row, int col);
    QPoint uciToPosition(const QString& uci);
    QString boardToFEN(ChessBoard* board, PieceColor turn);
};

#endif // UCIENGINE_H
