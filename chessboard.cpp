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

    // 放置兵
    for (int col = 0; col < 8; ++col) {
        m_board[1][col] = new Pawn(PieceColor::BLACK, QPoint(col, 1));
        m_board[6][col] = new Pawn(PieceColor::WHITE, QPoint(col, 6));
    }

    // 放置車
    m_board[0][0] = new Rook(PieceColor::BLACK, QPoint(0, 0));
    m_board[0][7] = new Rook(PieceColor::BLACK, QPoint(7, 0));
    m_board[7][0] = new Rook(PieceColor::WHITE, QPoint(0, 7));
    m_board[7][7] = new Rook(PieceColor::WHITE, QPoint(7, 7));

    // 放置馬
    m_board[0][1] = new Knight(PieceColor::BLACK, QPoint(1, 0));
    m_board[0][6] = new Knight(PieceColor::BLACK, QPoint(6, 0));
    m_board[7][1] = new Knight(PieceColor::WHITE, QPoint(1, 7));
    m_board[7][6] = new Knight(PieceColor::WHITE, QPoint(6, 7));

    // 放置象
    m_board[0][2] = new Bishop(PieceColor::BLACK, QPoint(2, 0));
    m_board[0][5] = new Bishop(PieceColor::BLACK, QPoint(5, 0));
    m_board[7][2] = new Bishop(PieceColor::WHITE, QPoint(2, 7));
    m_board[7][5] = new Bishop(PieceColor::WHITE, QPoint(5, 7));

    // 放置后
    m_board[0][3] = new Queen(PieceColor::BLACK, QPoint(3, 0));
    m_board[7][3] = new Queen(PieceColor::WHITE, QPoint(3, 7));

    // 放置王
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

    // 檢查棋子是否可以根據其規則移動
    if (!piece->isValidMove(to, const_cast<ChessBoard*>(this))) return false;
    
    // 王車易位移動的額外驗證
    if (piece->getType() == PieceType::KING && abs(to.x() - from.x()) == 2) {
        bool kingSide = to.x() > from.x();
        if (!canCastle(m_currentTurn, kingSide)) return false;
    }
    
    // 檢查此移動是否會使國王處於被將軍狀態
    if (wouldBeInCheck(from, to, m_currentTurn)) return false;
    
    return true;
}

