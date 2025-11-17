#include "chesspiece.h"
#include "chessboard.h"
#include <cmath>
#include <QPixmap>

ChessPiece::ChessPiece(PieceType type, PieceColor color, QPoint position)
    : m_type(type), m_color(color), m_position(position), m_hasMoved(false) {
}

QString ChessPiece::getSymbol() const {
    QString symbols[6][2] = {
        {QString::fromUtf8("\u2659"), QString::fromUtf8("\u265f")}, // Pawn
        {QString::fromUtf8("\u2656"), QString::fromUtf8("\u265c")}, // Rook
        {QString::fromUtf8("\u2658"), QString::fromUtf8("\u265e")}, // Knight
        {QString::fromUtf8("\u2657"), QString::fromUtf8("\u265d")}, // Bishop
        {QString::fromUtf8("\u2655"), QString::fromUtf8("\u265b")}, // Queen
        {QString::fromUtf8("\u2654"), QString::fromUtf8("\u265a")}  // King
    };

    int typeIndex = static_cast<int>(m_type);
    int colorIndex = m_color == PieceColor::WHITE ? 0 : 1;
    return symbols[typeIndex][colorIndex];
}

// 新增：根據類型/顏色回傳對應資源路徑的 QPixmap（若沒有圖檔會回傳 isNull() 的 QPixmap）
QPixmap ChessPiece::getPixmap() const {
    static const char* paths[6][2] = {
        {":/images/images/pawn.png",  ":/images/images/b_pawn.png"},
        {":/images/images/rook.png",  ":/images/images/b_rook.png"},
        {":/images/images/knight.png",":/images/images/b_knight.png"},
        {":/images/images/biship.png",":/images/images/b_biship.png"},
        {":/images/images/queen.png", ":/images/images/b_queen.png"},
        {":/images/images/king.png",  ":/image/images/b_king.png"}
    };

    int t = static_cast<int>(m_type);
    int c = (m_color == PieceColor::WHITE) ? 0 : 1;
    QString resourcePath = QString::fromUtf8(paths[t][c]);
    QPixmap pix(resourcePath);
    return pix; // 若找不到資源，pix.isNull() == true
}

// Pawn implementation
Pawn::Pawn(PieceColor color, QPoint position)
    : ChessPiece(PieceType::PAWN, color, position) {
}

bool Pawn::isValidMove(QPoint newPos, ChessBoard* board) const {
    if (!board->isValidPosition(newPos)) return false;
    if (newPos == m_position) return false;

    int direction = (m_color == PieceColor::WHITE) ? -1 : 1;
    int dy = newPos.y() - m_position.y();
    int dx = newPos.x() - m_position.x();

    ChessPiece* targetPiece = board->getPieceAt(newPos);

    // Forward move
    if (dx == 0) {
        if (dy == direction && targetPiece == nullptr) {
            return true;
        }
        // Initial two-square move
        if (dy == 2 * direction && !m_hasMoved && targetPiece == nullptr) {
            QPoint middlePos(m_position.x(), m_position.y() + direction);
            if (board->getPieceAt(middlePos) == nullptr) {
                return true;
            }
        }
    }
    // Diagonal capture
    else if (abs(dx) == 1 && dy == direction) {
        if (targetPiece != nullptr && targetPiece->getColor() != m_color) {
            return true;
        }
        // En passant
        if (newPos == board->getEnPassantTarget()) {
            return true;
        }
    }

    return false;
}

// Rook implementation
Rook::Rook(PieceColor color, QPoint position)
    : ChessPiece(PieceType::ROOK, color, position) {
}

bool Rook::isValidMove(QPoint newPos, ChessBoard* board) const {
    if (!board->isValidPosition(newPos)) return false;
    if (newPos == m_position) return false;

    int dx = newPos.x() - m_position.x();
    int dy = newPos.y() - m_position.y();

    // Must move in straight line
    if (dx != 0 && dy != 0) return false;

    // Check if path is clear
    int stepX = (dx != 0) ? (dx > 0 ? 1 : -1) : 0;
    int stepY = (dy != 0) ? (dy > 0 ? 1 : -1) : 0;

    int currentX = m_position.x() + stepX;
    int currentY = m_position.y() + stepY;

    while (currentX != newPos.x() || currentY != newPos.y()) {
        if (board->getPieceAt(currentY, currentX) != nullptr) {
            return false;
        }
        currentX += stepX;
        currentY += stepY;
    }

    // Check target square
    ChessPiece* targetPiece = board->getPieceAt(newPos);
    if (targetPiece != nullptr && targetPiece->getColor() == m_color) {
        return false;
    }

    return true;
}

