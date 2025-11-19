# Game Settings Flow Implementation

## 功能說明 (Feature Description)

實現了以下需求：
> 當開啟新遊戲時跳出設定時間畫面，設定是否開啟悔棋，當遊戲開啟後不能調整設定直到遊戲結束。

Translation:
> When opening a new game, show a time setting screen and configure whether to enable undo. Settings cannot be adjusted during an active game until it ends.

## 功能流程 (Feature Flow)

### 1. 應用程式啟動 (Application Start)
```
應用程式啟動
    ↓
設定按鈕：啟用 ✓
遊戲狀態：未開始
    ↓
用戶可以瀏覽介面
```

### 2. 開始新遊戲 (Start New Game)
```
用戶點擊「New Game」按鈕
    ↓
顯示設定對話框
    ├─ 時間限制設定 (Time Limit)
    ├─ 悔棋開關 (Undo Enable/Disable)
    ├─ 棋盤顏色 (Board Colors)
    ├─ 音量控制 (Volume)
    └─ 語言選擇 (Language)
    ↓
用戶配置設定
    ↓
用戶點擊「OK」接受設定
    ↓
設定被保存並應用
    ↓
棋盤重置
    ↓
遊戲開始
    ↓
設定按鈕：停用 ✗
遊戲狀態：進行中
```

### 3. 遊戲進行中 (During Game)
```
遊戲進行中
    ↓
設定按鈕：停用 ✗（灰色顯示）
    ↓
如果用戶嘗試點擊設定按鈕
    ↓
顯示訊息：「設定在遊戲進行中無法變更」
    ↓
繼續遊戲
```

### 4. 遊戲結束 (Game Ends)
```
發生將死或逼和
    ↓
遊戲結束
    ↓
顯示遊戲結束對話框
    ↓
設定按鈕：啟用 ✓（重新啟用）
遊戲狀態：已結束
    ↓
用戶可以：
    ├─ 點擊「New Game」開始新遊戲（重新配置設定）
    └─ 點擊「Settings」調整設定（不開始遊戲）
```

## 技術實現 (Technical Implementation)

### 新增的代碼 (Added Code)

#### 1. 在 `mychess.h` 中新增狀態追蹤
```cpp
// Track if game has started
bool m_gameInProgress;
```

#### 2. 在建構函式中初始化
```cpp
myChess::myChess(QWidget *parent)
    : QMainWindow(parent)
    // ... other initializations ...
    , m_gameInProgress(false)  // 遊戲初始為未開始狀態
{
    // ...
}
```

#### 3. 修改 `onNewGame()` 方法
```cpp
void myChess::onNewGame() {
    // 先顯示設定對話框
    SettingsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        // 載入並應用新設定
        loadSettings();
        applySettings();
        
        // 重置棋盤並開始遊戲
        m_chessBoard->reset();
        m_hasSelection = false;
        clearHighlights();
        updateBoard();
        
        // 標記遊戲進行中並停用設定按鈕
        m_gameInProgress = true;
        m_settingsButton->setEnabled(false);
        
        // 啟動計時器（如果有設定時間限制）
        if (m_timeLimitMinutes > 0) {
            startMoveTimer();
        }
    }
}
```

#### 4. 修改 `showGameOverDialog()` 方法
```cpp
void myChess::showGameOverDialog() {
    // 遊戲結束，重新啟用設定按鈕
    m_gameInProgress = false;
    m_settingsButton->setEnabled(true);
    
    // 顯示遊戲結束對話框
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Game Over"));
    msgBox.setText(m_chessBoard->getGameStatus());
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}
```

#### 5. 在 `onSettings()` 中新增檢查
```cpp
void myChess::onSettings() {
    // 防止在遊戲進行中變更設定
    if (m_gameInProgress) {
        QMessageBox::information(this, tr("Settings Locked"),
            tr("Settings cannot be changed during an active game. "
               "Please finish or start a new game."));
        return;
    }
    
    // 顯示設定對話框
    SettingsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        // ...
    }
}
```

## 狀態轉換圖 (State Transition Diagram)

```
   [應用程式啟動]
         |
         v
    [遊戲未開始]
    Settings: ✓
         |
         | (點擊 New Game)
         v
   [顯示設定對話框]
         |
         | (點擊 OK)
         v
    [遊戲進行中]
    Settings: ✗
         |
         | (將死/逼和)
         v
    [遊戲結束]
    Settings: ✓
         |
         +----------+
         |          |
    (New Game) (Settings)
         |          |
         v          v
   [設定對話框] [設定對話框]
```

## 使用者體驗改進 (User Experience Improvements)

### Before (之前)
1. 點擊「New Game」→ 確認對話框 → 遊戲開始
2. 隨時可以變更設定（包括遊戲進行中）
3. 可能導致不公平（例如中途開啟悔棋）

### After (之後)
1. 點擊「New Game」→ 設定對話框 → 配置選項 → 遊戲開始
2. 遊戲開始後設定被鎖定
3. 遊戲結束後可以變更設定
4. 確保公平性（設定在遊戲開始前確定）

## 測試場景 (Test Scenarios)

### 場景 1：正常新遊戲流程
1. 啟動應用程式
2. 點擊「New Game」
3. 在設定對話框中配置選項
4. 點擊「OK」
5. ✓ 驗證：遊戲開始，設定按鈕停用

### 場景 2：取消新遊戲
1. 點擊「New Game」
2. 在設定對話框中配置選項
3. 點擊「Cancel」
4. ✓ 驗證：遊戲不開始，保持在原狀態

### 場景 3：遊戲中嘗試變更設定
1. 開始新遊戲
2. 遊戲進行中
3. 嘗試點擊「Settings」按鈕
4. ✓ 驗證：按鈕停用或顯示警告訊息

### 場景 4：遊戲結束後變更設定
1. 完成一局遊戲（將死或逼和）
2. 點擊遊戲結束對話框的「OK」
3. 點擊「Settings」
4. ✓ 驗證：可以正常開啟設定對話框並變更

### 場景 5：連續新遊戲
1. 完成一局遊戲
2. 點擊「New Game」
3. 重新配置設定
4. 開始新遊戲
5. ✓ 驗證：每次新遊戲都可以重新配置設定

## 向後相容性 (Backward Compatibility)

✓ 保持所有現有功能
✓ 設定持久化仍正常運作
✓ 悔棋、計時器、音效、顏色等功能不受影響
✓ 無破壞性變更

## 程式碼統計 (Code Statistics)

- 檔案修改：2 個 (`mychess.h`, `mychess.cpp`)
- 新增行數：~20 行
- 修改行數：~7 行
- 總變更：27 行
- 新增成員變數：1 個 (`m_gameInProgress`)

## 結論 (Conclusion)

這個實現完全符合需求：
1. ✓ 開啟新遊戲時顯示設定畫面
2. ✓ 可以設定時間限制和悔棋選項
3. ✓ 遊戲開始後無法調整設定
4. ✓ 遊戲結束後可以重新調整設定

實現方式簡潔、最小化修改、並保持向後相容性。
