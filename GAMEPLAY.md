# Chess Game - Gameplay Guide

Complete guide to playing the chess game, including all features, controls, and rules.

## Table of Contents
- [Getting Started](#getting-started)
- [Game Controls](#game-controls)
- [How to Play](#how-to-play)
- [Moving Pieces](#moving-pieces)
- [Visual Indicators](#visual-indicators)
- [Chess Rules](#chess-rules)
- [Special Moves](#special-moves)
- [Game States](#game-states)
- [Tips and Strategies](#tips-and-strategies)

## Getting Started

### Launching the Game
1. Build and run the application (see [README.md](README.md) for build instructions)
2. The game window will open with a fresh chess game ready to play
3. White always moves first in chess

### Game Window Layout
The game window consists of three main sections:
- **Top Bar**: Turn indicator (left) and game status (right)
- **Chess Board**: 8×8 grid with pieces in their starting positions
- **Bottom Controls**: New Game and Undo buttons

## Game Controls

### Primary Controls
- **New Game Button**: Start a fresh game, resetting all pieces to their starting positions
- **Undo Button**: Currently a placeholder for future undo functionality

### Mouse Controls
The game supports two methods for moving pieces:

#### Method 1: Click-to-Move
1. Click on a piece to select it
2. Valid moves will be highlighted
3. Click on a highlighted square to move the piece
4. Click on the selected piece again to deselect it

#### Method 2: Drag-and-Drop
1. Click and hold on a piece
2. Drag the piece to the destination square
3. Release the mouse button to complete the move
4. If you cancel the drag, the piece returns to its original position

Both methods work seamlessly and can be used interchangeably during the game!

## How to Play

### Basic Gameplay
1. **White moves first** - The game starts with White's turn
2. **Select a piece** - Click or drag any white piece
3. **Move to a valid square** - Only legal moves are allowed
4. **Turns alternate** - After White moves, it becomes Black's turn
5. **Capture opponent pieces** - Move to a square occupied by an opponent's piece
6. **Protect your King** - The game prevents moves that would put your King in check
7. **Win by checkmate** - Trap the opponent's King with no escape

### Turn-Based Play
- Players alternate turns: White → Black → White → Black...
- You can only move pieces of your color
- You must make a move on your turn (no passing)
- The turn indicator at the top shows whose turn it is

## Moving Pieces

### Piece Movement Rules

#### Pawn (♙/♟)
- **Forward Movement**: Moves one square forward (cannot capture)
- **First Move**: Can move two squares forward on its first move
- **Capture**: Captures diagonally forward one square
- **Promotion**: Automatically promotes to a Queen when reaching the opposite end
- **En Passant**: Special capture move (see [Special Moves](#special-moves))

#### Rook (♖/♜)
- Moves horizontally or vertically any number of squares
- Cannot jump over other pieces
- Used in castling with the King

#### Knight (♘/♞)
- Moves in an "L" shape: 2 squares in one direction, 1 square perpendicular
- The only piece that can jump over other pieces
- Can capture the piece it lands on

#### Bishop (♗/♝)
- Moves diagonally any number of squares
- Cannot jump over other pieces
- Each bishop stays on its starting color (light or dark)

#### Queen (♕/♛)
- Most powerful piece
- Combines Rook and Bishop movement
- Moves horizontally, vertically, or diagonally any number of squares
- Cannot jump over other pieces

#### King (♔/♚)
- Most important piece - if checkmated, you lose
- Moves one square in any direction (horizontal, vertical, or diagonal)
- Cannot move into check (a square attacked by opponent pieces)
- Used in castling with the Rook

## Visual Indicators

The game provides clear visual feedback through colors and highlights:

### Square Colors
- **Light Squares** (#F0D9B5): Cream-colored squares
- **Dark Squares** (#B58863): Brown-colored squares
- Standard chess board pattern with alternating colors

### Highlighting

#### Selected Piece (Gold #FFD700)
- When you click on a piece, its square turns gold
- Indicates the currently selected piece
- Click again to deselect

#### Valid Moves
- **Blue Border** (#0066FF, 3px): Empty squares you can move to
- **Red Border** (#FF0000, 3px): Squares with opponent pieces you can capture

#### King in Check (Red Background #FF6B6B)
- When a King is under attack, its square background turns red
- **Highest priority** - always visible even when piece is selected
- Status message also displays "White/Black is in check!"

### Visual Priority
1. **Red Background** - King in check (highest priority, always visible)
2. **Gold Background** - Selected piece
3. **Colored Borders** - Valid move destinations (blue) or captures (red)
4. **Default** - Standard light/dark board pattern

## Chess Rules

### Basic Rules
1. **Check**: When your King is under attack, you are "in check"
   - You must move out of check on your next turn
   - You cannot make a move that puts your own King in check
   - King's square turns red when in check

2. **Checkmate**: When your King is in check and you have no legal moves to escape
   - The game is over
   - The opponent wins

3. **Stalemate**: When you have no legal moves but are NOT in check
   - The game is a draw (tie)
   - Neither player wins

4. **Captures**: Moving to a square occupied by an opponent's piece
   - The opponent's piece is removed from the board
   - You cannot capture your own pieces

5. **Turn Order**: Players must alternate turns
   - White always moves first
   - You cannot skip your turn

### Move Validation
The game automatically prevents illegal moves:
- ✅ Only pieces of the current player can be moved
- ✅ Each piece must follow its movement rules
- ✅ Cannot move through other pieces (except Knights)
- ✅ Cannot put your own King in check
- ✅ Must move out of check if in check
- ✅ Cannot castle through or into check

## Special Moves

### Castling
A special move involving the King and a Rook, performed as a single move.

**How to Castle:**
1. Click on your King (or start dragging it)
2. Move the King two squares toward the Rook
3. The Rook automatically moves to the square next to the King

**Conditions for Castling:**
- Neither the King nor the Rook has moved yet
- No pieces between the King and Rook
- The King is not in check
- The King does not move through check
- The King does not end up in check

**Types:**
- **Kingside Castling**: King moves toward the h-file (right side)
  - Notation: O-O
  - King moves from e1 to g1 (White) or e8 to g8 (Black)
  - Rook moves from h1 to f1 (White) or h8 to f8 (Black)

- **Queenside Castling**: King moves toward the a-file (left side)
  - Notation: O-O-O
  - King moves from e1 to c1 (White) or e8 to c8 (Black)
  - Rook moves from a1 to d1 (White) or a8 to d8 (Black)

### En Passant
A special pawn capture move.

**When Available:**
- Opponent's pawn moves two squares forward from its starting position
- Your pawn is on the 5th rank (for White) or 4th rank (for Black)
- Your pawn is directly beside the opponent's pawn

**How to Perform:**
1. Move your pawn diagonally behind the opponent's pawn
2. The opponent's pawn is captured as if it had only moved one square
3. This move is only valid immediately after the opponent's two-square pawn advance

**Example:**
- Black pawn on e7 moves to e5 (passing e6)
- White pawn on d5 can capture "en passant" by moving to e6
- Black pawn on e5 is removed from the board

### Pawn Promotion
When a pawn reaches the opposite end of the board, it is promoted.

**Current Implementation:**
- Pawns automatically promote to a Queen
- A Queen is the most powerful piece in chess
- Future versions may add a dialog to choose promotion piece (Queen, Rook, Bishop, or Knight)

**When it Happens:**
- White pawn reaches rank 8 (top row)
- Black pawn reaches rank 1 (bottom row)

## Game States

### In Progress
- **Status**: "Game in progress"
- Normal gameplay
- Players alternate turns
- All moves are validated

### Check
- **Status**: "White is in check!" or "Black is in check!"
- King's square turns red
- Player in check must move out of check
- Cannot make any move that leaves King in check

### Checkmate
- **Status**: "White wins by checkmate!" or "Black wins by checkmate!"
- King is in check with no legal moves to escape
- Game ends immediately
- Winner is announced in a dialog box
- Board becomes non-interactive

### Stalemate
- **Status**: "Stalemate - Draw!"
- Current player has no legal moves
- King is NOT in check
- Game ends in a draw (tie)
- Announced in a dialog box
- Board becomes non-interactive

## Tips and Strategies

### For Beginners
1. **Protect Your King**: Always ensure your King is safe
2. **Control the Center**: Try to control the four center squares (d4, d5, e4, e5)
3. **Develop Your Pieces**: Move your Knights and Bishops early in the game
4. **Castle Early**: Get your King to safety with castling
5. **Think Ahead**: Consider your opponent's possible responses
6. **Don't Rush**: Take your time to find the best move

### Using the Interface
1. **Preview Moves**: Click on a piece to see all valid moves before committing
2. **Change Your Mind**: Click on a selected piece again to deselect it
3. **Try Both Methods**: Use click-to-move for precision, drag-and-drop for speed
4. **Watch for Red**: If your King's square turns red, you must address the check
5. **Check Borders**: Blue borders = safe moves, Red borders = captures

### Common Mistakes to Avoid
1. **Leaving King in Check**: The game prevents this, but plan your moves carefully
2. **Ignoring Threats**: Watch for opponent pieces attacking your valuable pieces
3. **Moving Without Purpose**: Every move should have a strategic goal
4. **Forgetting Special Moves**: Use castling, en passant, and promotion to your advantage
5. **Not Thinking Ahead**: Consider the consequences of your moves

### Advanced Tactics
1. **Forks**: Attack two pieces at once (especially effective with Knights)
2. **Pins**: Attack a piece that cannot move without exposing a more valuable piece
3. **Skewers**: Attack a valuable piece forcing it to move and exposing a less valuable piece
4. **Discovered Attacks**: Move one piece to reveal an attack from another piece
5. **Sacrifices**: Give up a piece to gain a strategic advantage

## Game Features Summary

### ✅ Implemented Features
- Complete chess rules implementation
- All standard piece movements
- Check, checkmate, and stalemate detection
- Move validation (prevents illegal moves)
- Castling (kingside and queenside)
- En passant capture
- Pawn promotion (automatic to Queen)
- Drag-and-drop movement
- Click-to-move interface
- King in check highlighting (red square)
- Valid move highlighting (blue/red borders)
- Turn-based gameplay with turn indicator
- New game functionality
- Game over dialogs
- Visual feedback for all game states
- Square aspect ratio maintained when resizing window

### 🔄 Placeholder Features
- Undo/Redo functionality (button present, not fully implemented)
- Move history with algebraic notation
- Piece promotion choice dialog (currently auto-promotes to Queen)

### 💡 Future Enhancement Ideas
- AI opponent with difficulty levels
- Timer/clock for timed games
- Save/load game state
- Move history panel
- Highlighted last move
- Move suggestions/hints
- Sound effects
- Animations
- Online multiplayer
- Opening book database
- Game analysis tools

## Troubleshooting

### "I can't move that piece!"
- Make sure it's your turn (check the turn indicator)
- Verify the move follows the piece's movement rules
- Ensure you're not trying to move into check
- Check if you're in check - you must move out of check

### "Why can't I castle?"
- Both the King and Rook must not have moved yet
- No pieces can be between the King and Rook
- The King cannot be in check
- The King cannot move through or into check
- The game will only highlight valid castling moves

### "The drag didn't work"
- Make sure you're dragging your own piece (correct color for your turn)
- Drag to a valid destination (highlighted square)
- If the game is over, dragging is disabled

### "What happened to my pawn?"
- If a pawn reached the opposite end, it automatically promoted to a Queen
- Look for a Queen where your pawn used to be

## Learning Resources

### Understanding Chess Notation
While not displayed in the current version, chess moves are traditionally recorded using algebraic notation:
- Squares are labeled a1-h8 (column letter + row number)
- Pieces: K=King, Q=Queen, R=Rook, B=Bishop, N=Knight, (Pawn has no letter)
- Example: "e4" means pawn to e4, "Nf3" means knight to f3
- Capture: "x" (e.g., "Bxe5" = bishop captures on e5)
- Check: "+" (e.g., "Qh5+")
- Checkmate: "#" (e.g., "Qxf7#")
- Castling: "O-O" (kingside) or "O-O-O" (queenside)

### Practice Strategies
1. **Play Complete Games**: Finish every game to learn endgame tactics
2. **Analyze Your Moves**: Think about what worked and what didn't
3. **Try Different Openings**: Experiment with different first moves
4. **Practice Special Moves**: Get comfortable with castling and en passant
5. **Study Common Patterns**: Learn to recognize checkmate patterns

### External Resources
To improve your chess skills, consider:
- Online tutorials and videos
- Chess books for beginners
- Playing against computer opponents at various levels
- Solving chess puzzles and tactics
- Watching games by strong players

---

**Enjoy the game! Whether you're a beginner learning the rules or an experienced player practicing tactics, this chess game provides a complete and user-friendly interface for playing the classic game of chess.**
