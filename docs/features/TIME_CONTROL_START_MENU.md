# Time Control in Start Menu - Implementation Guide

## 概述 / Overview

根據問題陳述的要求，已實現以下功能：
According to the problem statement, the following features have been implemented:

1. ✅ **將時間設定放在開始選單** - Time settings placed in start menu
2. ✅ **使用滑桿條調整時間** - Use slider to adjust time
3. ✅ **1~60秒，1分鐘以後為分鐘為單位** - 1-60 seconds, then minutes after 1 minute
4. ✅ **新增每下一著新增秒數** - Add increment seconds per move
5. ✅ **當秒數小於10秒要顯示小數點後2位** - Display 2 decimal places when < 10 seconds

## 功能說明 / Feature Description

### 1. 開始選單時間控制 / Start Menu Time Control

開始遊戲時，會顯示一個對話框，包含時間控制設定：
When starting a game, a dialog shows with time control settings:

```
┌─────────────────────────────────────┐
│     Welcome to Chess Game           │
│                                      │
│  ┌─ Time Control ─────────────────┐ │
│  │ ☐ Enable Time Control          │ │
│  │                                 │ │
│  │ Total Time per Player:          │ │
│  │ [━━━━━━●━━━━━━━━━━━━━━━━━━]    │ │
│  │        10 seconds                │ │
│  │                                 │ │
│  │ Increment per move (seconds): 0 │ │
│  └─────────────────────────────────┘ │
│                                      │
│        [  Start Game  ]              │
└─────────────────────────────────────┘
```

### 2. 滑桿範圍 / Slider Range

滑桿值 1-120 對應：
Slider values 1-120 correspond to:

| 滑桿值 | 時間 | 說明 |
|--------|------|------|
| 1-60   | 1-60 秒 | 直接對應秒數 |
| 61     | 1 分鐘 | 60 秒 |
| 70     | 10 分鐘 | 600 秒 |
| 80     | 20 分鐘 | 1200 秒 |
| 120    | 60 分鐘 | 3600 秒 |

**範例 / Examples:**
- 滑桿 = 10 → 顯示 "10 seconds"
- 滑桿 = 30 → 顯示 "30 seconds"
- 滑桿 = 61 → 顯示 "1 minutes"
- 滑桿 = 70 → 顯示 "10 minutes"

### 3. 每步增加時間 / Increment per Move

- 可設定 0-60 秒
- 每次成功走棋後，會自動增加設定的秒數到剛走棋的一方
- 適用於費雪制（Fischer）時間控制

**範例 / Example:**
```
初始時間：10秒，增加量：2秒
Initial: 10s, Increment: 2s

白方走棋 → 白方時間 +2秒
White moves → White gets +2s

黑方走棋 → 黑方時間 +2秒
Black moves → Black gets +2s
```

### 4. 時間顯示格式 / Time Display Format

根據剩餘時間自動選擇格式：
Format automatically chosen based on remaining time:

| 剩餘時間 | 顯示格式 | 範例 |
|----------|----------|------|
| ≥ 10 秒 | MM:SS | 00:15, 01:30, 10:00 |
| < 10 秒 | S.CC | 9.90, 5.00, 1.23, 0.10 |

**說明 / Description:**
- `MM` = 分鐘 (minutes, 2位數)
- `SS` = 秒數 (seconds, 2位數)
- `S` = 秒數 (seconds, 無前導零)
- `CC` = 百分之一秒 (centiseconds, 2位數)

**實際顯示範例 / Display Examples:**
```
15.00 秒 → 顯示 "00:15"
10.00 秒 → 顯示 "00:10"
9.90 秒  → 顯示 "9.90"
5.00 秒  → 顯示 "5.00"
1.23 秒  → 顯示 "1.23"
0.10 秒  → 顯示 "0.10"
```

## 技術實現 / Technical Implementation

### 時間精度 / Time Precision

- **內部儲存**: 毫秒 (milliseconds)
- **計時器間隔**: 100ms (0.1秒)
- **顯示精度**: 
  - < 10秒：百分之一秒 (0.01s)
  - ≥ 10秒：秒 (1s)

### 核心函數 / Core Functions

#### 1. 時間格式化 / Time Formatting
```cpp
QString formatTime(int milliseconds) {
    int totalSeconds = milliseconds / 1000;
    int minutes = totalSeconds / 60;
    int secs = totalSeconds % 60;
    int centiseconds = (milliseconds % 1000) / 10;
    
    if (totalSeconds < 10) {
        return QString("%1.%2").arg(secs)
                               .arg(centiseconds, 2, 10, QChar('0'));
    } else {
        return QString("%1:%2").arg(minutes, 2, 10, QChar('0'))
                               .arg(secs, 2, 10, QChar('0'));
    }
}
```