bool ChessBoard::movePiece(QPoint from, QPoint to, bool checkOnly) {
    ChessPiece* piece = getPieceAt(from);
    if (piece == nullptr) return false;

    // 檢查是否為正確玩家的回合
    if (piece->getColor() != m_currentTurn) return false;

    // 檢查移動對於棋子是否有效
    if (!piece->isValidMove(to, this)) return false;

    // 王車易位移動的額外驗證
    if (piece->getType() == PieceType::KING && abs(to.x() - from.x()) == 2) {
        bool kingSide = to.x() > from.x();
        if (!canCastle(m_currentTurn, kingSide)) return false;
    }

    // 檢查此移動是否會使國王處於被將軍狀態
    if (wouldBeInCheck(from, to, m_currentTurn)) return false;

    if (checkOnly) return true;

    // 為歷史記錄移動
    Move move;
    move.from = from;
    move.to = to;
    move.capturedPiece = getPieceAt(to);

    // 處理吃過路兵
    if (piece->getType() == PieceType::PAWN && to == m_enPassantTarget) {
        int captureRow = (piece->getColor() == PieceColor::WHITE) ? to.y() + 1 : to.y() - 1;
        move.capturedPiece = m_board[captureRow][to.x()];
        move.wasEnPassant = true;
        delete m_board[captureRow][to.x()];
        m_board[captureRow][to.x()] = nullptr;
    } else if (move.capturedPiece != nullptr) {
        delete move.capturedPiece;
    }

    // 清除吃過路兵目標
    m_enPassantTarget = QPoint(-1, -1);

    // 如果兵移動兩格則設定吃過路兵目標
    if (piece->getType() == PieceType::PAWN) {
        int dy = to.y() - from.y();
        if (abs(dy) == 2) {
            m_enPassantTarget = QPoint(from.x(), from.y() + dy / 2);
        }

        // 處理兵升變
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

            // 檢查新的當前玩家是否被將死/僵局
            if (isCheckmate(m_currentTurn)) {
                m_isGameOver = true;
                PieceColor winner = (m_currentTurn == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
                m_gameStatus = (winner == PieceColor::WHITE) ?
                                   "White wins by checkmate!" : "Black wins by checkmate!";
            } else if (isStalemate(m_currentTurn)) {
                m_isGameOver = true;
                m_gameStatus = "Stalemate - Draw!";
            } else if (isKingInCheck(m_currentTurn)) {
                m_gameStatus = (m_currentTurn == PieceColor::WHITE) ?
                                   "White is in check!" : "Black is in check!";
            } else {
                m_gameStatus = "Game in progress";
            }

            return true;
        }
    }

    // 處理王車易位
    if (piece->getType() == PieceType::KING && abs(to.x() - from.x()) == 2) {
        move.wasCastling = true;
        bool kingSide = to.x() > from.x();
        performCastling(m_currentTurn, kingSide);
        m_moveHistory.append(move);
        switchTurn();

        // 檢查新的當前玩家是否被將死/僵局
        if (isCheckmate(m_currentTurn)) {
            m_isGameOver = true;
            PieceColor winner = (m_currentTurn == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
            m_gameStatus = (winner == PieceColor::WHITE) ?
                               "White wins by checkmate!" : "Black wins by checkmate!";
        } else if (isStalemate(m_currentTurn)) {
            m_isGameOver = true;
            m_gameStatus = "Stalemate - Draw!";
        } else if (isKingInCheck(m_currentTurn)) {
            m_gameStatus = (m_currentTurn == PieceColor::WHITE) ?
                               "White is in check!" : "Black is in check!";
        } else {
            m_gameStatus = "Game in progress";
        }

        return true;
    }

    // 執行移動
    m_board[to.y()][to.x()] = piece;
    m_board[from.y()][from.x()] = nullptr;
    piece->setPosition(to);
    piece->setMoved(true);

    m_moveHistory.append(move);
    switchTurn();

    // 檢查新的當前玩家（即將移動的玩家）是否被將死/僵局
    if (isCheckmate(m_currentTurn)) {
        m_isGameOver = true;
        // 剛剛移動的玩家（當前回合的相反方）獲勝
        PieceColor winner = (m_currentTurn == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
        m_gameStatus = (winner == PieceColor::WHITE) ?
                           "White wins by checkmate!" : "Black wins by checkmate!";
    } else if (isStalemate(m_currentTurn)) {
        m_isGameOver = true;
        m_gameStatus = "Stalemate - Draw!";
    } else if (isKingInCheck(m_currentTurn)) {
        m_gameStatus = (m_currentTurn == PieceColor::WHITE) ?
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

    // 在當前棋盤上使用攻擊檢查輔助函式
    return isSquareAttacked(m_board, kingPos, opponentColor);
}

// 新的輔助函式：在任意棋盤指標陣列上尋找國王位置
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

// 新的輔助函式：檢查提供的棋盤上 'square' 是否被 attackerColor 的任何棋子攻擊
bool ChessBoard::isSquareAttacked(ChessPiece* const board[8][8], QPoint square, PieceColor attackerColor) const {
    int sx = square.x();
    int sy = square.y();

    auto inBounds = [](int r, int c) { return r >= 0 && r < 8 && c >= 0 && c < 8; };

    // 1) 兵的攻擊
    if (attackerColor == PieceColor::WHITE) {
        // 白兵從下方一列（較高 y）攻擊格子
        int py = sy + 1;
        if (inBounds(py, sx - 1)) {
            ChessPiece* p = board[py][sx - 1];
            if (p && p->getType() == PieceType::PAWN && p->getColor() == attackerColor) return true;
        }
        if (inBounds(py, sx + 1)) {
            ChessPiece* p = board[py][sx + 1];
            if (p && p->getType() == PieceType::PAWN && p->getColor() == attackerColor) return true;
        }
    } else { // 黑兵
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

    // 2) 馬的攻擊
    const int knOff[8][2] = {{1,2},{2,1},{2,-1},{1,-2},{-1,-2},{-2,-1},{-2,1},{-1,2}};
    for (auto &o : knOff) {
        int nx = sx + o[0], ny = sy + o[1];
        if (!inBounds(ny, nx)) continue;
        ChessPiece* p = board[ny][nx];
        if (p && p->getType() == PieceType::KNIGHT && p->getColor() == attackerColor) return true;
    }

    // 3) 滑動棋子：車/后（正交方向）
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
                    break; // 被其他棋子阻擋
                }
            }
            nx += dx; ny += dy;
        }
    }

    // 4) 滑動棋子：象/后（對角線方向）
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
                    break; // 阻擋
                }
            }
            nx += dx; ny += dy;
        }
    }

    // 5) 國王鄰接（對手國王在格子旁邊）
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

