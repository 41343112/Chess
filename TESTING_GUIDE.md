# Testing Guide for Computer Opponent Feature

## Overview
This document provides a comprehensive testing guide for the newly implemented computer opponent feature with difficulty levels.

## Build Instructions

### Prerequisites
- Qt5 (5.12+) or Qt6 development environment
- C++17 compatible compiler
- qmake build tool

### Build Steps
```bash
cd /home/runner/work/Chess/Chess
qmake Chess.pro
make
./Chess  # or Chess.exe on Windows
```

## Test Cases

### 1. Start Dialog Tests

#### Test 1.1: Game Mode Selection
**Steps:**
1. Launch the application
2. Observe the start dialog
3. Verify "玩家對戰" (Human vs Human) radio button is present
4. Verify "與電腦對戰" (Human vs Computer) radio button is present
5. Verify "玩家對戰" is selected by default

**Expected Result:** Both options visible, Human vs Human selected by default

#### Test 1.2: Computer Options Visibility
**Steps:**
1. Launch the application
2. Verify computer options are hidden when "玩家對戰" is selected
3. Click "與電腦對戰"
4. Verify computer options become visible

**Expected Result:** Computer options (difficulty, color) appear when computer mode selected

#### Test 1.3: Difficulty Selection
**Steps:**
1. Select "與電腦對戰"
2. Check difficulty dropdown has three options: 簡單, 中等, 困難
3. Verify 中等 (Medium) is selected by default

**Expected Result:** All three difficulty levels available, Medium is default

#### Test 1.4: Color Selection
**Steps:**
1. Select "與電腦對戰"
2. Verify "白方（先手）" and "黑方（後手）" options are present
3. Verify white is selected by default

**Expected Result:** Both color options available, white selected by default

### 2. Gameplay Tests - Easy Difficulty

#### Test 2.1: Computer Plays as Black (Human White)
**Steps:**
1. Start game with: Computer mode, Easy difficulty, Player as White
2. Make a move (e.g., e2-e4)
3. Wait for computer response

**Expected Result:**
- Status shows "Computer is thinking..."
- Computer makes a random valid move within ~0.5 seconds
- Status updates to reflect current game state
- Timer increments correctly

#### Test 2.2: Computer Plays as White (Human Black)
**Steps:**
1. Start game with: Computer mode, Easy difficulty, Player as Black
2. Observe computer's first move

**Expected Result:**
- Computer automatically makes first move after ~0.5 seconds
- Player can then make their move
- Game flow continues normally

#### Test 2.3: Easy Difficulty Behavior
**Steps:**
1. Play several moves against Easy AI
2. Observe AI's move patterns

**Expected Result:**
- Moves appear random
- AI doesn't consistently capture pieces
- AI doesn't prioritize center control

### 3. Gameplay Tests - Medium Difficulty

#### Test 3.1: Capture Preference
**Steps:**
1. Start game with Medium difficulty
2. Set up position where AI can capture a piece
3. Observe AI's move

**Expected Result:**
- AI should capture the piece if it's a good trade
- AI considers piece values

#### Test 3.2: Center Control
**Steps:**
1. Start game with Medium difficulty
2. Observe opening moves
3. Check if AI moves pieces toward center

**Expected Result:**
- AI tends to control center squares (d4, d5, e4, e5)
- AI develops pieces actively

### 4. Gameplay Tests - Hard Difficulty

#### Test 4.1: Strategic Play
**Steps:**
1. Start game with Hard difficulty
2. Try to set up a trap or tactical sequence
3. Observe if AI avoids obvious blunders

**Expected Result:**
- AI should avoid simple tactics
- AI takes longer to think (up to 2-3 seconds)
- AI should play coherently

#### Test 4.2: Response Time
**Steps:**
1. Start game with Hard difficulty
2. Measure time between player move and computer response

**Expected Result:**
- Response time between 0.5-3 seconds depending on position complexity
- No crashes or freezes

### 5. UI/UX Tests

#### Test 5.1: Player Cannot Move During Computer Turn
**Steps:**
1. Start computer game
2. Make a move
3. Immediately try to move another piece before computer responds

