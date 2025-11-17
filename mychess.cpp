#include "mychess.h"
#include "ui_mychess.h"
#include <QApplication>
#include <QPainter>

// ChessSquare 實作
ChessSquare::ChessSquare(int row, int col, QWidget* parent)
    : QPushButton(parent), m_row(row), m_col(col),
    m_highlightType(None), m_isSelected(false), m_isInCheck(false),
    m_isDragging(false) {
    m_isLight = (row + col) % 2 == 0;
    // 降低最小尺寸以便棋盤可以更多縮小
    setMinimumSize(16, 16);
    // 使用 Expanding 讓網格單元擴展以填充棋盤；啟用高度隨寬度變化
    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    policy.setHeightForWidth(true);  // 啟用高度隨寬度變化的長寬比
    setSizePolicy(policy);
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

void ChessSquare::updateStyle() {
    QString baseColor = m_isLight ? "#F0D9B5" : "#B58863";
    QString selectedColor = "#FFD700";
    QString checkColor = "#FF6B6B";  // 將軍時的紅色

    QString bgColor = baseColor;
    QString borderColor = "#000";
    int borderWidth = 1;

    // 優先順序: 將軍 > 已選擇 > 高亮顯示
    if (m_isInCheck) {
        bgColor = checkColor;
    } else if (m_isSelected) {
        bgColor = selectedColor;
    }

    // 為可移動和可吃子的格子添加彩色邊框
    if (m_highlightType == Movable) {
        borderColor = "#0066FF";  // 藍色表示可移動格子
        borderWidth = 3;
    } else if (m_highlightType == Capturable) {
        borderColor = "#FF0000";  // 紅色表示可吃子格子
        borderWidth = 3;
    }

    setStyleSheet(QString("QPushButton { background-color: %1; border: %2px solid %3; }")
                      .arg(bgColor).arg(borderWidth).arg(borderColor));
}

QSize ChessSquare::sizeHint() const {
    // 返回方形尺寸提示
    int size = 60;  // 預設大小
    return QSize(size, size);
}

bool ChessSquare::hasHeightForWidth() const {
    return true;
}

int ChessSquare::heightForWidth(int width) const {
    // 返回與寬度相同的高度值以保持方形長寬比
    return width;
}

void ChessSquare::resizeEvent(QResizeEvent* event) {
    QPushButton::resizeEvent(event);

    // 根據格子大小調整字體大小以保持文字可讀性
    int size = qMin(width(), height());
    int fontSize = qMax(8, size / 2);
    setFont(QFont("Arial", fontSize));
}

void ChessSquare::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
    } else if (event->button() == Qt::RightButton) {
        // 右鍵點擊：如果正在拖曳則取消拖曳
        if (m_isDragging) {
            // 取消拖曳 - 恢復棋子文字
            setText(m_draggedPieceText);
            m_draggedPieceText.clear();
            m_isDragging = false;

            // 通知父視窗取消拖曳
            myChess* parent = qobject_cast<myChess*>(window());
            if (parent) {
                parent->onSquareDragCancelled(m_row, m_col);
            }

            return;
        }
        // 移除右鍵標記功能
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

    // 只有當此格子上有棋子時才允許拖曳
    if (text().isEmpty()) {
        return;
    }

    // 儲存棋子文字並暫時從原格子隱藏
    m_draggedPieceText = text();
    setText("");
    m_isDragging = true;  // 設定拖曳標誌

    // 檢查父視窗是否允許拖曳
    myChess* parent = qobject_cast<myChess*>(window());
    bool dragAllowed = false;
    if (parent) {
        dragAllowed = parent->onSquareDragStarted(m_row, m_col);
    }

    // 如果不允許拖曳（例如：遊戲結束或錯誤回合），恢復文字並返回
    if (!dragAllowed) {
        setText(m_draggedPieceText);
        m_draggedPieceText.clear();
        return;
    }

    QDrag* drag = new QDrag(this);
    QMimeData* mimeData = new QMimeData;

    // 在 mime 資料中儲存來源位置
    mimeData->setText(QString("%1,%2").arg(m_row).arg(m_col));
    drag->setMimeData(mimeData);

    // 建立僅顯示棋子符號的像素圖（不含背景）
    QPixmap pixmap(size());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setFont(font());
    painter.setPen(Qt::black);

    // 將棋子符號繪製於中心
    QRect textRect = rect();
    painter.drawText(textRect, Qt::AlignCenter, m_draggedPieceText);

    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos());

    Qt::DropAction dropAction = drag->exec(Qt::MoveAction);

    // 清除拖曳標誌
    m_isDragging = false;

    // 如果拖曳未被接受（取消或失敗），恢復棋子文字
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

            // 通知父視窗處理移動
            myChess* parent = qobject_cast<myChess*>(window());
            if (parent) {
                parent->onSquareDragEnded(m_row, m_col);
            }

            event->acceptProposedAction();
        }
    }
}

