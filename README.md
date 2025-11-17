# Chess Game - Like Chess.com

A complete chess game implementation in Qt/C++ with a graphical user interface, similar to Chess.com.

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
  - Selected piece highlighting
  - Valid move highlighting
  - Turn indicator
  - Game status display
- ✅ Click-to-select, click-to-move interface
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

## Building and Running

### Requirements
- Qt 5 or Qt 6 (with widgets module)
- C++17 compatible compiler
- qmake or CMake

### Build with qmake
```bash
qmake Chess.pro
make
./Chess
```

### Build with Qt Creator
1. Open `Chess.pro` in Qt Creator
2. Configure project for your Qt kit
3. Build and run

## How to Play

1. **Starting a Game**
   - White moves first
   - Click on a piece to select it
   - Valid moves will be highlighted in green
   - Click on a highlighted square to move

2. **Making Moves**
   - Only pieces of the current player can be selected
   - Moves must follow chess rules
   - The game prevents illegal moves that would put your king in check

3. **Special Moves**
   - **Castling**: Click on your king, then click two squares toward the rook
   - **En Passant**: Available when opponent's pawn moves two squares forward
   - **Pawn Promotion**: Pawns automatically promote to Queens upon reaching the opposite end

4. **Winning**
   - Checkmate: Opponent's king is in check with no legal moves
   - Stalemate: Current player has no legal moves but is not in check (Draw)

5. **Game Controls**
   - **New Game**: Start a fresh game
   - **Undo**: (Note: Feature placeholder in current version)

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
- Opening book and game analysis
- Online multiplayer
- AI opponent

## License

This project is open source and available for educational purposes.
