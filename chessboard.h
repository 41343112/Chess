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
    bool movedPieceHadMoved;  // Track if the moved piece had moved before this move
    QPoint previousEnPassantTarget;  // Store the en passant target before this move
    PieceType movedPieceType;  // Store the type of piece that moved (needed for promotion undo)
    PieceColor movedPieceColor;  // Store the color of piece that moved

    Move() : capturedPiece(nullptr), wasCastling(false), wasEnPassant(false),
        wasPromotion(false), promotedTo(PieceType::QUEEN), movedPieceHadMoved(false),
        previousEnPassantTarget(-1, -1), movedPieceType(PieceType::PAWN), 
        movedPieceColor(PieceColor::WHITE) {}
} ;

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
    bool isCheckmate(PieceColor color);  // Checks if king is in check with no valid moves
    bool isStalemate(PieceColor color);  // Checks if not in check but no valid moves
    bool isInsufficientMaterial() const;  // Checks if there's insufficient material for checkmate

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

    bool undo();  // Undo the last move

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
