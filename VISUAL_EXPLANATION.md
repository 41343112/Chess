# Visual Explanation: Drag Hotspot Fix

## The Problem (Before Fix)

When you clicked different parts of a chess piece, the drag icon would appear at different positions relative to your cursor:

```
Scenario 1: Click on TOP-LEFT corner
┌─────────────┐
│  User       │
│  clicks → ● │  (clicked here)
│             │
│    ♔        │  (piece icon)
│             │
└─────────────┘

During drag:
    ●───────────   (cursor here)
    ↓
┌─────────────┐
│    ♔        │   (piece appears with top-left at cursor)
└─────────────┘


Scenario 2: Click on CENTER
┌─────────────┐
│             │
│    ♔ ● ←────  (clicked here)
│             │
└─────────────┘

During drag:
       ●        (cursor here)
       ↓
┌─────────────┐
│    ♔        │   (piece appears centered at cursor)
└─────────────┘


Scenario 3: Click on BOTTOM-RIGHT corner
┌─────────────┐
│    ♔        │
│             │
│          ● ←  (clicked here)
└─────────────┘

During drag:
                ●  (cursor here)
                ↓
          ┌─────────────┐
          │    ♔        │   (piece appears with bottom-right at cursor)
          └─────────────┘

PROBLEM: Inconsistent! The piece jumps to different positions depending on where you click.
```

## The Solution (After Fix)

Now, no matter where you click on the piece, the drag icon ALWAYS centers on your cursor:

```
Scenario 1: Click on TOP-LEFT corner
┌─────────────┐
│  User       │
│  clicks → ● │  (clicked here)
│             │
│    ♔        │
│             │
└─────────────┘

During drag:
       ●        (cursor here)
       ↓
┌─────────────┐
│    ♔        │   (piece centered at cursor!)
└─────────────┘


Scenario 2: Click on CENTER
┌─────────────┐
│             │
│    ♔ ● ←────  (clicked here)
│             │
└─────────────┘

During drag:
       ●        (cursor here)
       ↓
┌─────────────┐
│    ♔        │   (piece centered at cursor!)
└─────────────┘


Scenario 3: Click on BOTTOM-RIGHT corner
┌─────────────┐
│    ♔        │
│             │
│          ● ←  (clicked here)
└─────────────┘

During drag:
       ●        (cursor here)
       ↓
┌─────────────┐
│    ♔        │   (piece centered at cursor!)
└─────────────┘

SOLUTION: Consistent! The piece always appears centered, providing predictable feedback.
```

## Technical Implementation

```cpp
// BEFORE: Hotspot at click position
drag->setHotSpot(event->pos());
// event->pos() = where user clicked relative to button
// Example: User clicks at (10, 5) on button → hotspot is (10, 5)
// Result: Top-left of drag pixmap offset by (10, 5)

// AFTER: Hotspot at center of pixmap
drag->setHotSpot(QPoint(dragPixmap.width() / 2, dragPixmap.height() / 2));
// Always calculates center of the pixmap
// Example: 60x60 pixmap → hotspot is (30, 30) = center
// Result: Center of drag pixmap always at cursor
```

## Visual Comparison

```
BEFORE (Inconsistent):
Click top:     ●─[♔]     Click center:    [●]     Click bottom:  [♔]─●
               ↑                           ↑                         ↑
          cursor here                cursor here                cursor here

AFTER (Consistent):
Click top:      [●]      Click center:    [●]     Click bottom:  [●]
                ↑                          ↑                        ↑
           cursor here               cursor here               cursor here
           piece centered            piece centered            piece centered
```

## User Experience Benefits

1. **Predictability**: User always knows where the piece will appear
2. **Professionalism**: Matches behavior of commercial chess apps (Chess.com, Lichess)
3. **Intuitiveness**: Center-centering feels natural and balanced
4. **Precision**: Easier to see where the piece will land on the target square

## Real-World Analogy

Think of picking up a physical chess piece:

**Before Fix:** Like picking up a piece by different corners - it wobbles and moves awkwardly
**After Fix:** Like picking up a piece by its center - it stays balanced and moves smoothly

The fix makes the digital experience feel more like the physical experience!
