#include "mychess.h"
#include "ui_mychess.h"
#include <QApplication>

// ChessSquare implementation
ChessSquare::ChessSquare(int row, int col, QWidget* parent)
    : QPushButton(parent), m_row(row), m_col(col), 
      m_isHighlighted(false), m_isSelected(false), m_isInCheck(false) {
    m_isLight = (row + col) % 2 == 0;
    setFixedSize(70, 70);
    setFont(QFont("Arial", 32));
    updateStyle();
    setAcceptDrops(true);
}

void ChessSquare::setPiece(ChessPiece* piece) {
    if (piece != nullptr) {
        setText(piece->getSymbol());
    } else {
        setText("");
    }
}

void ChessSquare::setHighlight(bool highlight) {
    m_isHighlighted = highlight;
    updateStyle();
}

void ChessSquare::setSelected(bool selected) {
    m_isSelected = selected;
    updateStyle();
}

void ChessSquare::setInCheck(bool inCheck) {
    m_isInCheck = inCheck;
    updateStyle();
}

void ChessSquare::updateStyle() {
    QString baseColor = m_isLight ? "#F0D9B5" : "#B58863";
    QString selectedColor = "#FFD700";
    QString highlightColor = "#90EE90";
    QString checkColor = "#FF6B6B";  // Red color for check
    
    QString bgColor = baseColor;
    if (m_isInCheck) {
        bgColor = checkColor;
    } else if (m_isSelected) {
        bgColor = selectedColor;
    } else if (m_isHighlighted) {
        bgColor = highlightColor;
    }
    
    setStyleSheet(QString("QPushButton { background-color: %1; border: 1px solid #000; }").arg(bgColor));
}

void ChessSquare::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
    }
    QPushButton::mousePressEvent(event);
}

void ChessSquare::mouseMoveEvent(QMouseEvent* event) {
    if (!(event->buttons() & Qt::LeftButton)) {
        return;
    }
    
    if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
        return;
    }
    
    // Only allow dragging if there's a piece on this square
    if (text().isEmpty()) {
        return;
    }
    
    QDrag* drag = new QDrag(this);
    QMimeData* mimeData = new QMimeData;
    
    // Store the source position in the mime data
    mimeData->setText(QString("%1,%2").arg(m_row).arg(m_col));
    drag->setMimeData(mimeData);
    
    // Create a pixmap showing the piece being dragged
    QPixmap pixmap(size());
    pixmap.fill(Qt::transparent);
    render(&pixmap);
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos());
    
    // Emit signal that drag started
    myChess* parent = qobject_cast<myChess*>(window());
    if (parent) {
        parent->onSquareDragStarted(m_row, m_col);
    }
    
    Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
    
    QPushButton::mouseMoveEvent(event);
}

void ChessSquare::mouseReleaseEvent(QMouseEvent* event) {
    QPushButton::mouseReleaseEvent(event);
}

void ChessSquare::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void ChessSquare::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasText()) {
        QString sourceData = event->mimeData()->text();
        QStringList coords = sourceData.split(',');
        if (coords.size() == 2) {
            int sourceRow = coords[0].toInt();
            int sourceCol = coords[1].toInt();
            
            // Notify parent to handle the move
            myChess* parent = qobject_cast<myChess*>(window());
            if (parent) {
                parent->onSquareDragEnded(m_row, m_col);
            }
            
            event->acceptProposedAction();
        }
    }
}

// myChess implementation
myChess::myChess(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::myChess)
    , m_hasSelection(false)
    , m_selectedSquare(-1, -1)
{
    ui->setupUi(this);
    setWindowTitle("Chess Game - Like Chess.com");
    
    m_chessBoard = new ChessBoard();
    
    setupUI();
    updateBoard();
}

myChess::~myChess()
{
    delete m_chessBoard;
    delete ui;
}

void myChess::setupUI() {
    // Create central widget with layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Status labels
    QHBoxLayout* statusLayout = new QHBoxLayout();
    m_turnLabel = new QLabel("Turn: White", this);
    m_turnLabel->setFont(QFont("Arial", 14, QFont::Bold));
    m_statusLabel = new QLabel("Game in progress", this);
    m_statusLabel->setFont(QFont("Arial", 12));
    statusLayout->addWidget(m_turnLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(m_statusLabel);
    mainLayout->addLayout(statusLayout);
    
    // Chess board
    QGridLayout* boardLayout = new QGridLayout();
    boardLayout->setSpacing(0);
    
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            m_squares[row][col] = new ChessSquare(row, col, this);
            connect(m_squares[row][col], &QPushButton::clicked, 
                    this, &myChess::onSquareClicked);
            boardLayout->addWidget(m_squares[row][col], row, col);
        }
    }
    
    QWidget* boardWidget = new QWidget(this);
    boardWidget->setLayout(boardLayout);
    mainLayout->addWidget(boardWidget, 0, Qt::AlignCenter);
    
    // Control buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_newGameButton = new QPushButton("New Game", this);
    m_newGameButton->setFont(QFont("Arial", 12));
    m_newGameButton->setMinimumWidth(120);
    connect(m_newGameButton, &QPushButton::clicked, this, &myChess::onNewGame);
    
    m_undoButton = new QPushButton("Undo", this);
    m_undoButton->setFont(QFont("Arial", 12));
    m_undoButton->setMinimumWidth(120);
    connect(m_undoButton, &QPushButton::clicked, this, &myChess::onUndo);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_newGameButton);
    buttonLayout->addWidget(m_undoButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    setCentralWidget(centralWidget);
    resize(700, 800);
}