// Knight implementation
Knight::Knight(PieceColor color, QPoint position)
    : ChessPiece(PieceType::KNIGHT, color, position) {
}

bool Knight::isValidMove(QPoint newPos, ChessBoard* board) const {
    if (!board->isValidPosition(newPos)) return false;
    if (newPos == m_position) return false;

    int dx = abs(newPos.x() - m_position.x());
    int dy = abs(newPos.y() - m_position.y());

    // L-shape: 2 squares in one direction, 1 in the other
    if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2))) {
        return false;
    }

    ChessPiece* targetPiece = board->getPieceAt(newPos);
    if (targetPiece != nullptr && targetPiece->getColor() == m_color) {
        return false;
    }

    return true;
}

// Bishop implementation
Bishop::Bishop(PieceColor color, QPoint position)
    : ChessPiece(PieceType::BISHOP, color, position) {
}

bool Bishop::isValidMove(QPoint newPos, ChessBoard* board) const {
    if (!board->isValidPosition(newPos)) return false;
    if (newPos == m_position) return false;

    int dx = newPos.x() - m_position.x();
    int dy = newPos.y() - m_position.y();

    // Must move diagonally
    if (abs(dx) != abs(dy)) return false;

    // Check if path is clear
    int stepX = dx > 0 ? 1 : -1;
    int stepY = dy > 0 ? 1 : -1;

    int currentX = m_position.x() + stepX;
    int currentY = m_position.y() + stepY;

    while (currentX != newPos.x() || currentY != newPos.y()) {
        if (board->getPieceAt(currentY, currentX) != nullptr) {
            return false;
        }
        currentX += stepX;
        currentY += stepY;
    }

    // Check target square
    ChessPiece* targetPiece = board->getPieceAt(newPos);
    if (targetPiece != nullptr && targetPiece->getColor() == m_color) {
        return false;
    }

    return true;
}

// Queen implementation
Queen::Queen(PieceColor color, QPoint position)
    : ChessPiece(PieceType::QUEEN, color, position) {
}

bool Queen::isValidMove(QPoint newPos, ChessBoard* board) const {
    if (!board->isValidPosition(newPos)) return false;
    if (newPos == m_position) return false;

    int dx = newPos.x() - m_position.x();
    int dy = newPos.y() - m_position.y();

    // Must move in straight line or diagonal
    if (dx != 0 && dy != 0 && abs(dx) != abs(dy)) return false;

    // Check if path is clear
    int stepX = (dx != 0) ? (dx > 0 ? 1 : -1) : 0;
    int stepY = (dy != 0) ? (dy > 0 ? 1 : -1) : 0;

    int currentX = m_position.x() + stepX;
    int currentY = m_position.y() + stepY;

    while (currentX != newPos.x() || currentY != newPos.y()) {
        if (board->getPieceAt(currentY, currentX) != nullptr) {
            return false;
        }
        currentX += stepX;
        currentY += stepY;
    }

    // Check target square
    ChessPiece* targetPiece = board->getPieceAt(newPos);
    if (targetPiece != nullptr && targetPiece->getColor() == m_color) {
        return false;
    }

    return true;
}

// King implementation
King::King(PieceColor color, QPoint position)
    : ChessPiece(PieceType::KING, color, position) {
}

bool King::isValidMove(QPoint newPos, ChessBoard* board) const {
    if (!board->isValidPosition(newPos)) return false;
    if (newPos == m_position) return false;

    int dx = abs(newPos.x() - m_position.x());
    int dy = abs(newPos.y() - m_position.y());

    // Normal king move (one square in any direction)
    if (dx <= 1 && dy <= 1) {
        ChessPiece* targetPiece = board->getPieceAt(newPos);
        if (targetPiece != nullptr && targetPiece->getColor() == m_color) {
            return false;
        }
        return true;
    }

    // Castling (handled separately in ChessBoard)
    if (!m_hasMoved && dy == 0 && dx == 2) {
        return true;
    }

    return false;
}
