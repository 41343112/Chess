#include "chessboard.h"
#include "chesspiece.h"
#include <iostream>

int main() {
    ChessBoard board;
    
    // Bishop is at position (2, 7) in QPoint format, meaning x=2, y=7
    // In board array terms, that's m_board[7][2] (row 7, col 2)
    
    ChessPiece* bishop = board.getPieceAt(7, 2);  // row=7, col=2
    std::cout << "Bishop at board[7][2]: " << bishop->getSymbol().toStdString() << "\n";
    std::cout << "Bishop position: " << bishop->getPosition().x() << "," << bishop->getPosition().y() << "\n";
    
    // Now check what happens when we call getPieceAt(currentX, currentY) 
    // during path checking
    // From (2,7) to (4,5): dx=2, dy=-2
    // stepX=1, stepY=-1
    // First check: currentX=3, currentY=6
    // When called as getPieceAt(currentX, currentY) = getPieceAt(3, 6)
    // This is interpreted as getPieceAt(row=3, col=6) which is m_board[3][6]
    
    std::cout << "\nWhat getPieceAt(3, 6) returns (row=3, col=6):\n";
    ChessPiece* p1 = board.getPieceAt(3, 6);
    if (p1) {
        std::cout << "Found: " << p1->getSymbol().toStdString() << "\n";
    } else {
        std::cout << "Empty\n";
    }
    
    std::cout << "\nWhat should be checked: getPieceAt(6, 3) (row=6, col=3):\n";
    ChessPiece* p2 = board.getPieceAt(6, 3);
    if (p2) {
        std::cout << "Found: " << p2->getSymbol().toStdString() << " (this is the pawn!)\n";
    } else {
        std::cout << "Empty\n";
    }
    
    return 0;
}
