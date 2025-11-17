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
#include "chessboard.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class myChess;
}
QT_END_NAMESPACE

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
    
private:
    int m_row;
    int m_col;
    bool m_isLight;
    HighlightType m_highlightType;
    bool m_isSelected;
    bool m_isInCheck;
    QPoint m_dragStartPosition;
    QString m_draggedPieceText;  // Store piece text during drag
    
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

private slots:
    void onSquareClicked();
    void onNewGame();
    void onUndo();

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
    
    void setupUI();
    void updateBoard();
    void clearHighlights();
    void highlightValidMoves(QPoint from);
    void showGameOverDialog();
};
#endif // MYCHESS_H
