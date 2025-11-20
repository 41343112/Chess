# 電腦對戰功能 (Computer Opponent Feature)

## 概述 (Overview)

本功能新增了與電腦對戰的能力，玩家可以選擇三種不同的難度等級。

This feature adds the ability to play against a computer opponent with three different difficulty levels.

## 功能特色 (Features)

### 遊戲模式 (Game Modes)
1. **玩家對戰** (Human vs Human) - 傳統的兩人對戰模式
2. **與電腦對戰** (Human vs Computer) - 與 AI 對手對戰

### 難度等級 (Difficulty Levels)

#### 簡單 (Easy)
- **策略**: 隨機選擇合法移動
- **適合**: 初學者，剛接觸國際象棋的玩家
- **特點**: 完全隨機，不考慮棋局優勢

#### 中等 (Medium)  
- **策略**: 基於棋子價值評估和位置判斷
- **評估因素**:
  - 棋子材質價值 (兵=100, 馬=320, 象=330, 車=500, 后=900, 王=20000)
  - 中心控制獎勵 (+30 分)
  - 優先吃子
  - 考慮將軍狀態
- **適合**: 有基本經驗的玩家
- **特點**: 會尋找吃子機會和控制中心

#### 困難 (Hard)
- **策略**: Minimax 演算法搭配 Alpha-Beta 剪枝
- **搜尋深度**: 3 層 (3-ply)
- **評估函數**: 綜合考慮材質、位置、將軍/將死狀態
- **適合**: 有經驗的玩家
- **特點**: 能預測多步，選擇最優策略

### 玩家顏色選擇 (Color Selection)

在與電腦對戰模式下，玩家可以選擇:
- **白方（先手）** - 玩家先移動
- **黑方（後手）** - 電腦先移動

## 使用說明 (How to Use)

### 開始新遊戲
1. 啟動遊戲後，會顯示開始對話框
2. 選擇「與電腦對戰」
3. 選擇難度：簡單、中等、或困難
4. 選擇您的顏色：白方（先手）或黑方（後手）
5. 設定時間控制（可選）
6. 點擊「開始遊戲」

### 遊戲進行中
- 輪到您時，可以正常移動棋子（點擊或拖曳）
- 輪到電腦時，會顯示「Computer is thinking...」
- 電腦會在 0.5 秒後自動移動
- 所有標準功能仍然可用：悔棋、翻轉棋盤、音效等

### 限制
- 您無法在電腦回合時移動棋子
- 電腦的兵升變總是選擇后

## 技術實作 (Technical Implementation)

### ChessAI 類別
核心 AI 邏輯實作在 `chessai.h` 和 `chessai.cpp` 中。

#### 主要方法:
```cpp
// 取得最佳移動
QPair<QPoint, QPoint> getBestMove(ChessBoard* board, PieceColor aiColor);

// 各難度策略
QPair<QPoint, QPoint> getRandomMove(...);        // 簡單
QPair<QPoint, QPoint> getBasicEvaluationMove(...); // 中等  
QPair<QPoint, QPoint> getMinimaxMove(...);       // 困難
```

#### 評估函數:
```cpp
int evaluateBoard(ChessBoard* board, PieceColor aiColor);
```
考慮因素：
- 材質平衡
- 位置優勢（中心控制）
- 將軍狀態獎勵/懲罰

#### Minimax 演算法:
```cpp
int minimax(ChessBoard* board, int depth, int alpha, int beta, 
            bool maximizingPlayer, PieceColor aiColor);
```
- 使用 alpha-beta 剪枝優化
- 深度 3 層搜尋
- 考慮將死和逼和

### UI 整合

#### StartDialog 擴充
- 新增遊戲模式選擇
- 難度下拉選單
- 顏色選擇單選按鈕

#### myChess 擴充
- `makeComputerMove()`: 執行電腦移動
- `m_isComputerGame`: 標記是否為電腦遊戲
- `m_computerColor`: 電腦的顏色
- `m_isComputerThinking`: 防止玩家在電腦思考時操作

## 未來改進 (Future Improvements)

可能的增強功能：
- [ ] 更高難度（深度 4-5 層）
- [ ] 開局庫支援
- [ ] 殘局表格
- [ ] 可調整搜尋深度
- [ ] 位置評估表（Piece-Square Tables）
- [ ] 更智慧的兵升變選擇
- [ ] 顯示電腦的思考過程
- [ ] 多執行緒搜尋以支援更深層次

## 已知問題 (Known Issues)

- 困難難度可能在複雜局面下需要幾秒鐘思考時間
- 電腦總是將兵升變為后（最佳選擇，但缺乏變化）

## 測試建議 (Testing Guidelines)

### 功能測試
- [ ] 測試所有三種難度等級
- [ ] 測試玩家為白方和黑方
- [ ] 驗證電腦無法在玩家回合時移動
- [ ] 測試與時間控制的整合
- [ ] 測試悔棋功能在電腦遊戲中的行為
- [ ] 驗證遊戲結束條件（將死、逼和）

### 效能測試
- [ ] 測量簡單難度的回應時間（應 < 0.1 秒）
- [ ] 測量中等難度的回應時間（應 < 0.5 秒）
- [ ] 測量困難難度的回應時間（應 < 3 秒）

### AI 品質測試
- [ ] 簡單：確認移動是隨機的
- [ ] 中等：確認會優先吃子
- [ ] 困難：確認能避免明顯的失誤

## 參考資料 (References)

- Minimax 演算法: https://en.wikipedia.org/wiki/Minimax
- Alpha-Beta 剪枝: https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning
- 象棋程式設計: https://www.chessprogramming.org/
