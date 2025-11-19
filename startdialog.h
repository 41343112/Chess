#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>

class StartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartDialog(QWidget *parent = nullptr);
    ~StartDialog();

private:
    void setupUI();
    
    QLabel* m_titleLabel;
    QPushButton* m_startButton;
};

#endif // STARTDIALOG_H
