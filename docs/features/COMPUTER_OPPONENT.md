# 電腦對戰功能 (Computer Opponent Feature)

## 概述 (Overview)

本功能整合了世界級的 Stockfish 國際象棋引擎，玩家可以透過滑桿選擇 0-20 的技能等級與電腦對戰。

This feature integrates the world-class Stockfish chess engine, allowing players to play against the computer with skill levels from 0 to 20 adjustable via a slider.

## 功能特色 (Features)

### 遊戲模式 (Game Modes)
1. **玩家對戰** (Human vs Human) - 傳統的兩人對戰模式
2. **與電腦對戰** (Human vs Computer) - 與 Stockfish 引擎對戰

### 難度等級 (Difficulty Levels)

本遊戲使用 **Stockfish 引擎**，提供連續的技能等級調整（0-20）：

#### 等級 0-5：初學者 (Beginner)
- **策略**: Stockfish 在最低技能設定下運作
- **適合**: 剛接觸國際象棋的玩家
- **特點**: 會犯錯，給予玩家學習機會
- **估計 Elo**: 約 1000-1400

#### 等級 6-10：中等 (Intermediate)  
- **策略**: 平衡的遊戲強度
- **適合**: 有基本經驗的玩家
- **特點**: 會考慮戰術，但不會過於深入計算
- **估計 Elo**: 約 1400-1800

#### 等級 11-15：進階 (Advanced)
- **策略**: 強大的戰術計算
- **適合**: 有經驗的玩家
- **特點**: 深入計算，很少犯錯
- **估計 Elo**: 約 1800-2200

#### 等級 16-20：專家 (Expert)
- **策略**: 接近完整的 Stockfish 實力
- **適合**: 高級玩家和專家
- **特點**: 世界級水準，幾乎完美的遊戲
- **估計 Elo**: 2200+ (等級 20 可達 3000+)

### 玩家顏色選擇 (Color Selection)

在與電腦對戰模式下，玩家可以選擇:
- **白方（先手）** - 玩家先移動
- **黑方（後手）** - 電腦先移動

## 使用說明 (How to Use)

### 開始新遊戲
1. 啟動遊戲後，會顯示開始對話框
2. 選擇「與電腦對戰」
3. 使用滑桿調整難度等級（0-20）
4. 選擇您的顏色：白方（先手）或黑方（後手）
5. 設定時間控制（可選）
6. 點擊「開始遊戲」

### 遊戲進行中
- 輪到您時，可以正常移動棋子（點擊或拖曳）
- 輪到電腦時，會顯示「Computer is thinking...」
- Stockfish 引擎會根據技能等級快速計算最佳移動
- 思考時間會根據技能等級自動調整（100ms 到 1100ms）
- 所有標準功能仍然可用：悔棋、翻轉棋盤、音效等

### 限制
- 您無法在電腦回合時移動棋子
- 電腦的兵升變總是選擇后

## 技術實作 (Technical Implementation)

### StockfishEngine 類別
核心引擎整合實作在 `stockfishengine.h` 和 `stockfishengine.cpp` 中。

#### 主要方法:
```cpp
// 初始化引擎
bool initialize();

// 設定技能等級（0-20）
void setSkillLevel(int level);

// 取得最佳移動
QPair<QPoint, QPoint> getBestMove(ChessBoard* board, PieceColor aiColor);

// 關閉引擎
void shutdown();
```

#### UCI 協議通訊:
- 使用 QProcess 與 Stockfish 引擎通訊
- 實作標準 UCI（Universal Chess Interface）協議
- 命令包括：`uci`, `isready`, `position fen`, `go movetime`, `quit`

#### FEN 轉換:
```cpp
QString boardToFEN(ChessBoard* board, PieceColor currentTurn);
```
將內部棋盤狀態轉換為 FEN（Forsyth-Edwards Notation）格式，供 Stockfish 使用。

#### UCI 移動解析:
```cpp
QPair<QPoint, QPoint> uciMoveToPoints(const QString& uciMove);
```
將 Stockfish 的 UCI 移動格式（例如 "e2e4"）轉換為棋盤座標。

