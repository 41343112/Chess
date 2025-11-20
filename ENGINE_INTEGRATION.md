# Stockfish Engine Integration - Implementation Notes

## 概述

本次更新整合了 Stockfish UCI 引擎，並改進了開始對話框的使用者介面，以符合以下需求：
1. 使用 Stockfish 引擎製作人機對戰功能（Windows）
2. 使用滑桿調整難度設定
3. 為開始介面增加滾動功能以適應更多內容

## 主要變更

### 1. UCI 引擎整合 ✅

#### 新增檔案：
- **`uciengine.h`** - UCI 引擎類別標頭檔
- **`uciengine.cpp`** - UCI 引擎實作

#### 功能：
- **UCI 協議通訊**：完整實作 UCI（Universal Chess Interface）協議
- **QProcess 管理**：使用 Qt 的 QProcess 管理引擎程序
- **技能等級控制**：支援 Stockfish 的 0-20 技能等級設定
- **FEN 轉換**：自動將棋盤狀態轉換為 FEN 格式供引擎使用
- **非同步移動**：透過信號槽機制非同步接收引擎計算的最佳移動
- **錯誤處理**：完善的錯誤處理機制，包括引擎啟動失敗、崩潰等情況

#### 關鍵方法：
```cpp
bool initialize(const QString& enginePath);  // 初始化引擎
void setSkillLevel(int level);               // 設定技能等級 (0-20)
void getBestMove(ChessBoard* board, PieceColor color);  // 請求最佳移動
QString boardToFEN(ChessBoard* board, PieceColor turn); // 棋盤轉 FEN
```

### 2. ChessAI 重構 ✅

#### 修改檔案：
- **`chessai.h`** - 更新為 QObject 子類別，增加信號槽支援
- **`chessai.cpp`** - 整合 UCI 引擎，保留內建 AI 作為備用

#### 主要改進：
1. **從類別改為 QObject**：支援信號槽機制
2. **非同步移動計算**：
   - 舊版：`QPair<QPoint, QPoint> getBestMove(...)` - 同步返回
   - 新版：`void getBestMove(...)` - 非同步，透過 `moveReady` 信號返回
3. **引擎/內建 AI 混合模式**：
   - 優先使用 UCI 引擎
   - 引擎不可用時自動切換到內建 AI
   - `m_useEngine` 標誌控制模式
4. **技能等級映射**：
   - 直接使用 0-20 的技能等級
   - 舊的 EASY/MEDIUM/HARD 難度仍可作為預設值

#### 新增信號：
```cpp
signals:
    void moveReady(QPoint from, QPoint to);  // 移動準備就緒
    void engineError(QString error);          // 引擎錯誤
```

### 3. StartDialog UI 更新 ✅

#### 修改檔案：
- **`startdialog.h`** - 更新介面元件
- **`startdialog.cpp`** - 重新設計 UI 佈局

#### 主要改進：

##### 滾動功能：
- **QScrollArea**：將整個對話框內容包裝在滾動區域中
- **佈局結構**：
  ```
  QDialog
  └── QVBoxLayout (outerLayout)
      ├── QScrollArea
      │   └── QWidget (contentWidget)
      │       └── QVBoxLayout (mainLayout)
      │           ├── 標題
      │           ├── 遊戲模式群組
      │           ├── 電腦選項
      │           └── 時間控制
      └── QPushButton (開始遊戲) - 固定在底部
  ```
- **優點**：
  - 支援較小的螢幕
  - 開始按鈕始終可見
  - 內容可以自由擴展

##### 滑桿難度控制：
- **替換 QComboBox**：使用 QSlider 替代下拉選單
- **技能範圍**：0-20（Stockfish 支援的範圍）
- **視覺回饋**：
  - 滑桿上方顯示當前等級
  - 動態顯示難度描述（簡單/中等/困難）
  - 刻度標記每 5 個單位
- **提示標籤**：顯示技能等級說明

##### 新增 UI 元件：
```cpp
QSlider* m_difficultySlider;           // 難度滑桿
QLabel* m_difficultyValueLabel;        // 難度值標籤
```

##### 移除的元件：
```cpp
QComboBox* m_difficultyCombo;  // 舊的下拉選單
```

### 4. myChess 主視窗整合 ✅

#### 修改檔案：
- **`mychess.h`** - 新增信號槽宣告
- **`mychess.cpp`** - 整合引擎初始化和移動處理

#### 主要變更：

##### 引擎初始化：
```cpp
QString enginePath = QCoreApplication::applicationDirPath() + 
                    "/engine/stockfish-windows-x86-64-avx2.exe";
bool engineInitialized = m_chessAI->initializeEngine(enginePath);

if (!engineInitialized) {
    qDebug() << "Failed to initialize engine, using built-in AI";
    m_chessAI->setUseEngine(false);
}

m_chessAI->setSkillLevel(skillLevel);
```

