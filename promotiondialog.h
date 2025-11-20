#ifndef PROMOTIONDIALOG_H
#define PROMOTIONDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include "chesspiece.h"

class PromotionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PromotionDialog(PieceColor color, QWidget *parent = nullptr);
    ~PromotionDialog();
    
    PieceType getSelectedPieceType() const { return m_selectedPieceType; }

private slots:
    void onQueenClicked();
    void onRookClicked();
    void onBishopClicked();
    void onKnightClicked();

private:
    void setupUI();
    QPushButton* createPieceButton(PieceType type, const QString& symbol);
    
    PieceColor m_color;
    PieceType m_selectedPieceType;
    
    QPushButton* m_queenButton;
    QPushButton* m_rookButton;
    QPushButton* m_bishopButton;
    QPushButton* m_knightButton;
};

#endif // PROMOTIONDIALOG_H
