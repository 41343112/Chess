# Stockfish Engine Integration

## Overview

The Chess application now integrates the Stockfish chess engine, providing professional-level computer opponents for Hard difficulty. Stockfish is one of the world's strongest open-source chess engines.

## Features

### Automatic Activation
- **Hard Difficulty**: Automatically uses Stockfish engine when "Human vs Computer" mode is selected with "Hard" difficulty
- **Easy and Medium Difficulties**: Continue to use built-in AI algorithms (random moves and basic evaluation)

### UCI Protocol Support
- Uses standard UCI (Universal Chess Interface) protocol to communicate with Stockfish
- Supports complete chess rules, including:
  - Castling
  - En passant
  - Pawn promotion
  - Check, checkmate, and stalemate detection

### Cross-Platform Support
- **Windows**: Auto-detects `engine/stockfish-windows-x86-64-avx2.exe`
- **Linux**: Auto-detects engine in application directory or system paths (`/usr/games/stockfish`, `/usr/bin/stockfish`)
- **macOS**: Auto-detects engine in application directory or Homebrew paths (`/usr/local/bin/stockfish`, `/opt/homebrew/bin/stockfish`)

### Fallback Mechanism
- If Stockfish engine fails to start or respond, system automatically falls back to built-in Minimax algorithm
- Ensures game experience is not interrupted by engine issues

## Technical Implementation

### New Class: StockfishEngine

**Files**: `stockfishengine.h`, `stockfishengine.cpp`

**Main Functions**:
```cpp
bool initialize(const QString& enginePath);  // Initialize engine
void setPosition(const QString& fen);        // Set board position (FEN format)
void setPositionWithMoves(const QVector<QString>& moves);  // Set position using move list
QString getBestMove(int thinkingTimeMs);     // Get best move
void setSkillLevel(int level);               // Set skill level (0-20)
```

**Features**:
- Uses `QProcess` to manage Stockfish process
- Asynchronous engine output handling
- Supports UCI commands: `uci`, `isready`, `position`, `go`, `setoption`
- Automatically parses engine responses

### ChessAI Class Updates

**New Methods**:
```cpp
void setUseStockfish(bool use);              // Enable/disable Stockfish
bool isUsingStockfish() const;               // Check if using Stockfish
```

**Helper Functions**:
```cpp
QString moveToUci(QPoint from, QPoint to) const;           // Convert board coordinates to UCI format
QPair<QPoint, QPoint> uciToMove(const QString& uci) const; // Convert UCI format to board coordinates
QString boardToFen(ChessBoard* board, PieceColor currentTurn) const;  // Generate FEN string
QVector<QString> getMovesInUci(ChessBoard* board) const;   // Get move history (UCI format)
```

### Integration Point

**File**: `mychess.cpp`

When player selects "Hard" difficulty in start dialog:
```cpp
m_chessAI = new ChessAI(AIDifficulty::HARD);
m_chessAI->setUseStockfish(true);  // Enable Stockfish
```

## Usage Instructions

### How to Play

1. **Launch Game**: Run the Chess application
2. **Select Game Mode**: Choose "Human vs Computer" in start dialog
3. **Set Difficulty**:
   - **Easy**: Random moves (does not use Stockfish)
   - **Medium**: Basic evaluation moves (does not use Stockfish)
   - **Hard**: Uses Stockfish engine (professional level)
4. **Choose Color**: Select white or black pieces
5. **Start Game**: Click "Start Game" button

### Stockfish Thinking Time

Automatically adjusted based on difficulty:
- **Easy**: 100ms (not actually used, as Easy uses random algorithm)
- **Medium**: 500ms (not actually used, as Medium uses basic evaluation)
- **Hard**: 2000ms (2 seconds)

### Skill Level

Stockfish supports skill levels 0-20:
- **Easy**: Skill level 5
- **Medium**: Skill level 10
- **Hard**: Skill level 20 (strongest)

## Troubleshooting

### Stockfish Engine Not Found

**Symptoms**: After selecting Hard difficulty, computer moves at same speed as Medium difficulty

**Cause**: Stockfish engine file does not exist or path is incorrect

**Solution**:
1. Confirm `engine/` directory exists in application root directory
2. Confirm Stockfish executable exists:
   - Windows: `engine/stockfish-windows-x86-64-avx2.exe`
   - Linux: `engine/stockfish-linux-x86-64`
   - macOS: `engine/stockfish-mac`
3. Check file permissions (Linux/macOS need execute permission):
   ```bash
   chmod +x engine/stockfish-linux-x86-64
   ```

### Engine Startup Failed

**View Logs**:
Application outputs Stockfish-related debug messages to console:
```
Starting Stockfish engine: /path/to/stockfish
Stockfish engine initialized successfully
```

If failed, shows:
```
Failed to initialize Stockfish engine
```

### Automatic Fallback to Minimax

If Stockfish cannot work properly, system automatically uses built-in Minimax algorithm:
```
Stockfish engine not ready, falling back to Minimax
```

This ensures the game can continue without interruption from engine issues.

## Performance Considerations

### Memory Usage
- Stockfish is a separate process and does not affect main application's memory usage
- Typical memory usage: approximately 20-50 MB (depends on search depth)

### CPU Usage
- Stockfish uses 100% of a single CPU core while thinking
- Thinking time limited to 2 seconds (Hard difficulty), no long waiting times

### Response Time
- Average response time: 1-2 seconds (Hard difficulty)
- Includes additional overhead for UCI communication and move parsing

## Future Enhancements

Possible improvements:
- [ ] Allow users to customize Stockfish thinking time
- [ ] Add "Super Hard" difficulty (longer thinking time)
- [ ] Support multi-threaded search (using Stockfish's Threads option)
- [ ] Add engine analysis feature (show evaluation score and best lines)
- [ ] Support other UCI engines (like Leela Chess Zero)
- [ ] Add opening book support
- [ ] Display engine thinking process (depth, nodes, etc.)

## Technical References

### UCI Protocol
- [UCI Protocol Specification](https://www.shredderchess.com/chess-features/uci-universal-chess-interface.html)
- [Stockfish UCI Commands](https://github.com/official-stockfish/Stockfish/wiki/UCI-&-Commands)

### Stockfish
- [Stockfish Official Website](https://stockfishchess.org/)
- [Stockfish GitHub Repository](https://github.com/official-stockfish/Stockfish)

### FEN Format
- [FEN - Forsyth-Edwards Notation](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation)

## Contributors

- Stockfish Integration Implementation: GitHub Copilot Agent
- Testing and Validation: 41343112

## License

- Stockfish Engine: GNU General Public License v3.0
- Chess Application Integration Code: Same license as main project
