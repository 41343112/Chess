#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "chesspiece.h"
#include <QVector>
#include <QPoint>
#include <QObject>
#include <memory>

struct Move {
    QPoint from;
    QPoint to;
    ChessPiece* capturedPiece;
    bool wasCastling;
    bool wasEnPassant;
    bool wasPromotion;
    PieceType promotedTo;
    bool movedPieceHadMoved;  // 追蹤移動的棋子在此移動前是否已移動過
    QPoint previousEnPassantTarget;  // 儲存此移動前的吃過路兵目標
    PieceType movedPieceType;  // 儲存移動的棋子類型（用於撤銷升變）
    PieceColor movedPieceColor;  // 儲存移動的棋子顏色

    Move() : capturedPiece(nullptr), wasCastling(false), wasEnPassant(false),
        wasPromotion(false), promotedTo(PieceType::QUEEN), movedPieceHadMoved(false),
        previousEnPassantTarget(-1, -1), movedPieceType(PieceType::PAWN), 
        movedPieceColor(PieceColor::WHITE) {}
} ;

class ChessBoard : public QObject {
    Q_OBJECT
    
public:
    ChessBoard();
    ~ChessBoard();

    void initializeBoard();
    void reset();

    ChessPiece* getPieceAt(QPoint pos) const;
    ChessPiece* getPieceAt(int row, int col) const;
    bool isValidPosition(QPoint pos) const;
    bool isValidPosition(int row, int col) const;

    bool movePiece(QPoint from, QPoint to, bool checkOnly = false);
    bool canMove(QPoint from, QPoint to) const;
    bool wouldBePromotion(QPoint from, QPoint to) const;  // 檢查移動是否會導致升變
    void setPromotionPieceType(PieceType type) { m_promotionPieceType = type; }  // 設定升變棋子類型

    bool isKingInCheck(PieceColor color) const;
    bool isCheckmate(PieceColor color);  // 檢查國王是否被將軍且無有效移動
    bool isStalemate(PieceColor color);  // 檢查是否未被將軍但無有效移動
    bool isInsufficientMaterial() const;  // 檢查是否棋子不足以將死

    PieceColor getCurrentTurn() const { return m_currentTurn; }
    void switchTurn();

    QVector<Move>& getMoveHistory() { return m_moveHistory; }
    const QVector<Move>& getMoveHistory() const { return m_moveHistory; }

    bool canCastle(PieceColor color, bool kingSide) const;
    void performCastling(PieceColor color, bool kingSide);

    QPoint getEnPassantTarget() const { return m_enPassantTarget; }
    void setEnPassantTarget(QPoint pos) { m_enPassantTarget = pos; }

    QString getGameStatus() const { return m_gameStatus; }
    bool isGameOver() const { return m_isGameOver; }
    void setGameOver(const QString& status);  // 設定遊戲結束狀態

    bool undo();  // 撤銷上一步移動
    void getBoardStateAtMove(int moveIndex, ChessPiece* outputBoard[8][8], PieceColor& turn) const;

private:
    ChessPiece* m_board[8][8];
    PieceColor m_currentTurn;
    QVector<Move> m_moveHistory;
    QPoint m_enPassantTarget;
    QString m_gameStatus;
    bool m_isGameOver;
    PieceType m_promotionPieceType;  // 儲存玩家選擇的升變棋子類型

    void clearBoard();
    bool wouldBeInCheck(QPoint from, QPoint to, PieceColor color) const; // 常量查詢
    QPoint findKing(PieceColor color) const;
    bool hasAnyValidMoves(PieceColor color);

    // 在提供的棋盤陣列上操作的輔助函數：
    QPoint findKingOnBoard(ChessPiece* const board[8][8], PieceColor color) const;
    bool isSquareAttacked(ChessPiece* const board[8][8], QPoint square, PieceColor attackerColor) const;
};

#endif // CHESSBOARD_H
