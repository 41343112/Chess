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
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    
    QSize sizeHint() const override {
        return QSize(600, 600);
    }
    
    bool hasHeightForWidth() const override {
        return true;
    }
    
    int heightForWidth(int width) const override {
        return width;
    }
    
protected:
    void resizeEvent(QResizeEvent* event) override {
        QWidget::resizeEvent(event);
        
        if (!layout()) return;
        
        // Get the current size
        int w = width();
        int h = height();
        
        // Calculate the largest square that fits
        int size = qMin(w, h);
        
        // Calculate position to center the square
        int x = (w - size) / 2;
        int y = (h - size) / 2;
        
        // Set the layout geometry to be square and centered
        layout()->setGeometry(QRect(x, y, size, size));
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
    
    void setupUI();
    void updateBoard();
    void clearHighlights();
    void highlightValidMoves(QPoint from);
    void showGameOverDialog();
    void playMoveSound(bool isCapture);
};
#endif // MYCHESS_H
