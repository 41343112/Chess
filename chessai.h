#ifndef CHESSAI_H
#define CHESSAI_H

#include "chessboard.h"
#include "chesspiece.h"
#include "uciengine.h"
#include <QPoint>
#include <QVector>
#include <QPair>
#include <QObject>

enum class AIDifficulty {
    EASY,
    MEDIUM,
    HARD
};

class ChessAI : public QObject {
    Q_OBJECT
    
public:
    ChessAI(AIDifficulty difficulty = AIDifficulty::MEDIUM, QObject* parent = nullptr);
    ~ChessAI();

    // 取得電腦的最佳移動（非同步，透過訊號返回）
    void getBestMove(ChessBoard* board, PieceColor aiColor);

    void setDifficulty(AIDifficulty difficulty);
    AIDifficulty getDifficulty() const { return m_difficulty; }
    
    // 設定引擎技能等級 (0-20)
    void setSkillLevel(int level);
    int getSkillLevel() const { return m_skillLevel; }
    
    // 初始化引擎
    bool initializeEngine(const QString& enginePath);
    
    // 使用引擎模式
    void setUseEngine(bool useEngine) { m_useEngine = useEngine; }
    bool isUsingEngine() const { return m_useEngine; }

signals:
    void moveReady(QPoint from, QPoint to);
    void engineError(QString error);

private slots:
    void onEngineMoveFound(QString fromUCI, QString toUCI);
    void onEngineError(QString error);

private:
    AIDifficulty m_difficulty;
    int m_skillLevel;
    bool m_useEngine;
    UCIEngine* m_engine;
    ChessBoard* m_currentBoard;
    PieceColor m_currentColor;

    // 不同難度的移動策略（備用，當引擎不可用時）
    QPair<QPoint, QPoint> getRandomMove(ChessBoard* board, PieceColor aiColor);
    QPair<QPoint, QPoint> getBasicEvaluationMove(ChessBoard* board, PieceColor aiColor);
    QPair<QPoint, QPoint> getMinimaxMove(ChessBoard* board, PieceColor aiColor);

    // 輔助函數
    QVector<QPair<QPoint, QPoint>> getAllValidMoves(ChessBoard* board, PieceColor color);
    int evaluateBoard(ChessBoard* board, PieceColor aiColor);
    int getPieceValue(ChessPiece* piece);
    int minimax(ChessBoard* board, int depth, int alpha, int beta, bool maximizingPlayer, PieceColor aiColor);
    QPoint uciToPosition(const QString& uci);
    void updateSkillLevelFromDifficulty();
};

#endif // CHESSAI_H
