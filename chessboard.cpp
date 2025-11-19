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
    // First, clean up captured pieces in move history
    for (Move& move : m_moveHistory) {
        if (move.capturedPiece != nullptr) {
            delete move.capturedPiece;
            move.capturedPiece = nullptr;
        }
    }
    m_moveHistory.clear();
    
    // Then clean up the board
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
    m_gameStatus = tr("Game in progress");
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

    // Check if the piece can move according to its rules
    if (!piece->isValidMove(to, const_cast<ChessBoard*>(this))) return false;

    // Additional validation for castling moves
    if (piece->getType() == PieceType::KING && abs(to.x() - from.x()) == 2) {
        bool kingSide = to.x() > from.x();
        if (!canCastle(m_currentTurn, kingSide)) return false;
    }

    // Check if this move would leave the king in check
    if (wouldBeInCheck(from, to, m_currentTurn)) return false;

    return true;
}

bool ChessBoard::movePiece(QPoint from, QPoint to, bool checkOnly) {
    ChessPiece* piece = getPieceAt(from);
    if (piece == nullptr) return false;

    // Check if it's the correct player's turn
    if (piece->getColor() != m_currentTurn) return false;

    // Check if the move is valid for the piece
    if (!piece->isValidMove(to, this)) return false;

    // Additional validation for castling moves
    if (piece->getType() == PieceType::KING && abs(to.x() - from.x()) == 2) {
        bool kingSide = to.x() > from.x();
        if (!canCastle(m_currentTurn, kingSide)) return false;
    }

    // Check if this move would leave the king in check
    if (wouldBeInCheck(from, to, m_currentTurn)) return false;

    if (checkOnly) return true;

    // Record move for history
    Move move;
    move.from = from;
    move.to = to;
    move.capturedPiece = getPieceAt(to);
    move.movedPieceHadMoved = piece->hasMoved();
    move.previousEnPassantTarget = m_enPassantTarget;
    move.movedPieceType = piece->getType();
    move.movedPieceColor = piece->getColor();

    // Handle en passant capture
    if (piece->getType() == PieceType::PAWN && to == m_enPassantTarget) {
        int captureRow = (piece->getColor() == PieceColor::WHITE) ? to.y() + 1 : to.y() - 1;
        move.capturedPiece = m_board[captureRow][to.x()];
        move.wasEnPassant = true;
        // Don't delete the captured piece - keep it for undo
        m_board[captureRow][to.x()] = nullptr;
    } else if (move.capturedPiece != nullptr) {
        // Don't delete the captured piece - keep it for undo
        // Just remove it from the board
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

            // Check for checkmate/stalemate/insufficient material for the NEW current player
            if (isCheckmate(m_currentTurn)) {
                m_isGameOver = true;
                PieceColor winner = (m_currentTurn == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
                m_gameStatus = (winner == PieceColor::WHITE) ?
                                   tr("White wins by checkmate!") : tr("Black wins by checkmate!");
            } else if (isStalemate(m_currentTurn)) {
                m_isGameOver = true;
                m_gameStatus = tr("Stalemate - Draw!");
            } else if (isInsufficientMaterial()) {
                m_isGameOver = true;
                m_gameStatus = tr("Draw - Insufficient Material!");
            } else if (isKingInCheck(m_currentTurn)) {
                m_gameStatus = (m_currentTurn == PieceColor::WHITE) ?
                                   tr("White is in check!") : tr("Black is in check!");
            } else {
                m_gameStatus = tr("Game in progress");
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

        // Check for checkmate/stalemate/insufficient material for the NEW current player
        if (isCheckmate(m_currentTurn)) {
            m_isGameOver = true;
            PieceColor winner = (m_currentTurn == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
            m_gameStatus = (winner == PieceColor::WHITE) ?
                               tr("White wins by checkmate!") : tr("Black wins by checkmate!");
        } else if (isStalemate(m_currentTurn)) {
            m_isGameOver = true;
            m_gameStatus = tr("Stalemate - Draw!");
        } else if (isInsufficientMaterial()) {
            m_isGameOver = true;
            m_gameStatus = tr("Draw - Insufficient Material!");
        } else if (isKingInCheck(m_currentTurn)) {
            m_gameStatus = (m_currentTurn == PieceColor::WHITE) ?
                               tr("White is in check!") : tr("Black is in check!");
        } else {
            m_gameStatus = tr("Game in progress");
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

    // Check for checkmate/stalemate/insufficient material for the NEW current player (who is about to move)
    if (isCheckmate(m_currentTurn)) {
        m_isGameOver = true;
        // The player who just moved (opposite of current turn) wins
        PieceColor winner = (m_currentTurn == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
        m_gameStatus = (winner == PieceColor::WHITE) ?
                           tr("White wins by checkmate!") : tr("Black wins by checkmate!");
    } else if (isStalemate(m_currentTurn)) {
        m_isGameOver = true;
        m_gameStatus = tr("Stalemate - Draw!");
    } else if (isInsufficientMaterial()) {
        m_isGameOver = true;
        m_gameStatus = tr("Draw - Insufficient Material!");
    } else if (isKingInCheck(m_currentTurn)) {
        m_gameStatus = (m_currentTurn == PieceColor::WHITE) ?
                           tr("White is in check!") : tr("Black is in check!");
    } else {
        m_gameStatus = tr("Game in progress");
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

    // Use the attack-check helper on the current board
    return isSquareAttacked(m_board, kingPos, opponentColor);
}

// New helper: find the king position on an arbitrary board pointer array
QPoint ChessBoard::findKingOnBoard(ChessPiece* const board[8][8], PieceColor color) const {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            ChessPiece* piece = board[row][col];
            if (piece != nullptr &&
                piece->getType() == PieceType::KING &&
                piece->getColor() == color) {
                return QPoint(col, row);
            }
        }
    }
    return QPoint(-1, -1);
}

// New helper: checks whether 'square' is attacked by any piece of attackerColor on the provided board
bool ChessBoard::isSquareAttacked(ChessPiece* const board[8][8], QPoint square, PieceColor attackerColor) const {
    int sx = square.x();
    int sy = square.y();

    auto inBounds = [](int r, int c) { return r >= 0 && r < 8 && c >= 0 && c < 8; };

    // 1) Pawn attacks
    if (attackerColor == PieceColor::WHITE) {
        // white pawns attack from one row below (higher y) to the square
        int py = sy + 1;
        if (inBounds(py, sx - 1)) {
            ChessPiece* p = board[py][sx - 1];
            if (p && p->getType() == PieceType::PAWN && p->getColor() == attackerColor) return true;
        }
        if (inBounds(py, sx + 1)) {
            ChessPiece* p = board[py][sx + 1];
            if (p && p->getType() == PieceType::PAWN && p->getColor() == attackerColor) return true;
        }
    } else { // BLACK pawns
        int py = sy - 1;
        if (inBounds(py, sx - 1)) {
            ChessPiece* p = board[py][sx - 1];
            if (p && p->getType() == PieceType::PAWN && p->getColor() == attackerColor) return true;
        }
        if (inBounds(py, sx + 1)) {
            ChessPiece* p = board[py][sx + 1];
            if (p && p->getType() == PieceType::PAWN && p->getColor() == attackerColor) return true;
        }
    }

    // 2) Knight attacks
    const int knOff[8][2] = {{1,2},{2,1},{2,-1},{1,-2},{-1,-2},{-2,-1},{-2,1},{-1,2}};
    for (auto &o : knOff) {
        int nx = sx + o[0], ny = sy + o[1];
        if (!inBounds(ny, nx)) continue;
        ChessPiece* p = board[ny][nx];
        if (p && p->getType() == PieceType::KNIGHT && p->getColor() == attackerColor) return true;
    }

    // 3) Sliding pieces: rook/queen (orthogonal)
    const int orth[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
    for (auto &d : orth) {
        int dx = d[0], dy = d[1];
        int nx = sx + dx, ny = sy + dy;
        while (inBounds(ny, nx)) {
            ChessPiece* p = board[ny][nx];
            if (p) {
                if (p->getColor() == attackerColor &&
                    (p->getType() == PieceType::ROOK || p->getType() == PieceType::QUEEN)) {
                    return true;
                } else {
                    break; // blocked by other piece
                }
            }
            nx += dx; ny += dy;
        }
    }

    // 4) Sliding pieces: bishop/queen (diagonals)
    const int diag[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
    for (auto &d : diag) {
        int dx = d[0], dy = d[1];
        int nx = sx + dx, ny = sy + dy;
        while (inBounds(ny, nx)) {
            ChessPiece* p = board[ny][nx];
            if (p) {
                if (p->getColor() == attackerColor &&
                    (p->getType() == PieceType::BISHOP || p->getType() == PieceType::QUEEN)) {
                    return true;
                } else {
                    break; // blocked
                }
            }
            nx += dx; ny += dy;
        }
    }

    // 5) King adjacency (opponent king next to square)
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            int nx = sx + dx, ny = sy + dy;
            if (!inBounds(ny, nx)) continue;
            ChessPiece* p = board[ny][nx];
            if (p && p->getType() == PieceType::KING && p->getColor() == attackerColor) return true;
        }
    }

    return false;
}

// wouldBeInCheck now simulates the move on a tmp board and uses the attack checker
bool ChessBoard::wouldBeInCheck(QPoint from, QPoint to, PieceColor color) const {
    // Create a shallow copy of the board pointers (do not duplicate pieces)
    ChessPiece* tmpBoard[8][8];
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            tmpBoard[r][c] = m_board[r][c];
        }
    }

    ChessPiece* movingPiece = tmpBoard[from.y()][from.x()];
    ChessPiece* capturedPiece = tmpBoard[to.y()][to.x()];
    if (!movingPiece) return false; // defensive

    // simulate move on tmpBoard
    tmpBoard[to.y()][to.x()] = movingPiece;
    tmpBoard[from.y()][from.x()] = nullptr;

    // If moving a king, its position will be at 'to'
    QPoint kingPos;
    if (movingPiece->getType() == PieceType::KING) {
        kingPos = to;
    } else {
        kingPos = findKingOnBoard(tmpBoard, color);
    }

    if (kingPos.x() == -1) {
        // no king found -- treat as not in check (or handle as error)
        return false;
    }

    PieceColor opponentColor = (color == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
    bool inCheck = isSquareAttacked(tmpBoard, kingPos, opponentColor);

    // tmpBoard is local; no need to undo real m_board
    return inCheck;
}

bool ChessBoard::hasAnyValidMoves(PieceColor color) {
    // Check if the player has any valid moves to escape checkmate
    // This covers all three escape conditions:
    // 1. King can move to a safe square
    // 2. A piece can capture the attacking piece
    // 3. A piece can block the attack

    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            ChessPiece* piece = m_board[fromRow][fromCol];
            if (piece == nullptr || piece->getColor() != color) continue;

            for (int toRow = 0; toRow < 8; ++toRow) {
                for (int toCol = 0; toCol < 8; ++toCol) {
                    QPoint from(fromCol, fromRow);
                    QPoint to(toCol, toRow);

                    // Check if the piece can move according to its rules
                    if (!piece->isValidMove(to, this)) continue;

                    // Additional validation for castling moves
                    // Castling has special requirements beyond basic move validation
                    if (piece->getType() == PieceType::KING && abs(to.x() - from.x()) == 2) {
                        bool kingSide = to.x() > from.x();
                        if (!canCastle(color, kingSide)) continue;
                    }

                    // Check if this move would leave the king in check
                    if (!wouldBeInCheck(from, to, color)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool ChessBoard::isCheckmate(PieceColor color) {
    // Checkmate occurs when ALL three conditions are met:
    // 1. The king is under attack (in check)
    if (!isKingInCheck(color)) return false;

    // 2. The king cannot escape by moving to a safe square
    // 3. No piece can capture the attacking piece
    // 4. No piece can block the attack
    // (Conditions 2-4 are all checked by hasAnyValidMoves)
    return !hasAnyValidMoves(color);
}

bool ChessBoard::isStalemate(PieceColor color) {
    if (isKingInCheck(color)) return false;
    return !hasAnyValidMoves(color);
}

bool ChessBoard::isInsufficientMaterial() const {
    // Count pieces for each side
    int whiteKnights = 0, blackKnights = 0;
    int whiteBishops = 0, blackBishops = 0;
    int whiteOther = 0, blackOther = 0; // pawns, rooks, queens
    
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            ChessPiece* piece = m_board[row][col];
            if (piece == nullptr || piece->getType() == PieceType::KING) {
                continue;
            }
            
            if (piece->getColor() == PieceColor::WHITE) {
                if (piece->getType() == PieceType::KNIGHT) {
                    whiteKnights++;
                } else if (piece->getType() == PieceType::BISHOP) {
                    whiteBishops++;
                } else {
                    whiteOther++;
                }
            } else {
                if (piece->getType() == PieceType::KNIGHT) {
                    blackKnights++;
                } else if (piece->getType() == PieceType::BISHOP) {
                    blackBishops++;
                } else {
                    blackOther++;
                }
            }
        }
    }
    
    // If either side has pawns, rooks, or queens, there's sufficient material
    if (whiteOther > 0 || blackOther > 0) {
        return false;
    }
    
    // King vs King
    if (whiteKnights == 0 && blackKnights == 0 && whiteBishops == 0 && blackBishops == 0) {
        return true;
    }
    
    // King + Bishop vs King
    if (whiteKnights == 0 && blackKnights == 0) {
        if ((whiteBishops == 1 && blackBishops == 0) || 
            (whiteBishops == 0 && blackBishops == 1)) {
            return true;
        }
    }
    
    // King + Knight vs King
    if (whiteBishops == 0 && blackBishops == 0) {
        if ((whiteKnights == 1 && blackKnights == 0) || 
            (whiteKnights == 0 && blackKnights == 1)) {
            return true;
        }
        // King + Knight vs King + Knight (two knights total)
        if (whiteKnights == 1 && blackKnights == 1) {
            return true;
        }
    }
    
    // King + two Knights vs King (technically can checkmate with help, but treating as draw per requirements)
    if (whiteBishops == 0 && blackBishops == 0) {
        if ((whiteKnights == 2 && blackKnights == 0) || 
            (whiteKnights == 0 && blackKnights == 2)) {
            return true;
        }
    }
    
    return false;
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

bool ChessBoard::undo() {
    // Cannot undo if there are no moves
    if (m_moveHistory.isEmpty()) {
        return false;
    }

    // Get the last move
    Move lastMove = m_moveHistory.takeLast();

    // Reset game over state if we're undoing
    m_isGameOver = false;

    // Switch turn back (since we switched it after making the move)
    switchTurn();

    // Restore en passant target
    m_enPassantTarget = lastMove.previousEnPassantTarget;

    // Handle castling undo
    if (lastMove.wasCastling) {
        // Determine which side and row
        int row = lastMove.to.y();
        bool kingSide = lastMove.to.x() > lastMove.from.x();
        int kingCol = 4;
        int rookCol = kingSide ? 7 : 0;
        int newKingCol = kingSide ? 6 : 2;
        int newRookCol = kingSide ? 5 : 3;

        ChessPiece* king = m_board[row][newKingCol];
        ChessPiece* rook = m_board[row][newRookCol];

        // Move king back
        m_board[row][kingCol] = king;
        m_board[row][newKingCol] = nullptr;
        king->setPosition(QPoint(kingCol, row));
        king->setMoved(lastMove.movedPieceHadMoved);

        // Move rook back
        m_board[row][rookCol] = rook;
        m_board[row][newRookCol] = nullptr;
        rook->setPosition(QPoint(rookCol, row));
        rook->setMoved(false);  // Rook wasn't moved before castling

        m_gameStatus = tr("Game in progress");
        return true;
    }

    // Handle promotion undo
    if (lastMove.wasPromotion) {
        // Delete the promoted piece (Queen)
        ChessPiece* promotedPiece = m_board[lastMove.to.y()][lastMove.to.x()];
        delete promotedPiece;

        // Create a new pawn at the original position
        ChessPiece* pawn = new Pawn(lastMove.movedPieceColor, lastMove.from);
        pawn->setMoved(lastMove.movedPieceHadMoved);
        m_board[lastMove.from.y()][lastMove.from.x()] = pawn;
        m_board[lastMove.to.y()][lastMove.to.x()] = nullptr;

        // Restore captured piece if any
        if (lastMove.capturedPiece != nullptr) {
            m_board[lastMove.to.y()][lastMove.to.x()] = lastMove.capturedPiece;
            lastMove.capturedPiece->setPosition(lastMove.to);
        }

        m_gameStatus = tr("Game in progress");
        return true;
    }

    // Handle en passant undo
    if (lastMove.wasEnPassant) {
        // Move the piece back
        ChessPiece* piece = m_board[lastMove.to.y()][lastMove.to.x()];
        m_board[lastMove.from.y()][lastMove.from.x()] = piece;
        m_board[lastMove.to.y()][lastMove.to.x()] = nullptr;
        piece->setPosition(lastMove.from);
        piece->setMoved(lastMove.movedPieceHadMoved);

        // Restore the captured pawn
        if (lastMove.capturedPiece != nullptr) {
            int captureRow = (piece->getColor() == PieceColor::WHITE) ? lastMove.to.y() + 1 : lastMove.to.y() - 1;
            m_board[captureRow][lastMove.to.x()] = lastMove.capturedPiece;
            lastMove.capturedPiece->setPosition(QPoint(lastMove.to.x(), captureRow));
        }

        m_gameStatus = tr("Game in progress");
        return true;
    }

    // Handle normal move undo
    ChessPiece* piece = m_board[lastMove.to.y()][lastMove.to.x()];
    
    // Move the piece back to its original position
    m_board[lastMove.from.y()][lastMove.from.x()] = piece;
    m_board[lastMove.to.y()][lastMove.to.x()] = nullptr;
    piece->setPosition(lastMove.from);
    piece->setMoved(lastMove.movedPieceHadMoved);

    // Restore captured piece if any
    if (lastMove.capturedPiece != nullptr) {
        m_board[lastMove.to.y()][lastMove.to.x()] = lastMove.capturedPiece;
        lastMove.capturedPiece->setPosition(lastMove.to);
    }

    // Update game status
    if (isKingInCheck(m_currentTurn)) {
        m_gameStatus = (m_currentTurn == PieceColor::WHITE) ?
                           tr("White is in check!") : tr("Black is in check!");
    } else {
        m_gameStatus = tr("Game in progress");
    }

    return true;
}

void ChessBoard::getBoardStateAtMove(int moveIndex, ChessPiece* outputBoard[8][8], PieceColor& turn) const {
    // Initialize output board to nullptr
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            outputBoard[i][j] = nullptr;
        }
    }
    
    // Create initial board state
    // Place pawns
    for (int col = 0; col < 8; ++col) {
        outputBoard[1][col] = new Pawn(PieceColor::BLACK, QPoint(col, 1));
        outputBoard[6][col] = new Pawn(PieceColor::WHITE, QPoint(col, 6));
    }

    // Place rooks
    outputBoard[0][0] = new Rook(PieceColor::BLACK, QPoint(0, 0));
    outputBoard[0][7] = new Rook(PieceColor::BLACK, QPoint(7, 0));
    outputBoard[7][0] = new Rook(PieceColor::WHITE, QPoint(0, 7));
    outputBoard[7][7] = new Rook(PieceColor::WHITE, QPoint(7, 7));

    // Place knights
    outputBoard[0][1] = new Knight(PieceColor::BLACK, QPoint(1, 0));
    outputBoard[0][6] = new Knight(PieceColor::BLACK, QPoint(6, 0));
    outputBoard[7][1] = new Knight(PieceColor::WHITE, QPoint(1, 7));
    outputBoard[7][6] = new Knight(PieceColor::WHITE, QPoint(6, 7));

    // Place bishops
    outputBoard[0][2] = new Bishop(PieceColor::BLACK, QPoint(2, 0));
    outputBoard[0][5] = new Bishop(PieceColor::BLACK, QPoint(5, 0));
    outputBoard[7][2] = new Bishop(PieceColor::WHITE, QPoint(2, 7));
    outputBoard[7][5] = new Bishop(PieceColor::WHITE, QPoint(5, 7));

    // Place queens
    outputBoard[0][3] = new Queen(PieceColor::BLACK, QPoint(3, 0));
    outputBoard[7][3] = new Queen(PieceColor::WHITE, QPoint(3, 7));

    // Place kings
    outputBoard[0][4] = new King(PieceColor::BLACK, QPoint(4, 0));
    outputBoard[7][4] = new King(PieceColor::WHITE, QPoint(4, 7));
    
    turn = PieceColor::WHITE;
    
    // If moveIndex is -1 or 0, return initial state
    if (moveIndex < 0) {
        return;
    }
    
    // Replay moves up to and including moveIndex
    for (int i = 0; i <= moveIndex && i < m_moveHistory.size(); ++i) {
        const Move& move = m_moveHistory[i];
        
        if (move.wasCastling) {
            // Handle castling
            int row = move.to.y();
            bool kingSide = move.to.x() > move.from.x();
            int kingCol = 4;
            int rookCol = kingSide ? 7 : 0;
            int newKingCol = kingSide ? 6 : 2;
            int newRookCol = kingSide ? 5 : 3;
            
            ChessPiece* king = outputBoard[row][kingCol];
            ChessPiece* rook = outputBoard[row][rookCol];
            
            outputBoard[row][newKingCol] = king;
            outputBoard[row][kingCol] = nullptr;
            king->setPosition(QPoint(newKingCol, row));
            king->setMoved(true);
            
            outputBoard[row][newRookCol] = rook;
            outputBoard[row][rookCol] = nullptr;
            rook->setPosition(QPoint(newRookCol, row));
            rook->setMoved(true);
        } else if (move.wasEnPassant) {
            // Handle en passant
            ChessPiece* piece = outputBoard[move.from.y()][move.from.x()];
            outputBoard[move.to.y()][move.to.x()] = piece;
            outputBoard[move.from.y()][move.from.x()] = nullptr;
            piece->setPosition(move.to);
            piece->setMoved(true);
            
            // Remove captured pawn
            int captureRow = (piece->getColor() == PieceColor::WHITE) ? move.to.y() + 1 : move.to.y() - 1;
            if (outputBoard[captureRow][move.to.x()] != nullptr) {
                delete outputBoard[captureRow][move.to.x()];
                outputBoard[captureRow][move.to.x()] = nullptr;
            }
        } else if (move.wasPromotion) {
            // Handle promotion
            ChessPiece* pawn = outputBoard[move.from.y()][move.from.x()];
            if (outputBoard[move.to.y()][move.to.x()] != nullptr) {
                delete outputBoard[move.to.y()][move.to.x()];
            }
            outputBoard[move.from.y()][move.from.x()] = nullptr;
            
            // Create promoted piece (Queen)
            ChessPiece* promotedPiece = new Queen(pawn->getColor(), move.to);
            promotedPiece->setMoved(true);
            outputBoard[move.to.y()][move.to.x()] = promotedPiece;
            
            delete pawn;
        } else {
            // Normal move
            ChessPiece* piece = outputBoard[move.from.y()][move.from.x()];
            if (outputBoard[move.to.y()][move.to.x()] != nullptr) {
                delete outputBoard[move.to.y()][move.to.x()];
            }
            outputBoard[move.to.y()][move.to.x()] = piece;
            outputBoard[move.from.y()][move.from.x()] = nullptr;
            if (piece != nullptr) {
                piece->setPosition(move.to);
                piece->setMoved(true);
            }
        }
        
        // Switch turn
        turn = (turn == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
    }
}