### 技能等級映射

Stockfish 提供兩種難度控制方式：
1. **Skill Level（0-20）**: 直接控制引擎強度
2. **UCI_LimitStrength + UCI_Elo**: 對於低技能等級（0-9），額外限制 Elo 評級

```cpp
// 等級 0-9：使用 Elo 限制
if (m_skillLevel < 10) {
    int elo = 1000 + (m_skillLevel * 100);  // 1000-1900 Elo
    sendCommand("setoption name UCI_LimitStrength value true");
    sendCommand(QString("setoption name UCI_Elo value %1").arg(elo));
}
```

### 跨平台支援

引擎自動偵測並使用適當的 Stockfish 可執行檔：
- **Windows**: `engine/stockfish-windows-x86-64-avx2.exe`
- **Linux**: `engine/stockfish-linux`
- **macOS**: `engine/stockfish-macos`

### UI 整合

#### StartDialog 擴充
- 新增遊戲模式選擇（玩家對戰 vs 電腦對戰）
- **難度滑桿**：QSlider 元件，範圍 0-20
- 即時顯示技能等級和描述性難度（初學者/中等/進階/專家）
- 顏色選擇單選按鈕（白方/黑方）

#### myChess 擴充
- `makeComputerMove()`: 執行電腦移動（使用 Stockfish）
- `m_stockfishEngine`: StockfishEngine 實例
- `m_isComputerGame`: 標記是否為電腦遊戲
- `m_computerColor`: 電腦的顏色
- `m_isComputerThinking`: 防止玩家在電腦思考時操作
- `m_skillLevel`: 當前 Stockfish 技能等級（0-20）

## 未來改進 (Future Improvements)

可能的增強功能：
- [ ] 顯示引擎評估分數
- [ ] 顯示電腦考慮的主要變化
- [ ] 可調整思考時間
- [ ] 支援 Stockfish 分析模式
- [ ] 多執行緒支援以加快計算
- [ ] 開局庫整合
- [ ] 殘局表格支援（Syzygy）
- [ ] 更智慧的兵升變選擇
- [ ] 動態難度調整

## 已知問題 (Known Issues)

- 電腦總是將兵升變為后（最佳選擇，但缺乏變化）
- 需要確保系統上有適合的 Stockfish 可執行檔
- 首次啟動引擎可能需要幾秒鐘初始化

## 測試建議 (Testing Guidelines)

### 功能測試
- [ ] 測試所有技能等級（0, 5, 10, 15, 20）
- [ ] 測試玩家為白方和黑方
- [ ] 驗證電腦無法在玩家回合時移動
- [ ] 測試與時間控制的整合
- [ ] 測試悔棋功能在電腦遊戲中的行為
- [ ] 驗證遊戲結束條件（將死、逼和）
- [ ] 測試引擎初始化失敗的錯誤處理

### 效能測試
- [ ] 測量不同技能等級的回應時間
- [ ] 驗證引擎不會造成 UI 凍結
- [ ] 測試多局遊戲的記憶體使用

### AI 品質測試
- [ ] 低等級（0-5）：確認會犯明顯錯誤
- [ ] 中等級（10）：確認會進行合理的戰術
- [ ] 高等級（20）：確認接近完美遊戲

### 跨平台測試
- [ ] Windows：測試 stockfish-windows-x86-64-avx2.exe
- [ ] Linux：測試 stockfish-linux
- [ ] macOS：測試 stockfish-macos（若可用）

## 參考資料 (References)

- Stockfish 官方網站: https://stockfishchess.org/
- Stockfish GitHub: https://github.com/official-stockfish/Stockfish
- UCI 協議說明: https://www.chessprogramming.org/UCI
- 象棋程式設計: https://www.chessprogramming.org/
## 參考資料 (References)

- Minimax 演算法: https://en.wikipedia.org/wiki/Minimax
- Alpha-Beta 剪枝: https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning
- 象棋程式設計: https://www.chessprogramming.org/
