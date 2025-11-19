# 🌐 Language Switcher - Quick Start Guide

## For Users

### How to Use

When you start the Chess game, you'll see the Start Dialog with a language selector:

```
┌─────────────────────────────────┐
│                                 │
│   Welcome to Chess Game         │
│                                 │
│   Language:  [English ▼]        │
│                                 │
│   [      Start Game      ]      │
│                                 │
└─────────────────────────────────┘
```

### Steps:

1. **Select Your Language**
   - Click on the language dropdown
   - Choose between:
     - English (Default)
     - 中文 (Chinese)

2. **See Instant Changes**
   - The dialog updates immediately
   - All text changes to your selected language
   - No need to restart the application!

3. **Start Playing**
   - Click "Start Game" button
   - Your language choice is saved automatically
   - Next time you open the game, it will remember your preference

### Example - Switching to Chinese:

**Before (English):**
```
Window Title: Chess Game
Title: Welcome to Chess Game
Label: Language:
Button: Start Game
```

**After (Chinese):**
```
Window Title: 國際象棋遊戲
Title: 歡迎來到國際象棋遊戲
Label: 語言：
Button: 開始遊戲
```

## For Developers

### Quick Integration

The language switcher is fully integrated with the existing translation system:

```cpp
// Language is saved to QSettings
QSettings settings("ChessGame", "Settings");
QString language = settings.value("language", "en").toString();

// Translation is applied dynamically
QTranslator translator;
if (language == "zh") {
    translator.load(":/translations/chess_zh_CN.qm");
    QApplication::installTranslator(&translator);
}
```

### Adding New Languages

To add support for new languages:

1. Create a new translation file:
   ```bash
   lupdate Chess.pro
   ```

2. Translate strings in Qt Linguist

3. Compile translation:
   ```bash
   lrelease new_language.ts -qm new_language.qm
   ```

4. Add to resources.qrc:
   ```xml
   <file>new_language.qm</file>
   ```

5. Update StartDialog:
   ```cpp
   m_languageComboBox->addItem("New Language", "new_code");
   ```

### Key Files

- **startdialog.h/cpp** - Language switcher implementation
- **chess_zh_CN.ts** - Chinese translation source
- **chess_zh_CN.qm** - Compiled Chinese translation
- **resources.qrc** - Resource file containing translations

## Technical Details

### Supported Languages

| Language | Code | Translation File | Status |
|----------|------|-----------------|--------|
| English  | en   | (built-in)      | ✅ Ready |
| Chinese  | zh   | chess_zh_CN.qm  | ✅ Ready |

### Settings Storage

- **Organization**: ChessGame
- **Application**: Settings
- **Key**: language
- **Type**: QString
- **Values**: "en", "zh"
- **Default**: "en"

### Translation Coverage

**StartDialog strings:**
- ✅ Window title
- ✅ Welcome message
- ✅ Language label
- ✅ Start button

**Other dialogs:**
- ✅ Settings dialog
- ✅ Main game window
- ✅ All game messages

## Troubleshooting

### Language doesn't change?
- Make sure the .qm file is compiled
- Check resources.qrc includes the translation file
- Verify the translation file path is correct

### Translation missing?
- Run `lupdate` to update .ts files
- Translate missing strings in Qt Linguist
- Run `lrelease` to compile .qm files

### Settings not saving?
- Check QSettings organization and application names match
- Verify write permissions for settings storage

## Notes

- Language changes in StartDialog are **immediate** (no restart needed)
- Language changes in Settings dialog require app restart (as documented)
- The StartDialog language setting is synchronized with Settings dialog
- All changes are automatically saved

## Summary

The language switcher provides:
- ✅ Easy language selection at startup
- ✅ Instant visual feedback
- ✅ Persistent preferences
- ✅ Seamless integration with existing translations
- ✅ No breaking changes to existing functionality

For detailed technical documentation, see [START_DIALOG_LANGUAGE_FEATURE.md](START_DIALOG_LANGUAGE_FEATURE.md).
