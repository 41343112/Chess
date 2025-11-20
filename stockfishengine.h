#ifndef STOCKFISHENGINE_H
#define STOCKFISHENGINE_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QPoint>
#include <QPair>
#include "chessboard.h"

class StockfishEngine : public QObject
{
    Q_OBJECT

public:
    explicit StockfishEngine(QObject *parent = nullptr);
    ~StockfishEngine();

    // Initialize the engine
    bool initialize();
    
    // Set skill level (0-20, where 20 is strongest)
    void setSkillLevel(int level);
    
    // Get best move for current position
    // Returns pair of from/to positions, or (-1,-1) if no valid move
    QPair<QPoint, QPoint> getBestMove(ChessBoard* board, PieceColor color);
    
    // Check if engine is ready
    bool isReady() const { return m_isReady; }
    
    // Shutdown the engine
    void shutdown();

signals:
    void engineReady();
    void bestMoveFound(QString move);
    void error(QString errorMessage);

private slots:
    void onReadyRead();
    void onErrorOccurred(QProcess::ProcessError error);

private:
    QProcess* m_process;
    QString m_enginePath;
    int m_skillLevel;
    bool m_isReady;
    bool m_isInitialized;
    QString m_pendingOutput;
    QString m_bestMove;
    
    // Helper methods
    QString findStockfishPath();
    void sendCommand(const QString& command);
    QString waitForResponse(const QString& expectedResponse, int timeoutMs = 5000);
    QString boardToFEN(ChessBoard* board, PieceColor currentTurn);
    QPair<QPoint, QPoint> uciMoveToPoints(const QString& uciMove);
};

#endif // STOCKFISHENGINE_H
