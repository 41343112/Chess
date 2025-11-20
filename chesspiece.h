#ifndef CHESSPIECE_H
#define CHESSPIECE_H

#include <QString>
#include <QPoint>
#include <QPixmap>    // 新增

enum class PieceType {
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING
};

enum class PieceColor {
    WHITE,
    BLACK
};

class ChessPiece {
public:
    ChessPiece(PieceType type, PieceColor color, QPoint position);
    virtual ~ChessPiece() = default;

    PieceType getType() const { return m_type; }
    PieceColor getColor() const { return m_color; }
    QPoint getPosition() const { return m_position; }
    void setPosition(QPoint pos) { m_position = pos; }
    bool hasMoved() const { return m_hasMoved; }
    void setMoved(bool moved) { m_hasMoved = moved; }

    QString getSymbol() const;

    // 新增：回傳棋子圖像（若無對應圖像可回傳 null pixmap）
    QPixmap getPixmap() const;

    // 檢查移動是否有效的虛擬方法
    virtual bool isValidMove(QPoint newPos, class ChessBoard* board) const = 0;

protected:
    PieceType m_type;
    PieceColor m_color;
    QPoint m_position;
    bool m_hasMoved;
};

class Pawn : public ChessPiece {
public:
    Pawn(PieceColor color, QPoint position);
    bool isValidMove(QPoint newPos, ChessBoard* board) const override;
};

class Rook : public ChessPiece {
public:
    Rook(PieceColor color, QPoint position);
    bool isValidMove(QPoint newPos, ChessBoard* board) const override;
};

class Knight : public ChessPiece {
public:
    Knight(PieceColor color, QPoint position);
    bool isValidMove(QPoint newPos, ChessBoard* board) const override;
};

class Bishop : public ChessPiece {
public:
    Bishop(PieceColor color, QPoint position);
    bool isValidMove(QPoint newPos, ChessBoard* board) const override;
};

class Queen : public ChessPiece {
public:
    Queen(PieceColor color, QPoint position);
    bool isValidMove(QPoint newPos, ChessBoard* board) const override;
};

class King : public ChessPiece {
public:
    King(PieceColor color, QPoint position);
    bool isValidMove(QPoint newPos, ChessBoard* board) const override;
};

#endif // CHESSPIECE_H
