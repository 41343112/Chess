#include "chessai.h"
#include "stockfishengine.h"
#include <QRandomGenerator>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <algorithm>
#include <limits>

ChessAI::ChessAI(AIDifficulty difficulty)
    : m_difficulty(difficulty)
    , m_useStockfish(false)
    , m_stockfishEngine(nullptr)
{
}

ChessAI::~ChessAI()
{
    if (m_stockfishEngine) {
        delete m_stockfishEngine;
        m_stockfishEngine = nullptr;
    }
}

QPair<QPoint, QPoint> ChessAI::getBestMove(ChessBoard* board, PieceColor aiColor)
{
    if (!board) {
        return QPair<QPoint, QPoint>(QPoint(-1, -1), QPoint(-1, -1));
    }

    // 如果啟用 Stockfish 且難度為 HARD，使用 Stockfish
    if (m_useStockfish && m_difficulty == AIDifficulty::HARD) {
        return getStockfishMove(board, aiColor);
    }

    switch (m_difficulty) {
    case AIDifficulty::EASY:
        return getRandomMove(board, aiColor);
    case AIDifficulty::MEDIUM:
        return getBasicEvaluationMove(board, aiColor);
    case AIDifficulty::HARD:
        return getMinimaxMove(board, aiColor);
    default:
        return getRandomMove(board, aiColor);
    }
}

QVector<QPair<QPoint, QPoint>> ChessAI::getAllValidMoves(ChessBoard* board, PieceColor color)
{
    QVector<QPair<QPoint, QPoint>> validMoves;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            ChessPiece* piece = board->getPieceAt(row, col);
            if (piece && piece->getColor() == color) {
                QPoint from(row, col);
                // 檢查所有可能的目標位置
                for (int toRow = 0; toRow < 8; ++toRow) {
                    for (int toCol = 0; toCol < 8; ++toCol) {
                        QPoint to(toRow, toCol);
                        if (board->canMove(from, to)) {
                            validMoves.append(qMakePair(from, to));
                        }
                    }
                }
            }
        }
    }

    return validMoves;
}

QPair<QPoint, QPoint> ChessAI::getRandomMove(ChessBoard* board, PieceColor aiColor)
{
    QVector<QPair<QPoint, QPoint>> validMoves = getAllValidMoves(board, aiColor);

    if (validMoves.isEmpty()) {
        return QPair<QPoint, QPoint>(QPoint(-1, -1), QPoint(-1, -1));
    }

    // 隨機選擇一個有效移動
    int randomIndex = QRandomGenerator::global()->bounded(validMoves.size());
    return validMoves[randomIndex];
}

int ChessAI::getPieceValue(ChessPiece* piece)
{
    if (!piece) return 0;

    switch (piece->getType()) {
    case PieceType::PAWN:
        return 100;
    case PieceType::KNIGHT:
        return 320;
    case PieceType::BISHOP:
        return 330;
    case PieceType::ROOK:
        return 500;
    case PieceType::QUEEN:
        return 900;
    case PieceType::KING:
        return 20000;
    default:
        return 0;
    }
}

int ChessAI::evaluateBoard(ChessBoard* board, PieceColor aiColor)
{
    int score = 0;
    PieceColor opponentColor = (aiColor == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            ChessPiece* piece = board->getPieceAt(row, col);
            if (piece) {
                int pieceValue = getPieceValue(piece);
                if (piece->getColor() == aiColor) {
                    score += pieceValue;
                } else {
                    score -= pieceValue;
                }
            }
        }
    }

    // 額外獎勵：控制中心
    for (int row = 3; row <= 4; ++row) {
        for (int col = 3; col <= 4; ++col) {
            ChessPiece* piece = board->getPieceAt(row, col);
            if (piece && piece->getColor() == aiColor) {
                score += 30;
            }
        }
    }

    // 檢查將軍狀態
    if (board->isKingInCheck(opponentColor)) {
        score += 50;
    }
    if (board->isKingInCheck(aiColor)) {
        score -= 50;
    }

    return score;
}

