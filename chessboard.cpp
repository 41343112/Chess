#include "chessboard.h"
#include <QDebug>

ChessBoard::ChessBoard()
    : m_currentTurn(PieceColor::WHITE), m_enPassantTarget(-1, -1), 
      m_isGameOver(false) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            m_board[i][j] = nullptr;
        }
    }
    initializeBoard();
}

ChessBoard::~ChessBoard() {
    clearBoard();
}

void ChessBoard::clearBoard() {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (m_board[i][j] != nullptr) {
                delete m_board[i][j];
                m_board[i][j] = nullptr;
            }
        }
    }
}

void ChessBoard::initializeBoard() {
    clearBoard();
    
    // Place pawns
    for (int col = 0; col < 8; ++col) {
        m_board[1][col] = new Pawn(PieceColor::BLACK, QPoint(col, 1));
        m_board[6][col] = new Pawn(PieceColor::WHITE, QPoint(col, 6));
    }
    
    // Place rooks
    m_board[0][0] = new Rook(PieceColor::BLACK, QPoint(0, 0));
    m_board[0][7] = new Rook(PieceColor::BLACK, QPoint(7, 0));
    m_board[7][0] = new Rook(PieceColor::WHITE, QPoint(0, 7));
    m_board[7][7] = new Rook(PieceColor::WHITE, QPoint(7, 7));
    
    // Place knights
    m_board[0][1] = new Knight(PieceColor::BLACK, QPoint(1, 0));
    m_board[0][6] = new Knight(PieceColor::BLACK, QPoint(6, 0));
    m_board[7][1] = new Knight(PieceColor::WHITE, QPoint(1, 7));
    m_board[7][6] = new Knight(PieceColor::WHITE, QPoint(6, 7));
    
    // Place bishops
    m_board[0][2] = new Bishop(PieceColor::BLACK, QPoint(2, 0));
    m_board[0][5] = new Bishop(PieceColor::BLACK, QPoint(5, 0));
    m_board[7][2] = new Bishop(PieceColor::WHITE, QPoint(2, 7));
    m_board[7][5] = new Bishop(PieceColor::WHITE, QPoint(5, 7));
    
    // Place queens
    m_board[0][3] = new Queen(PieceColor::BLACK, QPoint(3, 0));
    m_board[7][3] = new Queen(PieceColor::WHITE, QPoint(3, 7));
    
    // Place kings
    m_board[0][4] = new King(PieceColor::BLACK, QPoint(4, 0));
    m_board[7][4] = new King(PieceColor::WHITE, QPoint(4, 7));
    
    m_currentTurn = PieceColor::WHITE;
    m_enPassantTarget = QPoint(-1, -1);
    m_moveHistory.clear();
    m_isGameOver = false;
    m_gameStatus = "Game in progress";
}

void ChessBoard::reset() {
    initializeBoard();
}

ChessPiece* ChessBoard::getPieceAt(QPoint pos) const {
    return getPieceAt(pos.y(), pos.x());
}

ChessPiece* ChessBoard::getPieceAt(int row, int col) const {
    if (!isValidPosition(row, col)) return nullptr;
    return m_board[row][col];
}

bool ChessBoard::isValidPosition(QPoint pos) const {
    return isValidPosition(pos.y(), pos.x());
}

bool ChessBoard::isValidPosition(int row, int col) const {
    return row >= 0 && row < 8 && col >= 0 && col < 8;
}

bool ChessBoard::canMove(QPoint from, QPoint to) const {
    ChessPiece* piece = getPieceAt(from);
    if (piece == nullptr) return false;
    if (piece->getColor() != m_currentTurn) return false;
    
    return piece->isValidMove(to, const_cast<ChessBoard*>(this));
}

