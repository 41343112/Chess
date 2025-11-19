#include "mychess.h"
#include "ui_mychess.h"
#include "settingsdialog.h"
#include <QApplication>
#include <QPainter>
#include <QIcon>

// ChessSquare implementation
ChessSquare::ChessSquare(int row, int col, QWidget* parent)
    : QPushButton(parent), m_row(row), m_col(col),
    m_highlightType(None), m_isSelected(false), m_isInCheck(false),
    m_isDragging(false), m_piece(nullptr),
    m_lightColor("#F0D9B5"), m_darkColor("#B58863") {
    m_isLight = (row + col) % 2 == 0;
    // Lower minimum size so board can shrink more
    setMinimumSize(16, 16);
    // Use Expanding so grid cells expand to fill the board; enable height-for-width
    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    policy.setHeightForWidth(true);  // Enable height-for-width aspect ratio
    setSizePolicy(policy);
    setFont(QFont("Arial", 32));
    updateStyle();
    setAcceptDrops(true);

    // Ensure button displays icon centered
    setIcon(QIcon());
    setIconSize(size());
}

void ChessSquare::setPiece(ChessPiece* piece) {
    m_piece = piece;
    updatePieceDisplay();
}

void ChessSquare::updatePieceDisplay() {
    if (m_piece != nullptr) {
        QPixmap pix = m_piece->getPixmap();
        if (!pix.isNull()) {
            // Use icon (scaled to fit)
            int s = qMin(width(), height());
            // Ensure we have a reasonable size to scale to
            if (s > 0) {
                QPixmap scaled = pix.scaled(s, s, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                setIcon(QIcon(scaled));
                setIconSize(scaled.size());
            }
            setText("");
        } else {
            // fallback to text symbol
            setIcon(QIcon());
            setText(m_piece->getSymbol());
        }
    } else {
        setIcon(QIcon());
        setText("");
    }
}

void ChessSquare::setHighlight(HighlightType type) {
    m_highlightType = type;
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

void ChessSquare::setColors(const QColor& lightColor, const QColor& darkColor) {
    m_lightColor = lightColor;
    m_darkColor = darkColor;
    updateStyle();
}

void ChessSquare::updateStyle() {
    QString baseColor = m_isLight ? m_lightColor.name() : m_darkColor.name();
    QString selectedColor = "#FFD700";
    QString checkColor = "#FF6B6B";  // Red color for check

    QString bgColor = baseColor;
    QString borderColor = "#000";
    int borderWidth = 1;

    // Priority: check > selected > highlight
    if (m_isInCheck) {
        bgColor = checkColor;
    } else if (m_isSelected) {
        bgColor = selectedColor;
    }

    // Add colored borders for movable and capturable squares
    if (m_highlightType == Movable) {
        borderColor = "#0066FF";  // Blue for movable squares
        borderWidth = 4;
    } else if (m_highlightType == Capturable) {
        borderColor = "#FF0000";  // Red for capturable squares
        borderWidth =4;
    }

    setStyleSheet(QString("QPushButton { background-color: %1; border: %2px solid %3; }")
                      .arg(bgColor).arg(borderWidth).arg(borderColor));
}

QSize ChessSquare::sizeHint() const {
    // Return a square size hint
    int size = 60;  // Default size
    return QSize(size, size);
}

bool ChessSquare::hasHeightForWidth() const {
    return true;
}

int ChessSquare::heightForWidth(int width) const {
    // Return the same value for height to maintain square aspect ratio
    return width;
}

void ChessSquare::resizeEvent(QResizeEvent* event) {
    QPushButton::resizeEvent(event);

    // Adjust font size based on square size to keep text readable
    int size = qMin(width(), height());
    int fontSize = qMax(8, size / 2);
    setFont(QFont("Arial", fontSize));

    // Regenerate icon at new size if we have a piece with an icon
    if (m_piece != nullptr && !m_piece->getPixmap().isNull()) {
        updatePieceDisplay();
    } else if (!icon().isNull()) {
        // Update icon size for any existing icon
        setIconSize(QSize(size, size));
    }
}

void ChessSquare::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
    } else if (event->button() == Qt::RightButton) {
        // Right-click: cancel drag if currently dragging
        if (m_isDragging) {
            // Cancel drag - restore the piece text/icon
            if (!m_draggedPieceText.isEmpty()) setText(m_draggedPieceText);
            m_draggedPieceText.clear();
            m_isDragging = false;

            // Notify parent to cancel the drag
            myChess* parent = qobject_cast<myChess*>(window());
            if (parent) {
                parent->onSquareDragCancelled(m_row, m_col);
            }

            return;
        }
        // Right-click marker functionality removed
        return;
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

    // Only allow dragging if there's a piece on this square (text or icon)
    if (text().isEmpty() && icon().isNull()) {
        return;
    }

    // Store the piece text and temporarily hide it from the original square
    m_draggedPieceText = text();
    setText("");
    m_isDragging = true;  // Set dragging flag

    // Check with parent if drag should proceed
    myChess* parent = qobject_cast<myChess*>(window());
    bool dragAllowed = false;
    if (parent) {
        dragAllowed = parent->onSquareDragStarted(m_row, m_col);
    }

    // If drag not allowed (e.g., game over or wrong turn), restore text and return
    if (!dragAllowed) {
        setText(m_draggedPieceText);
        m_draggedPieceText.clear();
        return;
    }

    QDrag* drag = new QDrag(this);
    QMimeData* mimeData = new QMimeData;

    // Store the source position in the mime data
    mimeData->setText(QString("%1,%2").arg(m_row).arg(m_col));
    drag->setMimeData(mimeData);

    // Create a pixmap for the drag: prefer icon/pixmap if present
    QPixmap dragPixmap;
    if (!icon().isNull()) {
        dragPixmap = icon().pixmap(iconSize());
    } else {
        dragPixmap = QPixmap(size());
        dragPixmap.fill(Qt::transparent);
        QPainter painter(&dragPixmap);
        painter.setFont(font());
        painter.setPen(Qt::black);

        // Draw the piece symbol centered
        QRect textRect = rect();
        painter.drawText(textRect, Qt::AlignCenter, m_draggedPieceText);
    }

    drag->setPixmap(dragPixmap);
    // Center the drag icon on the mouse cursor
    drag->setHotSpot(QPoint(dragPixmap.width() / 2, dragPixmap.height() / 2));

    Qt::DropAction dropAction = drag->exec(Qt::MoveAction);

    // Clear dragging flag
    m_isDragging = false;

    // If drag was not accepted (cancelled or failed), restore the piece text/icon
    if (dropAction == Qt::IgnoreAction) {
        setText(m_draggedPieceText);
    }
    m_draggedPieceText.clear();

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
    , m_selectedSquare(-1, -1)
    , m_hasSelection(false)
    , m_isBoardFlipped(false)
    , m_boardWidget(nullptr)
    , m_minBoardSize(40) // 預設最小棋盤尺寸為 40 px
    , m_undoEnabled(true)
    , m_lightSquareColor("#F0D9B5")
    , m_darkSquareColor("#B58863")
    , m_volume(100)
    , m_timeLimitMinutes(0)
    , m_remainingSeconds(0)
{
    ui->setupUi(this);
    setWindowTitle("Chess Game - Like Chess.com");

    m_chessBoard = new ChessBoard();

    // Initialize move timer
    m_moveTimer = new QTimer(this);
    connect(m_moveTimer, &QTimer::timeout, this, &myChess::onTimerTimeout);

    // Initialize sound effects
    m_moveSound = new QSoundEffect(this);
    m_moveSound->setSource(QUrl("qrc:/sounds/sounds/move.wav"));
    m_moveSound->setVolume(1.0);

    m_captureSound = new QSoundEffect(this);
    m_captureSound->setSource(QUrl("qrc:/sounds/sounds/capture.wav"));
    m_captureSound->setVolume(1.0);

    m_checkSound = new QSoundEffect(this);
    m_checkSound->setSource(QUrl("qrc:/sounds/sounds/check.wav"));
    m_checkSound->setVolume(1.0);

    m_checkmateSound = new QSoundEffect(this);
    m_checkmateSound->setSource(QUrl("qrc:/sounds/sounds/checkmate.wav"));
    m_checkmateSound->setVolume(1.0);

    m_castlingSound = new QSoundEffect(this);
    m_castlingSound->setSource(QUrl("qrc:/sounds/sounds/castling.wav"));
    m_castlingSound->setVolume(1.0);

    loadSettings();
    setupUI();
    applySettings();
    updateBoard();
}

myChess::~myChess()
{
    delete m_chessBoard;
    delete ui;
}

void myChess::setMinBoardSize(int px) {
    if (px < 0) px = 0;
    m_minBoardSize = px;
    // 立即更新佈局（如已建立）
    if (m_boardWidget) {
        QResizeEvent fakeEvent(size(), size());
        resizeEvent(&fakeEvent);
    }
}

int myChess::minBoardSize() const {
    return m_minBoardSize;
}


void myChess::setupUI() {
    // Create central widget with layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    // Use small margins so the board area can be sized square by height-for-width
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(6);

    // Status labels
    QHBoxLayout* statusLayout = new QHBoxLayout();
    m_turnLabel = new QLabel("Turn: White", this);
    m_turnLabel->setFont(QFont("Arial", 14, QFont::Bold));
    m_timerLabel = new QLabel("", this);
    m_timerLabel->setFont(QFont("Arial", 12, QFont::Bold));
    m_timerLabel->setStyleSheet("QLabel { color: green; }");
    m_statusLabel = new QLabel("Game in progress", this);
    m_statusLabel->setFont(QFont("Arial", 12));
    statusLayout->addWidget(m_turnLabel);
    statusLayout->addWidget(m_timerLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(m_statusLabel);
    mainLayout->addLayout(statusLayout);

    // Chess board - with equal row and column stretch for proportional scaling
    QGridLayout* boardLayout = new QGridLayout();
    // Crucial: no spacing or margins so grid cells can divide the board area exactly
    boardLayout->setSpacing(0);
    boardLayout->setContentsMargins(0, 0, 0, 0);

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            m_squares[row][col] = new ChessSquare(row, col, this);
            connect(m_squares[row][col], &QPushButton::clicked,
                    this, &myChess::onSquareClicked);
            boardLayout->addWidget(m_squares[row][col], row, col);
        }
        // Set equal stretch for all rows to maintain square aspect
        boardLayout->setRowStretch(row, 1);
    }

    for (int col = 0; col < 8; ++col) {
        // Set equal stretch for all columns to maintain square aspect
        boardLayout->setColumnStretch(col, 1);
    }

    // Create board widget as a member so resizeEvent can control its size
    m_boardWidget = new SquareBoardWidget(this);
    m_boardWidget->setLayout(boardLayout);
    m_boardWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Add widget centered
    mainLayout->addWidget(m_boardWidget, 1, Qt::AlignCenter);
    mainLayout->setAlignment(m_boardWidget, Qt::AlignCenter);

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

    m_flipBoardButton = new QPushButton("Flip Board", this);
    m_flipBoardButton->setFont(QFont("Arial", 12));
    m_flipBoardButton->setMinimumWidth(120);
    connect(m_flipBoardButton, &QPushButton::clicked, this, &myChess::onFlipBoard);

    m_settingsButton = new QPushButton("Settings", this);
    m_settingsButton->setFont(QFont("Arial", 12));
    m_settingsButton->setMinimumWidth(120);
    connect(m_settingsButton, &QPushButton::clicked, this, &myChess::onSettings);

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_newGameButton);
    buttonLayout->addWidget(m_undoButton);
    buttonLayout->addWidget(m_flipBoardButton);
    buttonLayout->addWidget(m_settingsButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    setCentralWidget(centralWidget);
    resize(700, 800);
}

void myChess::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);

    // Ensure we have the central widget and the board widget
    QWidget* central = centralWidget();
    if (!central || !m_boardWidget) return;

    QLayout* mainLayout = central->layout();
    if (!mainLayout) return;

    // Compute available width and height for the board area by subtracting
    // the sizes of the top status and bottom button layouts (their sizeHints)
    int topH = 0;
    int bottomH = 0;
    if (mainLayout->count() >= 1 && mainLayout->itemAt(0)) {
        topH = mainLayout->itemAt(0)->sizeHint().height();
    }
    if (mainLayout->count() >= 3 && mainLayout->itemAt(2)) {
        bottomH = mainLayout->itemAt(2)->sizeHint().height();
    }

    QMargins margins = static_cast<QVBoxLayout*>(mainLayout)->contentsMargins();
    int spacing = static_cast<QVBoxLayout*>(mainLayout)->spacing();

    int availW = central->width() - margins.left() - margins.right();
    int availH = central->height() - margins.top() - margins.bottom()
                 - topH - bottomH - spacing*2;

    int size = qMax(0, qMin(availW, availH));

    // If available square is smaller than requested minimum, enforce window minimum so user cannot shrink further
    if (size < m_minBoardSize) {
        // Calculate required central widget size to host m_minBoardSize
        int requiredCentralW = margins.left() + margins.right() + m_minBoardSize;
        int requiredCentralH = margins.top() + margins.bottom() + topH + bottomH + spacing*2 + m_minBoardSize;

        // Estimate window frame overhead (width and height difference between window and central widget)
        int deltaW = width() - central->width();
        int deltaH = height() - central->height();

        // Compute required window size
        int reqWindowW = requiredCentralW + deltaW;
        int reqWindowH = requiredCentralH + deltaH;

        // Ensure positive
        reqWindowW = qMax(reqWindowW, m_minBoardSize + 100);
        reqWindowH = qMax(reqWindowH, m_minBoardSize + 200);

        // Set main window minimum size and resize now to enforce
        setMinimumSize(reqWindowW, reqWindowH);
        // Resize immediately to at least required size
        if (width() < reqWindowW || height() < reqWindowH) {
            resize(reqWindowW, reqWindowH);
            // After resize, availW/availH will be recalculated in next events
            return;
        }
    } else {
        // If we are larger than minimum, it's safe to reset main window minimum to a small value so user can shrink until m_minBoardSize
        // (keep a lower bound so nothing breaks)
        setMinimumSize(800, 540);
    }

    // Enforce board not larger than available square
    m_boardWidget->setMaximumSize(size, size);

    // The requested minimum is m_minBoardSize, but if available size is smaller we must allow smaller (we handled above by resizing)
    int minBoard = qMin(m_minBoardSize, size);
    m_boardWidget->setMinimumSize(minBoard, minBoard);

    // Resize immediately to the available square (but not less than the min set above)
    m_boardWidget->resize(size, size);

    // Re-assert center alignment
    if (auto vbox = qobject_cast<QBoxLayout*>(mainLayout)) {
        vbox->setAlignment(m_boardWidget, Qt::AlignCenter);
    } else {
        mainLayout->setAlignment(m_boardWidget, Qt::AlignCenter);
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
        
        // Restart timer for new game
        if (m_timeLimitMinutes > 0) {
            startMoveTimer();
        }
    }
}