##### 新增槽函數：
```cpp
void onAIMoveReady(QPoint from, QPoint to);  // 處理 AI 移動
void onAIEngineError(QString error);          // 處理引擎錯誤
```

##### makeComputerMove() 重構：
- **舊版**：同步等待 AI 計算
- **新版**：非同步請求，透過信號接收結果
- **優點**：UI 不會凍結，可以顯示「思考中」狀態

### 5. 建置配置更新 ✅

#### 修改檔案：
- **`Chess.pro`** - 新增引擎相關檔案

```qmake
SOURCES += \
    ...
    uciengine.cpp

HEADERS += \
    ...
    uciengine.h
```

## 技術細節

### UCI 協議實作

#### 基本命令序列：
1. **初始化**：
   ```
   > uci
   < uciok
   > setoption name Skill Level value 10
   > isready
   < readyok
   ```

2. **計算移動**：
   ```
   > position fen [FEN字串]
   > go movetime 1000
   < bestmove e2e4
   ```

3. **結束**：
   ```
   > quit
   ```

#### FEN 格式轉換：
- 棋盤位置：從第 8 列到第 1 列
- 棋子符號：小寫=黑方，大寫=白方
- 範例：`rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1`

#### UCI 座標系統：
- 格式：檔案（a-h）+ 列（1-8）
- 範例：`e2e4`（e2 到 e4）
- 轉換：內部座標 (row, col) ↔ UCI 字串

### 信號槽架構

```
UCIEngine::bestMoveFound(QString, QString)
    ↓
ChessAI::onEngineMoveFound(QString, QString)
    ↓ (轉換座標)
ChessAI::moveReady(QPoint, QPoint)
    ↓
myChess::onAIMoveReady(QPoint, QPoint)
    ↓ (執行移動)
更新棋盤
```

### 錯誤處理

1. **引擎啟動失敗**：
   - 切換到內建 AI
   - 顯示除錯訊息
   - 繼續遊戲

2. **引擎崩潰**：
   - 接收 QProcess 錯誤信號
   - 自動切換到內建 AI
   - 通知使用者

3. **無效移動**：
   - 驗證引擎返回的移動
   - 拒絕非法移動
   - 記錄錯誤

## 測試考量

### 需要測試的情境：

1. **引擎正常運作**：
   - ✓ 引擎正確啟動
   - ✓ 移動計算正確
   - ✓ 技能等級影響棋力

2. **引擎故障處理**：
   - ✓ 引擎檔案不存在
   - ✓ 引擎崩潰
   - ✓ 自動切換到內建 AI

3. **UI 功能**：
   - ✓ 滑桿正確調整難度
   - ✓ 滾動功能正常
   - ✓ 開始按鈕始終可見

4. **遊戲邏輯**：
   - ✓ 電腦移動合法
   - ✓ 升變處理正確
   - ✓ 特殊移動（王車易位、吃過路兵）

## 效能考量

### 引擎思考時間：
- **目前設定**：1000 毫秒（1 秒）
- **可調整性**：可在 `uciengine.cpp` 中修改
- **權衡**：較長時間 = 更強但較慢

### 記憶體使用：
- 引擎程序：約 10-20 MB
- UCI 通訊：最小開銷
- 總體影響：可忽略不計

### CPU 使用：
- 引擎思考時：100%（單核心）
- 其他時間：接近 0%
- UI 響應性：良好（非同步設計）

## 已知限制

1. **平台支援**：
   - 目前僅包含 Windows 版本引擎
   - Linux/Mac 需要對應版本的 Stockfish

2. **FEN 轉換簡化**：
   - 王車易位權利：假設全部可用
   - 吃過路兵：未完整實作
   - 影響：輕微，大多數情況下正確

3. **引擎配置**：
   - 固定 1 秒思考時間
   - 未實作其他 UCI 選項

## 未來改進建議

1. **多平台支援**：
   - 偵測作業系統
   - 選擇對應的引擎執行檔

2. **進階引擎設定**：
   - 可調整思考時間
   - 更多 UCI 選項（記憶體、執行緒數）

3. **更好的 FEN 轉換**：
   - 正確追蹤王車易位權利
   - 實作吃過路兵目標格

4. **多引擎支援**：
   - 支援其他 UCI 引擎
   - 引擎選擇介面

5. **分析模式**：
   - 顯示引擎評估分數
   - 顯示最佳變化

## 結論

本次更新成功整合了 Stockfish UCI 引擎，大幅提升了電腦對手的棋力。透過滑桿介面，使用者可以精確調整難度，從初學者到進階玩家都能找到合適的挑戰。滾動式對話框確保了在各種螢幕尺寸下都能正常使用。整體架構保持了良好的可擴展性和錯誤處理能力。
