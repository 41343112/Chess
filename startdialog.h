#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QCheckBox>
#include <QColor>

class StartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartDialog(QWidget *parent = nullptr);
    ~StartDialog();
    
    // Getters for time control settings
    bool isTimeControlEnabled() const;
    int getTimeControlSeconds() const;
    
    // Getters for board color settings
    QColor getLightSquareColor() const;
    QColor getDarkSquareColor() const;

private slots:
    void onTimeControlCheckChanged(int state);
    void onTimeSliderChanged(int value);
    void onLightColorButtonClicked();
    void onDarkColorButtonClicked();

private:
    void setupUI();
    void updateTimeLabel();
    void updateColorButtonStyle(QPushButton* button, const QColor& color);
    
    QLabel* m_titleLabel;
    QPushButton* m_startButton;
    
    // Time control widgets
    QCheckBox* m_timeControlCheckBox;
    QSlider* m_timeSlider;
    QLabel* m_timeValueLabel;
    
    // Board color widgets
    QPushButton* m_lightSquareColorButton;
    QPushButton* m_darkSquareColorButton;
    QColor m_lightSquareColor;
    QColor m_darkSquareColor;
    
    // Default colors
    static const QColor DEFAULT_LIGHT_COLOR;
    static const QColor DEFAULT_DARK_COLOR;
};

#endif // STARTDIALOG_H
