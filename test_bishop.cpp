#include "chessboard.h"
#include "chesspiece.h"
#include <iostream>

int main() {
    ChessBoard board;
    
    std::cout << "Initial board:\n";
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            ChessPiece* piece = board.getPieceAt(row, col);
            if (piece) {
                std::cout << piece->getSymbol().toStdString() << " ";
            } else {
                std::cout << ". ";
            }
        }
        std::cout << "\n";
    }
    
    // White bishop is at row 7, col 2
    std::cout << "\nWhite bishop at (2,7) trying to move diagonally:\n";
    ChessPiece* bishop = board.getPieceAt(7, 2);
    if (bishop) {
        std::cout << "Bishop: " << bishop->getSymbol().toStdString() << " at " 
                  << bishop->getPosition().x() << "," << bishop->getPosition().y() << "\n";
    }
    
    // Check pieces along the diagonal path from (2,7) to (4,5)
    // Path: (2,7) -> (3,6) -> (4,5)
    std::cout << "\nChecking diagonal path:\n";
    std::cout << "At (3,6): ";
    ChessPiece* p1 = board.getPieceAt(6, 3);
    if (p1) {
        std::cout << p1->getSymbol().toStdString() << " (pawn)\n";
    } else {
        std::cout << "empty\n";
    }
    
    std::cout << "At (4,5): ";
    ChessPiece* p2 = board.getPieceAt(5, 4);
    if (p2) {
        std::cout << p2->getSymbol().toStdString() << "\n";
    } else {
        std::cout << "empty\n";
    }
    
    // Test the move
    QPoint from(2, 7);
    QPoint to(4, 5);
    std::cout << "\nTesting bishop->isValidMove(QPoint(4,5), &board):\n";
    bool valid = bishop->isValidMove(to, &board);
    std::cout << "Result: " << (valid ? "TRUE" : "FALSE") << "\n";
    
    std::cout << "\nTesting board.canMove(QPoint(2,7), QPoint(4,5)):\n";
    bool canMove = board.canMove(from, to);
    std::cout << "Result: " << (canMove ? "TRUE" : "FALSE") << "\n";
    
    return 0;
}
