#include "startdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QApplication>
#include <QSettings>

StartDialog::StartDialog(QWidget *parent)
    : QDialog(parent), m_translator(new QTranslator(this))
{
    setupUI();
    
    // Load saved language preference
    QSettings settings("ChessGame", "Settings");
    QString savedLanguage = settings.value("language", "en").toString();
    
    // Set the combo box to match saved language
    int index = m_languageComboBox->findData(savedLanguage);
    if (index >= 0) {
        m_languageComboBox->setCurrentIndex(index);
        // Apply the language
        applyLanguage(savedLanguage);
    }
}

StartDialog::~StartDialog()
{
}

void StartDialog::setupUI()
{
    setWindowTitle(tr("Chess Game"));
    setModal(true);
    setMinimumSize(400, 350);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
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
    
    // Language selection
    QHBoxLayout* languageLayout = new QHBoxLayout();
    m_languageLabel = new QLabel(tr("Language:"), this);
    m_languageLabel->setFont(QFont("Arial", 12));
    
    m_languageComboBox = new QComboBox(this);
    m_languageComboBox->addItem("English", "en");
    m_languageComboBox->addItem("中文", "zh");
    m_languageComboBox->setFont(QFont("Arial", 12));
    m_languageComboBox->setMinimumHeight(35);
    m_languageComboBox->setStyleSheet(
        "QComboBox {"
        "    border: 2px solid #BDC3C7;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "    background-color: white;"
        "}"
        "QComboBox:hover {"
        "    border: 2px solid #3498DB;"
        "}"
        "QComboBox::drop-down {"
        "    border: none;"
        "}"
        "QComboBox::down-arrow {"
        "    image: none;"
        "    border-left: 5px solid transparent;"
        "    border-right: 5px solid transparent;"
        "    border-top: 5px solid #7F8C8D;"
        "}"
    );
    
    connect(m_languageComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StartDialog::onLanguageChanged);
    
    languageLayout->addStretch();
    languageLayout->addWidget(m_languageLabel);
    languageLayout->addWidget(m_languageComboBox);
    languageLayout->addStretch();
    mainLayout->addLayout(languageLayout);
    mainLayout->addSpacing(10);
    
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

void StartDialog::onLanguageChanged(int index)
{
    if (index < 0) return;
    
    QString language = m_languageComboBox->itemData(index).toString();
    
    // Save the language preference
    QSettings settings("ChessGame", "Settings");
    settings.setValue("language", language);
    
    // Apply the language change
    applyLanguage(language);
}

void StartDialog::applyLanguage(const QString& language)
{
    // Remove previous translator
    QApplication::removeTranslator(m_translator);
    
    // Load new translation
    if (language == "zh") {
        if (m_translator->load(":/translations/chess_zh_CN.qm")) {
            QApplication::installTranslator(m_translator);
        }
    }
    
    // Update all UI texts
    updateTexts();
}

void StartDialog::updateTexts()
{
    setWindowTitle(tr("Chess Game"));
    m_titleLabel->setText(tr("Welcome to Chess Game"));
    m_languageLabel->setText(tr("Language:"));
    m_startButton->setText(tr("Start Game"));
}