void myChess::onUndo() {
    // Attempt to undo the last move
    if (m_chessBoard->undo()) {
        // Clear any selection
        m_hasSelection = false;
        m_selectedSquare = QPoint(-1, -1);
        
        // Update the board display
        updateBoard();
        clearHighlights();
        
        // Update turn label
        QString turnText = (m_chessBoard->getCurrentTurn() == PieceColor::WHITE) ?
                               "Turn: White" : "Turn: Black";
        m_turnLabel->setText(turnText);
        
        // Update status label
        m_statusLabel->setText(m_chessBoard->getGameStatus());
    } else {
        QMessageBox::information(this, "Undo",
                                 "No moves to undo!");
    }
}

void myChess::showGameOverDialog() {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Game Over");
    msgBox.setText(m_chessBoard->getGameStatus());
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

bool myChess::onSquareDragStarted(int row, int col) {
    if (m_chessBoard->isGameOver()) {
        return false;
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
        return true;
    }

    return false;
}

void myChess::onSquareDragEnded(int row, int col) {
    if (m_chessBoard->isGameOver() || !m_hasSelection) {
        return;
    }

    QPoint targetPos(col, row);

    // Check if there's a piece at the destination for capture sound
    ChessPiece* targetPiece = m_chessBoard->getPieceAt(targetPos);
    bool isCapture = (targetPiece != nullptr);

    // Try to make the move
    bool moveSuccess = m_chessBoard->movePiece(m_selectedSquare, targetPos);
    m_hasSelection = false;
    clearHighlights();
    if (moveSuccess) {
        // Determine game state after move for sound
        QString gameStatus = m_chessBoard->getGameStatus();
        bool isCheckmate = gameStatus.contains("checkmate");
        bool isCheck = gameStatus.contains("check") && !isCheckmate;
        
        // Check if the last move was castling
        bool isCastling = false;
        const QVector<Move>& history = m_chessBoard->getMoveHistory();
        if (!history.isEmpty()) {
            isCastling = history.last().wasCastling;
        }
        
        playMoveSound(isCapture, isCheck, isCheckmate, isCastling);
        
        // Restart timer for next player's move
        if (m_timeLimitMinutes > 0 && !m_chessBoard->isGameOver()) {
            startMoveTimer();
        }
    }
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

void myChess::onSquareDragCancelled(int /*row*/, int /*col*/) {
    // Drag was cancelled via right-click
    m_hasSelection = false;
    clearHighlights();
    updateBoard();
}

void myChess::playMoveSound(bool isCapture, bool isCheck, bool isCheckmate, bool isCastling) {
    // Priority: checkmate > check > castling > capture > move
    if (isCheckmate) {
        m_checkmateSound->play();
    } else if (isCheck) {
        m_checkSound->play();
    } else if (isCastling) {
        m_castlingSound->play();
    } else if (isCapture) {
        m_captureSound->play();
    } else {
        m_moveSound->play();
    }
}

void myChess::onFlipBoard() {
    m_isBoardFlipped = !m_isBoardFlipped;

    // Clear any current selection and highlights
    m_hasSelection = false;
    clearHighlights();

    // Re-create the board layout with flipped orientation
    QGridLayout* boardLayout = nullptr;

    // Find the board layout
    QWidget* centralWidget = this->centralWidget();
    if (centralWidget) {
        QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(centralWidget->layout());
        if (mainLayout) {
            // The board widget is at index 1 (after status layout)
            QLayoutItem* item = mainLayout->itemAt(1);
            if (item && item->widget()) {
                SquareBoardWidget* boardWidget = qobject_cast<SquareBoardWidget*>(item->widget());
                if (boardWidget) {
                    boardLayout = qobject_cast<QGridLayout*>(boardWidget->layout());
                }
            }
        }
    }

    if (boardLayout) {
        // Remove all squares from the layout
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                boardLayout->removeWidget(m_squares[row][col]);
            }
        }

        // Re-add squares with flipped coordinates
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                int displayRow = m_isBoardFlipped ? (7 - row) : row;
                int displayCol = m_isBoardFlipped ? (7 - col) : col;
                boardLayout->addWidget(m_squares[row][col], displayRow, displayCol);
            }
        }
    }

    // Update the board display
    updateBoard();
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
            m_squares[row][col]->setHighlight(ChessSquare::None);
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
                // Check if destination has an opponent piece
                ChessPiece* targetPiece = m_chessBoard->getPieceAt(to);
                if (targetPiece != nullptr && targetPiece->getColor() != piece->getColor()) {
                    // Capturable square - red border
                    m_squares[row][col]->setHighlight(ChessSquare::Capturable);
                } else {
                    // Movable square (empty) - blue border
                    m_squares[row][col]->setHighlight(ChessSquare::Movable);
                }
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
            // Check if there's a piece at the destination for capture sound
            ChessPiece* targetPiece = m_chessBoard->getPieceAt(clickedPos);
            bool isCapture = (targetPiece != nullptr);

            bool moveSuccess = m_chessBoard->movePiece(m_selectedSquare, clickedPos);
            m_hasSelection = false;
            clearHighlights();
            if (moveSuccess) {
                // Determine game state after move for sound
                QString gameStatus = m_chessBoard->getGameStatus();
                bool isCheckmate = gameStatus.contains("checkmate");
                bool isCheck = gameStatus.contains("check") && !isCheckmate;
                
                // Check if the last move was castling
                bool isCastling = false;
                const QVector<Move>& history = m_chessBoard->getMoveHistory();
                if (!history.isEmpty()) {
                    isCastling = history.last().wasCastling;
                }
                
                playMoveSound(isCapture, isCheck, isCheckmate, isCastling);
                
                // Restart timer for next player's move
                if (m_timeLimitMinutes > 0 && !m_chessBoard->isGameOver()) {
                    startMoveTimer();
                }
            }
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

