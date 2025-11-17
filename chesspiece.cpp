#include "chesspiece.h"
#include "chessboard.h"
#include <cmath>

ChessPiece::ChessPiece(PieceType type, PieceColor color, QPoint position)
    : m_type(type), m_color(color), m_position(position), m_hasMoved(false) {
}

QString ChessPiece::getSymbol() const {
    QString symbols[6][2] = {
        {"♙", "♟"}, // Pawn
        {"♖", "♜"}, // Rook
        {"♘", "♞"}, // Knight
        {"♗", "♝"}, // Bishop
        {"♕", "♛"}, // Queen
        {"♔", "♚"}  // King
    };
    
    int typeIndex = static_cast<int>(m_type);
    int colorIndex = m_color == PieceColor::WHITE ? 0 : 1;
    return symbols[typeIndex][colorIndex];
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
        // This also checks for blocking: the middle square must be empty
        if (dy == 2 * direction && !m_hasMoved && targetPiece == nullptr) {
            QPoint middlePos(m_position.x(), m_position.y() + direction);
            if (board->getPieceAt(middlePos) == nullptr) {
                return true;  // Both middle and target squares are empty
            }
            // If middle square is blocked, the pawn cannot move two squares
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
    
    // Check if path is clear (requirement: blocked squares should not be shown as valid)
    // This ensures that when a piece blocks the path, all squares beyond it are invalid
    int stepX = (dx != 0) ? (dx > 0 ? 1 : -1) : 0;
    int stepY = (dy != 0) ? (dy > 0 ? 1 : -1) : 0;
    
    int currentX = m_position.x() + stepX;
    int currentY = m_position.y() + stepY;
    
    // Check each square along the path (excluding start and target)
    while (currentX != newPos.x() || currentY != newPos.y()) {
        if (board->getPieceAt(currentX, currentY) != nullptr) {
            // Path is blocked! This piece cannot move to the target square.
            // Consequently, all squares beyond this point are also invalid.
            return false;
        }
        currentX += stepX;
        currentY += stepY;
    }
    
    // Check target square - can move to empty square or capture enemy piece
    ChessPiece* targetPiece = board->getPieceAt(newPos);
    if (targetPiece != nullptr && targetPiece->getColor() == m_color) {
        return false;  // Cannot capture own piece
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
    
    // Check if path is clear (requirement: blocked squares should not be shown as valid)
    // This ensures that when a piece blocks the path, all squares beyond it are invalid
    int stepX = dx > 0 ? 1 : -1;
    int stepY = dy > 0 ? 1 : -1;
    
    int currentX = m_position.x() + stepX;
    int currentY = m_position.y() + stepY;
    
    // Check each square along the diagonal path (excluding start and target)
    while (currentX != newPos.x() || currentY != newPos.y()) {
        if (board->getPieceAt(currentX, currentY) != nullptr) {
            // Path is blocked! This piece cannot move to the target square.
            // Consequently, all squares beyond this point are also invalid.
            return false;
        }
        currentX += stepX;
        currentY += stepY;
    }
    
    // Check target square - can move to empty square or capture enemy piece
    ChessPiece* targetPiece = board->getPieceAt(newPos);
    if (targetPiece != nullptr && targetPiece->getColor() == m_color) {
        return false;  // Cannot capture own piece
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
    
    // Check if path is clear (requirement: blocked squares should not be shown as valid)
    // This ensures that when a piece blocks the path, all squares beyond it are invalid
    int stepX = (dx != 0) ? (dx > 0 ? 1 : -1) : 0;
    int stepY = (dy != 0) ? (dy > 0 ? 1 : -1) : 0;
    
    int currentX = m_position.x() + stepX;
    int currentY = m_position.y() + stepY;
    
    // Check each square along the path (excluding start and target)
    while (currentX != newPos.x() || currentY != newPos.y()) {
        if (board->getPieceAt(currentX, currentY) != nullptr) {
            // Path is blocked! This piece cannot move to the target square.
            // Consequently, all squares beyond this point are also invalid.
            return false;
        }
        currentX += stepX;
        currentY += stepY;
    }
    
    // Check target square - can move to empty square or capture enemy piece
    ChessPiece* targetPiece = board->getPieceAt(newPos);
    if (targetPiece != nullptr && targetPiece->getColor() == m_color) {
        return false;  // Cannot capture own piece
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
