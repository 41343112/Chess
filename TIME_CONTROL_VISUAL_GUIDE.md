# Time Control Visual Guide

## UI Layout

```
┌─────────────────────────────────────────────────────────┐
│  Chess Game - Like Chess.com                            │
├─────────────────────────────────────────────────────────┤
│  Turn: White                     Game in progress       │
│                                                          │
│  White: 10:00                           Black: 10:00    │ ← NEW!
│  ↑ Countdown                            Countdown ↑     │
│                                                          │
│  ┌─────────────────────────────────────────────────┐   │
│  │                                                   │   │
│  │                Chess Board (8x8)                 │   │
│  │                                                   │   │
│  │            [Pieces displayed here]               │   │
│  │                                                   │   │
│  └─────────────────────────────────────────────────┘   │
│                                                          │
│  [<| Back]  [< Prev]  [Next >]  [Current |>]           │
│                                                          │
│  [New Game] [Undo] [Flip Board] [Settings]             │
└─────────────────────────────────────────────────────────┘
```

## Settings Dialog - Time Control Section

```
┌──────────────────────────────────────┐
│  Settings                            │
├──────────────────────────────────────┤
│                                      │
│  ┌─ Undo Functionality ──────────┐  │
│  │ ☑ Enable Undo Button          │  │
│  └────────────────────────────────┘  │
│                                      │
│  ┌─ Chessboard Colors ───────────┐  │
│  │ Light Squares: [Color Button] │  │
│  │ Dark Squares:  [Color Button] │  │
│  │ [Reset to Default Colors]     │  │
│  └────────────────────────────────┘  │
│                                      │
│  ┌─ Language ────────────────────┐  │
│  │ Select Language: [English ▼]  │  │
│  │ Restart required for changes  │  │
│  └────────────────────────────────┘  │
│                                      │
│  ┌─ Time Control ────────────────┐  │  ← NEW!
│  │ ☑ Enable Time Control         │  │
│  │                               │  │
│  │ Total Time per Player:        │  │
│  │ (minutes): [10 ▲▼]            │  │
│  └────────────────────────────────┘  │
│                                      │
│  [Reset All Settings to Default]    │
│                                      │
│  [OK]                    [Cancel]   │
└──────────────────────────────────────┘
```

## Timer State Machine

```
                    ┌──────────────┐
                    │  Game Start  │
                    └──────┬───────┘
                           │
                           ↓
                   ┌───────────────┐
                   │ Reset Timers  │
                   │ White: 10:00  │
                   │ Black: 10:00  │
                   └───────┬───────┘
                           │
                           ↓
              ┌────────────────────────┐
              │ Start Timer (if enabled)│
              └────────┬────────────────┘
                       │
           ┌───────────┴───────────┐
           │                       │
           ↓                       ↓
    ┌──────────┐           ┌──────────┐
    │  White's │           │  Black's │
    │   Turn   │──Move─→   │   Turn   │──Move─┐
    │          │           │          │        │
    │ Timer    │           │ Timer    │        │
    │ Counting │           │ Counting │        │
    └────┬─────┘           └────┬─────┘        │
         │                      │               │
         │ Timeout              │ Timeout       │
         ↓                      ↓               │
    ┌─────────────────────────────────┐        │
    │  Game Over - Timeout            │        │
    │  Show Winner                    │        │
    │  Stop Timer                     │        │
    └─────────────────────────────────┘        │
                                                │
                                                │
         ┌──────────────────────────────────────┘
         │
         ↓
    ┌──────────────┐
    │  Next Turn   │
    └──────────────┘
```

## Timer Pause/Resume Flow

```
    Game Playing               History Viewing              Back to Current
    ┌──────────┐              ┌──────────┐                ┌──────────┐
    │  Timer   │──View────→   │  Timer   │─────Resume──→  │  Timer   │
    │  Running │   History    │  Paused  │                │  Running │
    └──────────┘              └──────────┘                └──────────┘
         ↑                                                      │
         │                                                      │
         └──────────────────────────────────────────────────────┘
```

## Time Display Format

```
Timer Display Examples:

Initial:     White: 10:00    Black: 10:00
After 30s:   White: 09:30    Black: 10:00
After move:  White: 09:30    Black: 09:58  (Black's turn, counting)
Low time:    White: 00:45    Black: 02:15
Critical:    White: 00:05    Black: 00:12

Format: MM:SS (Minutes:Seconds)
Always shows 2 digits for each component
Example: 01:05 = 1 minute 5 seconds
```

## Feature Interactions

```
┌─────────────────────────────────────────────────────────┐
│  Feature: Time Control                                  │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  Enabled via Settings:                                  │
│    ☑ Enable Time Control                               │
│    Total Time: [1-180] minutes                         │
│                                                          │
│  Interacts with:                                        │
│    • New Game     → Reset timers, start countdown      │
│    • Move Made    → Switch timer to other player       │
│    • Undo         → Timer continues for current player │
│    • View History → Pause timer                        │
│    • Back Current → Resume timer                       │
│    • Game Over    → Stop timer                         │
│    • Timeout      → End game, show winner              │
│                                                          │
│  Display:                                               │
│    • Visible only when enabled                         │
│    • White timer on left, Black timer on right         │
│    • Bold font, large size (16pt)                      │
│    • Updates every second                              │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

## Code Structure

```
settingsdialog.h/cpp
├── UI Components
│   ├── m_timeControlCheckBox (Enable/Disable)
│   └── m_timeControlSpinBox (Minutes 1-180)
├── Methods
│   ├── isTimeControlEnabled() → bool
│   ├── getTimeControlMinutes() → int
│   └── onTimeControlCheckChanged(int) → void
└── Settings Persistence
    ├── loadSettings() - Load from QSettings
    └── saveSettings() - Save to QSettings

mychess.h/cpp
├── UI Components
│   ├── m_whiteTimeLabel (Display white's time)
│   └── m_blackTimeLabel (Display black's time)
├── Timer Components
│   ├── m_gameTimer (QTimer for countdown)
│   ├── m_whiteTimeRemaining (seconds)
│   ├── m_blackTimeRemaining (seconds)
│   └── m_isTimerRunning (bool flag)
├── Settings
│   ├── m_timeControlEnabled (bool)
│   └── m_timeControlMinutes (int)
└── Methods
    ├── onTimerTick() - Countdown and check timeout
    ├── updateTimeDisplay() - Update labels
    ├── startTimer() - Begin countdown
    ├── stopTimer() - Pause countdown
    ├── resetTimers() - Initialize to starting time
    └── formatTime(int) - Convert seconds to MM:SS
```