// wouldBeInCheck 現在在臨時棋盤上模擬移動並使用攻擊檢查器
bool ChessBoard::wouldBeInCheck(QPoint from, QPoint to, PieceColor color) const {
    // 建立棋盤指標的淺複製（不複製棋子）
    ChessPiece* tmpBoard[8][8];
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            tmpBoard[r][c] = m_board[r][c];
        }
    }

    ChessPiece* movingPiece = tmpBoard[from.y()][from.x()];
    ChessPiece* capturedPiece = tmpBoard[to.y()][to.x()];
    if (!movingPiece) return false; // 防禦性檢查

    // 在 tmpBoard 上模擬移動
    tmpBoard[to.y()][to.x()] = movingPiece;
    tmpBoard[from.y()][from.x()] = nullptr;

    // 如果移動的是國王，其位置將在 'to'
    QPoint kingPos;
    if (movingPiece->getType() == PieceType::KING) {
        kingPos = to;
    } else {
        kingPos = findKingOnBoard(tmpBoard, color);
    }

    if (kingPos.x() == -1) {
        // 找不到國王 -- 視為不被將軍（或作為錯誤處理）
        return false;
    }

    PieceColor opponentColor = (color == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
    bool inCheck = isSquareAttacked(tmpBoard, kingPos, opponentColor);

    // tmpBoard 是本地的；不需要撤銷真正的 m_board
    return inCheck;
}

bool ChessBoard::hasAnyValidMoves(PieceColor color) {
    // 檢查玩家是否有任何有效移動來逃脫將死
    // 這涵蓋所有三種逃脫條件：
    // 1. 國王可以移動到安全格子
    // 2. 棋子可以吃掉攻擊的棋子
    // 3. 棋子可以阻擋攻擊
    
    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            ChessPiece* piece = m_board[fromRow][fromCol];
            if (piece == nullptr || piece->getColor() != color) continue;

            for (int toRow = 0; toRow < 8; ++toRow) {
                for (int toCol = 0; toCol < 8; ++toCol) {
                    QPoint from(fromCol, fromRow);
                    QPoint to(toCol, toRow);

                    // 檢查棋子是否可以根據其規則移動
                    if (!piece->isValidMove(to, this)) continue;
                    
                    // 王車易位移動的額外驗證
                    // 王車易位在基本移動驗證之外有特殊要求
                    if (piece->getType() == PieceType::KING && abs(to.x() - from.x()) == 2) {
                        bool kingSide = to.x() > from.x();
                        if (!canCastle(color, kingSide)) continue;
                    }
                    
                    // 檢查此移動是否會使國王處於被將軍狀態
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
    // 將死發生於滿足所有三個條件時：
    // 1. 國王被攻擊（被將軍）
    if (!isKingInCheck(color)) return false;
    
    // 2. 國王無法透過移動到安全格子來逃脫
    // 3. 沒有棋子可以吃掉攻擊的棋子
    // 4. 沒有棋子可以阻擋攻擊
    // （條件 2-4 都由 hasAnyValidMoves 檢查）
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

    // 檢查國王和車是否存在且未移動過
    if (king == nullptr || king->getType() != PieceType::KING || king->hasMoved())
        return false;
    if (rook == nullptr || rook->getType() != PieceType::ROOK || rook->hasMoved())
        return false;

    // 檢查國王是否被將軍
    if (isKingInCheck(color)) return false;

    // 檢查國王和車之間的格子是否為空
    int start = kingSide ? kingCol + 1 : rookCol + 1;
    int end = kingSide ? rookCol : kingCol;
    for (int col = start; col < end; ++col) {
        if (m_board[row][col] != nullptr) return false;
    }

    // 檢查國王是否經過或落在被攻擊的格子上
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

    // 移動國王
    m_board[row][newKingCol] = king;
    m_board[row][kingCol] = nullptr;
    king->setPosition(QPoint(newKingCol, row));
    king->setMoved(true);

    // 移動車
    m_board[row][newRookCol] = rook;
    m_board[row][rookCol] = nullptr;
    rook->setPosition(QPoint(newRookCol, row));
    rook->setMoved(true);
}
