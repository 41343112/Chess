#include "startdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
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
    setWindowTitle(QString::fromUtf8("象棋遊戲"));
    setModal(true);
    setMinimumSize(500, 450);
    
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
    m_timeSlider->setMinimum(0);  // 0 means no time limit
    m_timeSlider->setMaximum(120);  // 0-60 seconds, 61-120 for 1-60 minutes
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

void StartDialog::updateTimeLabel()
{
    int value = m_timeSlider->value();
    QString text;
    
    if (value == 0) {
        // 0 means no time limit
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
        // 0 means no time limit
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
