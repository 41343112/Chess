#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QColorDialog>
#include <QDialogButtonBox>
#include <QMessageBox>

const QColor SettingsDialog::DEFAULT_LIGHT_COLOR = QColor("#F0D9B5");
const QColor SettingsDialog::DEFAULT_DARK_COLOR = QColor("#B58863");

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent),
      m_lightSquareColor(DEFAULT_LIGHT_COLOR),
      m_darkSquareColor(DEFAULT_DARK_COLOR),
      m_language("en")
{
    setWindowTitle(tr("Settings"));
    setModal(true);
    setupUI();
    loadSettings();
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);

    // Undo functionality group
    QGroupBox* undoGroup = new QGroupBox(tr("Undo Functionality"), this);
    QVBoxLayout* undoLayout = new QVBoxLayout(undoGroup);
    m_undoEnabledCheckBox = new QCheckBox(tr("Enable Undo Button"), this);
    m_undoEnabledCheckBox->setChecked(true);
    undoLayout->addWidget(m_undoEnabledCheckBox);
    mainLayout->addWidget(undoGroup);

    // Chessboard colors group
    QGroupBox* colorGroup = new QGroupBox(tr("Chessboard Colors"), this);
    QFormLayout* colorLayout = new QFormLayout(colorGroup);
    
    m_lightSquareColorButton = new QPushButton(tr("Light Square Color"), this);
    m_lightSquareColorButton->setMinimumHeight(35);
    updateColorButtonStyle(m_lightSquareColorButton, m_lightSquareColor);
    connect(m_lightSquareColorButton, &QPushButton::clicked, this, &SettingsDialog::onLightColorButtonClicked);
    
    m_darkSquareColorButton = new QPushButton(tr("Dark Square Color"), this);
    m_darkSquareColorButton->setMinimumHeight(35);
    updateColorButtonStyle(m_darkSquareColorButton, m_darkSquareColor);
    connect(m_darkSquareColorButton, &QPushButton::clicked, this, &SettingsDialog::onDarkColorButtonClicked);
    
    m_resetColorsButton = new QPushButton(tr("Reset to Default Colors"), this);
    connect(m_resetColorsButton, &QPushButton::clicked, this, &SettingsDialog::onResetColorsClicked);
    
    colorLayout->addRow(tr("Light Squares:"), m_lightSquareColorButton);
    colorLayout->addRow(tr("Dark Squares:"), m_darkSquareColorButton);
    colorLayout->addRow(m_resetColorsButton);
    mainLayout->addWidget(colorGroup);

    // Volume control group
    QGroupBox* volumeGroup = new QGroupBox(tr("Sound Volume"), this);
    QVBoxLayout* volumeLayout = new QVBoxLayout(volumeGroup);
    
    QHBoxLayout* volumeSliderLayout = new QHBoxLayout();
    QLabel* volumeLabel = new QLabel(tr("Volume:"), this);
    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setMinimum(0);
    m_volumeSlider->setMaximum(100);
    m_volumeSlider->setValue(100);
    m_volumeSlider->setTickPosition(QSlider::TicksBelow);
    m_volumeSlider->setTickInterval(10);
    
    QLabel* volumeValueLabel = new QLabel("100%", this);
    connect(m_volumeSlider, &QSlider::valueChanged, [volumeValueLabel](int value) {
        volumeValueLabel->setText(QString("%1%").arg(value));
    });
    
    volumeSliderLayout->addWidget(volumeLabel);
    volumeSliderLayout->addWidget(m_volumeSlider, 1);
    volumeSliderLayout->addWidget(volumeValueLabel);
    volumeLayout->addLayout(volumeSliderLayout);
    mainLayout->addWidget(volumeGroup);

    // Time limit group
    QGroupBox* timeGroup = new QGroupBox(tr("Time Limit"), this);
    QFormLayout* timeLayout = new QFormLayout(timeGroup);
    
    m_timeLimitSpinBox = new QSpinBox(this);
    m_timeLimitSpinBox->setMinimum(0);
    m_timeLimitSpinBox->setMaximum(60);
    m_timeLimitSpinBox->setValue(0);
    m_timeLimitSpinBox->setSuffix(tr(" minutes"));
    m_timeLimitSpinBox->setSpecialValueText(tr("No limit"));
    
    QLabel* timeHelpLabel = new QLabel(tr("Set time limit per move (0 = no limit)"), this);
    timeHelpLabel->setStyleSheet("QLabel { color: gray; font-size: 10pt; }");
    
    timeLayout->addRow(tr("Minutes per move:"), m_timeLimitSpinBox);
    timeLayout->addRow(timeHelpLabel);
    mainLayout->addWidget(timeGroup);

    // Language selection group
    QGroupBox* languageGroup = new QGroupBox(tr("Language"), this);
    QFormLayout* languageLayout = new QFormLayout(languageGroup);
    
    m_languageComboBox = new QComboBox(this);
    m_languageComboBox->addItem("English", "en");
    m_languageComboBox->addItem("中文", "zh");
    
    languageLayout->addRow(tr("Select Language:"), m_languageComboBox);
    
    QLabel* languageHelpLabel = new QLabel(tr("Restart required for language changes to take effect"), this);
    languageHelpLabel->setStyleSheet("QLabel { color: gray; font-size: 10pt; }");
    languageLayout->addRow(languageHelpLabel);
    
    mainLayout->addWidget(languageGroup);

    // Reset to defaults button
    m_resetDefaultsButton = new QPushButton(tr("Reset All Settings to Default"), this);
    m_resetDefaultsButton->setStyleSheet("QPushButton { background-color: #FFE4B5; }");
    connect(m_resetDefaultsButton, &QPushButton::clicked, this, &SettingsDialog::onResetDefaultsClicked);
    mainLayout->addWidget(m_resetDefaultsButton);

    // Dialog buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onOkClicked);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SettingsDialog::onCancelClicked);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
    setMinimumWidth(400);
}

