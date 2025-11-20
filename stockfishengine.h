#ifndef STOCKFISHENGINE_H
#define STOCKFISHENGINE_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QPoint>
#include <QPair>
#include "chesspiece.h"

class StockfishEngine : public QObject {
    Q_OBJECT

public:
    explicit StockfishEngine(QObject* parent = nullptr);
    ~StockfishEngine();

    // 初始化引擎
    bool initialize(const QString& enginePath);
    
    // 檢查引擎是否準備好
    bool isReady() const { return m_isReady; }
    
    // 設定棋盤位置（FEN格式）
    void setPosition(const QString& fen);
    
    // 設定棋盤位置（從初始位置開始的移動列表）
    void setPositionWithMoves(const QVector<QString>& moves);
    
    // 取得最佳移動（UCI格式，例如 "e2e4"）
    QString getBestMove(int thinkingTimeMs = 1000);
    
    // 設定引擎技能等級 (0-20, 20是最強)
    void setSkillLevel(int level);
    
    // 停止引擎
    void stop();
    
    // 關閉引擎
    void quit();

signals:
    void engineReady();
    void bestMoveFound(const QString& move);
    void errorOccurred(const QString& error);

private slots:
    void onReadyRead();
    void onErrorOccurred(QProcess::ProcessError error);

private:
    QProcess* m_process;
    bool m_isReady;
    bool m_isInitialized;
    QString m_lastResponse;
    QString m_pendingBestMove;
    
    // 發送UCI命令
    void sendCommand(const QString& command);
    
    // 等待特定回應
    bool waitForResponse(const QString& expectedResponse, int timeoutMs = 5000);
    
    // 解析引擎輸出
    void parseOutput(const QString& output);
};

#endif // STOCKFISHENGINE_H
