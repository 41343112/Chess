# Chess Game - Like Chess.com

A complete chess game implementation in Qt/C++ with a graphical user interface, similar to Chess.com.

## 🎉 New Features
- **Flip Board Button**: Swap board orientation to view from either player's perspective! ⭐ NEW
- **Move Sound Effects**: Audio feedback for moves and captures ⭐ NEW
- **Drag-and-Drop Movement**: Move pieces by dragging them with your mouse!
- **King Check Highlighting**: King's square turns red when under attack
- **Enhanced Visual Feedback**: Clear visual indicators for all game states
- **Square Grid Maintained**: Chess board keeps perfect squares when resizing window

See [NEW_FEATURES.md](NEW_FEATURES.md) and [FEATURE_SUMMARY.md](FEATURE_SUMMARY.md) for detailed documentation of the new features.

## 📚 Documentation
- **[GAMEPLAY.md](GAMEPLAY.md)** - Complete gameplay guide with rules, controls, and strategies
- [NEW_FEATURES.md](NEW_FEATURES.md) - Recent feature additions and implementation details
- [UI_MOCKUP.md](UI_MOCKUP.md) - User interface design and mockups

## Table of Contents
- [Features](#features)
- [Architecture](#architecture)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Building and Running](#building-and-running)
  - [Troubleshooting](#troubleshooting)
- [How to Play](#how-to-play)
- [Game Status Messages](#game-status-messages)
- [Technical Implementation Details](#technical-implementation-details)
- [Future Enhancements](#future-enhancements)

## Features

### Complete Chess Rules Implementation
- ✅ All piece movements (Pawn, Rook, Knight, Bishop, Queen, King)
- ✅ Special moves:
  - Castling (kingside and queenside)
  - En passant capture
  - Pawn promotion to Queen
- ✅ Check detection
- ✅ Checkmate detection
- ✅ Stalemate detection
- ✅ Move validation (prevents illegal moves)
- ✅ Turn-based gameplay (White/Black alternating)

### User Interface
- ✅ 8x8 chessboard with alternating light and dark squares
- ✅ Unicode chess piece symbols (♔ ♕ ♖ ♗ ♘ ♙)
- ✅ Visual feedback:
  - Selected piece highlighting (gold)
  - Valid move highlighting (green)
  - **King in check highlighting (red)** ⭐ NEW
  - Turn indicator
  - Game status display
- ✅ Click-to-select, click-to-move interface
- ✅ **Drag-and-drop piece movement** ⭐ NEW
- ✅ **Flip board button** - View from either player's perspective ⭐ NEW
- ✅ **Move sound effects** - Audio feedback for moves and captures ⭐ NEW
- ✅ New Game button
- ✅ Game over dialog

## Architecture

### Class Structure

**ChessPiece** (Abstract Base Class)
- Base class for all chess pieces
- Defines common properties: type, color, position, movement status
- Pure virtual method `isValidMove()` for piece-specific move validation

**Piece Subclasses**
- `Pawn` - Forward movement, diagonal capture, en passant, promotion
- `Rook` - Horizontal and vertical movement
- `Knight` - L-shaped movement
- `Bishop` - Diagonal movement
- `Queen` - Combined rook and bishop movement
- `King` - One square in any direction, castling

**ChessBoard**
- Manages 8x8 board state
- Handles move execution and validation
- Tracks game state (turn, check, checkmate, stalemate)
- Maintains move history
- Implements special move logic (castling, en passant)

**myChess (Main Window)**
- Qt-based GUI
- ChessSquare buttons for board interaction
- Game controls (New Game, Undo)
- Status and turn displays

**ChessSquare**
- Custom QPushButton for each board square
- Visual states: normal, selected, highlighted
- Displays piece symbols

## Getting Started

### Quick Start Guide

**For experienced Qt developers:**
```bash
git clone https://github.com/41343112/Chess.git
cd Chess
qmake Chess.pro
make
./Chess
```

**For beginners:** Follow the detailed instructions below.

### Prerequisites

Before building the chess game, you need to install the following:

#### 1. Qt Framework (Qt 5.12+ or Qt 6.x)

**Linux (Ubuntu/Debian):**
```bash
# For Qt 5
sudo apt-get update
sudo apt-get install qt5-default qtbase5-dev qttools5-dev-tools

# For Qt 6
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-tools-dev
```

**macOS:**
```bash
# Using Homebrew
brew install qt@5
# or
brew install qt@6

# Add Qt to PATH (add to ~/.zshrc or ~/.bash_profile)
export PATH="/opt/homebrew/opt/qt@5/bin:$PATH"  # for Qt 5
# or
export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"  # for Qt 6
```

**Windows:**
1. Download Qt Online Installer from [qt.io/download](https://www.qt.io/download-qt-installer)
2. Run the installer and select:
   - Qt 5.15.x or Qt 6.x
   - MinGW compiler (or MSVC if you have Visual Studio)
   - Qt Creator IDE
3. Complete the installation

#### 2. C++ Compiler

**Linux:**
```bash
# GCC (usually pre-installed)
sudo apt-get install build-essential
g++ --version  # Verify installation
```

**macOS:**
```bash
# Xcode Command Line Tools
xcode-select --install
```

**Windows:**
- MinGW comes with Qt installer, or
- Install Visual Studio 2019/2022 with C++ workload

#### 3. Verify Installation

Check that Qt tools are available:
```bash
qmake --version
```

Expected output should show Qt version 5.x or 6.x.

### Building and Running

#### Method 1: Command Line (Quick Start)

**Step 1: Clone or Download the Repository**
```bash
# If using git
git clone https://github.com/41343112/Chess.git
cd Chess

# Or download and extract ZIP, then navigate to the directory
```

**Step 2: Build the Project**
```bash
# Generate Makefile
qmake Chess.pro

# Compile the project
make

# On Windows with MinGW, use:
# mingw32-make
```

**Step 3: Run the Application**
```bash
# Linux/macOS
./Chess

# Windows
Chess.exe
# or double-click Chess.exe in the build directory
```

#### Method 2: Qt Creator (Recommended for Beginners)

**Step 1: Open the Project**
1. Launch Qt Creator
2. Click "File" → "Open File or Project"
3. Navigate to the Chess directory
4. Select `Chess.pro` and click "Open"

**Step 2: Configure Project**
1. Qt Creator will show "Configure Project" screen
2. Select a Qt kit (e.g., "Desktop Qt 5.15.2 MinGW 64-bit")
3. Click "Configure Project"

**Step 3: Build and Run**
1. Click the green "Run" button (▶) in the bottom-left corner, or
2. Press `Ctrl+R` (Windows/Linux) or `Cmd+R` (macOS)
3. The chess game window will appear

### Troubleshooting

**Issue: "qmake: command not found"**
- Solution: Ensure Qt bin directory is in your PATH
- Linux/macOS: Add Qt path to ~/.bashrc or ~/.zshrc
- Windows: Add Qt bin directory to System Environment Variables

**Issue: "Project ERROR: Unknown module(s) in QT: widgets"**
- Solution: Install Qt widgets module:
  ```bash
  # Linux
  sudo apt-get install qtbase5-dev
  ```

**Issue: Build fails with C++17 errors**
- Solution: Ensure your compiler supports C++17
- Update GCC to version 7+ or use Clang 5+

**Issue: Application doesn't start on Windows**
- Solution: Copy Qt DLLs to the executable directory:
  ```bash
  # Navigate to Qt bin directory and copy required DLLs
  # For MinGW Qt 5.15:
  Qt5Core.dll, Qt5Gui.dll, Qt5Widgets.dll
  # Plus platform plugin: platforms/qwindows.dll
  ```
  Or run from Qt Creator which handles this automatically.

**Issue: Black screen or UI issues**
- Solution: Update graphics drivers
- Try running with software rendering: `export QT_QPA_PLATFORM=offscreen`

### What You'll See

Once the application is running, you'll see:

**Game Board:**
- 8×8 chessboard with alternating light (cream #F0D9B5) and dark (brown #B58863) squares
- Unicode chess pieces: ♔ ♕ ♖ ♗ ♘ ♙ (white) and ♚ ♛ ♜ ♝ ♞ ♟ (black)
- Clean, professional appearance similar to Chess.com

**Interactive Elements:**
- Click any piece to select it (highlighted in gold)
- Valid moves are highlighted in light green
- Turn indicator shows "Turn: White" or "Turn: Black"
- Status bar displays game state (check, checkmate, stalemate)
- "New Game" button to restart
- "Undo" button (placeholder for future functionality)

**Example Game State:**
```
Initial position with standard chess setup:
Rank 8: ♜ ♞ ♝ ♛ ♚ ♝ ♞ ♜
Rank 7: ♟ ♟ ♟ ♟ ♟ ♟ ♟ ♟
Rank 6-3: Empty squares
Rank 2: ♙ ♙ ♙ ♙ ♙ ♙ ♙ ♙
Rank 1: ♖ ♘ ♗ ♕ ♔ ♗ ♘ ♖
```

For detailed UI mockups, see [UI_MOCKUP.md](UI_MOCKUP.md).

## How to Play

**For a complete gameplay guide with detailed rules, strategies, and tips, see [GAMEPLAY.md](GAMEPLAY.md).**

### Quick Start

1. **Starting a Game**
   - White moves first
   - Select a piece by clicking on it OR by starting to drag it
   - Valid moves will be highlighted in green
   - Move the piece by clicking on a highlighted square OR by dragging and dropping

2. **Making Moves - Two Methods:**
   - **Click-to-Move**: Click a piece to select it, then click the destination square
   - **Drag-and-Drop**: Click and hold a piece, drag it to the destination, then release ⭐ NEW
   - Both methods work seamlessly together!

3. **Visual Indicators**
   - **Gold highlighting**: Selected piece
   - **Green highlighting**: Valid move destinations
   - **Red highlighting**: King is in check ⭐ NEW
   - Only pieces of the current player can be moved
   - The game prevents illegal moves that would put your king in check
   - **Sound feedback**: Hear distinct sounds for regular moves and captures ⭐ NEW

4. **Special Moves**
   - **Castling**: Click on your king, then click two squares toward the rook (or drag)
   - **En Passant**: Available when opponent's pawn moves two squares forward
   - **Pawn Promotion**: Pawns automatically promote to Queens upon reaching the opposite end

5. **Winning**
   - Checkmate: Opponent's king is in check with no legal moves
   - Stalemate: Current player has no legal moves but is not in check (Draw)

6. **Game Controls**
   - **New Game**: Start a fresh game
   - **Undo**: (Note: Feature placeholder in current version)
   - **Flip Board**: Rotate the board 180° to view from the other player's perspective ⭐ NEW

## Game Status Messages

- "Game in progress" - Normal play
- "White is in check!" / "Black is in check!" - King under attack
- "White wins by checkmate!" / "Black wins by checkmate!" - Game over
- "Stalemate - Draw!" - Game ends in a draw

## Technical Implementation Details

### Move Validation
- Two-stage validation:
  1. Piece-specific rules (each piece class)
  2. Board-level checks (wouldn't put king in check)

### Check Detection
- Scans all opponent pieces to see if they can attack the king
- Prevents moves that would leave own king in check

### Checkmate Detection
- King is in check
- No legal moves available to escape check

### Stalemate Detection
- King is not in check
- No legal moves available

## Future Enhancements

Possible additions for future versions:
- Full undo/redo functionality
- Move history display with algebraic notation
- Timer/clock for timed games
- Save/load game state
- Piece promotion choice dialog (currently auto-promotes to Queen)
- Volume control for sound effects
- Additional sound effects (check, checkmate, castling)
- Sound on/off toggle
- Animated board flip transition
- Opening book and game analysis
- Online multiplayer
- AI opponent

## License

This project is open source and available for educational purposes.
