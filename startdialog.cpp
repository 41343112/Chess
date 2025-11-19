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
    setWindowTitle(tr("Chess Game"));
    setModal(true);
    setMinimumSize(500, 450);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 30, 40, 30);
    
    // Title label
    m_titleLabel = new QLabel(tr("Welcome to Chess Game"), this);
    m_titleLabel->setFont(QFont("Arial", 24, QFont::Bold));
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("QLabel { color: #2C3E50; }");
    
    mainLayout->addWidget(m_titleLabel);
    mainLayout->addSpacing(10);
    
    // Time control group
    QGroupBox* timeGroup = new QGroupBox(tr("Time Control"), this);
    QVBoxLayout* timeLayout = new QVBoxLayout(timeGroup);
    timeLayout->setSpacing(15);
    
    // Enable time control checkbox
    m_timeControlCheckBox = new QCheckBox(tr("Enable Time Control"), this);
    m_timeControlCheckBox->setChecked(false);
    timeLayout->addWidget(m_timeControlCheckBox);
    
    // Time slider section
    QLabel* timeLabel = new QLabel(tr("Total Time per Player:"), this);
    timeLayout->addWidget(timeLabel);
    
    m_timeSlider = new QSlider(Qt::Horizontal, this);
    m_timeSlider->setMinimum(1);
    m_timeSlider->setMaximum(120);  // 1-60 seconds, 61-120 for 1-60 minutes
    m_timeSlider->setValue(10);  // Default to 10 seconds
    m_timeSlider->setEnabled(false);
    m_timeSlider->setTickPosition(QSlider::TicksBelow);
    m_timeSlider->setTickInterval(10);
    timeLayout->addWidget(m_timeSlider);
    
    m_timeValueLabel = new QLabel(this);
    m_timeValueLabel->setAlignment(Qt::AlignCenter);
    m_timeValueLabel->setStyleSheet("QLabel { font-size: 14pt; font-weight: bold; color: #2C3E50; }");
    updateTimeLabel();
    timeLayout->addWidget(m_timeValueLabel);
    
    // Increment section
    QHBoxLayout* incrementLayout = new QHBoxLayout();
    QLabel* incrementLabel = new QLabel(tr("Increment per move (seconds):"), this);
    m_incrementSpinBox = new QSpinBox(this);
    m_incrementSpinBox->setMinimum(0);
    m_incrementSpinBox->setMaximum(60);
    m_incrementSpinBox->setValue(0);
    m_incrementSpinBox->setEnabled(false);
    incrementLayout->addWidget(incrementLabel);
    incrementLayout->addWidget(m_incrementSpinBox);
    incrementLayout->addStretch();
    timeLayout->addLayout(incrementLayout);
    
    mainLayout->addWidget(timeGroup);
    
    // Connect signals
    connect(m_timeControlCheckBox, &QCheckBox::stateChanged, this, &StartDialog::onTimeControlCheckChanged);
    connect(m_timeSlider, &QSlider::valueChanged, this, &StartDialog::onTimeSliderChanged);
    
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
    
    setLayout(mainLayout);
}

void StartDialog::onTimeControlCheckChanged(int state)
{
    bool enabled = (state == Qt::Checked);
    m_timeSlider->setEnabled(enabled);
    m_incrementSpinBox->setEnabled(enabled);
}

void StartDialog::onTimeSliderChanged(int /* value */)
{
    updateTimeLabel();
}

void StartDialog::updateTimeLabel()
{
    int value = m_timeSlider->value();
    QString text;
    
    if (value <= 60) {
        // 1-60 seconds
        text = QString("%1 %2").arg(value).arg(tr("seconds"));
    } else {
        // 61-120 maps to 1-60 minutes
        int minutes = value - 60;
        text = QString("%1 %2").arg(minutes).arg(tr("minutes"));
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
    
    if (value <= 60) {
        // 1-60 seconds
        return value;
    } else {
        // 61-120 maps to 1-60 minutes, convert to seconds
        int minutes = value - 60;
        return minutes * 60;
    }
}

int StartDialog::getIncrementSeconds() const
{
    return m_incrementSpinBox->value();
}
