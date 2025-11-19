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
    int getTimeControlSeconds() const;
    int getIncrementSeconds() const;

private slots:
    void onTimeControlCheckChanged(int state);
    void onTimeSliderChanged(int value);

private:
    void setupUI();
    void updateTimeLabel();
    
    QLabel* m_titleLabel;
    QPushButton* m_startButton;
    
    // Time control widgets
    QCheckBox* m_timeControlCheckBox;
    QSlider* m_timeSlider;
    QLabel* m_timeValueLabel;
    QSpinBox* m_incrementSpinBox;
};

#endif // STARTDIALOG_H
