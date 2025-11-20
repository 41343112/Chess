# Stockfish 引擎整合

## 概述

Chess 應用程式現已整合 Stockfish 國際象棋引擎，為困難難度提供專業級的電腦對手。Stockfish 是世界上最強大的開源國際象棋引擎之一。

## 功能特點

### 自動啟用
- **困難難度**：當選擇「與電腦對戰」模式並設定為「困難」難度時，自動使用 Stockfish 引擎
- **簡單和中等難度**：繼續使用內建的 AI 演算法（隨機移動和基本評估）

### UCI 協定支援
- 使用標準的 UCI（Universal Chess Interface）協定與 Stockfish 通訊
- 支援完整的國際象棋規則，包括：
  - 王車易位
  - 吃過路兵
  - 兵升變
  - 將軍、將死、逼和檢測

### 跨平台支援
- **Windows**：自動偵測 `engine/stockfish-windows-x86-64-avx2.exe`
- **Linux**：自動偵測應用程式目錄中的引擎或系統路徑（`/usr/games/stockfish`, `/usr/bin/stockfish`）
- **macOS**：自動偵測應用程式目錄中的引擎或 Homebrew 路徑（`/usr/local/bin/stockfish`, `/opt/homebrew/bin/stockfish`）

### 容錯機制
- 如果 Stockfish 引擎無法啟動或回應，系統會自動切換回內建的 Minimax 演算法
- 確保遊戲體驗不會因引擎問題而中斷

## 技術實作

### 新增類別：StockfishEngine

**檔案**：`stockfishengine.h`, `stockfishengine.cpp`

**主要功能**：
```cpp
bool initialize(const QString& enginePath);  // 初始化引擎
void setPosition(const QString& fen);        // 設定棋盤位置（FEN格式）
void setPositionWithMoves(const QVector<QString>& moves);  // 使用移動列表設定位置
QString getBestMove(int thinkingTimeMs);     // 取得最佳移動
void setSkillLevel(int level);               // 設定技能等級（0-20）
```

**特點**：
- 使用 `QProcess` 管理 Stockfish 程序
- 非同步處理引擎輸出
- 支援 UCI 命令：`uci`, `isready`, `position`, `go`, `setoption`
- 自動解析引擎回應

### ChessAI 類別更新

**新增方法**：
```cpp
void setUseStockfish(bool use);              // 啟用/停用 Stockfish
bool isUsingStockfish() const;               // 檢查是否使用 Stockfish
```

**輔助函數**：
```cpp
QString moveToUci(QPoint from, QPoint to) const;           // 將棋盤座標轉換為 UCI 格式
QPair<QPoint, QPoint> uciToMove(const QString& uci) const; // 將 UCI 格式轉換為棋盤座標
QString boardToFen(ChessBoard* board, PieceColor currentTurn) const;  // 生成 FEN 字串
QVector<QString> getMovesInUci(ChessBoard* board) const;   // 取得移動歷史（UCI 格式）
```

### 整合點

**檔案**：`mychess.cpp`

當玩家在開始對話框中選擇「困難」難度時：
```cpp
m_chessAI = new ChessAI(AIDifficulty::HARD);
m_chessAI->setUseStockfish(true);  // 啟用 Stockfish
```

## 使用說明

### 遊玩方式

1. **啟動遊戲**：執行 Chess 應用程式
2. **選擇遊戲模式**：在開始對話框中選擇「與電腦對戰」
3. **設定難度**：
   - **簡單**：隨機移動（不使用 Stockfish）
   - **中等**：基本評估移動（不使用 Stockfish）
   - **困難**：使用 Stockfish 引擎（專業級）
4. **選擇顏色**：選擇白方或黑方
5. **開始遊戲**：點擊「開始遊戲」按鈕

### Stockfish 思考時間

根據難度自動調整：
- **簡單**：100 毫秒（實際上不會用到，因為簡單難度使用隨機演算法）
- **中等**：500 毫秒（實際上不會用到，因為中等難度使用基本評估）
- **困難**：2000 毫秒（2 秒）

### 技能等級

Stockfish 支援 0-20 的技能等級：
- **簡單**：技能等級 5
- **中等**：技能等級 10
- **困難**：技能等級 20（最強）

## 故障排除

### Stockfish 引擎未找到

**症狀**：選擇困難難度後，電腦移動速度與中等難度相同

**原因**：Stockfish 引擎檔案不存在或路徑不正確

**解決方法**：
1. 確認 `engine/` 目錄存在於應用程式根目錄
2. 確認 Stockfish 可執行檔存在：
   - Windows: `engine/stockfish-windows-x86-64-avx2.exe`
   - Linux: `engine/stockfish-linux-x86-64`
   - macOS: `engine/stockfish-mac`
3. 檢查檔案權限（Linux/macOS 需要執行權限）：
   ```bash
   chmod +x engine/stockfish-linux-x86-64
   ```

### 引擎啟動失敗

**查看日誌**：
應用程式會在控制台輸出 Stockfish 相關的調試訊息：
```
Starting Stockfish engine: /path/to/stockfish
Stockfish engine initialized successfully
```

如果失敗，會顯示：
```
Failed to initialize Stockfish engine
```

### 自動切換回 Minimax

如果 Stockfish 無法正常工作，系統會自動使用內建的 Minimax 演算法：
```
Stockfish engine not ready, falling back to Minimax
```

這確保遊戲可以繼續進行，不會因引擎問題而中斷。

## 性能考量

### 記憶體使用
- Stockfish 是一個獨立的程序，不會影響主應用程式的記憶體使用
- 典型記憶體使用：約 20-50 MB（取決於搜尋深度）

### CPU 使用
- Stockfish 在思考時會使用 100% 的單核心 CPU
- 思考時間限制為 2 秒（困難難度），不會造成長時間等待

### 回應時間
- 平均回應時間：1-2 秒（困難難度）
- 包含 UCI 通訊和移動解析的額外開銷

## 未來改進

可能的增強功能：
- [ ] 允許使用者自訂 Stockfish 思考時間
- [ ] 添加「超級困難」難度（更長的思考時間）
- [ ] 支援多執行緒搜尋（使用 Stockfish 的 Threads 選項）
- [ ] 添加引擎分析功能（顯示評估分數和最佳線路）
- [ ] 支援其他 UCI 引擎（如 Leela Chess Zero）
- [ ] 添加開局庫支援
- [ ] 顯示引擎思考過程（深度、節點數等）

## 技術參考

### UCI 協定
- [UCI Protocol Specification](https://www.shredderchess.com/chess-features/uci-universal-chess-interface.html)
- [Stockfish UCI Commands](https://github.com/official-stockfish/Stockfish/wiki/UCI-&-Commands)

### Stockfish
- [Stockfish Official Website](https://stockfishchess.org/)
- [Stockfish GitHub Repository](https://github.com/official-stockfish/Stockfish)

### FEN 格式
- [FEN - Forsyth-Edwards Notation](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation)

## 貢獻者

- Stockfish 整合實作：GitHub Copilot Agent
- 測試與驗證：41343112

## 授權

- Stockfish 引擎：GNU General Public License v3.0
- Chess 應用程式整合程式碼：與主專案相同的授權
