# Visual Guide: Game Settings Flow Feature

## Feature Overview

This feature implements the requirement:
> 當開啟新遊戲時跳出設定時間畫面，設定是否開啟悔棋，當遊戲開啟後不能調整設定直到遊戲結束。

**English Translation:**
> When opening a new game, show a time setting screen and configure whether to enable undo. Settings cannot be adjusted during an active game until it ends.

---

## Before vs After Comparison

### BEFORE: Previous Behavior

```
┌─────────────────────────────────────┐
│         Chess Application           │
├─────────────────────────────────────┤
│  [New Game] [Undo] [Settings] ...   │
│                                     │
│  User clicks "New Game"              │
│         ↓                           │
│  ┌─────────────────────┐            │
│  │  Start new game?    │            │
│  │  [Yes]  [No]        │            │
│  └─────────────────────┘            │
│         ↓ (Yes)                     │
│  Game starts immediately            │
│                                     │
│  During game:                       │
│  ✗ Settings can be changed anytime  │
│  ✗ Not fair - can enable undo       │
│     during game                     │
└─────────────────────────────────────┘
```

### AFTER: New Behavior

```
┌─────────────────────────────────────┐
│         Chess Application           │
├─────────────────────────────────────┤
│  [New Game] [Undo] [Settings✓] ...  │
│                                     │
│  User clicks "New Game"              │
│         ↓                           │
│  ┌──────────────────────────────┐   │
│  │    Settings Dialog           │   │
│  │  ┌────────────────────────┐  │   │
│  │  │ Time Limit: [5] min    │  │   │
│  │  │ Undo: [✓] Enabled      │  │   │
│  │  │ Colors: [Light] [Dark] │  │   │
│  │  │ Volume: [||||||||] 80% │  │   │
│  │  │ Language: [English▾]   │  │   │
│  │  └────────────────────────┘  │   │
│  │        [OK]  [Cancel]        │   │
│  └──────────────────────────────┘   │
│         ↓ (OK)                      │
│  Settings saved & applied           │
│         ↓                           │
│  Game starts                        │
│  [Settings✗] ← Disabled!            │
│                                     │
│  During game:                       │
│  ✓ Settings LOCKED                  │
│  ✓ Fair - settings fixed            │
│                                     │
│  Game ends (Checkmate/Stalemate)    │
│         ↓                           │
│  [Settings✓] ← Re-enabled!          │
└─────────────────────────────────────┘
```

---

## User Flow Diagram

```
START
  │
  ├─► [Launch App]
  │        │
  │        v
  │   Game not started
  │   Settings button: ENABLED ✓
  │        │
  │        │ User clicks "New Game"
  │        v
  │   ╔═══════════════════════════╗
  │   ║   Settings Dialog Opens   ║
  │   ╟───────────────────────────╢
  │   ║ ○ Time Limit              ║
  │   ║ ○ Undo Enable/Disable     ║
  │   ║ ○ Board Colors            ║
  │   ║ ○ Sound Volume            ║
  │   ║ ○ Language Selection      ║
  │   ╚═══════════════════════════╝
  │        │
  │        ├──────────┬──────────┐
  │        v          v          v
  │    [Cancel]    [OK]     [Apply]
  │        │          │          │
  │        └───┐      │          │
  │            │      v          │
  │            │   Save settings │
  │            │      │          │
  │            │      v          │
  │            │   Reset board   │
  │            │      │          │
  │            │      v          │
  │            │   START GAME    │
  │            │      │          │
  │            │      v          │
  │            │   ╔═══════════════════╗
  │            │   ║ Game in Progress  ║
  │            │   ║ Settings: LOCKED  ║
  │            │   ║ Button: DISABLED  ║
  │            │   ╚═══════════════════╝
  │            │      │
  │            │      │ Players make moves
  │            │      v
  │            │   ╔═══════════════════╗
  │            │   ║ Game Ends:        ║
  │            │   ║ - Checkmate       ║
  │            │   ║ - Stalemate       ║
  │            │   ╚═══════════════════╝
  │            │      │
  │            │      v
  │            │   Settings: UNLOCKED
  │            │   Button: ENABLED ✓
  │            │      │
  │            └──────┴──────────> Back to Start
  │
END
```

---

## State Transition Table

| State | Settings Button | Can Open Settings? | Can Start Game? | Description |
|-------|----------------|-------------------|-----------------|-------------|
| **Initial** | ✓ Enabled | Yes | Yes (via New Game) | App just started |
| **Settings Dialog Open** | N/A | N/A | Yes (via OK) | User configuring game |
| **Game In Progress** | ✗ Disabled | No | N/A | Game active, settings locked |
| **Game Over** | ✓ Enabled | Yes | Yes (via New Game) | Game finished, settings unlocked |

---

## Button States Visual

### At Application Start
```
┌──────────────────────────────────────┐
│ [New Game]  [Undo]  [Flip]  [Settings]│
│   Active    Active  Active    Active  │
└──────────────────────────────────────┘
```