// myChess 實作
myChess::myChess(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::myChess)
    , m_hasSelection(false)
    , m_selectedSquare(-1, -1)
    , m_isBoardFlipped(false)
    , m_boardWidget(nullptr)
    , m_minBoardSize(40) // 預設最小棋盤尺寸為 40 px
{
    ui->setupUi(this);
    setWindowTitle("Chess Game - Like Chess.com");

    m_chessBoard = new ChessBoard();

    // 初始化音效
    m_moveSound = new QSoundEffect(this);
    m_moveSound->setSource(QUrl("qrc:/sounds/sounds/move.wav"));
    m_moveSound->setVolume(0.5f);

    m_captureSound = new QSoundEffect(this);
    m_captureSound->setSource(QUrl("qrc:/sounds/sounds/capture.wav"));
    m_captureSound->setVolume(0.5f);

    setupUI();
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
    // 立即更新佈局（如果已建立）
    if (m_boardWidget) {
        // 觸發一次 resizeEvent 的處理
        QResizeEvent fakeEvent(size(), size());
        resizeEvent(&fakeEvent);
    }
}

int myChess::minBoardSize() const {
    return m_minBoardSize;
}

void myChess::setupUI() {
    // 建立中央元件與佈局
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    // 使用小邊距以便棋盤區域可以根據高度與寬度的關係調整為方形
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(6);

    // 狀態標籤
    QHBoxLayout* statusLayout = new QHBoxLayout();
    m_turnLabel = new QLabel("Turn: White", this);
    m_turnLabel->setFont(QFont("Arial", 14, QFont::Bold));
    m_statusLabel = new QLabel("Game in progress", this);
    m_statusLabel->setFont(QFont("Arial", 12));
    statusLayout->addWidget(m_turnLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(m_statusLabel);
    mainLayout->addLayout(statusLayout);

    // 棋盤 - 行列伸縮比例相同以保持比例縮放
    QGridLayout* boardLayout = new QGridLayout();
    // 重要：無間距或邊距，以便網格單元能精確劃分棋盤區域
    boardLayout->setSpacing(0);
    boardLayout->setContentsMargins(0, 0, 0, 0);

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            m_squares[row][col] = new ChessSquare(row, col, this);
            connect(m_squares[row][col], &QPushButton::clicked,
                    this, &myChess::onSquareClicked);
            boardLayout->addWidget(m_squares[row][col], row, col);
        }
        // 為所有列設定相同伸縮比以保持方形長寬比
        boardLayout->setRowStretch(row, 1);
    }

    for (int col = 0; col < 8; ++col) {
        // 為所有欄設定相同伸縮比以保持方形長寬比
        boardLayout->setColumnStretch(col, 1);
    }

    // 將棋盤元件建立為成員變數，以便 resizeEvent 可以控制其大小
    m_boardWidget = new SquareBoardWidget(this);
    m_boardWidget->setLayout(boardLayout);
    m_boardWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 將元件置中添加
    mainLayout->addWidget(m_boardWidget, 1, Qt::AlignCenter);
    mainLayout->setAlignment(m_boardWidget, Qt::AlignCenter);

    // 控制按鈕
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

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_newGameButton);
    buttonLayout->addWidget(m_undoButton);
    buttonLayout->addWidget(m_flipBoardButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    setCentralWidget(centralWidget);
    resize(700, 800);
}

