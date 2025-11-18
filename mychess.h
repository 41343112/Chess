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

// Helper widget to maintain square aspect ratio for the board
class SquareBoardWidget : public QWidget {
    Q_OBJECT

public:
    explicit SquareBoardWidget(QWidget* parent = nullptr) : QWidget(parent) {
        // Expanding so it gets available space; do NOT force large minimum here so it can shrink
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setContentsMargins(0, 0, 0, 0);
    }

    // Let layout system know we have a height-for-width relationship
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

        // Keep inner layout square within the widget
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
        Movable,      // Blue border - empty square
        Capturable    // Red border - has opponent piece
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
    QString m_draggedPieceText;  // Store piece text during drag
    bool m_isDragging;  // Track if currently dragging
    ChessPiece* m_piece;  // Store piece pointer for icon regeneration on resize

    void updateStyle();
    void updatePieceDisplay();  // Helper to update icon/text display
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
    // Override to control board size when window resizes
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
    QSoundEffect* m_checkSound;
    QSoundEffect* m_checkmateSound;

    bool m_isBoardFlipped;

    // Make the board widget a member so resizeEvent can control it
    SquareBoardWidget* m_boardWidget;

    // 可設定的最小棋盤尺寸(像素)，預設 40
    int m_minBoardSize;

    void setupUI();
    void updateBoard();
    void clearHighlights();
    void highlightValidMoves(QPoint from);
    void showGameOverDialog();
    void playMoveSound(bool isCapture);
};

#endif // MYCHESS_H
