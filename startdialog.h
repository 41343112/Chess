#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QTranslator>

class StartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartDialog(QWidget *parent = nullptr);
    ~StartDialog();

private slots:
    void onLanguageChanged(int index);

private:
    void setupUI();
    void updateTexts();
    void applyLanguage(const QString& language);
    
    QLabel* m_titleLabel;
    QLabel* m_languageLabel;
    QComboBox* m_languageComboBox;
    QPushButton* m_startButton;
    QTranslator* m_translator;
};

#endif // STARTDIALOG_H
