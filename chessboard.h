#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "chesspiece.h"
#include <QVector>
#include <QPoint>
#include <memory>

              struct Move {
    QPoint from;
    QPoint to;
    ChessPiece* capturedPiece;
    bool wasCastling;
    bool wasEnPassant;
    bool wasPromotion;
    PieceType promotedTo;

    Move() : capturedPiece(nullptr), wasCastling(false), wasEnPassant(false),
        wasPromotion(false), promotedTo(PieceType::QUEEN) {}
};

class ChessBoard {
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

    bool isKingInCheck(PieceColor color) const;
    bool isCheckmate(PieceColor color);
    bool isStalemate(PieceColor color);

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

private:
    ChessPiece* m_board[8][8];
    PieceColor m_currentTurn;
    QVector<Move> m_moveHistory;
    QPoint m_enPassantTarget;
    QString m_gameStatus;
    bool m_isGameOver;

    void clearBoard();
    bool wouldBeInCheck(QPoint from, QPoint to, PieceColor color) const; // const query
    QPoint findKing(PieceColor color) const;
    bool hasAnyValidMoves(PieceColor color);

    // helpers that operate on a provided board array:
    QPoint findKingOnBoard(ChessPiece* const board[8][8], PieceColor color) const;
    bool isSquareAttacked(ChessPiece* const board[8][8], QPoint square, PieceColor attackerColor) const;
};

#endif // CHESSBOARD_H
