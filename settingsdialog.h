#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QColor>
#include <QSettings>
#include <QComboBox>
#include <QTranslator>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    // Getters for settings
    bool isUndoEnabled() const;
    QColor getLightSquareColor() const;
    QColor getDarkSquareColor() const;

    // Load/Save settings
    void loadSettings();
    void saveSettings();

private slots:
    void onLightColorButtonClicked();
    void onDarkColorButtonClicked();
    void onResetColorsClicked();
    void onResetDefaultsClicked();
    void onOkClicked();
    void onCancelClicked();

private:
    void setupUI();
    void updateColorButtonStyle(QPushButton* button, const QColor& color);

    QCheckBox* m_undoEnabledCheckBox;
    QPushButton* m_lightSquareColorButton;
    QPushButton* m_darkSquareColorButton;
    QPushButton* m_resetColorsButton;
    QPushButton* m_resetDefaultsButton;
    
    QColor m_lightSquareColor;
    QColor m_darkSquareColor;

    // Default colors
    static const QColor DEFAULT_LIGHT_COLOR;
    static const QColor DEFAULT_DARK_COLOR;
};

#endif // SETTINGSDIALOG_H
