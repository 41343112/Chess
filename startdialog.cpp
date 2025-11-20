#include "startdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFont>

StartDialog::StartDialog(QWidget *parent)
    : QDialog(parent),
      m_gameModeGroup(new QButtonGroup(this)),
      m_colorGroup(new QButtonGroup(this))
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
    setMinimumSize(500, 600);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 30, 40, 30);
    
    // 標題標籤
    m_titleLabel = new QLabel(QString::fromUtf8("歡迎來到象棋遊戲"), this);
    m_titleLabel->setFont(QFont("Arial", 24, QFont::Bold));
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("QLabel { color: #2C3E50; }");
    
    mainLayout->addWidget(m_titleLabel);
    mainLayout->addSpacing(10);
    
    // 遊戲模式群組
    QGroupBox* gameModeGroup = new QGroupBox(QString::fromUtf8("遊戲模式"), this);
    QVBoxLayout* gameModeLayout = new QVBoxLayout(gameModeGroup);
    gameModeLayout->setSpacing(10);
    
    m_humanVsHumanRadio = new QRadioButton(QString::fromUtf8("玩家對戰"), this);
    m_humanVsComputerRadio = new QRadioButton(QString::fromUtf8("與電腦對戰"), this);
    
    m_humanVsHumanRadio->setChecked(true);
    
    m_gameModeGroup->addButton(m_humanVsHumanRadio, 0);
    m_gameModeGroup->addButton(m_humanVsComputerRadio, 1);
    
    gameModeLayout->addWidget(m_humanVsHumanRadio);
    gameModeLayout->addWidget(m_humanVsComputerRadio);
    
    mainLayout->addWidget(gameModeGroup);
    
    // 電腦對戰選項（初始隱藏）
    m_computerOptionsWidget = new QWidget(this);
    QVBoxLayout* computerOptionsLayout = new QVBoxLayout(m_computerOptionsWidget);
    computerOptionsLayout->setSpacing(10);
    computerOptionsLayout->setContentsMargins(20, 10, 20, 10);
    
    // 難度選擇 - 使用滑桿
    QLabel* difficultyLabel = new QLabel(QString::fromUtf8("難度等級 (Stockfish 技能等級)："), this);
    computerOptionsLayout->addWidget(difficultyLabel);
    
    m_difficultySlider = new QSlider(Qt::Horizontal, this);
    m_difficultySlider->setMinimum(0);  // 最弱
    m_difficultySlider->setMaximum(20); // 最強
    m_difficultySlider->setValue(10);   // 預設中等
    m_difficultySlider->setTickPosition(QSlider::TicksBelow);
    m_difficultySlider->setTickInterval(2);
    computerOptionsLayout->addWidget(m_difficultySlider);
    
    m_difficultyValueLabel = new QLabel(this);
    m_difficultyValueLabel->setAlignment(Qt::AlignCenter);
    m_difficultyValueLabel->setStyleSheet("QLabel { font-size: 14pt; font-weight: bold; color: #2C3E50; }");
    updateDifficultyLabel();
    computerOptionsLayout->addWidget(m_difficultyValueLabel);
    
    // 連接難度滑桿訊號
    connect(m_difficultySlider, &QSlider::valueChanged, this, &StartDialog::onDifficultySliderChanged);
    
    computerOptionsLayout->addSpacing(10);
    
    // 顏色選擇
    QLabel* colorLabel = new QLabel(QString::fromUtf8("您的顏色："), this);
    computerOptionsLayout->addWidget(colorLabel);
    
    m_playAsWhiteRadio = new QRadioButton(QString::fromUtf8("白方（先手）"), this);
    m_playAsBlackRadio = new QRadioButton(QString::fromUtf8("黑方（後手）"), this);
    m_playAsWhiteRadio->setChecked(true);
    
    m_colorGroup->addButton(m_playAsWhiteRadio, 0);
    m_colorGroup->addButton(m_playAsBlackRadio, 1);
    
    computerOptionsLayout->addWidget(m_playAsWhiteRadio);
    computerOptionsLayout->addWidget(m_playAsBlackRadio);
    
    m_computerOptionsWidget->setVisible(false);
    mainLayout->addWidget(m_computerOptionsWidget);
    
    // 連接遊戲模式改變訊號
    connect(m_humanVsHumanRadio, &QRadioButton::toggled, this, &StartDialog::onGameModeChanged);
    connect(m_humanVsComputerRadio, &QRadioButton::toggled, this, &StartDialog::onGameModeChanged);
    
    // 時間控制群組
    QGroupBox* timeGroup = new QGroupBox(QString::fromUtf8("時間控制"), this);
    QVBoxLayout* timeLayout = new QVBoxLayout(timeGroup);
    timeLayout->setSpacing(15);
    
    // 時間滑桿區段
    QLabel* timeLabel = new QLabel(QString::fromUtf8("每位玩家總時間："), this);
    timeLayout->addWidget(timeLabel);
    
    m_timeSlider = new QSlider(Qt::Horizontal, this);
    m_timeSlider->setMinimum(0);  // 0 = 無限制
    m_timeSlider->setMaximum(90);  // 0=無限制，1-30=30-59秒，31-90=1-60分鐘
    m_timeSlider->setValue(0);  // 預設為無限制
    m_timeSlider->setTickPosition(QSlider::TicksBelow);
    m_timeSlider->setTickInterval(10);
    timeLayout->addWidget(m_timeSlider);
    
    m_timeValueLabel = new QLabel(this);
    m_timeValueLabel->setAlignment(Qt::AlignCenter);
    m_timeValueLabel->setStyleSheet("QLabel { font-size: 14pt; font-weight: bold; color: #2C3E50; }");
    updateTimeLabel();
    timeLayout->addWidget(m_timeValueLabel);
    
    // 增量區段
    QLabel* incrementLabel = new QLabel(QString::fromUtf8("每著增加秒數："), this);
    timeLayout->addWidget(incrementLabel);
    
    m_incrementSlider = new QSlider(Qt::Horizontal, this);
    m_incrementSlider->setMinimum(0);
    m_incrementSlider->setMaximum(60);
    m_incrementSlider->setValue(0);
    m_incrementSlider->setTickPosition(QSlider::TicksBelow);
    m_incrementSlider->setTickInterval(5);
    timeLayout->addWidget(m_incrementSlider);
    
    m_incrementValueLabel = new QLabel(this);
    m_incrementValueLabel->setAlignment(Qt::AlignCenter);
    m_incrementValueLabel->setStyleSheet("QLabel { font-size: 12pt; color: #2C3E50; }");
    updateIncrementLabel();
    timeLayout->addWidget(m_incrementValueLabel);
    
    mainLayout->addWidget(timeGroup);
    
    // 連接訊號
    connect(m_timeSlider, &QSlider::valueChanged, this, &StartDialog::onTimeSliderChanged);
    connect(m_incrementSlider, &QSlider::valueChanged, this, &StartDialog::onIncrementSliderChanged);
    
    mainLayout->addStretch();
    
    // 開始按鈕
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