QPair<QPoint, QPoint> ChessAI::getBasicEvaluationMove(ChessBoard* board, PieceColor aiColor)
{
    QVector<QPair<QPoint, QPoint>> validMoves = getAllValidMoves(board, aiColor);

    if (validMoves.isEmpty()) {
        return QPair<QPoint, QPoint>(QPoint(-1, -1), QPoint(-1, -1));
    }

    QPair<QPoint, QPoint> bestMove = validMoves[0];
    int bestScore = std::numeric_limits<int>::min();

    // 保存當前回合
    PieceColor originalTurn = board->getCurrentTurn();

    for (const auto& move : validMoves) {
        // 模擬移動
        ChessPiece* capturedPiece = board->getPieceAt(move.second);
        int captureValue = (capturedPiece) ? getPieceValue(capturedPiece) : 0;
        
        // 簡單評估：吃子的價值
        int moveScore = captureValue;
        
        // 優先考慮中心控制
        int toRow = move.second.x();
        int toCol = move.second.y();
        if (toRow >= 3 && toRow <= 4 && toCol >= 3 && toCol <= 4) {
            moveScore += 30;
        }

        if (moveScore > bestScore) {
            bestScore = moveScore;
            bestMove = move;
        }
    }

    return bestMove;
}

int ChessAI::minimax(ChessBoard* board, int depth, int alpha, int beta, bool maximizingPlayer, PieceColor aiColor)
{
    if (depth == 0) {
        return evaluateBoard(board, aiColor);
    }

    PieceColor currentColor = maximizingPlayer ? aiColor : 
                              (aiColor == PieceColor::WHITE ? PieceColor::BLACK : PieceColor::WHITE);
    
    // 檢查遊戲結束狀態
    if (board->isCheckmate(currentColor)) {
        return maximizingPlayer ? -100000 : 100000;
    }
    if (board->isStalemate(currentColor)) {
        return 0;
    }

    QVector<QPair<QPoint, QPoint>> moves = getAllValidMoves(board, currentColor);
    if (moves.isEmpty()) {
        return 0;
    }

    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        for (const auto& move : moves) {
            // 保存狀態
            QVector<Move> savedHistory = board->getMoveHistory();
            PieceColor savedTurn = board->getCurrentTurn();
            
            // 執行移動
            if (board->movePiece(move.first, move.second)) {
                int eval = minimax(board, depth - 1, alpha, beta, false, aiColor);
                
                // 撤銷移動
                board->undo();
                
                maxEval = std::max(maxEval, eval);
                alpha = std::max(alpha, eval);
                if (beta <= alpha) {
                    break; // Beta cutoff
                }
            }
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (const auto& move : moves) {
            // 保存狀態
            QVector<Move> savedHistory = board->getMoveHistory();
            PieceColor savedTurn = board->getCurrentTurn();
            
            // 執行移動
            if (board->movePiece(move.first, move.second)) {
                int eval = minimax(board, depth - 1, alpha, beta, true, aiColor);
                
                // 撤銷移動
                board->undo();
                
                minEval = std::min(minEval, eval);
                beta = std::min(beta, eval);
                if (beta <= alpha) {
                    break; // Alpha cutoff
                }
            }
        }
        return minEval;
    }
}

QPair<QPoint, QPoint> ChessAI::getMinimaxMove(ChessBoard* board, PieceColor aiColor)
{
    QVector<QPair<QPoint, QPoint>> validMoves = getAllValidMoves(board, aiColor);

    if (validMoves.isEmpty()) {
        return QPair<QPoint, QPoint>(QPoint(-1, -1), QPoint(-1, -1));
    }

    QPair<QPoint, QPoint> bestMove = validMoves[0];
    int bestScore = std::numeric_limits<int>::min();
    int searchDepth = 3; // 深度 3-4 ply 為困難難度

    for (const auto& move : validMoves) {
        // 執行移動
        if (board->movePiece(move.first, move.second)) {
            int score = minimax(board, searchDepth - 1, 
                              std::numeric_limits<int>::min(), 
                              std::numeric_limits<int>::max(), 
                              false, aiColor);
            
            // 撤銷移動
            board->undo();
            
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
        }
    }

    return bestMove;
}

