# Settings Feature Documentation

## Overview
A comprehensive settings dialog has been added to the Chess application, allowing users to customize their gaming experience with four main configuration options.

## Features Implemented

### 1. Settings Button
A new "Settings" button has been added to the main window alongside the existing "New Game", "Undo", and "Flip Board" buttons.

### 2. Settings Dialog
When clicked, the Settings button opens a dialog with four configuration sections:

#### a) Undo Functionality (悔棋功能)
- **Control Type**: Checkbox
- **Feature**: Enable or disable the Undo button
- **Default**: Enabled (checked)
- **Behavior**: When disabled, the Undo button becomes grayed out and cannot be clicked

#### b) Chessboard Colors (自訂棋盤顏色)
- **Control Type**: Color picker buttons
- **Features**:
  - Customize light square color (default: #F0D9B5 - beige)
  - Customize dark square color (default: #B58863 - brown)
  - Reset to default colors button
- **Behavior**: 
  - Click on color buttons to open Qt color picker dialog
  - Selected colors are immediately reflected on the button
  - Colors are applied to the entire chessboard when settings are saved

#### c) Sound Volume (音量)
- **Control Type**: Horizontal slider (0-100%)
- **Feature**: Adjust volume for all game sound effects
- **Default**: 100%
- **Behavior**: 
  - Real-time display of current volume percentage
  - Applies to all sound effects: move, capture, check, checkmate, and castling sounds

#### d) Time Limit (下棋時間限制)
- **Control Type**: Spin box (0-60 minutes)
- **Feature**: Set a time limit per move
- **Default**: 0 (no limit)
- **Special Value**: 0 displays as "No limit"
- **Behavior**:
  - When set > 0, a timer appears showing remaining time for current move
  - Timer counts down every second
  - Color changes: Green (normal) → Orange (< 60s) → Red (< 30s)
  - Warning dialog appears when time expires
  - Timer automatically resets after each move

## Technical Implementation

### New Files Created
1. **settingsdialog.h** - Header file for settings dialog class
2. **settingsdialog.cpp** - Implementation of settings dialog

### Modified Files
1. **Chess.pro** - Added new source and header files
2. **mychess.h** - Added settings-related member variables and methods
3. **mychess.cpp** - Implemented settings functionality and timer logic

### Key Classes and Methods

#### SettingsDialog
- `SettingsDialog(QWidget *parent)` - Constructor
- `loadSettings()` - Load settings from QSettings
- `saveSettings()` - Save settings to QSettings
- `isUndoEnabled()` - Get undo enabled state
- `getLightSquareColor()` - Get light square color
- `getDarkSquareColor()` - Get dark square color
- `getVolume()` - Get volume level
- `getTimeLimitMinutes()` - Get time limit in minutes

#### myChess (New Methods)
- `onSettings()` - Handle Settings button click
- `loadSettings()` - Load settings from persistent storage
- `applySettings()` - Apply current settings to the game
- `startMoveTimer()` - Start the move timer
- `stopMoveTimer()` - Stop the move timer
- `onTimerTimeout()` - Handle timer countdown

#### ChessSquare (New Method)
- `setColors(lightColor, darkColor)` - Update square colors

### Settings Persistence
Settings are automatically saved and loaded using Qt's QSettings class with:
- Organization: "ChessGame"
- Application: "Settings"

Settings persist between application sessions.

## User Interface Changes

### Main Window
- **New Button**: "Settings" button added to button row
- **New Label**: Timer label displays remaining time when time limit is active
- **Timer Colors**: 
  - Green: > 60 seconds remaining
  - Orange: 30-60 seconds remaining
  - Red: < 30 seconds remaining

### Settings Dialog Layout
The dialog is organized into four group boxes:
1. Undo Functionality
2. Chessboard Colors (with reset button)
3. Sound Volume (with percentage display)
4. Time Limit (with help text)

Standard OK/Cancel buttons at the bottom allow users to save or discard changes.

## Code Quality
- ✅ Compiles successfully with no errors
- ⚠️ Minor warnings for unused variables (pre-existing)
- ✅ Follows existing code style and patterns
- ✅ Properly integrated with Qt signal/slot mechanism
- ✅ Memory management handled correctly

## Testing Recommendations

### Manual Testing
1. **Settings Dialog**:
   - Open settings dialog and verify all controls are present
   - Change each setting and verify it saves correctly
   - Close and reopen dialog to verify persistence

2. **Undo Toggle**:
   - Disable undo and verify button becomes grayed out
   - Enable undo and verify button works normally

3. **Color Customization**:
   - Change light and dark square colors
   - Verify colors apply to entire board
   - Test reset to defaults button

4. **Volume Control**:
   - Set volume to different levels
   - Make moves and verify sound volume changes
   - Test at 0%, 50%, and 100%

5. **Time Limit**:
   - Set time limit to 1 minute
   - Verify timer appears and counts down
   - Verify color changes at appropriate thresholds
   - Let timer expire and verify warning appears
   - Make a move and verify timer resets

## Future Enhancements
- Add keyboard shortcuts for opening settings
- Add more sound effect options
- Add option to auto-pass turn when time expires
- Add time bank/increment options
- Add theme presets for common color schemes
