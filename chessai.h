#ifndef CHESSAI_H
#define CHESSAI_H

#include "chessboard.h"
#include "chesspiece.h"
#include <QPoint>
#include <QVector>
#include <QPair>

enum class AIDifficulty {
    EASY,
    MEDIUM,
    HARD
};

class ChessAI {
public:
    ChessAI(AIDifficulty difficulty = AIDifficulty::MEDIUM);
    ~ChessAI();

    // 取得電腦的最佳移動
    QPair<QPoint, QPoint> getBestMove(ChessBoard* board, PieceColor aiColor);

    void setDifficulty(AIDifficulty difficulty) { m_difficulty = difficulty; }
    AIDifficulty getDifficulty() const { return m_difficulty; }

private:
    AIDifficulty m_difficulty;

    // 不同難度的移動策略
    QPair<QPoint, QPoint> getRandomMove(ChessBoard* board, PieceColor aiColor);
    QPair<QPoint, QPoint> getBasicEvaluationMove(ChessBoard* board, PieceColor aiColor);
    QPair<QPoint, QPoint> getMinimaxMove(ChessBoard* board, PieceColor aiColor);

    // 輔助函數
    QVector<QPair<QPoint, QPoint>> getAllValidMoves(ChessBoard* board, PieceColor color);
    int evaluateBoard(ChessBoard* board, PieceColor aiColor);
    int getPieceValue(ChessPiece* piece);
    int minimax(ChessBoard* board, int depth, int alpha, int beta, bool maximizingPlayer, PieceColor aiColor);
};

#endif // CHESSAI_H