void myChess::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);

    // 確保我們有中央元件和棋盤元件
    QWidget* central = centralWidget();
    if (!central || !m_boardWidget) return;

    QLayout* mainLayout = central->layout();
    if (!mainLayout) return;

    // 計算棋盤區域可用的寬度和高度，減去
    // 頂部狀態和底部按鈕佈局的大小（它們的 sizeHints）
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

    // 如果可用方形小於請求的最小值，強制視窗最小值以便用戶無法進一步縮小
    if (size < m_minBoardSize) {
        // 計算容納 m_minBoardSize 所需的中央元件大小
        int requiredCentralW = margins.left() + margins.right() + m_minBoardSize;
        int requiredCentralH = margins.top() + margins.bottom() + topH + bottomH + spacing*2 + m_minBoardSize;

        // 估計視窗框架開銷（視窗與中央元件之間的寬度和高度差）
        int deltaW = width() - central->width();
        int deltaH = height() - central->height();

        // 計算所需的視窗大小
        int reqWindowW = requiredCentralW + deltaW;
        int reqWindowH = requiredCentralH + deltaH;

        // 確保為正值
        reqWindowW = qMax(reqWindowW, m_minBoardSize + 100);
        reqWindowH = qMax(reqWindowH, m_minBoardSize + 200);

        // 設定主視窗最小尺寸並立即調整大小以強制執行
        setMinimumSize(reqWindowW, reqWindowH);
        // 立即調整到至少所需大小
        if (width() < reqWindowW || height() < reqWindowH) {
            resize(reqWindowW, reqWindowH);
            // 調整大小後，availW/availH 將在下次事件中重新計算
            return;
        }
    } else {
        // 如果我們大於最小值，可以安全地將主視窗最小值重設為小值，以便用戶可以縮小至 m_minBoardSize
        // （保持下限以免損壞）
        setMinimumSize(800, 540);
    }

    // 強制棋盤不大於可用方形
    m_boardWidget->setMaximumSize(size, size);

    // 請求的最小值是 m_minBoardSize，但如果可用大小較小，我們必須允許較小（我們在上面透過調整大小處理）
    int minBoard = qMin(m_minBoardSize, size);
    m_boardWidget->setMinimumSize(minBoard, minBoard);

    // 立即調整大小到可用方形（但不小於上面設定的最小值）
    m_boardWidget->resize(size, size);

    // 重新斷言置中對齊
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
    }
}

