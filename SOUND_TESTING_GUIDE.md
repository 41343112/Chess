# Sound Effects Testing Guide

## 修正無法輸出音效 - Testing Instructions

This guide helps you test and verify that the sound effects fix is working correctly.

## Prerequisites

### 1. Build Requirements
- Qt 5.12+ or Qt 6.x with Qt Multimedia module
- C++17 compatible compiler
- Audio output device (speakers or headphones)

### 2. Build the Application
```bash
cd Chess
qmake Chess.pro
make
```

### 3. Check Console Output
When you start the application, you should see debug messages in the console:
```
Move sound loaded successfully
Capture sound loaded successfully
Check sound loaded successfully
Checkmate sound loaded successfully
```

If you see error messages instead:
```
Failed to load move sound: qrc:/sounds/sounds/move.wav
```
This indicates a resource loading problem.

## Testing Each Sound Effect

### Test 1: Move Sound (移動音效)
**Expected**: Hear `move.wav` sound effect

**Steps**:
1. Start a new game
2. Move white pawn from e2 to e4 (click e2, then click e4)
3. **Listen**: You should hear a "move" sound

**Console Output**:
```
Playing move sound
```

### Test 2: Capture Sound (吃子音效)
**Expected**: Hear `capture.wav` sound effect

**Steps**:
1. Continue the game above
2. Move black pawn from d7 to d5
3. Move white pawn from e4 to capture d5 (exd5)
4. **Listen**: You should hear a "capture" sound (different from move sound)

**Console Output**:
```
Playing capture sound
```

### Test 3: Check Sound (攻擊國王音效)
**Expected**: Hear `check.wav` sound effect

**Setup**: This requires putting the opponent's king in check

**Steps**:
1. Start a new game
2. Play the following moves:
   - e4 (white pawn)
   - e5 (black pawn)
   - Qh5 (white queen moves to h5)
   - Nc6 (black knight)
   - Qxf7+ (white queen captures f7 pawn and checks king)
3. **Listen**: You should hear a "check" sound when Qxf7+ is played
4. Notice: Even though this is a capture, the check sound plays instead (priority system)

**Console Output**:
```
Playing check sound
```

### Test 4: Checkmate Sound (將殺音效)
**Expected**: Hear `checkmate.wav` sound effect

**Setup**: This is the famous "Fool's Mate"

**Steps**:
1. Start a new game
2. Play the following moves:
   - f3 (white pawn)
   - e5 (black pawn)
   - g4 (white pawn)
   - Qh4# (black queen delivers checkmate)
3. **Listen**: You should hear a "checkmate" sound (the most dramatic sound)

**Console Output**:
```
Playing checkmate sound
```

## Troubleshooting

### Issue: No Sound at All

**Check 1: System Volume**
- Ensure system volume is not muted
- Check that speakers/headphones are connected
- Test with other applications to verify audio works

**Check 2: Qt Multimedia Backend**
On Linux:
```bash
# Install required packages
sudo apt-get install libqt5multimedia5-plugins
sudo apt-get install gstreamer1.0-plugins-base gstreamer1.0-plugins-good
```

On macOS:
```bash
# Usually works out of the box, but ensure audio permissions are granted
```

On Windows:
```bash
# Ensure Qt multimedia DLLs are in the same directory as Chess.exe
# Required: Qt5Multimedia.dll (or Qt6Multimedia.dll)
```

**Check 3: Console Output**
Look for error messages like:
```
Failed to load move sound: qrc:/sounds/sounds/move.wav
```

If you see this, the resource file may not be properly compiled. Try:
```bash
make clean
qmake Chess.pro
make
```

**Check 4: Sound Status**
If you see:
```
Cannot play move sound - not ready: qrc:/sounds/sounds/move.wav status: 0
```

Status meanings:
- 0 = Null (not loaded)
- 1 = Loading (in progress)
- 2 = Ready (loaded successfully)
- 3 = Error (failed to load)

### Issue: Sounds Play but Are Too Quiet

The volume is set to 50% (0.5). To increase volume, edit `mychess.cpp`:
```cpp
void myChess::initializeSoundEffects() {
    m_moveSound->setVolume(0.8);  // Change from 0.5 to 0.8 (80%)
    // ... repeat for other sounds
}
```

### Issue: Sounds Repeat Multiple Times

This should not happen after the fix. If it does, verify:
```cpp
m_moveSound->setLoopCount(1);  // Should be 1, not QSoundEffect::Infinite
```

### Issue: Wrong Sound Plays

Verify the priority system is working:
1. Capture + Check → Should play Check sound
2. Capture + Checkmate → Should play Checkmate sound
3. Normal capture → Should play Capture sound
4. Normal move → Should play Move sound

Check console output to see which sound is actually being played.

## Sound File Information

All sound files are in WAV format:
- **move.wav**: 40 KB, 16-bit stereo, 48 kHz
- **capture.wav**: 51 KB, 16-bit stereo, 48 kHz
- **check.wav**: 92 KB, 16-bit stereo, 48 kHz
- **checkmate.wav**: 46 KB, 16-bit mono, 48 kHz

These files are embedded in the application binary via Qt resources.

## Advanced Debugging

### Enable More Verbose Logging

Set Qt debug environment variable before running:
```bash
# Linux/macOS
export QT_LOGGING_RULES="qt.multimedia*=true"
./Chess

# Windows
set QT_LOGGING_RULES=qt.multimedia*=true
Chess.exe
```

### Check Resource Compilation

Verify resources are compiled into the binary:
```bash
# Linux/macOS
rcc -list resources.qrc

# Should show:
# sounds/move.wav
# sounds/capture.wav
# sounds/check.wav
# sounds/checkmate.wav
```

### Test Sound Files Directly

Extract and test sound files:
```bash
# Extract resource to file
./Chess &  # Start app in background
# Use a Qt resource browser tool, or play files directly from sounds/ directory
aplay sounds/move.wav  # Linux
afplay sounds/move.wav  # macOS
```

## Expected Results Summary

✅ **All tests pass**: Sound effects work correctly
- Console shows "loaded successfully" messages on startup
- Console shows "Playing [type] sound" messages during gameplay
- Audio plays through speakers/headphones
- Correct sound plays for each action type

❌ **Tests fail**: Troubleshoot using the sections above

## Reporting Issues

If sound effects still don't work after following this guide, please report:

1. **Operating System**: Windows 10/11, macOS version, Linux distro
2. **Qt Version**: Run `qmake --version`
3. **Console Output**: Copy all messages from application startup
4. **Test Results**: Which specific tests failed
5. **Audio Hardware**: Type of output device (speakers, headphones, etc.)
6. **Other Applications**: Do other Qt multimedia applications work?

## References

- Qt QSoundEffect Documentation: https://doc.qt.io/qt-6/qsoundeffect.html
- Qt Resource System: https://doc.qt.io/qt-6/resources.html
- Qt Multimedia: https://doc.qt.io/qt-6/qtmultimedia-index.html