void ChessAI::setUseStockfish(bool use)
{
    m_useStockfish = use;
    
    if (m_useStockfish && !m_stockfishEngine) {
        // 初始化 Stockfish 引擎
        m_stockfishEngine = new StockfishEngine();
        
        // 尋找 Stockfish 可執行檔
        QString enginePath;
        
#ifdef Q_OS_WIN
        enginePath = QCoreApplication::applicationDirPath() + "/engine/stockfish-windows-x86-64-avx2.exe";
#elif defined(Q_OS_LINUX)
        enginePath = QCoreApplication::applicationDirPath() + "/engine/stockfish-linux-x86-64";
        // 如果找不到，嘗試系統路徑
        if (!QFile::exists(enginePath)) {
            enginePath = "/usr/games/stockfish";
        }
        if (!QFile::exists(enginePath)) {
            enginePath = "/usr/bin/stockfish";
        }
#elif defined(Q_OS_MAC)
        enginePath = QCoreApplication::applicationDirPath() + "/engine/stockfish-mac";
        // 如果找不到，嘗試 Homebrew 路徑
        if (!QFile::exists(enginePath)) {
            enginePath = "/usr/local/bin/stockfish";
        }
        if (!QFile::exists(enginePath)) {
            enginePath = "/opt/homebrew/bin/stockfish";
        }
#endif
        
        qDebug() << "Attempting to initialize Stockfish at:" << enginePath;
        
        if (!m_stockfishEngine->initialize(enginePath)) {
            qWarning() << "Failed to initialize Stockfish engine";
            delete m_stockfishEngine;
            m_stockfishEngine = nullptr;
            m_useStockfish = false;
        } else {
            qDebug() << "Stockfish engine initialized successfully";
            
            // 根據難度設定技能等級
            int skillLevel = 20; // 預設最高技能等級
            switch (m_difficulty) {
            case AIDifficulty::EASY:
                skillLevel = 5;
                break;
            case AIDifficulty::MEDIUM:
                skillLevel = 10;
                break;
            case AIDifficulty::HARD:
                skillLevel = 20;
                break;
            }
            m_stockfishEngine->setSkillLevel(skillLevel);
        }
    } else if (!m_useStockfish && m_stockfishEngine) {
        // 關閉引擎
        delete m_stockfishEngine;
        m_stockfishEngine = nullptr;
    }
}

QPair<QPoint, QPoint> ChessAI::getStockfishMove(ChessBoard* board, PieceColor aiColor)
{
    if (!m_stockfishEngine || !m_stockfishEngine->isReady()) {
        qWarning() << "Stockfish engine not ready, falling back to Minimax";
        return getMinimaxMove(board, aiColor);
    }

    // 取得移動歷史（UCI 格式）
    QVector<QString> moves = getMovesInUci(board);
    
    // 設定棋盤位置
    m_stockfishEngine->setPositionWithMoves(moves);
    
    // 根據難度設定思考時間
    int thinkingTime = 1000; // 預設 1 秒
    switch (m_difficulty) {
    case AIDifficulty::EASY:
        thinkingTime = 100;
        break;
    case AIDifficulty::MEDIUM:
        thinkingTime = 500;
        break;
    case AIDifficulty::HARD:
        thinkingTime = 2000;
        break;
    }
    
    // 取得最佳移動
    QString uciMove = m_stockfishEngine->getBestMove(thinkingTime);
    
    if (uciMove.isEmpty()) {
        qWarning() << "Stockfish returned empty move, falling back to Minimax";
        return getMinimaxMove(board, aiColor);
    }
    
    qDebug() << "Stockfish suggests move:" << uciMove;
    
    // 將 UCI 移動轉換為 QPoint 座標
    return uciToMove(uciMove);
}

QString ChessAI::moveToUci(QPoint from, QPoint to) const
{
    // 將棋盤座標 (row, col) 轉換為 UCI 格式 (例如 e2e4)
    // 行：0-7 對應到 8-1
    // 列：0-7 對應到 a-h
    
    char fromFile = 'a' + from.y();
    char fromRank = '8' - from.x();
    char toFile = 'a' + to.y();
    char toRank = '8' - to.x();
    
    return QString("%1%2%3%4").arg(fromFile).arg(fromRank).arg(toFile).arg(toRank);
}

