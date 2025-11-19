# Settings Feature - Visual Guide

## Main Window Layout (Updated)

```
┌─────────────────────────────────────────────────────────────┐
│  Chess Game - Like Chess.com                          [_][□][X]│
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  Turn: White  ⏱ Time: 5:00      Game in progress            │
│  ────────────────────────────────────────────────────────    │
│                                                               │
│         ┌─────────────────────────────────┐                  │
│         │  ♜ ♞ ♝ ♛ ♚ ♝ ♞ ♜ │                  │
│         │  ♟ ♟ ♟ ♟ ♟ ♟ ♟ ♟ │                  │
│         │                  │                  │
│         │     CHESSBOARD   │                  │
│         │                  │                  │
│         │  ♙ ♙ ♙ ♙ ♙ ♙ ♙ ♙ │                  │
│         │  ♖ ♘ ♗ ♕ ♔ ♗ ♘ ♖ │                  │
│         └─────────────────────────────────┘                  │
│                                                               │
│    [New Game]  [Undo]  [Flip Board]  [Settings]  ← NEW!     │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

## Settings Dialog

```
┌─────────────────────────────────────────────────────────────┐
│  Settings                                            [_][X]  │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌─ Undo Functionality ─────────────────────────────────┐   │
│  │  ☑ Enable Undo Button                               │   │
│  └──────────────────────────────────────────────────────┘   │
│                                                               │
│  ┌─ Chessboard Colors ──────────────────────────────────┐   │
│  │  Light Squares:  [  #F0D9B5  ] ← Click to change    │   │
│  │  Dark Squares:   [  #B58863  ] ← Click to change    │   │
│  │  [ Reset to Default Colors ]                         │   │
│  └──────────────────────────────────────────────────────┘   │
│                                                               │
│  ┌─ Sound Volume ───────────────────────────────────────┐   │
│  │  Volume: [━━━━━━━━━━━━━━━━━━━━━○] 100%              │   │
│  └──────────────────────────────────────────────────────┘   │
│                                                               │
│  ┌─ Time Limit ─────────────────────────────────────────┐   │
│  │  Minutes per move: [  5 ▼] minutes                   │   │
│  │  Set time limit per move (0 = no limit)              │   │
│  └──────────────────────────────────────────────────────┘   │
│                                                               │
│                               [  OK  ]  [ Cancel ]           │
└─────────────────────────────────────────────────────────────┘
```

## Timer Display States

### Normal (> 60 seconds)
```
Turn: White  ⏱ Time: 5:23  ← Green text
```

### Warning (30-60 seconds)
```
Turn: White  ⏱ Time: 0:45  ← Orange text
```

### Critical (< 30 seconds)
```
Turn: White  ⏱ Time: 0:15  ← Red text, blinking
```

### Time Expired
```
Turn: White  ⏱ Time's up!  ← Red text
[Dialog appears: "Time limit exceeded for White's move!"]
```

## Color Picker Example

When clicking a color button:
```
┌─────────────────────────────────────────┐
│  Choose Light Square Color         [X]  │
├─────────────────────────────────────────┤
│                                          │
│  ┌────────────────────────────────────┐ │
│  │   [Color Wheel]                    │ │
│  │                                    │ │
│  │   Current: #F0D9B5                 │ │
│  │   RGB: R:240 G:217 B:181           │ │
│  │   HTML: #F0D9B5                    │ │
│  └────────────────────────────────────┘ │
│                                          │
│              [  OK  ]  [ Cancel ]        │
└─────────────────────────────────────────┘
```

## Example Use Cases

### Use Case 1: Change Board Colors
1. User clicks "Settings" button
2. In the dialog, clicks "Light Squares" button
3. Color picker opens, user selects new color (e.g., white)
4. Clicks "OK" in color picker
5. Clicks "Dark Squares" button  
6. Selects new color (e.g., dark blue)
7. Clicks "OK" in settings dialog
8. Board immediately updates with new colors

### Use Case 2: Set Time Limit
1. User clicks "Settings" button
2. Changes "Minutes per move" to 5
3. Clicks "OK"
4. New game starts with timer showing "Time: 5:00"
5. Timer counts down as player thinks
6. When time < 30s, display turns red
7. After move, timer resets to 5:00 for other player

### Use Case 3: Adjust Volume
1. User clicks "Settings" button
2. Drags volume slider to 50%
3. Clicks "OK"
4. All game sounds now play at half volume

### Use Case 4: Disable Undo
1. User clicks "Settings" button
2. Unchecks "Enable Undo Button"
3. Clicks "OK"
4. Undo button becomes grayed out
5. Players cannot undo moves

## File Structure

```
Chess/
├── Chess.pro                  (Updated: added new files)
├── mychess.h                  (Updated: settings methods)
├── mychess.cpp                (Updated: settings implementation)
├── settingsdialog.h           (New: dialog class)
├── settingsdialog.cpp         (New: dialog implementation)
├── SETTINGS_FEATURE.md        (New: feature docs)
├── IMPLEMENTATION_SUMMARY.md  (New: summary)
└── VISUAL_GUIDE.md            (This file)
```

## Settings Storage

Settings are stored in OS-specific locations:
- **Linux**: `~/.config/ChessGame/Settings.conf`
- **Windows**: `HKEY_CURRENT_USER\Software\ChessGame\Settings`
- **macOS**: `~/Library/Preferences/com.ChessGame.Settings.plist`

Stored values:
```ini
[General]
undoEnabled=true
lightSquareColor=#F0D9B5
darkSquareColor=#B58863
volume=100
timeLimitMinutes=0
```

## Code Flow

```
User clicks Settings button
    ↓
onSettings() called
    ↓
SettingsDialog created and shown
    ↓
User adjusts settings
    ↓
User clicks OK
    ↓
saveSettings() writes to QSettings
    ↓
Dialog closes and returns Accepted
    ↓
loadSettings() reads from QSettings
    ↓
applySettings() applies to game:
    - Update button states
    - Update board colors
    - Update sound volumes
    - Start/stop timer
```

## Testing Checklist

- [ ] Settings dialog opens correctly
- [ ] Undo toggle works (enables/disables button)
- [ ] Light color picker works
- [ ] Dark color picker works
- [ ] Reset colors button works
- [ ] Volume slider works (0%, 50%, 100%)
- [ ] Time limit works (no limit, 1 min, 5 min)
- [ ] Timer displays correctly
- [ ] Timer changes color at thresholds
- [ ] Time expiry shows warning
- [ ] Settings persist after restart
- [ ] All settings save/load correctly

## Known Limitations

1. Timer does not auto-forfeit when time expires (by design)
2. No separate volume controls per sound type
3. No theme presets for popular color schemes
4. No keyboard shortcuts for settings dialog

These are potential future enhancements.