void myChess::onUndo() {
    // 這是簡化版本 - 完整實作需要從移動歷史中恢復
    // 先前的棋盤狀態
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

bool myChess::onSquareDragStarted(int row, int col) {
    if (m_chessBoard->isGameOver()) {
        return false;
    }

    QPoint clickedPos(col, row);
    ChessPiece* piece = m_chessBoard->getPieceAt(clickedPos);

    // 只允許拖曳當前玩家的棋子
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

    // 檢查目標位置是否有棋子以播放吃子音效
    ChessPiece* targetPiece = m_chessBoard->getPieceAt(targetPos);
    bool isCapture = (targetPiece != nullptr);

    // 嘗試移動
    bool moveSuccess = m_chessBoard->movePiece(m_selectedSquare, targetPos);
    m_hasSelection = false;
    clearHighlights();
    if (moveSuccess) {
        // 移動時清除紅色標記
        playMoveSound(isCapture);  // 播放適當的音效
    }
    updateBoard();

    if (!moveSuccess) {
        // 無效移動 - 檢查是否選擇了不同的棋子
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
    // 透過右鍵點擊取消拖曳
    m_hasSelection = false;
    clearHighlights();
    updateBoard();
}

void myChess::playMoveSound(bool isCapture) {
    if (isCapture) {
        m_captureSound->play();
    } else {
        m_moveSound->play();
    }
}

void myChess::onFlipBoard() {
    m_isBoardFlipped = !m_isBoardFlipped;

    // 清除任何當前選擇和高亮顯示
    m_hasSelection = false;
    clearHighlights();


    // 以翻轉方向重新建立棋盤佈局
    // 我們需要從佈局中移除所有元件並重新添加它們
    QGridLayout* boardLayout = nullptr;

    // 尋找棋盤佈局
    QWidget* centralWidget = this->centralWidget();
    if (centralWidget) {
        QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(centralWidget->layout());
        if (mainLayout) {
            // 棋盤元件在索引 1（狀態佈局之後）
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
        // 從佈局中移除所有格子
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                boardLayout->removeWidget(m_squares[row][col]);
            }
        }

        // 以翻轉座標重新添加格子
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                int displayRow = m_isBoardFlipped ? (7 - row) : row;
                int displayCol = m_isBoardFlipped ? (7 - col) : col;
                boardLayout->addWidget(m_squares[row][col], displayRow, displayCol);
            }
        }
    }

    // 更新棋盤顯示
    updateBoard();
}

void myChess::updateBoard() {
    // 首先，清除所有將軍高亮顯示
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            m_squares[row][col]->setInCheck(false);
        }
    }

    // 更新棋盤上的棋子
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            ChessPiece* piece = m_chessBoard->getPieceAt(row, col);
            m_squares[row][col]->setPiece(piece);

            // 如果國王被將軍則高亮顯示
            if (piece != nullptr && piece->getType() == PieceType::KING) {
                if (m_chessBoard->isKingInCheck(piece->getColor())) {
                    m_squares[row][col]->setInCheck(true);
                }
            }
        }
    }

    // 更新回合標籤
    QString turnText = (m_chessBoard->getCurrentTurn() == PieceColor::WHITE) ?
                           "Turn: White" : "Turn: Black";
    m_turnLabel->setText(turnText);

    // 更新狀態標籤
    m_statusLabel->setText(m_chessBoard->getGameStatus());

    // 檢查遊戲是否結束
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
                // 檢查目標位置是否有對手棋子
                ChessPiece* targetPiece = m_chessBoard->getPieceAt(to);
                if (targetPiece != nullptr && targetPiece->getColor() != piece->getColor()) {
                    // 可吃子格子 - 紅色邊框
                    m_squares[row][col]->setHighlight(ChessSquare::Capturable);
                } else {
                    // 可移動格子（空的）- 藍色邊框
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
        // 第一次點擊 - 選擇一個棋子
        ChessPiece* piece = m_chessBoard->getPieceAt(clickedPos);
        if (piece != nullptr && piece->getColor() == m_chessBoard->getCurrentTurn()) {
            m_selectedSquare = clickedPos;
            m_hasSelection = true;
            clearHighlights();
            square->setSelected(true);
            highlightValidMoves(clickedPos);
        }
    } else {
        // 第二次點擊 - 嘗試移動
        if (clickedPos == m_selectedSquare) {
            // 取消選擇
            m_hasSelection = false;
            clearHighlights();
        } else {
            // 嘗試移動
            // 檢查目標位置是否有棋子以播放吃子音效
            ChessPiece* targetPiece = m_chessBoard->getPieceAt(clickedPos);
            bool isCapture = (targetPiece != nullptr);

            bool moveSuccess = m_chessBoard->movePiece(m_selectedSquare, clickedPos);
            m_hasSelection = false;
            clearHighlights();
            if (moveSuccess) {
                playMoveSound(isCapture);  // 播放適當的音效
            }
            updateBoard();

            if (!moveSuccess) {
                // 無效移動，可能選擇了不同的棋子
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