bool ChessBoard::movePiece(QPoint from, QPoint to, bool checkOnly) {
    ChessPiece* piece = getPieceAt(from);
    if (piece == nullptr) return false;
    
    // Check if it's the correct player's turn
    if (piece->getColor() != m_currentTurn) return false;
    
    // Check if the move is valid for the piece
    if (!piece->isValidMove(to, this)) return false;
    
    // Check if this move would leave the king in check
    if (wouldBeInCheck(from, to, m_currentTurn)) return false;
    
    if (checkOnly) return true;
    
    // Record move for history
    Move move;
    move.from = from;
    move.to = to;
    move.capturedPiece = getPieceAt(to);
    
    // Handle en passant capture
    if (piece->getType() == PieceType::PAWN && to == m_enPassantTarget) {
        int captureRow = (piece->getColor() == PieceColor::WHITE) ? to.y() + 1 : to.y() - 1;
        move.capturedPiece = m_board[captureRow][to.x()];
        move.wasEnPassant = true;
        delete m_board[captureRow][to.x()];
        m_board[captureRow][to.x()] = nullptr;
    } else if (move.capturedPiece != nullptr) {
        delete move.capturedPiece;
    }
    
    // Clear en passant target
    m_enPassantTarget = QPoint(-1, -1);
    
    // Set en passant target if pawn moved two squares
    if (piece->getType() == PieceType::PAWN) {
        int dy = to.y() - from.y();
        if (abs(dy) == 2) {
            m_enPassantTarget = QPoint(from.x(), from.y() + dy / 2);
        }
        
        // Handle pawn promotion
        if ((piece->getColor() == PieceColor::WHITE && to.y() == 0) ||
            (piece->getColor() == PieceColor::BLACK && to.y() == 7)) {
            move.wasPromotion = true;
            move.promotedTo = PieceType::QUEEN;
            delete piece;
            piece = new Queen(m_currentTurn, to);
            m_board[from.y()][from.x()] = nullptr;
            m_board[to.y()][to.x()] = piece;
            piece->setMoved(true);
            m_moveHistory.append(move);
            switchTurn();
            
            // Check for checkmate/stalemate
            PieceColor opponentColor = (m_currentTurn == PieceColor::WHITE) ? 
                PieceColor::BLACK : PieceColor::WHITE;
            if (isCheckmate(opponentColor)) {
                m_isGameOver = true;
                m_gameStatus = (m_currentTurn == PieceColor::WHITE) ? 
                    "Black wins by checkmate!" : "White wins by checkmate!";
            } else if (isStalemate(opponentColor)) {
                m_isGameOver = true;
                m_gameStatus = "Stalemate - Draw!";
            } else if (isKingInCheck(opponentColor)) {
                m_gameStatus = (opponentColor == PieceColor::WHITE) ? 
                    "White is in check!" : "Black is in check!";
            } else {
                m_gameStatus = "Game in progress";
            }
            
            return true;
        }
    }
    
    // Handle castling
    if (piece->getType() == PieceType::KING && abs(to.x() - from.x()) == 2) {
        move.wasCastling = true;
        bool kingSide = to.x() > from.x();
        performCastling(m_currentTurn, kingSide);
        m_moveHistory.append(move);
        switchTurn();
        
        // Check for checkmate/stalemate
        PieceColor opponentColor = (m_currentTurn == PieceColor::WHITE) ? 
            PieceColor::BLACK : PieceColor::WHITE;
        if (isCheckmate(opponentColor)) {
            m_isGameOver = true;
            m_gameStatus = (m_currentTurn == PieceColor::WHITE) ? 
                "Black wins by checkmate!" : "White wins by checkmate!";
        } else if (isStalemate(opponentColor)) {
            m_isGameOver = true;
            m_gameStatus = "Stalemate - Draw!";
        } else if (isKingInCheck(opponentColor)) {
            m_gameStatus = (opponentColor == PieceColor::WHITE) ? 
                "White is in check!" : "Black is in check!";
        } else {
            m_gameStatus = "Game in progress";
        }
        
        return true;
    }
    
    // Execute the move
    m_board[to.y()][to.x()] = piece;
    m_board[from.y()][from.x()] = nullptr;
    piece->setPosition(to);
    piece->setMoved(true);
    
    m_moveHistory.append(move);
    switchTurn();
    
    // Check for checkmate/stalemate
    PieceColor opponentColor = (m_currentTurn == PieceColor::WHITE) ? 
        PieceColor::BLACK : PieceColor::WHITE;
    if (isCheckmate(opponentColor)) {
        m_isGameOver = true;
        m_gameStatus = (m_currentTurn == PieceColor::WHITE) ? 
            "Black wins by checkmate!" : "White wins by checkmate!";
    } else if (isStalemate(opponentColor)) {
        m_isGameOver = true;
        m_gameStatus = "Stalemate - Draw!";
    } else if (isKingInCheck(opponentColor)) {
        m_gameStatus = (opponentColor == PieceColor::WHITE) ? 
            "White is in check!" : "Black is in check!";
    } else {
        m_gameStatus = "Game in progress";
    }
    
    return true;
}

void ChessBoard::switchTurn() {
    m_currentTurn = (m_currentTurn == PieceColor::WHITE) ? 
        PieceColor::BLACK : PieceColor::WHITE;
}