**Expected Result:**
- Click/drag is ignored during computer's turn
- Status shows "Computer is thinking..."
- Pieces don't respond to user input

#### Test 5.2: Drag and Drop Works
**Steps:**
1. Start computer game as white
2. Try dragging a piece to move

**Expected Result:**
- Drag and drop works normally for human moves
- Computer responds after human's drag-drop move

#### Test 5.3: Status Messages
**Steps:**
1. Play a game and observe status label

**Expected Result:**
- Shows "Computer is thinking..." during AI turn
- Shows check/checkmate messages correctly
- Updates turn indicator properly

### 6. Integration Tests

#### Test 6.1: Undo in Computer Game
**Steps:**
1. Start computer game
2. Make several moves
3. Click Undo button

**Expected Result:**
- Undo should undo last move (computer or human)
- Can undo multiple times
- Game state remains consistent

#### Test 6.2: Time Control
**Steps:**
1. Start computer game with time control enabled (e.g., 5 minutes)
2. Play game and observe timers

**Expected Result:**
- Both players' timers work correctly
- Increment works after each move
- Time updates during thinking

#### Test 6.3: Sound Effects
**Steps:**
1. Start computer game
2. Play moves and observe sounds

**Expected Result:**
- Move sound plays for both human and computer moves
- Capture sound plays when piece is captured
- Check/checkmate sounds work

#### Test 6.4: Flip Board
**Steps:**
1. Start computer game
2. Click flip board button during game

**Expected Result:**
- Board flips correctly
- Game continues normally
- Computer moves still work

### 7. Edge Case Tests

#### Test 7.1: Promotion
**Steps:**
1. Advance a pawn to 8th rank in computer game
2. Observe promotion

**Expected Result:**
- Human: Promotion dialog appears
- Computer: Automatically promotes to Queen

#### Test 7.2: Checkmate
**Steps:**
1. Play until checkmate position

**Expected Result:**
- Game ends correctly
- Checkmate message displays
- No more moves allowed

#### Test 7.3: Stalemate
**Steps:**
1. Create stalemate position

**Expected Result:**
- Game recognizes stalemate
- Displays stalemate message
- Game ends in draw

#### Test 7.4: Game End and New Game
**Steps:**
1. Complete a computer game
2. Click "New Game"

**Expected Result:**
- Start dialog appears again
- Can select new mode/difficulty
- Game resets properly

### 8. Performance Tests

#### Test 8.1: Easy Difficulty Response
**Steps:**
1. Start Easy game
2. Time computer responses over 10 moves

**Expected Result:**
- Average response time < 0.1 seconds (plus 0.5s delay)

#### Test 8.2: Medium Difficulty Response
**Steps:**
1. Start Medium game
2. Time computer responses over 10 moves

**Expected Result:**
- Average response time < 0.5 seconds (plus 0.5s delay)

#### Test 8.3: Hard Difficulty Response
**Steps:**
1. Start Hard game
2. Time computer responses in complex positions

**Expected Result:**
- Response time < 3 seconds in most positions
- No UI freezing

### 9. Regression Tests

#### Test 9.1: Human vs Human Still Works
**Steps:**
1. Start game with "玩家對戰" mode
2. Play a game

**Expected Result:**
- Traditional two-player mode works exactly as before
- No AI interference

#### Test 9.2: All Existing Features Work
**Steps:**
1. Test settings dialog
2. Test language switching
3. Test sound controls
4. Test board colors

**Expected Result:**
- All pre-existing features work unchanged
- No regression in functionality

## Bug Reporting Template

If you find a bug, please report it with:
- **Steps to Reproduce:** Detailed steps
- **Expected Behavior:** What should happen
- **Actual Behavior:** What actually happened
- **Game Mode:** Human/Computer, Difficulty level
- **Player Color:** White/Black
- **Screenshot:** If applicable
- **Error Messages:** Any errors or crashes

## Success Criteria

Feature is considered complete when:
- [ ] All test cases pass
- [ ] No critical bugs
- [ ] Performance meets requirements
- [ ] Code review completed
- [ ] Documentation complete
- [ ] No regression in existing features
