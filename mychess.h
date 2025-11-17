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
#include "chessboard.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class myChess;
}
QT_END_NAMESPACE

class ChessSquare : public QPushButton {
    Q_OBJECT
    
public:
    ChessSquare(int row, int col, QWidget* parent = nullptr);
    
    void setPiece(ChessPiece* piece);
    void setHighlight(bool highlight);
    void setSelected(bool selected);
    
    int getRow() const { return m_row; }
    int getCol() const { return m_col; }
    
private:
    int m_row;
    int m_col;
    bool m_isLight;
    bool m_isHighlighted;
    bool m_isSelected;
    
    void updateStyle();
};

class myChess : public QMainWindow
{
    Q_OBJECT

public:
    myChess(QWidget *parent = nullptr);
    ~myChess();

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
