#include "chessai.h"
#include <QRandomGenerator>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <algorithm>
#include <limits>

ChessAI::ChessAI(AIDifficulty difficulty, QObject* parent)
    : QObject(parent),
      m_difficulty(difficulty),
      m_skillLevel(10),
      m_useEngine(true),
      m_engine(nullptr),
      m_currentBoard(nullptr),
      m_currentColor(PieceColor::WHITE)
{
    m_engine = new UCIEngine(this);
    
    connect(m_engine, &UCIEngine::bestMoveFound, this, &ChessAI::onEngineMoveFound);
    connect(m_engine, &UCIEngine::engineError, this, &ChessAI::onEngineError);
    
    // 根據難度設定技能等級
    updateSkillLevelFromDifficulty();
}

ChessAI::~ChessAI()
{
    // m_engine 會被 Qt 的父物件系統自動刪除
}

void ChessAI::updateSkillLevelFromDifficulty()
{
    switch (m_difficulty) {
    case AIDifficulty::EASY:
        m_skillLevel = 5;  // 低技能
        break;
    case AIDifficulty::MEDIUM:
        m_skillLevel = 10;  // 中等技能
        break;
    case AIDifficulty::HARD:
        m_skillLevel = 20;  // 最高技能
        break;
    }
    
    if (m_engine) {
        m_engine->setSkillLevel(m_skillLevel);
    }
}

bool ChessAI::initializeEngine(const QString& enginePath)
{
    if (!m_engine) {
        return false;
    }
    
    bool success = m_engine->initialize(enginePath);
    if (success) {
        m_engine->setSkillLevel(m_skillLevel);
    }
    
    return success;
}

void ChessAI::setDifficulty(AIDifficulty difficulty)
{
    m_difficulty = difficulty;
    updateSkillLevelFromDifficulty();
}

void ChessAI::setSkillLevel(int level)
{
    m_skillLevel = qBound(0, level, 20);
    if (m_engine) {
        m_engine->setSkillLevel(m_skillLevel);
    }
}

void ChessAI::getBestMove(ChessBoard* board, PieceColor aiColor)
{
    m_currentBoard = board;
    m_currentColor = aiColor;
    
    if (m_useEngine && m_engine && m_engine->isReady()) {
        // 使用 UCI 引擎
        m_engine->getBestMove(board, aiColor);
    } else {
        // 使用內建 AI（備用）
        QPair<QPoint, QPoint> move;
        
        switch (m_difficulty) {
        case AIDifficulty::EASY:
            move = getRandomMove(board, aiColor);
            break;
        case AIDifficulty::MEDIUM:
            move = getBasicEvaluationMove(board, aiColor);
            break;
        case AIDifficulty::HARD:
            move = getMinimaxMove(board, aiColor);
            break;
        default:
            move = getRandomMove(board, aiColor);
            break;
        }
        
        // 發射移動訊號
        if (move.first != QPoint(-1, -1)) {
            emit moveReady(move.first, move.second);
        } else {
            emit engineError("No valid moves available");
        }
    }
}

void ChessAI::onEngineMoveFound(QString fromUCI, QString toUCI)
{
    QPoint from = uciToPosition(fromUCI);
    QPoint to = uciToPosition(toUCI);
    
    if (from != QPoint(-1, -1) && to != QPoint(-1, -1)) {
        emit moveReady(from, to);
    } else {
        emit engineError("Invalid move from engine");
    }
}

void ChessAI::onEngineError(QString error)
{
    qDebug() << "Engine error:" << error;
    
    // 引擎錯誤時，使用內建 AI 作為備用
    m_useEngine = false;
    if (m_currentBoard) {
        getBestMove(m_currentBoard, m_currentColor);
    }
}

QPoint ChessAI::uciToPosition(const QString& uci)
{
    if (uci.length() < 2) return QPoint(-1, -1);
    
    int col = uci[0].toLatin1() - 'a';
    int row = uci[1].toLatin1() - '1';
    
    if (row < 0 || row > 7 || col < 0 || col > 7) {
        return QPoint(-1, -1);
    }
    
    return QPoint(row, col);
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
