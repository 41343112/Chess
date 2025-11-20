#!/bin/bash
# Simple build verification script for Chess project with AI

echo "=== Chess Build Verification Script ==="
echo ""

# Check for required files
echo "Checking for required files..."
FILES=(
    "chessai.h"
    "chessai.cpp"
    "startdialog.h"
    "startdialog.cpp"
    "mychess.h"
    "mychess.cpp"
    "Chess.pro"
)

for file in "${FILES[@]}"; do
    if [ -f "$file" ]; then
        echo "✓ $file exists"
    else
        echo "✗ $file missing"
        exit 1
    fi
done

echo ""
echo "Verifying Chess.pro includes AI files..."
if grep -q "chessai.cpp" Chess.pro && grep -q "chessai.h" Chess.pro; then
    echo "✓ Chess.pro includes AI files"
else
    echo "✗ Chess.pro missing AI files"
    exit 1
fi

echo ""
echo "Checking key AI functions..."
if grep -q "getBestMove" chessai.h && grep -q "getRandomMove" chessai.cpp; then
    echo "✓ AI functions present"
else
    echo "✗ AI functions missing"
    exit 1
fi

echo ""
echo "Checking StartDialog extensions..."
if grep -q "GameMode" startdialog.h && grep -q "ComputerDifficulty" startdialog.h; then
    echo "✓ StartDialog has game mode and difficulty support"
else
    echo "✗ StartDialog missing required enums"
    exit 1
fi

echo ""
echo "Checking myChess AI integration..."
if grep -q "m_chessAI" mychess.h && grep -q "makeComputerMove" mychess.h; then
    echo "✓ myChess has AI integration"
else
    echo "✗ myChess missing AI integration"
    exit 1
fi

echo ""
echo "Checking documentation..."
if [ -f "docs/features/COMPUTER_OPPONENT.md" ]; then
    echo "✓ Documentation exists"
else
    echo "✗ Documentation missing"
    exit 1
fi

echo ""
echo "=== All verifications passed! ==="
echo ""
echo "To build the project, run:"
echo "  qmake Chess.pro"
echo "  make"
echo ""
echo "Note: Qt5 or Qt6 development tools must be installed."
