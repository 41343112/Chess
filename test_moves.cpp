#include "chessboard.h"
#include "chesspiece.h"
#include <iostream>

int main() {
    ChessBoard board;
    
    // Test 1: Check if a rook at (0,7) can move to (1,7) which has a friendly knight
    std::cout << "Test 1: White rook at (0,7) trying to move to (1,7) with friendly knight\n";
    ChessPiece* rook = board.getPieceAt(7, 0);  // White rook
    ChessPiece* knight = board.getPieceAt(7, 1);  // White knight
    
    if (rook) {
        std::cout << "Rook found at (0,7): " << rook->getSymbol().toStdString() << "\n";
    }
    if (knight) {
        std::cout << "Knight found at (1,7): " << knight->getSymbol().toStdString() << "\n";
    }
    
    QPoint from(0, 7);
    QPoint to(1, 7);
    bool canMove = board.canMove(from, to);
    std::cout << "Can white rook move to friendly knight? " << (canMove ? "YES (BUG!)" : "NO (correct)") << "\n\n";
    
    // Test 2: Check if a rook can move to an empty square
    std::cout << "Test 2: White rook at (0,7) trying to move to (0,5) which is empty\n";
    QPoint to2(0, 5);
    bool canMove2 = board.canMove(from, to2);
    std::cout << "Can white rook move to empty square? " << (canMove2 ? "YES (correct)" : "NO (BUG!)") << "\n\n";
    
    // Test 3: Check if bishop can move through pieces
    std::cout << "Test 3: White bishop at (2,7) trying to move to (4,5) (blocked by pawn at 3,6)\n";
    QPoint bishopFrom(2, 7);
    QPoint bishopTo(4, 5);
    bool canMove3 = board.canMove(bishopFrom, bishopTo);
    std::cout << "Can white bishop move through pawn? " << (canMove3 ? "YES (BUG!)" : "NO (correct)") << "\n";
    
    return 0;
}
