#include "promotiondialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>

PromotionDialog::PromotionDialog(PieceColor color, QWidget *parent)
    : QDialog(parent),
      m_color(color),
      m_selectedPieceType(PieceType::QUEEN)  // Default to Queen
{
    setWindowTitle(tr("Pawn Promotion"));
    setModal(true);
    setupUI();
}

PromotionDialog::~PromotionDialog()
{
}

void PromotionDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    // Title label
    QLabel* titleLabel = new QLabel(tr("Choose promotion piece:"), this);
    titleLabel->setFont(QFont("Arial", 16, QFont::Bold));
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // Buttons layout
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(15);
    
    // Create buttons for each piece type
    m_queenButton = createPieceButton(PieceType::QUEEN, 
        m_color == PieceColor::WHITE ? "♕" : "♛");
    m_rookButton = createPieceButton(PieceType::ROOK, 
        m_color == PieceColor::WHITE ? "♖" : "♜");
    m_bishopButton = createPieceButton(PieceType::BISHOP, 
        m_color == PieceColor::WHITE ? "♗" : "♝");
    m_knightButton = createPieceButton(PieceType::KNIGHT, 
        m_color == PieceColor::WHITE ? "♘" : "♞");
    
    // Connect buttons
    connect(m_queenButton, &QPushButton::clicked, this, &PromotionDialog::onQueenClicked);
    connect(m_rookButton, &QPushButton::clicked, this, &PromotionDialog::onRookClicked);
    connect(m_bishopButton, &QPushButton::clicked, this, &PromotionDialog::onBishopClicked);
    connect(m_knightButton, &QPushButton::clicked, this, &PromotionDialog::onKnightClicked);
    
    buttonsLayout->addWidget(m_queenButton);
    buttonsLayout->addWidget(m_rookButton);
    buttonsLayout->addWidget(m_bishopButton);
    buttonsLayout->addWidget(m_knightButton);
    
    mainLayout->addLayout(buttonsLayout);
    
    // Add labels below buttons
    QHBoxLayout* labelsLayout = new QHBoxLayout();
    labelsLayout->setSpacing(15);
    
    QLabel* queenLabel = new QLabel(tr("Queen"), this);
    queenLabel->setAlignment(Qt::AlignCenter);
    queenLabel->setMinimumWidth(80);
    
    QLabel* rookLabel = new QLabel(tr("Rook"), this);
    rookLabel->setAlignment(Qt::AlignCenter);
    rookLabel->setMinimumWidth(80);
    
    QLabel* bishopLabel = new QLabel(tr("Bishop"), this);
    bishopLabel->setAlignment(Qt::AlignCenter);
    bishopLabel->setMinimumWidth(80);
    
    QLabel* knightLabel = new QLabel(tr("Knight"), this);
    knightLabel->setAlignment(Qt::AlignCenter);
    knightLabel->setMinimumWidth(80);
    
    labelsLayout->addWidget(queenLabel);
    labelsLayout->addWidget(rookLabel);
    labelsLayout->addWidget(bishopLabel);
    labelsLayout->addWidget(knightLabel);
    
    mainLayout->addLayout(labelsLayout);
    
    setMinimumSize(420, 200);
}

QPushButton* PromotionDialog::createPieceButton(PieceType type, const QString& symbol)
{
    QPushButton* button = new QPushButton(symbol, this);
    button->setFont(QFont("Arial", 48));
    button->setMinimumSize(80, 80);
    button->setStyleSheet(
        "QPushButton {"
        "    background-color: #F0D9B5;"
        "    border: 2px solid #8B4513;"
        "    border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #FFD700;"
        "    border: 3px solid #DAA520;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #FFA500;"
        "}"
    );
    return button;
}

void PromotionDialog::onQueenClicked()
{
    m_selectedPieceType = PieceType::QUEEN;
    accept();
}

void PromotionDialog::onRookClicked()
{
    m_selectedPieceType = PieceType::ROOK;
    accept();
}

void PromotionDialog::onBishopClicked()
{
    m_selectedPieceType = PieceType::BISHOP;
    accept();
}

void PromotionDialog::onKnightClicked()
{
    m_selectedPieceType = PieceType::KNIGHT;
    accept();
}