void SettingsDialog::updateColorButtonStyle(QPushButton* button, const QColor& color)
{
    QString style = QString(
        "QPushButton {"
        "    background-color: %1;"
        "    border: 2px solid #555;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "    color: %2;"
        "}"
        "QPushButton:hover {"
        "    border: 2px solid #888;"
        "}"
    ).arg(color.name())
     .arg(color.lightness() > 128 ? "black" : "white");
    
    button->setStyleSheet(style);
}

void SettingsDialog::onLightColorButtonClicked()
{
    QColor color = QColorDialog::getColor(m_lightSquareColor, this, tr("Choose Light Square Color"));
    if (color.isValid()) {
        m_lightSquareColor = color;
        updateColorButtonStyle(m_lightSquareColorButton, m_lightSquareColor);
    }
}

void SettingsDialog::onDarkColorButtonClicked()
{
    QColor color = QColorDialog::getColor(m_darkSquareColor, this, tr("Choose Dark Square Color"));
    if (color.isValid()) {
        m_darkSquareColor = color;
        updateColorButtonStyle(m_darkSquareColorButton, m_darkSquareColor);
    }
}

void SettingsDialog::onResetColorsClicked()
{
    m_lightSquareColor = DEFAULT_LIGHT_COLOR;
    m_darkSquareColor = DEFAULT_DARK_COLOR;
    updateColorButtonStyle(m_lightSquareColorButton, m_lightSquareColor);
    updateColorButtonStyle(m_darkSquareColorButton, m_darkSquareColor);
}

void SettingsDialog::onResetDefaultsClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Reset to Defaults"), 
        tr("Are you sure you want to reset all settings to their default values?"),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // Reset all settings to defaults
        m_undoEnabledCheckBox->setChecked(true);
        m_lightSquareColor = DEFAULT_LIGHT_COLOR;
        m_darkSquareColor = DEFAULT_DARK_COLOR;
        updateColorButtonStyle(m_lightSquareColorButton, m_lightSquareColor);
        updateColorButtonStyle(m_darkSquareColorButton, m_darkSquareColor);
        m_volumeSlider->setValue(100);
        m_timeLimitSpinBox->setValue(0);
        m_languageComboBox->setCurrentIndex(0); // English
    }
}

void SettingsDialog::onOkClicked()
{
    saveSettings();
    accept();
}

void SettingsDialog::onCancelClicked()
{
    reject();
}

bool SettingsDialog::isUndoEnabled() const
{
    return m_undoEnabledCheckBox->isChecked();
}

QColor SettingsDialog::getLightSquareColor() const
{
    return m_lightSquareColor;
}

QColor SettingsDialog::getDarkSquareColor() const
{
    return m_darkSquareColor;
}

int SettingsDialog::getVolume() const
{
    return m_volumeSlider->value();
}

int SettingsDialog::getTimeLimitMinutes() const
{
    return m_timeLimitSpinBox->value();
}

QString SettingsDialog::getLanguage() const
{
    return m_languageComboBox->currentData().toString();
}

void SettingsDialog::loadSettings()
{
    QSettings settings("ChessGame", "Settings");
    
    m_undoEnabledCheckBox->setChecked(settings.value("undoEnabled", true).toBool());
    
    m_lightSquareColor = settings.value("lightSquareColor", DEFAULT_LIGHT_COLOR).value<QColor>();
    m_darkSquareColor = settings.value("darkSquareColor", DEFAULT_DARK_COLOR).value<QColor>();
    updateColorButtonStyle(m_lightSquareColorButton, m_lightSquareColor);
    updateColorButtonStyle(m_darkSquareColorButton, m_darkSquareColor);
    
    m_volumeSlider->setValue(settings.value("volume", 100).toInt());
    m_timeLimitSpinBox->setValue(settings.value("timeLimitMinutes", 0).toInt());
    
    // Load language preference
    QString language = settings.value("language", "en").toString();
    int index = m_languageComboBox->findData(language);
    if (index >= 0) {
        m_languageComboBox->setCurrentIndex(index);
    }
}

void SettingsDialog::saveSettings()
{
    QSettings settings("ChessGame", "Settings");
    
    settings.setValue("undoEnabled", m_undoEnabledCheckBox->isChecked());
    settings.setValue("lightSquareColor", m_lightSquareColor);
    settings.setValue("darkSquareColor", m_darkSquareColor);
    settings.setValue("volume", m_volumeSlider->value());
    settings.setValue("timeLimitMinutes", m_timeLimitSpinBox->value());
    settings.setValue("language", m_languageComboBox->currentData().toString());
}
