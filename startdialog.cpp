#include "startdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFont>
#include <QScrollArea>

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
    
    // 建立主佈局，包含滾動區域
    QVBoxLayout* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);
    
    // 建立滾動區域
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // 建立內容小工具
    QWidget* contentWidget = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(contentWidget);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 30, 40, 30);
    
    // 標題標籤
    m_titleLabel = new QLabel(QString::fromUtf8("歡迎來到象棋遊戲"), contentWidget);
    m_titleLabel->setFont(QFont("Arial", 24, QFont::Bold));
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("QLabel { color: #2C3E50; }");
    
    mainLayout->addWidget(m_titleLabel);
    mainLayout->addSpacing(10);
    
    // 遊戲模式群組
    QGroupBox* gameModeGroup = new QGroupBox(QString::fromUtf8("遊戲模式"), contentWidget);
    QVBoxLayout* gameModeLayout = new QVBoxLayout(gameModeGroup);
    gameModeLayout->setSpacing(10);
    
    m_humanVsHumanRadio = new QRadioButton(QString::fromUtf8("玩家對戰"), contentWidget);
    m_humanVsComputerRadio = new QRadioButton(QString::fromUtf8("與電腦對戰"), contentWidget);
    
    m_humanVsHumanRadio->setChecked(true);
    
    m_gameModeGroup->addButton(m_humanVsHumanRadio, 0);
    m_gameModeGroup->addButton(m_humanVsComputerRadio, 1);
    
    gameModeLayout->addWidget(m_humanVsHumanRadio);
    gameModeLayout->addWidget(m_humanVsComputerRadio);
    
    mainLayout->addWidget(gameModeGroup);
    
    // 電腦對戰選項（初始隱藏）
    m_computerOptionsWidget = new QWidget(contentWidget);
    QVBoxLayout* computerOptionsLayout = new QVBoxLayout(m_computerOptionsWidget);
    computerOptionsLayout->setSpacing(10);
    computerOptionsLayout->setContentsMargins(20, 10, 20, 10);
    
    // 難度選擇 - 使用滑桿
    QLabel* difficultyLabel = new QLabel(QString::fromUtf8("引擎難度等級："), contentWidget);
    computerOptionsLayout->addWidget(difficultyLabel);
    
    m_difficultySlider = new QSlider(Qt::Horizontal, contentWidget);
    m_difficultySlider->setMinimum(0);   // 最低技能
    m_difficultySlider->setMaximum(20);  // 最高技能
    m_difficultySlider->setValue(10);    // 預設中等
    m_difficultySlider->setTickPosition(QSlider::TicksBelow);
    m_difficultySlider->setTickInterval(5);
    computerOptionsLayout->addWidget(m_difficultySlider);
    
    m_difficultyValueLabel = new QLabel(contentWidget);
    m_difficultyValueLabel->setAlignment(Qt::AlignCenter);
    m_difficultyValueLabel->setStyleSheet("QLabel { font-size: 12pt; font-weight: bold; color: #2C3E50; }");
    updateDifficultyLabel();
    computerOptionsLayout->addWidget(m_difficultyValueLabel);
    
    // 難度說明標籤
    QLabel* difficultyHintLabel = new QLabel(
        QString::fromUtf8("提示：0=極簡單，10=中等，20=極困難"),
        contentWidget
    );
    difficultyHintLabel->setStyleSheet("QLabel { font-size: 9pt; color: #7F8C8D; font-style: italic; }");
    difficultyHintLabel->setAlignment(Qt::AlignCenter);
    computerOptionsLayout->addWidget(difficultyHintLabel);
    
    // 顏色選擇
    QLabel* colorLabel = new QLabel(QString::fromUtf8("您的顏色："), contentWidget);
    computerOptionsLayout->addWidget(colorLabel);
    
    m_playAsWhiteRadio = new QRadioButton(QString::fromUtf8("白方（先手）"), contentWidget);
    m_playAsBlackRadio = new QRadioButton(QString::fromUtf8("黑方（後手）"), contentWidget);
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
    connect(m_difficultySlider, &QSlider::valueChanged, this, &StartDialog::onDifficultySliderChanged);
    
    // 時間控制群組
    QGroupBox* timeGroup = new QGroupBox(QString::fromUtf8("時間控制"), contentWidget);
    QVBoxLayout* timeLayout = new QVBoxLayout(timeGroup);
    timeLayout->setSpacing(15);
    
    // 時間滑桿區段
    QLabel* timeLabel = new QLabel(QString::fromUtf8("每位玩家總時間："), contentWidget);
    timeLayout->addWidget(timeLabel);
    
    m_timeSlider = new QSlider(Qt::Horizontal, contentWidget);
    m_timeSlider->setMinimum(0);  // 0 = 無限制
    m_timeSlider->setMaximum(90);  // 0=無限制，1-30=30-59秒，31-90=1-60分鐘
    m_timeSlider->setValue(0);  // 預設為無限制
    m_timeSlider->setTickPosition(QSlider::TicksBelow);
    m_timeSlider->setTickInterval(10);
    timeLayout->addWidget(m_timeSlider);
    
    m_timeValueLabel = new QLabel(contentWidget);
    m_timeValueLabel->setAlignment(Qt::AlignCenter);
    m_timeValueLabel->setStyleSheet("QLabel { font-size: 14pt; font-weight: bold; color: #2C3E50; }");
    updateTimeLabel();
    timeLayout->addWidget(m_timeValueLabel);
    
    // 增量區段
    QLabel* incrementLabel = new QLabel(QString::fromUtf8("每著增加秒數："), contentWidget);
    timeLayout->addWidget(incrementLabel);
    
    m_incrementSlider = new QSlider(Qt::Horizontal, contentWidget);
    m_incrementSlider->setMinimum(0);
    m_incrementSlider->setMaximum(60);
    m_incrementSlider->setValue(0);
    m_incrementSlider->setTickPosition(QSlider::TicksBelow);
    m_incrementSlider->setTickInterval(5);
    timeLayout->addWidget(m_incrementSlider);
    
    m_incrementValueLabel = new QLabel(contentWidget);
    m_incrementValueLabel->setAlignment(Qt::AlignCenter);
    m_incrementValueLabel->setStyleSheet("QLabel { font-size: 12pt; color: #2C3E50; }");
    updateIncrementLabel();
    timeLayout->addWidget(m_incrementValueLabel);
    
    mainLayout->addWidget(timeGroup);
    
    // 連接訊號
    connect(m_timeSlider, &QSlider::valueChanged, this, &StartDialog::onTimeSliderChanged);
    connect(m_incrementSlider, &QSlider::valueChanged, this, &StartDialog::onIncrementSliderChanged);
    
    mainLayout->addStretch();
    
    // 設定內容小工具到滾動區域
    scrollArea->setWidget(contentWidget);
    outerLayout->addWidget(scrollArea);
    
    // 開始按鈕（固定在底部，不在滾動區域內）
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
        "    margin: 10px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #229954;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #1E8449;"
        "}"
    );
    
    connect(m_startButton, &QPushButton::clicked, this, &QDialog::accept);
    
    outerLayout->addWidget(m_startButton);
    
    setLayout(outerLayout);
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

int StartDialog::getDifficultyLevel() const
{
    // 返回 0-20 的引擎技能等級
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

void StartDialog::onDifficultySliderChanged(int /* value */)
{
    updateDifficultyLabel();
}

void StartDialog::updateDifficultyVisibility()
{
    bool isComputerMode = m_humanVsComputerRadio->isChecked();
    m_computerOptionsWidget->setVisible(isComputerMode);
}

void StartDialog::updateDifficultyLabel()
{
    int level = m_difficultySlider->value();
    QString difficultyText;
    
    if (level <= 5) {
        difficultyText = QString::fromUtf8("等級 %1 - 簡單").arg(level);
    } else if (level <= 15) {
        difficultyText = QString::fromUtf8("等級 %1 - 中等").arg(level);
    } else {
        difficultyText = QString::fromUtf8("等級 %1 - 困難").arg(level);
    }
    
    m_difficultyValueLabel->setText(difficultyText);
}
