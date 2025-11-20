#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QComboBox>
#include <QRadioButton>
#include <QButtonGroup>

enum class GameMode {
    HUMAN_VS_HUMAN,
    HUMAN_VS_COMPUTER
};

enum class ComputerDifficulty {
    EASY,
    MEDIUM,
    HARD
};

class StartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartDialog(QWidget *parent = nullptr);
    ~StartDialog();
    
    // Getters for time control settings
    int getTimeControlSeconds() const;  // Returns time in seconds (0 = no limit, 30-59 seconds, or 1-60 minutes in seconds)
    int getIncrementSeconds() const;
    
    // Getters for game mode and difficulty
    GameMode getGameMode() const;
    int getDifficulty() const;  // Returns skill level 0-20
    bool isPlayerWhite() const;  // In computer mode, is player white or black

private slots:
    void onTimeSliderChanged(int value);
    void onIncrementSliderChanged(int value);
    void onGameModeChanged();
    void onDifficultySliderChanged(int value);

private:
    void setupUI();
    void updateTimeLabel();
    void updateIncrementLabel();
    void updateDifficultyVisibility();
    void updateDifficultyLabel();
    
    QLabel* m_titleLabel;
    QPushButton* m_startButton;
    
    // Time control widgets
    QSlider* m_timeSlider;
    QLabel* m_timeValueLabel;
    QSlider* m_incrementSlider;
    QLabel* m_incrementValueLabel;
    
    // Game mode widgets
    QRadioButton* m_humanVsHumanRadio;
    QRadioButton* m_humanVsComputerRadio;
    QButtonGroup* m_gameModeGroup;
    
    // Computer game widgets
    QWidget* m_computerOptionsWidget;
    QSlider* m_difficultySlider;
    QLabel* m_difficultyValueLabel;
    QRadioButton* m_playAsWhiteRadio;
    QRadioButton* m_playAsBlackRadio;
    QButtonGroup* m_colorGroup;
};

#endif // STARTDIALOG_H
