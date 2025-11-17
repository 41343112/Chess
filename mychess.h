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
#include <QDrag>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QSoundEffect>
#include "chessboard.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class myChess;
}
QT_END_NAMESPACE

// 幫助維持棋盤方形比例的輔助元件
class SquareBoardWidget : public QWidget {
    Q_OBJECT

public:
    explicit SquareBoardWidget(QWidget* parent = nullptr) : QWidget(parent) {
        // 擴展以獲取可用空間；不要強制大的最小值以便可以縮小
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setContentsMargins(0, 0, 0, 0);
    }

    // 讓佈局系統知道我們有高度隨寬度變化的關係
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

        // 在元件內保持內部佈局為正方形
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
    QString m_draggedPieceText;  // 拖曳期間儲存棋子文字
    bool m_isDragging;  // 追蹤是否正在拖曳

    void updateStyle();
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

    // 設定/取得棋盤最小尺寸（像素）
    void setMinBoardSize(int px);
    int  minBoardSize() const;

protected:
    // 重寫以控制視窗調整大小時的棋盤尺寸
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onSquareClicked();
    void onNewGame();
    void onUndo();
    void onFlipBoard();

private:
    Ui::myChess *ui;
    ChessBoard* m_chessBoard;
    ChessSquare* m_squares[8][8];
    QPoint m_selectedSquare;
    bool m_hasSelection;

    QLabel* m_statusLabel;
    QLabel* m_turnLabel;
    QPushButton* m_newGameButton;
    QPushButton* m_undoButton;
    QPushButton* m_flipBoardButton;

    QSoundEffect* m_moveSound;
    QSoundEffect* m_captureSound;

    bool m_isBoardFlipped;

    // 將棋盤元件設為成員變數，以便 resizeEvent 可以控制它
    SquareBoardWidget* m_boardWidget;

    // 可設定的最小棋盤尺寸（像素），預設 40
    int m_minBoardSize;

    void setupUI();
    void updateBoard();
    void clearHighlights();
    void highlightValidMoves(QPoint from);
    void showGameOverDialog();
    void playMoveSound(bool isCapture);
};

#endif // MYCHESS_H
