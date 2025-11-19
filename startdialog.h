#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>

class StartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartDialog(QWidget *parent = nullptr);
    ~StartDialog();
    
    // Getters for time control settings
    int getTimeControlSeconds() const;  // Returns time in seconds (0 = no limit, 30-59 seconds, or 1-60 minutes in seconds)
    int getIncrementSeconds() const;

private slots:
    void onTimeSliderChanged(int value);
    void onIncrementSliderChanged(int value);

private:
    void setupUI();
    void updateTimeLabel();
    void updateIncrementLabel();
    
    QLabel* m_titleLabel;
    QPushButton* m_startButton;
    
    // Time control widgets
    QSlider* m_timeSlider;
    QLabel* m_timeValueLabel;
    QSlider* m_incrementSlider;
    QLabel* m_incrementValueLabel;
};

#endif // STARTDIALOG_H
