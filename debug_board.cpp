#include "chessboard.h"
#include "chesspiece.h"
#include <iostream>

int main() {
    ChessBoard board;
    
    std::cout << "Board layout (row, col):\n";
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            ChessPiece* piece = board.getPieceAt(row, col);
            if (piece) {
                std::cout << piece->getSymbol().toStdString() << " ";
            } else {
                std::cout << ". ";
            }
        }
        std::cout << "  (row " << row << ")\n";
    }
    std::cout << "  0 1 2 3 4 5 6 7  (cols)\n\n";
    
    // Now test with QPoint
    std::cout << "Testing QPoint(0, 7):\n";
    ChessPiece* p1 = board.getPieceAt(QPoint(0, 7));
    if (p1) {
        std::cout << "Found: " << p1->getSymbol().toStdString() << " at position " 
                  << p1->getPosition().x() << "," << p1->getPosition().y() << "\n";
    }
    
    std::cout << "\nTesting board.getPieceAt(7, 0):\n";
    ChessPiece* p2 = board.getPieceAt(7, 0);
    if (p2) {
        std::cout << "Found: " << p2->getSymbol().toStdString() << " at position " 
                  << p2->getPosition().x() << "," << p2->getPosition().y() << "\n";
    }
    
    return 0;
}
