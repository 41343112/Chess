#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QCheckBox>

class StartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartDialog(QWidget *parent = nullptr);
    ~StartDialog();
    
    // Getters for time control settings
    bool isTimeControlEnabled() const;
    int getTimeControlSeconds() const;  // Returns -1 for unlimited time
    int getIncrementSeconds() const;

private slots:
    void onTimeControlCheckChanged(int state);
    void onUnlimitedTimeCheckChanged(int state);
    void onTimeSliderChanged(int value);
    void onIncrementSliderChanged(int value);

private:
    void setupUI();
    void updateTimeLabel();
    void updateIncrementLabel();
    
    QLabel* m_titleLabel;
    QPushButton* m_startButton;
    
    // Time control widgets
    QCheckBox* m_timeControlCheckBox;
    QCheckBox* m_unlimitedTimeCheckBox;
    QSlider* m_timeSlider;
    QLabel* m_timeValueLabel;
    QSlider* m_incrementSlider;
    QLabel* m_incrementValueLabel;
};

#endif // STARTDIALOG_H
