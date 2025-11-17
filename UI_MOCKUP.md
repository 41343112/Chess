# UI Mockup

This file shows what the Chess game interface looks like.

## Initial Board State

```
╔══════════════════════════════════════════════════════════════════════╗
║                      Chess Game - Like Chess.com                     ║
╠══════════════════════════════════════════════════════════════════════╣
║                                                                      ║
║  Turn: White                              Game in progress          ║
║                                                                      ║
║    ╔════════════════════════════════════════════════════╗           ║
║    ║  a    b    c    d    e    f    g    h              ║           ║
║  8 ║ ♜ │ ♞ │ ♝ │ ♛ │ ♚ │ ♝ │ ♞ │ ♜ │                   ║           ║
║    ║───┼───┼───┼───┼───┼───┼───┼───┤                   ║           ║
║  7 ║ ♟ │ ♟ │ ♟ │ ♟ │ ♟ │ ♟ │ ♟ │ ♟ │                   ║           ║
║    ║───┼───┼───┼───┼───┼───┼───┼───┤                   ║           ║
║  6 ║   │   │   │   │   │   │   │   │                   ║           ║
║    ║───┼───┼───┼───┼───┼───┼───┼───┤                   ║           ║
║  5 ║   │   │   │   │   │   │   │   │                   ║           ║
║    ║───┼───┼───┼───┼───┼───┼───┼───┤                   ║           ║
║  4 ║   │   │   │   │   │   │   │   │                   ║           ║
║    ║───┼───┼───┼───┼───┼───┼───┼───┤                   ║           ║
║  3 ║   │   │   │   │   │   │   │   │                   ║           ║
║    ║───┼───┼───┼───┼───┼───┼───┼───┤                   ║           ║
║  2 ║ ♙ │ ♙ │ ♙ │ ♙ │ ♙ │ ♙ │ ♙ │ ♙ │                   ║           ║
║    ║───┼───┼───┼───┼───┼───┼───┼───┤                   ║           ║
║  1 ║ ♖ │ ♘ │ ♗ │ ♕ │ ♔ │ ♗ │ ♘ │ ♖ │                   ║           ║
║    ╚════════════════════════════════════════════════════╝           ║
║                                                                      ║
║              [ New Game ]            [ Undo ]                        ║
║                                                                      ║
╚══════════════════════════════════════════════════════════════════════╝
```

## Selected Piece with Valid Moves

When a white pawn on e2 is selected, valid moves are highlighted:

```
╔══════════════════════════════════════════════════════════════════════╗
║                      Chess Game - Like Chess.com                     ║
╠══════════════════════════════════════════════════════════════════════╣
║                                                                      ║
║  Turn: White                              Game in progress          ║
║                                                                      ║
║    ╔════════════════════════════════════════════════════╗           ║
║    ║  a    b    c    d    e    f    g    h              ║           ║
║  8 ║ ♜ │ ♞ │ ♝ │ ♛ │ ♚ │ ♝ │ ♞ │ ♜ │                   ║           ║
║    ║───┼───┼───┼───┼───┼───┼───┼───┤                   ║           ║
║  7 ║ ♟ │ ♟ │ ♟ │ ♟ │ ♟ │ ♟ │ ♟ │ ♟ │                   ║           ║
║    ║───┼───┼───┼───┼───┼───┼───┼───┤                   ║           ║
║  6 ║   │   │   │   │   │   │   │   │                   ║           ║
║    ║───┼───┼───┼───┼───┼───┼───┼───┤                   ║           ║
║  5 ║   │   │   │   │   │   │   │   │                   ║           ║
║    ║───┼───┼───┼───┼───┼───┼───┼───┤                   ║           ║
║  4 ║   │   │   │   │ ✓ │   │   │   │ ← Valid move     ║           ║
║    ║───┼───┼───┼───┼───┼───┼───┼───┤                   ║           ║
║  3 ║   │   │   │   │ ✓ │   │   │   │ ← Valid move     ║           ║
║    ║───┼───┼───┼───┼───┼───┼───┼───┤                   ║           ║
║  2 ║ ♙ │ ♙ │ ♙ │ ♙ │[♙]│ ♙ │ ♙ │ ♙ │ ← Selected       ║           ║
║    ║───┼───┼───┼───┼───┼───┼───┼───┤                   ║           ║
║  1 ║ ♖ │ ♘ │ ♗ │ ♕ │ ♔ │ ♗ │ ♘ │ ♖ │                   ║           ║
║    ╚════════════════════════════════════════════════════╝           ║
║                                                                      ║
║              [ New Game ]            [ Undo ]                        ║
║                                                                      ║
╚══════════════════════════════════════════════════════════════════════╝
```

## Visual Legend

| Element | Description |
|---------|-------------|
| Normal square | Default board colors (beige/brown alternating) |
| `[♙]` Selected | Gold background (#FFD700) |
| `✓` Valid move | Light green background (#90EE90) |

## Color Scheme

| Element | Color | Hex Code |
|---------|-------|----------|
| Light squares | Cream/Beige | #F0D9B5 |
| Dark squares | Brown | #B58863 |
| Selected piece | Gold | #FFD700 |
| Valid moves | Light Green | #90EE90 |

## Features

- **Click to select**: Click on a piece to select it
- **Valid move highlighting**: All legal moves are shown in green
- **Move execution**: Click a highlighted square to move
- **Turn indicator**: Shows which player's turn it is
- **Status display**: Shows game state (check, checkmate, stalemate)
- **New Game**: Reset the board to start over
- **Game over dialog**: Appears when checkmate or stalemate occurs

## Interaction Flow

1. **First Click**: Select a piece (your turn only)
   - Piece becomes highlighted in gold
   - Valid moves shown in green
   
2. **Second Click**: Choose destination
   - Move executes if valid
   - Board updates
   - Turn switches
   - Or select a different piece

3. **Invalid Moves**: Rejected automatically
   - Cannot move opponent's pieces
   - Cannot make illegal moves
   - Cannot leave king in check

## Game States

The status bar shows:
- "Game in progress" - Normal play
- "White is in check!" - White king under attack
- "Black is in check!" - Black king under attack
- "White wins by checkmate!" - Black is checkmated
- "Black wins by checkmate!" - White is checkmated
- "Stalemate - Draw!" - No legal moves, not in check
