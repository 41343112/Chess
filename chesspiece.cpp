#include "chesspiece.h"
#include "chessboard.h"
#include <cmath>
#include <QPixmap>

ChessPiece::ChessPiece(PieceType type, PieceColor color, QPoint position)
    : m_type(type), m_color(color), m_position(position), m_hasMoved(false) {
}

QString ChessPiece::getSymbol() const {
    QString symbols[6][2] = {
        {QString::fromUtf8("\u2659"), QString::fromUtf8("\u265f")}, // 兵
        {QString::fromUtf8("\u2656"), QString::fromUtf8("\u265c")}, // 車
        {QString::fromUtf8("\u2658"), QString::fromUtf8("\u265e")}, // 馬
        {QString::fromUtf8("\u2657"), QString::fromUtf8("\u265d")}, // 象
        {QString::fromUtf8("\u2655"), QString::fromUtf8("\u265b")}, // 后
        {QString::fromUtf8("\u2654"), QString::fromUtf8("\u265a")}  // 王
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
        {":/images/images/king.png",  ":/images/images/b_king.png"}
    };

    int t = static_cast<int>(m_type);
    int c = (m_color == PieceColor::WHITE) ? 0 : 1;
    QString resourcePath = QString::fromUtf8(paths[t][c]);
    QPixmap pix(resourcePath);
    return pix; // 若找不到資源，pix.isNull() == true
}

// 兵的實作
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

    // 向前移動
    if (dx == 0) {
        if (dy == direction && targetPiece == nullptr) {
            return true;
        }
        // 初始兩格移動
        if (dy == 2 * direction && !m_hasMoved && targetPiece == nullptr) {
            QPoint middlePos(m_position.x(), m_position.y() + direction);
            if (board->getPieceAt(middlePos) == nullptr) {
                return true;
            }
        }
    }
    // 對角線吃子
    else if (abs(dx) == 1 && dy == direction) {
        if (targetPiece != nullptr && targetPiece->getColor() != m_color) {
            return true;
        }
        // 吃過路兵
        if (newPos == board->getEnPassantTarget()) {
            return true;
        }
    }

    return false;
}

// 車的實作
Rook::Rook(PieceColor color, QPoint position)
    : ChessPiece(PieceType::ROOK, color, position) {
}

bool Rook::isValidMove(QPoint newPos, ChessBoard* board) const {
    if (!board->isValidPosition(newPos)) return false;
    if (newPos == m_position) return false;

    int dx = newPos.x() - m_position.x();
    int dy = newPos.y() - m_position.y();

    // 必須沿直線移動
    if (dx != 0 && dy != 0) return false;

    // 檢查路徑是否暢通
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

    // 檢查目標位置
    ChessPiece* targetPiece = board->getPieceAt(newPos);
    if (targetPiece != nullptr && targetPiece->getColor() == m_color) {
        return false;
    }

    return true;
}

// 馬的實作
Knight::Knight(PieceColor color, QPoint position)
    : ChessPiece(PieceType::KNIGHT, color, position) {
}

bool Knight::isValidMove(QPoint newPos, ChessBoard* board) const {
    if (!board->isValidPosition(newPos)) return false;
    if (newPos == m_position) return false;

    int dx = abs(newPos.x() - m_position.x());
    int dy = abs(newPos.y() - m_position.y());

    // L形移動：一個方向移動2格，另一個方向移動1格
    if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2))) {
        return false;
    }

    ChessPiece* targetPiece = board->getPieceAt(newPos);
    if (targetPiece != nullptr && targetPiece->getColor() == m_color) {
        return false;
    }

    return true;
}

// 象的實作
Bishop::Bishop(PieceColor color, QPoint position)
    : ChessPiece(PieceType::BISHOP, color, position) {
}

bool Bishop::isValidMove(QPoint newPos, ChessBoard* board) const {
    if (!board->isValidPosition(newPos)) return false;
    if (newPos == m_position) return false;

    int dx = newPos.x() - m_position.x();
    int dy = newPos.y() - m_position.y();

    // 必須沿對角線移動
    if (abs(dx) != abs(dy)) return false;

    // 檢查路徑是否暢通
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

    // 檢查目標位置
    ChessPiece* targetPiece = board->getPieceAt(newPos);
    if (targetPiece != nullptr && targetPiece->getColor() == m_color) {
        return false;
    }

    return true;
}

// 后的實作
Queen::Queen(PieceColor color, QPoint position)
    : ChessPiece(PieceType::QUEEN, color, position) {
}

bool Queen::isValidMove(QPoint newPos, ChessBoard* board) const {
    if (!board->isValidPosition(newPos)) return false;
    if (newPos == m_position) return false;

    int dx = newPos.x() - m_position.x();
    int dy = newPos.y() - m_position.y();

    // 必須沿直線或對角線移動
    if (dx != 0 && dy != 0 && abs(dx) != abs(dy)) return false;

    // 檢查路徑是否暢通
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

    // 檢查目標位置
    ChessPiece* targetPiece = board->getPieceAt(newPos);
    if (targetPiece != nullptr && targetPiece->getColor() == m_color) {
        return false;
    }

    return true;
}

// 王的實作
King::King(PieceColor color, QPoint position)
    : ChessPiece(PieceType::KING, color, position) {
}

bool King::isValidMove(QPoint newPos, ChessBoard* board) const {
    if (!board->isValidPosition(newPos)) return false;
    if (newPos == m_position) return false;

    int dx = abs(newPos.x() - m_position.x());
    int dy = abs(newPos.y() - m_position.y());

    // 王的正常移動（任意方向一格）
    if (dx <= 1 && dy <= 1) {
        ChessPiece* targetPiece = board->getPieceAt(newPos);
        if (targetPiece != nullptr && targetPiece->getColor() == m_color) {
            return false;
        }
        return true;
    }

    // 王車易位（在 ChessBoard 中另外處理）
    if (!m_hasMoved && dy == 0 && dx == 2) {
        return true;
    }

    return false;
}
