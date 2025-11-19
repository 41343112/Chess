# Visual Guide - Settings Dialog New Features

## How to Access the New Features

### Step 1: Open Settings
Click the **Settings** button in the main window

```
┌────────────────────────────────────────┐
│  Chess Game - Like Chess.com          │
├────────────────────────────────────────┤
│                                        │
│         [Chess Board]                  │
│                                        │
├────────────────────────────────────────┤
│  [New Game]  [Undo]  [Flip Board]     │
│              [Settings] ← Click here   │
└────────────────────────────────────────┘
```

### Step 2: Settings Dialog Opens

```
┌──────────────────────────────────────────────┐
│               Settings                        │
├──────────────────────────────────────────────┤
│                                               │
│  ┌─ Undo Functionality ──────────────────┐   │
│  │ ☑ Enable Undo Button                  │   │
│  └────────────────────────────────────────┘   │
│                                               │
│  ┌─ Chessboard Colors ───────────────────┐   │
│  │ Light Squares: [░░░░░ Choose Color ]  │   │
│  │ Dark Squares:  [▓▓▓▓▓ Choose Color ]  │   │
│  │ [Reset to Default Colors]             │   │
│  └────────────────────────────────────────┘   │
│                                               │
│  ┌─ Sound Volume ─────────────────────────┐   │
│  │ Volume: ━━━━━●━━━━━ 100%              │   │
│  └────────────────────────────────────────┘   │
│                                               │
│  ┌─ Time Limit ───────────────────────────┐   │
│  │ Minutes per move: [0] minutes          │   │
│  │ Set time limit per move (0 = no limit) │   │
│  └────────────────────────────────────────┘   │
│                                               │
│  ┌─ Language ──────────────────────── ⭐ NEW │
│  │ Select Language: [English      ▼]     │   │
│  │ Restart required for language changes │   │
│  └────────────────────────────────────────┘   │
│                                               │
│  ┌────────────────────────────────────────┐   │
│  │   Reset All Settings to Default        │   │ ⭐ NEW
│  │         (Light Yellow Button)          │   │
│  └────────────────────────────────────────┘   │
│                                               │
│               [OK]    [Cancel]                │
│                                               │
└──────────────────────────────────────────────┘
```

## Feature 1: Language Selection

### How to Change Language

1. **Find the Language section** (near the bottom, above Reset button)

```
┌─ Language ──────────────────────────────────┐
│ Select Language: [English      ▼]           │
│ Restart required for language changes       │
└──────────────────────────────────────────────┘
```

2. **Click the dropdown** to see available languages

```
┌─ Language ──────────────────────────────────┐
│ Select Language: ┌─────────────┐            │
│                  │ English  ◀─ │ Current    │
│                  │ 中文        │            │
│                  └─────────────┘            │
│ Restart required for language changes       │
└──────────────────────────────────────────────┘
```

3. **Select your preferred language**

```
┌─ Language ──────────────────────────────────┐
│ Select Language: [中文          ▼] ✓ Changed│
│ Restart required for language changes       │
└──────────────────────────────────────────────┘
```

4. **Click OK** to save

5. **You will see this notification:**

```
┌────────────────────────────────────┐
│         Language Changed           │
├────────────────────────────────────┤
│ Please restart the application for │
│ language changes to take effect.   │
│                                    │
│              [OK]                  │
└────────────────────────────────────┘
```

## Feature 2: Reset to Defaults

### How to Reset All Settings

1. **Find the Reset button** (highlighted in light yellow)

```
┌──────────────────────────────────────────────┐
│  ... other settings above ...                │
│                                               │
│  ┌────────────────────────────────────────┐   │
│  │   Reset All Settings to Default        │   │
│  │   (This button has light yellow bg)    │   │ ← Click here
│  └────────────────────────────────────────┘   │
│                                               │
│               [OK]    [Cancel]                │
└──────────────────────────────────────────────┘
```

2. **Click the button** - A confirmation dialog appears:

```
┌────────────────────────────────────┐
│       Reset to Defaults            │
├────────────────────────────────────┤
│ Are you sure you want to reset    │
│ all settings to their default     │
│ values?                            │
│                                    │
│          [Yes]    [No]             │
└────────────────────────────────────┘
```

3. **Click Yes** to confirm

4. **All settings immediately reset** to defaults:

```
Before Reset:                After Reset:
Undo: ☐ Disabled           → Undo: ☑ Enabled
Light: #AABBCC             → Light: #F0D9B5
Dark: #112233              → Dark: #B58863
Volume: 50%                → Volume: 100%
Time: 5 minutes            → Time: 0 minutes
Language: 中文             → Language: English
```

## Default Values Reference

When you click "Reset All Settings to Default", these values are applied:

| Setting | Default Value |
|---------|--------------|
| Undo Enabled | ✓ Yes (Checked) |
| Light Square Color | #F0D9B5 (Beige) |
| Dark Square Color | #B58863 (Brown) |
| Volume | 100% |
| Time Limit | 0 minutes (No limit) |
| Language | English |

## Quick Reference

### Language Options
- **English** - Full UI in English
- **中文** - Full UI in Chinese (requires restart)

### Reset Button Color
- **#FFE4B5** - Moccasin (Light Yellow)
- Makes the button easy to find
- Indicates it's a special action

### Confirmation Required
The reset button shows a confirmation dialog to prevent accidental clicks:
- **Yes** - Resets all settings immediately
- **No** - Cancels the operation, no changes made

## Tips

💡 **Language Selection:**
- Choose your language before customizing other settings
- Remember to restart the application after changing language
- Your other settings will be preserved

💡 **Reset to Defaults:**
- Use this if you've made changes and want to start over
- The confirmation dialog protects against accidental resets
- After resetting, you can customize again
- You can still click Cancel to discard the reset

💡 **Settings Persistence:**
- All settings are automatically saved when you click OK
- Settings are stored in system configuration
- Settings persist across application restarts
- Each user can have their own settings

## Keyboard Shortcuts (Standard Qt)
- **Enter** - Confirm/OK
- **Escape** - Cancel
- **Tab** - Navigate between controls
- **Space** - Click focused button

## Troubleshooting

### Language doesn't change after selecting
✅ **Solution:** Remember to restart the application

### Reset button doesn't work
✅ **Solution:** You must click "Yes" in the confirmation dialog

### Settings not saved
✅ **Solution:** Make sure to click "OK", not "Cancel"

### Can't find the new features
✅ **Solution:** Scroll down in the Settings dialog to see Language and Reset button at the bottom

## Summary

Both new features are located in the **Settings Dialog**:

1. **Language Selection**
   - Location: Second to last section
   - Type: Dropdown menu
   - Options: English / 中文
   - Action: Requires restart

2. **Reset to Defaults**
   - Location: Last item before OK/Cancel
   - Type: Button (light yellow)
   - Action: Immediate with confirmation
   - Resets: ALL settings

Enjoy your customized Chess experience! 🎮♟️
