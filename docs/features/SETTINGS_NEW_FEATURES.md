# 設定對話框新功能 / Settings Dialog New Features

## 中文說明 (Chinese Description)

### 新增功能

#### 1. 語言選擇 (Language Selection)
- **位置**: 設定對話框中的「語言」區段
- **選項**: 
  - English (英文)
  - 中文 (Chinese)
- **使用方式**:
  1. 點擊「Settings」按鈕開啟設定對話框
  2. 在「語言」區段選擇想要的語言
  3. 點擊「OK」儲存設定
  4. 系統會提示「需要重新啟動應用程式才能使語言變更生效」
  5. 重新啟動應用程式後，界面將顯示選擇的語言

#### 2. 恢復預設值按鈕 (Reset to Defaults)
- **位置**: 設定對話框底部，對話框按鈕上方
- **按鈕名稱**: 「Reset All Settings to Default」（重置所有設定為預設值）
- **背景顏色**: 淺黃色 (#FFE4B5) 便於識別
- **使用方式**:
  1. 點擊「Reset All Settings to Default」按鈕
  2. 系統會顯示確認對話框：「確定要將所有設定重置為預設值嗎？」
  3. 點擊「Yes」確認重置
  4. 所有設定將立即恢復為預設值：
     - 悔棋功能：啟用
     - 淺色方格顏色：#F0D9B5 (米色)
     - 深色方格顏色：#B58863 (棕色)
     - 音量：100%
     - 每步時間限制：0 分鐘 (無限制)
     - 語言：English (英文)

---

## English Description

### New Features

#### 1. Language Selection
- **Location**: "Language" section in Settings dialog
- **Options**: 
  - English
  - 中文 (Chinese)
- **How to use**:
  1. Click the "Settings" button to open the settings dialog
  2. Select your preferred language in the "Language" section
  3. Click "OK" to save settings
  4. The system will show a message: "Please restart the application for language changes to take effect"
  5. After restarting the application, the interface will display in the selected language

#### 2. Reset to Defaults Button
- **Location**: Bottom of settings dialog, above the dialog buttons
- **Button Name**: "Reset All Settings to Default"
- **Background Color**: Light yellow (#FFE4B5) for easy identification
- **How to use**:
  1. Click the "Reset All Settings to Default" button
  2. A confirmation dialog will appear: "Are you sure you want to reset all settings to their default values?"
  3. Click "Yes" to confirm the reset
  4. All settings will immediately revert to their default values:
     - Undo enabled: true
     - Light square color: #F0D9B5 (beige)
     - Dark square color: #B58863 (brown)
     - Volume: 100%
     - Time limit per move: 0 minutes (no limit)
     - Language: English

---

## Settings Dialog Layout (設定對話框布局)

```
┌─────────────────────────────────────────────┐
│          Settings (設定)                     │
├─────────────────────────────────────────────┤
│                                              │
│  [Undo Functionality]                        │
│  ☑ Enable Undo Button                       │
│                                              │
│  [Chessboard Colors]                         │
│  Light Squares: [Color Button]              │
│  Dark Squares:  [Color Button]              │
│  [Reset to Default Colors]                   │
│                                              │
│  [Sound Volume]                              │
│  Volume: ━━━━●━━━━━ 100%                    │
│                                              │
│  [Time Limit]                                │
│  Minutes per move: [0] minutes              │
│  Set time limit per move (0 = no limit)     │
│                                              │
│  [Language] ⭐ NEW                           │
│  Select Language: [English ▼]               │
│  Restart required for language changes      │
│                                              │
│  [Reset All Settings to Default] ⭐ NEW     │
│                                              │
│                    [OK] [Cancel]             │
└─────────────────────────────────────────────┘
```

## Technical Notes (技術說明)

### Default Values (預設值)
- Undo enabled: `true`
- Light square color: `#F0D9B5`
- Dark square color: `#B58863`
- Volume: `100`
- Time limit: `0` (no limit)
- Language: `"en"` (English)

### Storage (儲存)
- All settings are stored using `QSettings`
- Organization: "ChessGame"
- Application: "Settings"
- Language key: "language"

### Supported Languages (支援的語言)
1. **English** (英文) - code: `"en"`
2. **中文** (Chinese) - code: `"zh"`

## Benefits (優點)

### Language Selection (語言選擇)
✅ 支援多語言用戶 / Support multilingual users
✅ 改善用戶體驗 / Improve user experience
✅ 易於擴展更多語言 / Easy to extend to more languages

### Reset to Defaults (恢復預設值)
✅ 快速修復設定錯誤 / Quickly fix configuration errors
✅ 恢復初始狀態 / Restore initial state
✅ 方便測試不同設定 / Convenient for testing different settings
✅ 包含確認對話框防止誤操作 / Includes confirmation dialog to prevent accidental resets
