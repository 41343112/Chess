#ifndef MYCHESS_H
#define MYCHESS_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QMouseEvent>
#include <QEvent>
#include <QDrag>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QSoundEffect>
#include <QTimer>
#include <QSettings>
#include <QTranslator>
#include "chessboard.h"
#include "chessai.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class myChess;
}
QT_END_NAMESPACE

// 輔助小工具，用於維持棋盤的正方形比例
class SquareBoardWidget : public QWidget {
    Q_OBJECT

public:
    explicit SquareBoardWidget(QWidget* parent = nullptr) : QWidget(parent) {
        // 擴展以獲得可用空間；不強制設定大的最小值以便縮小
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setContentsMargins(0, 0, 0, 0);
    }

    // 讓佈局系統知道我們有高度與寬度的關聯
    bool hasHeightForWidth() const override { return true; }
    int heightForWidth(int width) const override { return width; }

    QSize sizeHint() const override {
        int size = qMin(width(), height());
        if (size <= 0) size = 600;
        return QSize(size, size);
    }

protected:
    void resizeEvent(QResizeEvent* event) override {
        QWidget::resizeEvent(event);

        // 保持內部佈局在小工具內呈正方形
        int w = width();
        int h = height();
        int size = qMin(w, h);
        int x = (w - size) / 2;
        int y = (h - size) / 2;
        if (layout()) {
            layout()->setContentsMargins(0, 0, 0, 0);
            layout()->setSpacing(0);
            layout()->setGeometry(QRect(x, y, size, size));
        }
    }
};

class ChessSquare : public QPushButton {
    Q_OBJECT

public:
    enum HighlightType {
        None,
        Movable,      // 藍色邊框 - 空格
        Capturable    // 紅色邊框 - 有對手棋子
    };

    ChessSquare(int row, int col, QWidget* parent = nullptr);

    void setPiece(ChessPiece* piece);
    void setHighlight(HighlightType type);
    void setSelected(bool selected);
    void setInCheck(bool inCheck);
    void setColors(const QColor& lightColor, const QColor& darkColor);

    int getRow() const { return m_row; }
    int getCol() const { return m_col; }

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    QSize sizeHint() const override;

    bool hasHeightForWidth() const override;
    int heightForWidth(int width) const override;

private:
    int m_row;
    int m_col;
    bool m_isLight;
    HighlightType m_highlightType;
    bool m_isSelected;
    bool m_isInCheck;
    QPoint m_dragStartPosition;
    QString m_draggedPieceText;  // 在拖曳期間儲存棋子文字
    bool m_isDragging;  // 追蹤是否正在拖曳
    ChessPiece* m_piece;  // 儲存棋子指標，用於調整大小時重新生成圖示
    QColor m_lightColor;
    QColor m_darkColor;

    void updateStyle();
    void updatePieceDisplay();  // 更新圖示/文字顯示的輔助函數
};

class myChess : public QMainWindow
{
    Q_OBJECT

public:
    myChess(QWidget *parent = nullptr);
    ~myChess();

    bool onSquareDragStarted(int row, int col);
    void onSquareDragEnded(int row, int col);
    void onSquareDragCancelled(int row, int col);

    // 設定/取得棋盤最小尺寸(像素)
    void setMinBoardSize(int px);
    int  minBoardSize() const;

protected:
    // 重載以在視窗調整大小時控制棋盤大小
    void resizeEvent(QResizeEvent* event) override;
    // 事件過濾器用於捕捉拖曳期間的右鍵點擊
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onSquareClicked();
    void onNewGame();
    void onUndo();
    void onFlipBoard();
    void onSettings();
    void showStartDialog();
    void onPreviousMove();
    void onNextMove();
    void onBackToStart();
    void onBackToCurrent();
    void onTimerTick();
    void makeComputerMove();  // 新增：電腦移動

private:
    Ui::myChess *ui;
    ChessBoard* m_chessBoard;
    ChessSquare* m_squares[8][8];
    QPoint m_selectedSquare;
    bool m_hasSelection;

    QLabel* m_statusLabel;
    QLabel* m_turnLabel;
    QLabel* m_whiteTimeLabel;
    QLabel* m_blackTimeLabel;
    QPushButton* m_newGameButton;
    QPushButton* m_undoButton;
    QPushButton* m_flipBoardButton;
    QPushButton* m_settingsButton;
    QPushButton* m_previousMoveButton;
    QPushButton* m_nextMoveButton;
    QPushButton* m_backToStartButton;
    QPushButton* m_backToCurrentButton;

    QSoundEffect* m_moveSound;
    QSoundEffect* m_captureSound;
    QSoundEffect* m_checkSound;
    QSoundEffect* m_checkmateSound;
    QSoundEffect* m_castlingSound;

    bool m_isBoardFlipped;

    // 將棋盤小工具設為成員，以便 resizeEvent 可以控制它
    SquareBoardWidget* m_boardWidget;

    // 可設定的最小棋盤尺寸(像素)，預設 40
    int m_minBoardSize;

    // 設定
    bool m_undoEnabled;
    QColor m_lightSquareColor;
    QColor m_darkSquareColor;
    bool m_timeControlEnabled;
    int m_timeControlMinutes;
    int m_incrementSeconds;  // 每步移動增加的秒數
    
    // 時間控制
    QTimer* m_gameTimer;
    int m_whiteTimeRemaining;  // 以毫秒為單位（更精確）
    int m_blackTimeRemaining;  // 以毫秒為單位（更精確）
    bool m_isTimerRunning;
    bool m_firstMoveMade;  // 追蹤是否已進行第一步移動以啟動計時器

    // 用於查看歷史記錄的導航狀態
    int m_viewingPosition;  // -1 表示查看目前位置，0+ 表示查看歷史記錄
    bool m_isViewingHistory;
    ChessPiece* m_tempViewBoard[8][8];  // 用於查看歷史記錄的臨時棋盤
    
    // 拖曳狀態追蹤
    bool m_isDragInProgress;
    QPoint m_dragSourceSquare;
    
    // AI 相關
    ChessAI* m_chessAI;
    bool m_isComputerGame;
    PieceColor m_computerColor;
    bool m_isComputerThinking;

    void setupUI();
    void updateBoard();
    void clearHighlights();
    void highlightValidMoves(QPoint from);
    void showGameOverDialog();
    void playMoveSound(bool isCapture, bool isCheck, bool isCheckmate, bool isCastling = false);
    void loadSettings();
    void applySettings();
    void updateNavigationButtons();
    void displayBoardAtPosition(int position);
    void clearTempViewBoard();
    void updateTimeDisplay();
    void startTimer();
    void stopTimer();
    void resetTimers();
    void addIncrement();
    QString formatTime(int milliseconds);
};

#endif // MYCHESS_H