void myChess::onSettings() {
    SettingsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        loadSettings();
        applySettings();
    }
}

void myChess::loadSettings() {
    QSettings settings("ChessGame", "Settings");
    
    m_undoEnabled = settings.value("undoEnabled", true).toBool();
    m_lightSquareColor = settings.value("lightSquareColor", QColor("#F0D9B5")).value<QColor>();
    m_darkSquareColor = settings.value("darkSquareColor", QColor("#B58863")).value<QColor>();
    m_volume = settings.value("volume", 100).toInt();
    m_timeLimitMinutes = settings.value("timeLimitMinutes", 0).toInt();
}

void myChess::applySettings() {
    // Apply undo button visibility
    m_undoButton->setEnabled(m_undoEnabled);
    
    // Apply colors to all squares
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            m_squares[row][col]->setColors(m_lightSquareColor, m_darkSquareColor);
        }
    }
    
    // Apply volume to all sound effects
    qreal volume = m_volume / 100.0;
    m_moveSound->setVolume(volume);
    m_captureSound->setVolume(volume);
    m_checkSound->setVolume(volume);
    m_checkmateSound->setVolume(volume);
    m_castlingSound->setVolume(volume);
    
    // Start timer if time limit is set and game is in progress
    if (m_timeLimitMinutes > 0 && !m_chessBoard->isGameOver()) {
        startMoveTimer();
    } else {
        stopMoveTimer();
    }
}