QPair<QPoint, QPoint> ChessAI::uciToMove(const QString& uci) const
{
    // 將 UCI 格式 (例如 e2e4) 轉換為棋盤座標
    if (uci.length() < 4) {
        return QPair<QPoint, QPoint>(QPoint(-1, -1), QPoint(-1, -1));
    }
    
    int fromCol = uci[0].toLatin1() - 'a';
    int fromRow = '8' - uci[1].toLatin1();
    int toCol = uci[2].toLatin1() - 'a';
    int toRow = '8' - uci[3].toLatin1();
    
    return QPair<QPoint, QPoint>(QPoint(fromRow, fromCol), QPoint(toRow, toCol));
}

QString ChessAI::boardToFen(ChessBoard* board, PieceColor currentTurn) const
{
    // 生成 FEN 字串
    QString fen;
    
    // 1. 棋盤位置
    for (int row = 0; row < 8; ++row) {
        int emptyCount = 0;
        for (int col = 0; col < 8; ++col) {
            ChessPiece* piece = board->getPieceAt(row, col);
            if (piece) {
                if (emptyCount > 0) {
                    fen += QString::number(emptyCount);
                    emptyCount = 0;
                }
                
                char pieceChar = ' ';
                switch (piece->getType()) {
                case PieceType::PAWN:   pieceChar = 'p'; break;
                case PieceType::KNIGHT: pieceChar = 'n'; break;
                case PieceType::BISHOP: pieceChar = 'b'; break;
                case PieceType::ROOK:   pieceChar = 'r'; break;
                case PieceType::QUEEN:  pieceChar = 'q'; break;
                case PieceType::KING:   pieceChar = 'k'; break;
                default: break;
                }
                
                if (piece->getColor() == PieceColor::WHITE) {
                    pieceChar = toupper(pieceChar);
                }
                
                fen += pieceChar;
            } else {
                emptyCount++;
            }
        }
        
        if (emptyCount > 0) {
            fen += QString::number(emptyCount);
        }
        
        if (row < 7) {
            fen += '/';
        }
    }
    
    // 2. 當前回合
    fen += (currentTurn == PieceColor::WHITE) ? " w " : " b ";
    
    // 3. 王車易位權利（簡化版本 - 可能需要根據實際狀態調整）
    QString castling;
    if (board->canCastle(PieceColor::WHITE, true)) castling += 'K';
    if (board->canCastle(PieceColor::WHITE, false)) castling += 'Q';
    if (board->canCastle(PieceColor::BLACK, true)) castling += 'k';
    if (board->canCastle(PieceColor::BLACK, false)) castling += 'q';
    if (castling.isEmpty()) castling = "-";
    fen += castling + " ";
    
    // 4. 吃過路兵目標方格
    QPoint enPassant = board->getEnPassantTarget();
    if (enPassant.x() >= 0 && enPassant.y() >= 0) {
        char file = 'a' + enPassant.y();
        char rank = '8' - enPassant.x();
        fen += QString("%1%2").arg(file).arg(rank);
    } else {
        fen += "-";
    }
    
    // 5. 半步計數和移動數（簡化版本）
    fen += " 0 1";
    
    return fen;
}

QVector<QString> ChessAI::getMovesInUci(ChessBoard* board) const
{
    QVector<QString> uciMoves;
    
    const QVector<Move>& history = board->getMoveHistory();
    for (const Move& move : history) {
        QString uci = moveToUci(move.from, move.to);
        
        // 如果是升變，添加升變棋子
        if (move.wasPromotion) {
            char promotionChar = 'q'; // 預設為后
            switch (move.promotedTo) {
            case PieceType::QUEEN:  promotionChar = 'q'; break;
            case PieceType::ROOK:   promotionChar = 'r'; break;
            case PieceType::BISHOP: promotionChar = 'b'; break;
            case PieceType::KNIGHT: promotionChar = 'n'; break;
            default: break;
            }
            uci += promotionChar;
        }
        
        uciMoves.append(uci);
    }
    
    return uciMoves;
}
