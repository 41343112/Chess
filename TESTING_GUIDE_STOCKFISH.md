# Testing Guide for Stockfish Integration

## Prerequisites

1. **Qt Development Environment**:
   - Qt 5.12+ or Qt 6.x
   - Qt Creator (recommended) or command-line tools
   - C++17 compatible compiler

2. **System Requirements**:
   - Windows: Works out of the box
   - Linux: Works out of the box
   - macOS: Requires manual Stockfish setup (see below)

## macOS Setup (Required)

If you're on macOS, download and install Stockfish:

**Option 1: Homebrew**
```bash
brew install stockfish
```

**Option 2: Manual Download**
1. Download from https://stockfishchess.org/download/
2. Extract and rename to `stockfish-macos`
3. Move to `engine/` directory
4. Make executable: `chmod +x engine/stockfish-macos`

## Building the Application

### Using Qt Creator (Recommended)
1. Open Qt Creator
2. File → Open File or Project
3. Select `Chess.pro`
4. Click "Configure Project"
5. Click the green "Run" button (▶) or press Ctrl+R

### Using Command Line
```bash
cd /path/to/Chess
qmake Chess.pro
make          # On Linux/macOS
# or
mingw32-make  # On Windows with MinGW
```

## Testing Plan

### 1. Basic Functionality Tests

**Test 1: Engine Initialization**
- [ ] Start the application
- [ ] Select "與電腦對戰" (Play vs Computer)
- [ ] Click "開始遊戲" (Start Game)
- [ ] Verify no error messages appear
- [ ] Check console/debug output for "Stockfish engine initialized successfully"

**Test 2: Slider Difficulty Control**
- [ ] Open start dialog
- [ ] Select computer mode
- [ ] Move slider to position 0
- [ ] Verify label shows "等級 0 (初學者)"
- [ ] Move slider to position 10
- [ ] Verify label shows "等級 10 (中等)"
- [ ] Move slider to position 20
- [ ] Verify label shows "等級 20 (專家)"

**Test 3: Basic Move Execution**
- [ ] Start game with skill level 10
- [ ] Choose white (player goes first)
- [ ] Make a move (e.g., e2 to e4)
- [ ] Verify "Computer is thinking..." appears
- [ ] Verify computer makes a move within 1-2 seconds
- [ ] Verify game continues normally

### 2. Skill Level Tests

**Test 4: Beginner Level (0-5)**
- [ ] Start game with skill level 0
- [ ] Play 5-10 moves
- [ ] Computer should make questionable/weak moves
- [ ] Verify you can win relatively easily

**Test 5: Expert Level (20)**
- [ ] Start game with skill level 20
- [ ] Play 10-15 moves
- [ ] Computer should play very strong moves
- [ ] Verify it's challenging to find good moves

### 3. Game Flow Tests

**Test 6: Computer as White**
- [ ] Start game with computer mode
- [ ] Select black (computer goes first)
- [ ] Verify computer makes the first move automatically
- [ ] Continue game normally

**Test 7: Game Completion**
- [ ] Play a game to checkmate
- [ ] Verify game ends properly
- [ ] Check game over dialog appears

**Test 8: Undo with Computer**
- [ ] Start computer game
- [ ] Make several moves
- [ ] Click "Undo" button
- [ ] Verify moves are undone correctly

### 4. Error Handling Tests

**Test 9: Missing Engine (Manual Test)**
If you want to test error handling:
- [ ] Temporarily rename engine directory
- [ ] Start computer game
- [ ] Verify friendly error message appears
- [ ] Restore engine directory

### 5. Performance Tests

**Test 10: Response Time**
Measure computer thinking time at different skill levels:

| Skill Level | Expected Time | Actual Time |
|-------------|---------------|-------------|
| 0           | ~0.1s         | ______      |
| 5           | ~0.3s         | ______      |
| 10          | ~0.6s         | ______      |
| 15          | ~0.9s         | ______      |
| 20          | ~1.1s         | ______      |

**Test 11: Memory Usage**
- [ ] Start game
- [ ] Play 50+ moves
- [ ] Monitor memory usage
- [ ] Verify no memory leaks (should be stable)

### 6. Integration Tests

**Test 12: Sound Effects**
- [ ] Enable sound in settings
- [ ] Make moves
- [ ] Verify capture sounds work
- [ ] Verify check sound works

**Test 13: Time Control**
- [ ] Set time control (e.g., 5 minutes)
- [ ] Start computer game
- [ ] Verify timer works for both sides
- [ ] Verify increment works if set

**Test 14: Board Flip**
- [ ] Start computer game
- [ ] Click "Flip Board" button
- [ ] Verify board orientation changes
- [ ] Verify game continues normally

### 7. Chinese Language Tests

**Test 15: UI Language**
- [ ] Verify all labels are in Chinese
- [ ] Check difficulty descriptions (初學者/中等/進階/專家)
- [ ] Verify status messages are correct
- [ ] Check error messages (if any) are in Chinese

## Debug Output to Check

When running, look for these console messages:

```
Found Stockfish at: [path]
Sending to Stockfish: uci
Stockfish output: [UCI response]
Stockfish engine initialized successfully
Position FEN: [FEN string]
Sending to Stockfish: position fen [...]
Sending to Stockfish: go movetime [ms]
Best move from Stockfish: [move]
```

## Common Issues and Solutions

### Issue 1: "Stockfish engine not found"
**Solution**: Check that the engine binary exists in `engine/` directory and is executable.

### Issue 2: "Failed to start Stockfish engine"
**Solution**: 
- On Linux/macOS: `chmod +x engine/stockfish-linux` or `engine/stockfish-macos`
- Check file permissions
- Verify binary is for correct platform

### Issue 3: Engine doesn't respond
**Solution**:
- Check console output for errors
- Verify engine process is running
- Restart application

### Issue 4: Compilation errors with Qt version
**Solution**: Ensure Qt 5.12+ or 6.x is installed. Check Qt version with `qmake --version`.

## Success Criteria

The implementation is successful if:

- ✅ Engine initializes without errors
- ✅ Computer makes legal moves at all skill levels
- ✅ Slider control works smoothly (0-20)
- ✅ Game plays to completion
- ✅ No crashes or freezes
- ✅ Chinese language UI works correctly
- ✅ Performance is acceptable (< 2s per move at any skill level)
- ✅ All existing features (undo, sounds, timer) still work

## Reporting Issues

If you find any issues, please report:
1. Qt version (qmake --version)
2. Operating system and version
3. Skill level being tested
4. Steps to reproduce
5. Console/debug output
6. Expected vs actual behavior

## Advanced Testing

For thorough testing, consider:
- Playing complete games at different skill levels
- Testing all special moves (castling, en passant, promotion)
- Testing edge cases (stalemate, threefold repetition)
- Testing with very short time controls
- Testing rapid fire games

## Automated Testing (Future)

Consider adding:
- Unit tests for FEN conversion
- Unit tests for UCI move parsing
- Integration tests for engine communication
- Performance benchmarks

## Conclusion

This testing guide ensures the Stockfish integration works correctly across all scenarios. Follow the tests systematically and check off each item as you complete it.