#### 2. 增加時間 / Add Increment
```cpp
void addIncrement() {
    if (m_incrementSeconds > 0 && m_timeControlEnabled) {
        int incrementMs = m_incrementSeconds * 1000;
        // 增加時間到剛走棋的一方
        // Add time to the player who just moved
        if (m_chessBoard->getCurrentTurn() == PieceColor::WHITE) {
            m_blackTimeRemaining += incrementMs;
        } else {
            m_whiteTimeRemaining += incrementMs;
        }
        updateTimeDisplay();
    }
}
```

#### 3. 計時器更新 / Timer Update
```cpp
void onTimerTick() {
    // 每100ms減少當前玩家的時間
    // Decrease current player's time by 100ms
    if (m_chessBoard->getCurrentTurn() == PieceColor::WHITE) {
        m_whiteTimeRemaining -= 100;
        if (m_whiteTimeRemaining <= 0) {
            // 白方超時
            // White runs out of time
            showTimeoutDialog();
        }
    } else {
        m_blackTimeRemaining -= 100;
        if (m_blackTimeRemaining <= 0) {
            // 黑方超時
            // Black runs out of time
            showTimeoutDialog();
        }
    }
    updateTimeDisplay();
}
```

## 使用流程 / Usage Flow

### 開始新遊戲 / Starting a New Game

1. **啟動應用程式** / Launch application
   - 自動顯示開始選單

2. **設定時間控制** / Configure time control
   - ✅ 勾選 "Enable Time Control"
   - 🎚️ 調整滑桿選擇時間（1-60秒 或 1-60分鐘）
   - ⏱️ 設定每步增加秒數（0-60秒）

3. **開始遊戲** / Start game
   - 點擊 "Start Game"
   - 計時器開始倒數

### 遊戲進行中 / During Game

```
┌─────────────────────────────────────┐
│ White: 9.87    Turn: White   Black: 10:00 │
├─────────────────────────────────────┤
│                                      │
│         [Chess Board]                │
│                                      │
└─────────────────────────────────────┘
```

- 白方時間 < 10秒 → 顯示小數點
- 黑方時間 ≥ 10秒 → 顯示 MM:SS 格式
- 每次走棋後自動增加設定的秒數

## 測試結果 / Test Results

### 單元測試 / Unit Tests

✅ **時間格式化測試** - Time Format Tests
```
✓ 9.9 seconds: 9.90
✓ 9.5 seconds: 9.50
✓ 5 seconds: 5.00
✓ 1.234 seconds: 1.23
✓ 0.1 seconds: 0.10
✓ 10 seconds: 00:10
✓ 15 seconds: 00:15
✓ 1 minute: 01:00
✓ 1:30: 01:30
✓ 10 minutes: 10:00
```

✅ **滑桿範圍測試** - Slider Range Tests
```
✓ 1 second: slider=1 -> 1s
✓ 30 seconds: slider=30 -> 30s
✓ 60 seconds: slider=60 -> 60s
✓ 1 minute: slider=61 -> 60s
✓ 10 minutes: slider=70 -> 600s
✓ 20 minutes: slider=80 -> 1200s
✓ 60 minutes: slider=120 -> 3600s
```

## 檔案變更 / Files Changed

1. **startdialog.h/cpp**
   - 新增時間控制 UI 元件
   - 滑桿、勾選框、增加量輸入框

2. **mychess.h/cpp**
   - 更新計時器邏輯（100ms 間隔）
   - 時間儲存改為毫秒
   - 新增 addIncrement() 函數
   - 更新 formatTime() 顯示小數點

3. **chess_zh_CN.ts**
   - 新增中文翻譯

## 相容性 / Compatibility

- ✅ 與現有設定對話框相容
- ✅ 支援英文/中文介面
- ✅ 編譯無錯誤或警告
- ✅ 向後相容（未啟用時無影響）

## 注意事項 / Notes

1. **計時器精度**: 使用 100ms 間隔以提供流暢的小數顯示
2. **增加時間**: 只在成功走棋後才增加時間
3. **遊戲結束**: 時間歸零時遊戲立即結束
4. **歷史瀏覽**: 瀏覽歷史棋步時計時器會暫停