void StartDialog::onTimeSliderChanged(int /* value */)
{
    updateTimeLabel();
}

void StartDialog::onIncrementSliderChanged(int /* value */)
{
    updateIncrementLabel();
}

void StartDialog::updateTimeLabel()
{
    int value = m_timeSlider->value();
    QString text;
    
    if (value == 0) {
        // 無限制
        text = QString::fromUtf8("無限制");
    } else if (value <= 30) {
        // 1-30 對應到 30-59 秒
        int seconds = 29 + value;  // value=1 -> 30秒，value=30 -> 59秒
        text = QString::fromUtf8("%1 秒").arg(seconds);
    } else {
        // 31-90 對應到 1-60 分鐘
        int minutes = value - 30;  // value=31 -> 1分鐘，value=90 -> 60分鐘
        text = QString::fromUtf8("%1 分鐘").arg(minutes);
    }
    
    m_timeValueLabel->setText(text);
}

void StartDialog::updateIncrementLabel()
{
    int value = m_incrementSlider->value();
    m_incrementValueLabel->setText(QString::fromUtf8("%1 秒").arg(value));
}

int StartDialog::getTimeControlSeconds() const
{
    int value = m_timeSlider->value();
    
    if (value == 0) {
        // 無限制 - 返回 0 表示沒有時間控制
        return 0;
    } else if (value <= 30) {
        // 1-30 對應到 30-59 秒
        return 29 + value;  // value=1 -> 30秒，value=30 -> 59秒
    } else {
        // 31-90 對應到 1-60 分鐘，轉換為秒數
        int minutes = value - 30;  // value=31 -> 1分鐘，value=90 -> 60分鐘
        return minutes * 60;
    }
}

int StartDialog::getIncrementSeconds() const
{
    return m_incrementSlider->value();
}

GameMode StartDialog::getGameMode() const
{
    if (m_humanVsComputerRadio->isChecked()) {
        return GameMode::HUMAN_VS_COMPUTER;
    }
    return GameMode::HUMAN_VS_HUMAN;
}

int StartDialog::getDifficulty() const
{
    // Return skill level directly (0-20)
    return m_difficultySlider->value();
}

bool StartDialog::isPlayerWhite() const
{
    return m_playAsWhiteRadio->isChecked();
}

void StartDialog::onGameModeChanged()
{
    updateDifficultyVisibility();
}

void StartDialog::updateDifficultyVisibility()
{
    bool isComputerMode = m_humanVsComputerRadio->isChecked();
    m_computerOptionsWidget->setVisible(isComputerMode);
}

void StartDialog::onDifficultySliderChanged(int /* value */)
{
    updateDifficultyLabel();
}

void StartDialog::updateDifficultyLabel()
{
    int level = m_difficultySlider->value();
    QString description;
    
    if (level <= 5) {
        description = QString::fromUtf8("初學者");
    } else if (level <= 10) {
        description = QString::fromUtf8("中等");
    } else if (level <= 15) {
        description = QString::fromUtf8("進階");
    } else {
        description = QString::fromUtf8("專家");
    }
    
    m_difficultyValueLabel->setText(QString::fromUtf8("等級 %1 (%2)").arg(level).arg(description));
}
}
