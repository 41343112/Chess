# Time Control Feature

## Overview
This document describes the newly implemented time control feature for the chess game.

## Problem Statement Requirements
The feature was implemented based on the following requirements:
1. ✅ 按右鍵取消拖曳 (Right-click to cancel drag) - Already existed
2. ✅ 新增設定總時間 (Add setting for total time) - Implemented
3. ✅ 不是每一著的時間 (Not per-move time) - Implemented as total time
4. ✅ 雙方都要顯示 (Display for both sides) - Both White and Black timers shown

## Features

### Settings Dialog
A new "Time Control" section has been added to the Settings dialog with:
- **Enable Time Control** checkbox - Turn time control on/off
- **Total Time per Player** spinbox - Set total time in minutes (1-180 minutes)
- Default: Disabled, 10 minutes when enabled

### UI Display
Two time labels are displayed prominently at the top of the game window:
- **White Timer** - Displayed on the left side
- **Black Timer** - Displayed on the right side
- Format: MM:SS (e.g., "White: 10:00", "Black: 09:45")
- Labels are only visible when time control is enabled

### Timer Behavior
1. **Game Start**: 
   - Timers are initialized based on the setting (e.g., 10:00)
   - White's timer starts counting down immediately
   
2. **During Game**:
   - Only the current player's timer counts down
   - Timer switches to the other player after each move
   - Updates every second
   
3. **Time Expiration**:
   - Game ends immediately when a player runs out of time
   - Dialog shows which player lost by timeout
   - Status label updates to show the result
   
4. **Pause Conditions**:
   - Timer pauses when viewing move history
   - Timer resumes when returning to current position
   - Timer stops when game ends normally (checkmate, stalemate)

### Settings Persistence
Time control settings are saved and loaded using QSettings:
- `timeControlEnabled` - Boolean flag
- `timeControlMinutes` - Integer value for minutes

## Implementation Details

### Modified Files
1. **settingsdialog.h/cpp**
   - Added `m_timeControlCheckBox` and `m_timeControlSpinBox`
   - Added getters: `isTimeControlEnabled()`, `getTimeControlMinutes()`
   - Added slot: `onTimeControlCheckChanged(int state)`
   - Settings load/save for time control options

2. **mychess.h/cpp**
   - Added `m_gameTimer` (QTimer for countdown)
   - Added `m_whiteTimeLabel` and `m_blackTimeLabel` (QLabel for display)
   - Added time tracking: `m_whiteTimeRemaining`, `m_blackTimeRemaining` (in seconds)
   - Added settings: `m_timeControlEnabled`, `m_timeControlMinutes`
   - Added methods:
     - `onTimerTick()` - Handles countdown and timeout
     - `updateTimeDisplay()` - Updates timer labels
     - `startTimer()` - Starts the countdown
     - `stopTimer()` - Pauses the countdown
     - `resetTimers()` - Resets to initial time
     - `formatTime(int seconds)` - Formats seconds as MM:SS

### Timer Logic Flow
```
New Game → Reset Timers → Start Timer (if enabled)
    ↓
Timer Tick (every 1 second)
    ↓
Decrement Current Player's Time
    ↓
Check if Time Expired → Yes → Show Timeout Dialog, Stop Timer
    ↓ No
Update Display
    ↓
Continue...
```

## Usage

### Enabling Time Control
1. Click the "Settings" button
2. Check "Enable Time Control"
3. Set desired total time (default 10 minutes)
4. Click "OK"
5. Start a new game for settings to take effect

### During Gameplay
- Watch the timers count down at the top of the window
- White timer shows "White: MM:SS"
- Black timer shows "Black: MM:SS"
- The current player's time decreases each second
- Game ends if either player runs out of time

### Disabling Time Control
1. Click the "Settings" button
2. Uncheck "Enable Time Control"
3. Click "OK"
4. Timer labels will be hidden in the next game

## Testing Notes
This implementation requires Qt 5 or Qt 6 to compile and run. The feature has been implemented with:
- Proper timer initialization and cleanup
- Memory-safe handling of timer objects
- Integration with existing game state management
- Proper pause/resume behavior during history viewing
- Translation support ready for Chinese/English languages

## Future Enhancements (Not Implemented)
- Time increment per move (Fischer time)
- Different time controls for each player
- Time bonus/delay options
- Visual warning when time is low (e.g., < 1 minute)
- Sound alert for low time