void myChess::startMoveTimer() {
    if (m_timeLimitMinutes <= 0) {
        m_timerLabel->setText("");
        return;
    }
    
    m_remainingSeconds = m_timeLimitMinutes * 60;
    m_moveTimer->start(1000); // Update every second
    
    // Update label
    int minutes = m_remainingSeconds / 60;
    int seconds = m_remainingSeconds % 60;
    m_timerLabel->setText(QString("Time: %1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0')));
    m_timerLabel->setStyleSheet("QLabel { color: green; }");
}

void myChess::stopMoveTimer() {
    m_moveTimer->stop();
    m_timerLabel->setText("");
}

void myChess::onTimerTimeout() {
    if (m_remainingSeconds > 0) {
        m_remainingSeconds--;
        
        int minutes = m_remainingSeconds / 60;
        int seconds = m_remainingSeconds % 60;
        m_timerLabel->setText(QString("Time: %1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0')));
        
        // Change color when time is running low (less than 30 seconds)
        if (m_remainingSeconds <= 30) {
            m_timerLabel->setStyleSheet("QLabel { color: red; }");
        } else if (m_remainingSeconds <= 60) {
            m_timerLabel->setStyleSheet("QLabel { color: orange; }");
        }
    } else {
        // Time's up!
        m_moveTimer->stop();
        m_timerLabel->setText("Time's up!");
        m_timerLabel->setStyleSheet("QLabel { color: red; }");
        
        QMessageBox::warning(this, "Time Limit Exceeded",
                           QString("Time limit exceeded for %1's move!")
                           .arg(m_chessBoard->getCurrentTurn() == PieceColor::WHITE ? "White" : "Black"));
        
        // Optionally, you could auto-pass the turn or end the game here
        // For now, just show warning and continue the game
    }
}
