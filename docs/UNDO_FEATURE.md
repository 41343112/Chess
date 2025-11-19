# 悔棋功能實現文檔 (Undo Feature Implementation)

## 概述 (Overview)

本文檔詳細說明國際象棋遊戲中完整悔棋功能的實現。悔棋功能允許玩家撤銷上一步移動，包括所有特殊走法。

This document details the implementation of the complete undo functionality in the chess game. The undo feature allows players to revert the last move, including all special moves.

## 功能特性 (Features)

### 支援的走法類型 (Supported Move Types)

1. **普通移動** (Normal Moves)
   - 將棋子移動到空位
   - 完全恢復原始位置和狀態

2. **吃子移動** (Capture Moves)
   - 恢復被吃掉的棋子
   - 將移動的棋子返回原位

3. **王車易位** (Castling)
   - 王翼易位 (Kingside: O-O)
   - 后翼易位 (Queenside: O-O-O)
   - 恢復國王和車的位置及移動狀態

4. **吃過路兵** (En Passant)
   - 恢復被吃掉的兵
   - 恢復正確的位置關係

5. **兵升變** (Pawn Promotion)
   - 刪除升變後的棋子（后）
   - 重新創建原始的兵
   - 如果升變時吃子，恢復被吃的棋子

## 技術實現 (Technical Implementation)

### 1. Move 結構體增強 (Enhanced Move Structure)

```cpp
struct Move {
    QPoint from;                        // 起始位置
    QPoint to;                          // 目標位置
    ChessPiece* capturedPiece;          // 被吃掉的棋子（不立即刪除）
    bool wasCastling;                   // 是否為王車易位
    bool wasEnPassant;                  // 是否為吃過路兵
    bool wasPromotion;                  // 是否為兵升變
    PieceType promotedTo;               // 升變成的棋子類型
    bool movedPieceHadMoved;            // 移動前的 hasMoved 狀態
    QPoint previousEnPassantTarget;     // 移動前的吃過路兵目標
    PieceType movedPieceType;           // 移動棋子的類型
    PieceColor movedPieceColor;         // 移動棋子的顏色
};
```

### 2. 記憶體管理策略 (Memory Management Strategy)

#### 之前的問題 (Previous Issue)
- 被吃掉的棋子立即被刪除
- 無法恢復已刪除的棋子

#### 新的解決方案 (New Solution)
- 被吃掉的棋子存儲在 Move 結構中
- 只在 `clearBoard()` 時刪除
- 悔棋時將棋子恢復到棋盤上

### 3. ChessBoard::undo() 實現流程 (Implementation Flow)

```
1. 檢查移動歷史是否為空
   ↓
2. 取出最後一步移動
   ↓
3. 重置遊戲結束狀態
   ↓
4. 切換回合（回到上一個玩家）
   ↓
5. 恢復吃過路兵目標
   ↓
6. 根據移動類型執行特定的撤銷邏輯：
   - 王車易位：恢復國王和車
   - 兵升變：刪除后，創建兵
   - 吃過路兵：恢復被吃的兵
   - 普通移動：恢復棋子位置
   ↓
7. 如果有被吃的棋子，恢復到棋盤上
   ↓
8. 更新遊戲狀態
   ↓
9. 返回成功
```

### 4. UI 更新邏輯 (UI Update Logic)

在 `myChess::onUndo()` 中：
1. 調用 `m_chessBoard->undo()`
2. 清除選擇狀態
3. 更新棋盤顯示 `updateBoard()`
4. 清除高亮 `clearHighlights()`
5. 更新回合標籤
6. 更新狀態標籤

## 使用方法 (Usage)

### 基本操作 (Basic Operation)

1. **執行移動**
   - 進行任何合法的移動

2. **點擊悔棋按鈕**
   - 撤銷最後一步移動
   - 遊戲狀態恢復到上一步

3. **多次悔棋**
   - 可以連續點擊悔棋按鈕
   - 逐步撤銷多個移動
   - 直到沒有可撤銷的移動

### 限制 (Limitations)

- 無法撤銷已經開始新遊戲的移動
- 新遊戲會清空所有移動歷史

## 測試場景 (Test Scenarios)

### 測試 1: 普通移動 (Normal Move)
```
1. 白方 e2-e4
2. 點擊悔棋
3. 驗證：兵回到 e2，hasMoved = false
```

### 測試 2: 吃子移動 (Capture Move)
```
1. 白方 Nf3
2. 黑方 d5
3. 白方 Nxd5 (馬吃兵)
4. 點擊悔棋
5. 驗證：馬回到 f3，黑兵恢復到 d5
```

### 測試 3: 王車易位 (Castling)
```
1. 設置王車易位條件
2. 白方 O-O
3. 點擊悔棋
4. 驗證：國王回到 e1，車回到 h1，兩者 hasMoved = false
```

### 測試 4: 吃過路兵 (En Passant)
```
1. 白兵 e5
2. 黑方 d7-d5
3. 白方 exd6 (吃過路兵)
4. 點擊悔棋
5. 驗證：白兵回到 e5，黑兵恢復到 d5
```

### 測試 5: 兵升變 (Pawn Promotion)
```
1. 白兵到達 e7
2. 白方 e7-e8=Q
3. 點擊悔棋
4. 驗證：后被刪除，白兵回到 e7
```

### 測試 6: 連續悔棋 (Multiple Undo)
```
1. 執行 5 步移動
2. 連續點擊悔棋 5 次
3. 驗證：回到初始狀態
```

## 記憶體安全 (Memory Safety)

### 記憶體洩漏防護 (Memory Leak Prevention)

1. **捕獲的棋子管理**
   - 存儲在 Move 結構中
   - 悔棋時恢復到棋盤
   - clearBoard() 時統一刪除

2. **升變棋子管理**
   - 悔棋時刪除升變後的棋子
   - 創建新的兵替代

3. **清理時機**
   - 新遊戲：clearBoard() 刪除所有棋子
   - 程序退出：析構函數處理清理

## 已知問題與解決方案 (Known Issues and Solutions)

### 無已知問題 (No Known Issues)

目前實現已經處理了所有常見場景和邊界情況。

## 未來改進 (Future Improvements)

1. **重做功能** (Redo)
   - 實現重做功能
   - 保存撤銷的移動

2. **移動歷史顯示** (Move History Display)
   - 顯示所有移動的記錄
   - 使用代數記譜法

3. **選擇性悔棋** (Selective Undo)
   - 點擊歷史記錄跳轉到特定狀態

## 相關文件 (Related Files)

- `chessboard.h` - Move 結構和 undo() 聲明
- `chessboard.cpp` - undo() 實現
- `mychess.cpp` - UI 集成和 onUndo() 處理
- `README.md` - 用戶文檔

## 版本歷史 (Version History)

- **v1.0** (2025-11-19): 完整實現悔棋功能
  - 支援所有走法類型
  - 完整的記憶體管理
  - UI 集成
