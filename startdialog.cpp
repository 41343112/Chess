#include "startdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFont>
#include <QColorDialog>

const QColor StartDialog::DEFAULT_LIGHT_COLOR = QColor("#F0D9B5");
const QColor StartDialog::DEFAULT_DARK_COLOR = QColor("#B58863");

StartDialog::StartDialog(QWidget *parent)
    : QDialog(parent),
      m_lightSquareColor(DEFAULT_LIGHT_COLOR),
      m_darkSquareColor(DEFAULT_DARK_COLOR)
{
    setupUI();
}

StartDialog::~StartDialog()
{
}

void StartDialog::setupUI()
{
    setWindowTitle(QString::fromUtf8("象棋遊戲"));
    setModal(true);
    setMinimumSize(500, 500);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 30, 40, 30);
    
    // Title label
    m_titleLabel = new QLabel(QString::fromUtf8("歡迎來到象棋遊戲"), this);
    m_titleLabel->setFont(QFont("Arial", 24, QFont::Bold));
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("QLabel { color: #2C3E50; }");
    
    mainLayout->addWidget(m_titleLabel);
    mainLayout->addSpacing(10);
    
    // Time control group
    QGroupBox* timeGroup = new QGroupBox(QString::fromUtf8("時間控制"), this);
    QVBoxLayout* timeLayout = new QVBoxLayout(timeGroup);
    timeLayout->setSpacing(15);
    
    // Enable time control checkbox
    m_timeControlCheckBox = new QCheckBox(QString::fromUtf8("啟用時間控制"), this);
    m_timeControlCheckBox->setChecked(false);
    timeLayout->addWidget(m_timeControlCheckBox);
    
    // Time slider section
    QLabel* timeLabel = new QLabel(QString::fromUtf8("每位玩家總時間："), this);
    timeLayout->addWidget(timeLabel);
    
    m_timeSlider = new QSlider(Qt::Horizontal, this);
    m_timeSlider->setMinimum(0);  // 0 for unlimited
    m_timeSlider->setMaximum(120);  // 0=unlimited, 1-60 seconds, 61-120 for 1-60 minutes
    m_timeSlider->setValue(30);  // Default to 30 seconds
    m_timeSlider->setEnabled(false);
    m_timeSlider->setTickPosition(QSlider::TicksBelow);
    m_timeSlider->setTickInterval(10);
    timeLayout->addWidget(m_timeSlider);
    
    m_timeValueLabel = new QLabel(this);
    m_timeValueLabel->setAlignment(Qt::AlignCenter);
    m_timeValueLabel->setStyleSheet("QLabel { font-size: 14pt; font-weight: bold; color: #2C3E50; }");
    updateTimeLabel();
    timeLayout->addWidget(m_timeValueLabel);
    
    mainLayout->addWidget(timeGroup);
    
    // Board colors group
    QGroupBox* colorGroup = new QGroupBox(QString::fromUtf8("棋盤顏色"), this);
    QFormLayout* colorLayout = new QFormLayout(colorGroup);
    
    m_lightSquareColorButton = new QPushButton(QString::fromUtf8("淺色方格"), this);
    m_lightSquareColorButton->setMinimumHeight(35);
    updateColorButtonStyle(m_lightSquareColorButton, m_lightSquareColor);
    connect(m_lightSquareColorButton, &QPushButton::clicked, this, &StartDialog::onLightColorButtonClicked);
    
    m_darkSquareColorButton = new QPushButton(QString::fromUtf8("深色方格"), this);
    m_darkSquareColorButton->setMinimumHeight(35);
    updateColorButtonStyle(m_darkSquareColorButton, m_darkSquareColor);
    connect(m_darkSquareColorButton, &QPushButton::clicked, this, &StartDialog::onDarkColorButtonClicked);
    
    colorLayout->addRow(QString::fromUtf8("淺色方格："), m_lightSquareColorButton);
    colorLayout->addRow(QString::fromUtf8("深色方格："), m_darkSquareColorButton);
    mainLayout->addWidget(colorGroup);
    
    // Connect signals
    connect(m_timeControlCheckBox, &QCheckBox::stateChanged, this, &StartDialog::onTimeControlCheckChanged);
    connect(m_timeSlider, &QSlider::valueChanged, this, &StartDialog::onTimeSliderChanged);
    
    mainLayout->addStretch();
    
    // Start button
    m_startButton = new QPushButton(QString::fromUtf8("開始遊戲"), this);
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
    
    setLayout(mainLayout);
}

void StartDialog::onTimeControlCheckChanged(int state)
{
    bool enabled = (state == Qt::Checked);
    m_timeSlider->setEnabled(enabled);
}

void StartDialog::onTimeSliderChanged(int /* value */)
{
    updateTimeLabel();
}

void StartDialog::onLightColorButtonClicked()
{
    QColor color = QColorDialog::getColor(m_lightSquareColor, this, QString::fromUtf8("選擇淺色方格顏色"));
    if (color.isValid()) {
        m_lightSquareColor = color;
        updateColorButtonStyle(m_lightSquareColorButton, m_lightSquareColor);
    }
}

void StartDialog::onDarkColorButtonClicked()
{
    QColor color = QColorDialog::getColor(m_darkSquareColor, this, QString::fromUtf8("選擇深色方格顏色"));
    if (color.isValid()) {
        m_darkSquareColor = color;
        updateColorButtonStyle(m_darkSquareColorButton, m_darkSquareColor);
    }
}

void StartDialog::updateColorButtonStyle(QPushButton* button, const QColor& color)
{
    QString style = QString(
        "QPushButton {"
        "    background-color: %1;"
        "    border: 2px solid #555;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "    color: %2;"
        "}"
        "QPushButton:hover {"
        "    border: 2px solid #888;"
        "}"
    ).arg(color.name())
     .arg(color.lightness() > 128 ? "black" : "white");
    
    button->setStyleSheet(style);
}

void StartDialog::updateTimeLabel()
{
    int value = m_timeSlider->value();
    QString text;
    
    if (value == 0) {
        // Unlimited time
        text = QString::fromUtf8("無限制");
    } else if (value <= 60) {
        // 1-60 seconds
        text = QString::fromUtf8("%1 秒").arg(value);
    } else {
        // 61-120 maps to 1-60 minutes
        int minutes = value - 60;
        text = QString::fromUtf8("%1 分鐘").arg(minutes);
    }
    
    m_timeValueLabel->setText(text);
}

bool StartDialog::isTimeControlEnabled() const
{
    return m_timeControlCheckBox->isChecked();
}

int StartDialog::getTimeControlSeconds() const
{
    int value = m_timeSlider->value();
    
    if (value == 0) {
        // Unlimited time - return 0 or a very large value
        return 0;
    } else if (value <= 60) {
        // 1-60 seconds
        return value;
    } else {
        // 61-120 maps to 1-60 minutes, convert to seconds
        int minutes = value - 60;
        return minutes * 60;
    }
}

QColor StartDialog::getLightSquareColor() const
{
    return m_lightSquareColor;
}

QColor StartDialog::getDarkSquareColor() const
{
    return m_darkSquareColor;
}