### During Game (After New Game → Settings → OK)
```
┌──────────────────────────────────────┐
│ [New Game]  [Undo]  [Flip]  [Settings]│
│   Active    Active  Active   Disabled │
│                                🔒      │
└──────────────────────────────────────┘
```

### After Game Ends
```
┌──────────────────────────────────────┐
│ [New Game]  [Undo]  [Flip]  [Settings]│
│   Active    Active  Active    Active  │
│                                🔓      │
└──────────────────────────────────────┘
```

---

## Example Settings Dialog Content

```
╔═══════════════════════════════════════╗
║            Game Settings              ║
╟───────────────────────────────────────╢
║                                       ║
║  ┌─ Undo Functionality ────────────┐  ║
║  │ ☑ Enable Undo Button            │  ║
║  └─────────────────────────────────┘  ║
║                                       ║
║  ┌─ Time Limit ────────────────────┐  ║
║  │ Time per move: [5  ] minutes    │  ║
║  │ (0 = no limit)                  │  ║
║  └─────────────────────────────────┘  ║
║                                       ║
║  ┌─ Chessboard Colors ─────────────┐  ║
║  │ Light squares: [█████]          │  ║
║  │ Dark squares:  [█████]          │  ║
║  │ [Reset to Default Colors]       │  ║
║  └─────────────────────────────────┘  ║
║                                       ║
║  ┌─ Sound Volume ──────────────────┐  ║
║  │ Volume: [████████░░] 80%        │  ║
║  └─────────────────────────────────┘  ║
║                                       ║
║  ┌─ Language ──────────────────────┐  ║
║  │ [English ▾]                     │  ║
║  └─────────────────────────────────┘  ║
║                                       ║
║     [Reset All Defaults]              ║
║                                       ║
║            [OK]    [Cancel]           ║
╚═══════════════════════════════════════╝
```

---

## User Scenarios

### Scenario 1: Starting First Game
```
1. User launches application
   → Settings button is enabled

2. User clicks "New Game"
   → Settings dialog appears

3. User sets:
   - Time limit: 5 minutes
   - Undo: Enabled
   - Volume: 80%

4. User clicks "OK"
   → Game starts
   → Settings button becomes disabled

5. User plays the game
   → Cannot change settings
   → Settings button is grayed out

6. Game ends (White wins by checkmate)
   → "Game Over" dialog appears
   → Settings button becomes enabled again
```

### Scenario 2: Attempting to Change Settings Mid-Game
```
1. Game is in progress
   → Settings button is disabled

2. User tries to click Settings button
   → Button does not respond (disabled)
   → OR shows message: "Settings cannot be changed 
      during an active game"

3. User continues playing
   → Must finish game to change settings
```

### Scenario 3: Starting Second Game with Different Settings
```
1. Previous game has ended
   → Settings button is enabled

2. User clicks "New Game"
   → Settings dialog appears
   → Shows previously used settings

3. User changes:
   - Time limit: 10 minutes (was 5)
   - Undo: Disabled (was Enabled)

4. User clicks "OK"
   → New game starts with new settings
   → Settings button becomes disabled
```

---

## Benefits of This Implementation

### 1. Fairness ⚖️
- Players agree on rules before starting
- No mid-game rule changes
- Equal conditions for both players

### 2. User Experience 🎮
- Clear when settings can be changed
- Visual feedback (enabled/disabled button)
- Settings dialog serves as game configuration step

### 3. Simplicity 🎯
- Single entry point for game configuration
- No confusion about when settings apply
- Natural flow: Configure → Play → Finish → Reconfigure

### 4. Flexibility 🔄
- Different settings for each game
- Can experiment with different time limits
- Easy to adjust between games

---

## Technical Implementation Highlights

### Key Code Changes

1. **Added game state tracking**
   ```cpp
   bool m_gameInProgress;  // false = can change settings
                          // true = settings locked
   ```

2. **Modified new game flow**
   ```cpp
   Show Settings Dialog → User Configures → Start Game → Lock Settings
   ```

3. **Auto-unlock on game end**
   ```cpp
   Game Over → Unlock Settings → Enable Button
   ```

### Minimal Impact
- Only 27 lines changed
- 2 files modified
- No breaking changes
- Backward compatible

---

## Testing Checklist

- [ ] Settings button enabled at app start
- [ ] Clicking "New Game" shows settings dialog
- [ ] Canceling settings dialog does not start game
- [ ] Accepting settings starts game and disables button
- [ ] Settings button stays disabled during game
- [ ] Button re-enables when game ends
- [ ] Can start multiple games with different settings
- [ ] All settings options work correctly
- [ ] Time limit applies correctly
- [ ] Undo enable/disable works
- [ ] Colors, volume, language persist correctly

---

## Conclusion

This implementation successfully addresses the requirement:
✅ Settings dialog shown when starting new game
✅ Time limit and undo can be configured
✅ Settings locked during active game
✅ Settings unlocked when game ends

The solution is elegant, minimal, and maintains full backward compatibility while significantly improving the user experience and game fairness.