void myChess::updateBoard() {
    // First, clear all check highlights
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            m_squares[row][col]->setInCheck(false);
        }
    }
    
    // Update pieces on the board
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            ChessPiece* piece = m_chessBoard->getPieceAt(row, col);
            m_squares[row][col]->setPiece(piece);
            
            // Highlight king if it's in check
            if (piece != nullptr && piece->getType() == PieceType::KING) {
                if (m_chessBoard->isKingInCheck(piece->getColor())) {
                    m_squares[row][col]->setInCheck(true);
                }
            }
        }
    }
    
    // Update turn label
    QString turnText = (m_chessBoard->getCurrentTurn() == PieceColor::WHITE) ? 
        "Turn: White" : "Turn: Black";
    m_turnLabel->setText(turnText);
    
    // Update status label
    m_statusLabel->setText(m_chessBoard->getGameStatus());
    
    // Check if game is over
    if (m_chessBoard->isGameOver()) {
        showGameOverDialog();
    }
}

void myChess::clearHighlights() {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            m_squares[row][col]->setHighlight(false);
            m_squares[row][col]->setSelected(false);
        }
    }
}

void myChess::highlightValidMoves(QPoint from) {
    ChessPiece* piece = m_chessBoard->getPieceAt(from);
    if (piece == nullptr) return;
    
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            QPoint to(col, row);
            if (m_chessBoard->canMove(from, to)) {
                m_squares[row][col]->setHighlight(true);
            }
        }
    }
}

void myChess::onSquareClicked() {
    if (m_chessBoard->isGameOver()) {
        return;
    }
    
    ChessSquare* square = qobject_cast<ChessSquare*>(sender());
    if (square == nullptr) return;
    
    QPoint clickedPos(square->getCol(), square->getRow());
    
    if (!m_hasSelection) {
        // First click - select a piece
        ChessPiece* piece = m_chessBoard->getPieceAt(clickedPos);
        if (piece != nullptr && piece->getColor() == m_chessBoard->getCurrentTurn()) {
            m_selectedSquare = clickedPos;
            m_hasSelection = true;
            clearHighlights();
            square->setSelected(true);
            highlightValidMoves(clickedPos);
        }
    } else {
        // Second click - try to move
        if (clickedPos == m_selectedSquare) {
            // Deselect
            m_hasSelection = false;
            clearHighlights();
        } else {
            // Try to make the move
            bool moveSuccess = m_chessBoard->movePiece(m_selectedSquare, clickedPos);
            m_hasSelection = false;
            clearHighlights();
            updateBoard();
            
            if (!moveSuccess) {
                // Invalid move, maybe selecting a different piece
                ChessPiece* piece = m_chessBoard->getPieceAt(clickedPos);
                if (piece != nullptr && piece->getColor() == m_chessBoard->getCurrentTurn()) {
                    m_selectedSquare = clickedPos;
                    m_hasSelection = true;
                    square->setSelected(true);
                    highlightValidMoves(clickedPos);
                }
            }
        }
    }
}

void myChess::onNewGame() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "New Game", "Start a new game?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        m_chessBoard->reset();
        m_hasSelection = false;
        clearHighlights();
        updateBoard();
    }
}

void myChess::onUndo() {
    // This is a simplified version - a full implementation would need to restore
    // the previous board state from move history
    QMessageBox::information(this, "Undo", 
        "Undo feature is not fully implemented in this version.\n"
        "Use 'New Game' to start over.");
}

void myChess::showGameOverDialog() {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Game Over");
    msgBox.setText(m_chessBoard->getGameStatus());
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void myChess::onSquareDragStarted(int row, int col) {
    if (m_chessBoard->isGameOver()) {
        return;
    }
    
    QPoint clickedPos(col, row);
    ChessPiece* piece = m_chessBoard->getPieceAt(clickedPos);
    
    // Only allow dragging pieces of the current player
    if (piece != nullptr && piece->getColor() == m_chessBoard->getCurrentTurn()) {
        m_selectedSquare = clickedPos;
        m_hasSelection = true;
        clearHighlights();
        m_squares[row][col]->setSelected(true);
        highlightValidMoves(clickedPos);
    }
}

void myChess::onSquareDragEnded(int row, int col) {
    if (m_chessBoard->isGameOver() || !m_hasSelection) {
        return;
    }
    
    QPoint targetPos(col, row);
    
    // Try to make the move
    bool moveSuccess = m_chessBoard->movePiece(m_selectedSquare, targetPos);
    m_hasSelection = false;
    clearHighlights();
    updateBoard();
    
    if (!moveSuccess) {
        // Invalid move - check if selecting a different piece
        ChessPiece* piece = m_chessBoard->getPieceAt(targetPos);
        if (piece != nullptr && piece->getColor() == m_chessBoard->getCurrentTurn()) {
            m_selectedSquare = targetPos;
            m_hasSelection = true;
            m_squares[row][col]->setSelected(true);
            highlightValidMoves(targetPos);
        }
    }
}