QPoint ChessBoard::findKing(PieceColor color) const {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            ChessPiece* piece = m_board[row][col];
            if (piece != nullptr && 
                piece->getType() == PieceType::KING && 
                piece->getColor() == color) {
                return QPoint(col, row);
            }
        }
    }
    return QPoint(-1, -1);
}

bool ChessBoard::isKingInCheck(PieceColor color) const {
    QPoint kingPos = findKing(color);
    if (kingPos.x() == -1) return false;
    
    PieceColor opponentColor = (color == PieceColor::WHITE) ? 
        PieceColor::BLACK : PieceColor::WHITE;
    
    // Check if any opponent piece can attack the king
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            ChessPiece* piece = m_board[row][col];
            if (piece != nullptr && piece->getColor() == opponentColor) {
                if (piece->isValidMove(kingPos, const_cast<ChessBoard*>(this))) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool ChessBoard::wouldBeInCheck(QPoint from, QPoint to, PieceColor color) {
    // Temporarily make the move
    ChessPiece* movingPiece = m_board[from.y()][from.x()];
    ChessPiece* capturedPiece = m_board[to.y()][to.x()];
    QPoint originalPos = movingPiece->getPosition();
    
    m_board[to.y()][to.x()] = movingPiece;
    m_board[from.y()][from.x()] = nullptr;
    movingPiece->setPosition(to);
    
    // Check if king is in check
    bool inCheck = isKingInCheck(color);
    
    // Undo the move
    m_board[from.y()][from.x()] = movingPiece;
    m_board[to.y()][to.x()] = capturedPiece;
    movingPiece->setPosition(originalPos);
    
    return inCheck;
}

bool ChessBoard::hasAnyValidMoves(PieceColor color) {
    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            ChessPiece* piece = m_board[fromRow][fromCol];
            if (piece == nullptr || piece->getColor() != color) continue;
            
            for (int toRow = 0; toRow < 8; ++toRow) {
                for (int toCol = 0; toCol < 8; ++toCol) {
                    QPoint from(fromCol, fromRow);
                    QPoint to(toCol, toRow);
                    
                    if (piece->isValidMove(to, this) && 
                        !wouldBeInCheck(from, to, color)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool ChessBoard::isCheckmate(PieceColor color) {
    if (!isKingInCheck(color)) return false;
    return !hasAnyValidMoves(color);
}

bool ChessBoard::isStalemate(PieceColor color) {
    if (isKingInCheck(color)) return false;
    return !hasAnyValidMoves(color);
}

bool ChessBoard::canCastle(PieceColor color, bool kingSide) const {
    int row = (color == PieceColor::WHITE) ? 7 : 0;
    int kingCol = 4;
    int rookCol = kingSide ? 7 : 0;
    
    ChessPiece* king = m_board[row][kingCol];
    ChessPiece* rook = m_board[row][rookCol];
    
    // Check if king and rook exist and haven't moved
    if (king == nullptr || king->getType() != PieceType::KING || king->hasMoved()) 
        return false;
    if (rook == nullptr || rook->getType() != PieceType::ROOK || rook->hasMoved()) 
        return false;
    
    // Check if king is in check
    if (isKingInCheck(color)) return false;
    
    // Check if squares between king and rook are empty
    int start = kingSide ? kingCol + 1 : rookCol + 1;
    int end = kingSide ? rookCol : kingCol;
    for (int col = start; col < end; ++col) {
        if (m_board[row][col] != nullptr) return false;
    }
    
    // Check if king passes through or lands on attacked square
    int direction = kingSide ? 1 : -1;
    for (int i = 0; i <= 2; ++i) {
        QPoint pos(kingCol + i * direction, row);
        if (wouldBeInCheck(QPoint(kingCol, row), pos, color)) {
            return false;
        }
    }
    
    return true;
}

void ChessBoard::performCastling(PieceColor color, bool kingSide) {
    int row = (color == PieceColor::WHITE) ? 7 : 0;
    int kingCol = 4;
    int rookCol = kingSide ? 7 : 0;
    int newKingCol = kingSide ? 6 : 2;
    int newRookCol = kingSide ? 5 : 3;
    
    ChessPiece* king = m_board[row][kingCol];
    ChessPiece* rook = m_board[row][rookCol];
    
    // Move king
    m_board[row][newKingCol] = king;
    m_board[row][kingCol] = nullptr;
    king->setPosition(QPoint(newKingCol, row));
    king->setMoved(true);
    
    // Move rook
    m_board[row][newRookCol] = rook;
    m_board[row][rookCol] = nullptr;
    rook->setPosition(QPoint(newRookCol, row));
    rook->setMoved(true);
}
