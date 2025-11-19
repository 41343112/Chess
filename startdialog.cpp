#include "startdialog.h"
#include <QVBoxLayout>
#include <QFont>

StartDialog::StartDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

StartDialog::~StartDialog()
{
}

void StartDialog::setupUI()
{
    setWindowTitle(tr("Chess Game"));
    setModal(true);
    setMinimumSize(400, 300);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(30);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    
    // Title label
    m_titleLabel = new QLabel(tr("Welcome to Chess Game"), this);
    m_titleLabel->setFont(QFont("Arial", 24, QFont::Bold));
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("QLabel { color: #2C3E50; }");
    
    // Add some spacing
    mainLayout->addStretch();
    mainLayout->addWidget(m_titleLabel);
    mainLayout->addStretch();
    
    // Start button
    m_startButton = new QPushButton(tr("Start Game"), this);
    m_startButton->setFont(QFont("Arial", 16, QFont::Bold));
    m_startButton->setMinimumHeight(60);
    m_startButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #27AE60;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 10px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #229954;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #1E8449;"
        "}"
    );
    
    connect(m_startButton, &QPushButton::clicked, this, &QDialog::accept);
    
    mainLayout->addWidget(m_startButton);
    mainLayout->addStretch();
    
    setLayout(mainLayout);
}
