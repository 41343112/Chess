#include "mychess.h"
#include "ui_mychess.h"
#include "settingsdialog.h"
#include "startdialog.h"
#include "promotiondialog.h"
#include <QApplication>
#include <QPainter>
#include <QIcon>
#include <QKeyEvent>

// ChessSquare 實作
ChessSquare::ChessSquare(int row, int col, QWidget* parent)
    : QPushButton(parent), m_row(row), m_col(col),
    m_highlightType(None), m_isSelected(false), m_isInCheck(false),
    m_isDragging(false), m_piece(nullptr),
    m_lightColor("#F0D9B5"), m_darkColor("#B58863"), m_currentDrag(nullptr) {
    m_isLight = (row + col) % 2 == 0;
    // 降低最小尺寸以便棋盤可以進一步縮小
    setMinimumSize(16, 16);
    // 使用 Expanding 使網格單元擴展以填滿棋盤；啟用高度與寬度的關聯
    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    policy.setHeightForWidth(true);  // 啟用高度與寬度的比例
    setSizePolicy(policy);
    setFont(QFont("Arial", 32));
    updateStyle();
    setAcceptDrops(true);

    // 確保按鈕將圖示顯示在中央
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
            // 使用圖示（縮放以適應）
            int s = qMin(width(), height());
            // 確保有合理的尺寸可以縮放
            if (s > 0) {
                QPixmap scaled = pix.scaled(s, s, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                setIcon(QIcon(scaled));
                setIconSize(scaled.size());
            }
            setText("");
        } else {
            // 回退到文字符號
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
    QString checkColor = "#FF6B6B";  // 將軍的紅色

    QString bgColor = baseColor;
    QString borderColor = "#000";
    int borderWidth = 1;

    // 優先順序：將軍 > 選中 > 高亮
    if (m_isInCheck) {
        bgColor = checkColor;
    } else if (m_isSelected) {
        bgColor = selectedColor;
    }

    // 為可移動和可吃子的格子添加彩色邊框
    if (m_highlightType == Movable) {
        borderColor = "#0066FF";  // 藍色表示可移動的格子
        borderWidth = 4;
    } else if (m_highlightType == Capturable) {
        borderColor = "#FF0000";  // 紅色表示可吃子的格子
        borderWidth =4;
    }

    setStyleSheet(QString("QPushButton { background-color: %1; border: %2px solid %3; }")
                      .arg(bgColor).arg(borderWidth).arg(borderColor));
}

QSize ChessSquare::sizeHint() const {
    // 返回正方形尺寸提示
    int size = 60;  // 預設尺寸
    return QSize(size, size);
}

bool ChessSquare::hasHeightForWidth() const {
    return true;
}

int ChessSquare::heightForWidth(int width) const {
    // 返回相同的高度值以維持正方形比例
    return width;
}

void ChessSquare::resizeEvent(QResizeEvent* event) {
    QPushButton::resizeEvent(event);

    // 根據格子大小調整字體大小以保持文字可讀性
    int size = qMin(width(), height());
    int fontSize = qMax(8, size / 2);
    setFont(QFont("Arial", fontSize));

    // 如果有帶圖示的棋子，以新尺寸重新生成圖示
    if (m_piece != nullptr && !m_piece->getPixmap().isNull()) {
        updatePieceDisplay();
    } else if (!icon().isNull()) {
        // 更新任何現有圖示的尺寸
        setIconSize(QSize(size, size));
    }
}

void ChessSquare::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
    } else if (event->button() == Qt::RightButton) {
        // 右鍵點擊：如果正在拖曳則取消拖曳
        if (m_isDragging && m_currentDrag) {
            // 取消正在進行的拖曳操作
            // Qt 沒有直接的 cancel() 方法，但我們可以透過刪除 QDrag 物件來終止
            // 不過 QDrag::exec() 已經在執行中，我們需要用事件過濾器來處理
            // 這裡只是標記取消意圖，實際取消在 eventFilter 中處理
        }
        return;
    }
    QPushButton::mousePressEvent(event);
}

void ChessSquare::mouseMoveEvent(QMouseEvent* event) {
    // 檢查在潛在拖曳期間是否按下右鍵 - 取消拖曳
    if (event->buttons() & Qt::RightButton) {
        // 如果正在準備拖曳，取消它
        if (m_isDragging) {
            m_isDragging = false;
            m_draggedPieceText.clear();
            myChess* parent = qobject_cast<myChess*>(window());
            if (parent) {
                parent->onSquareDragCancelled(m_row, m_col);
            }
        }
        return;
    }

    if (!(event->buttons() & Qt::LeftButton)) {
        return;
    }

    if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
        return;
    }

    // 只允許拖曳有棋子的格子（文字或圖示）
    if (text().isEmpty() && icon().isNull()) {
        return;
    }

    // 儲存棋子文字並暫時從原始格子隱藏它
    m_draggedPieceText = text();
    setText("");
    m_isDragging = true;  // 設定拖曳旗標

    // 檢查父視窗是否允許拖曳繼續
    myChess* parent = qobject_cast<myChess*>(window());
    bool dragAllowed = false;
    if (parent) {
        dragAllowed = parent->onSquareDragStarted(m_row, m_col);
    }

    // 如果不允許拖曳（例如遊戲結束或錯誤回合），恢復文字並返回
    if (!dragAllowed) {
        setText(m_draggedPieceText);
        m_draggedPieceText.clear();
        m_isDragging = false;
        return;
    }

    // 在開始拖曳時安裝事件過濾器以偵測右鍵
    qApp->installEventFilter(this);

    QDrag* drag = new QDrag(this);
    m_currentDrag = drag;  // 儲存拖曳物件指標
    QMimeData* mimeData = new QMimeData;

    // 將來源位置儲存在 mime 資料中
    mimeData->setText(QString("%1,%2").arg(m_row).arg(m_col));
    drag->setMimeData(mimeData);

    // 為拖曳建立圖像：優先使用圖示/圖像（如果存在）
    QPixmap dragPixmap;
    if (!icon().isNull()) {
        dragPixmap = icon().pixmap(iconSize());
    } else {
        dragPixmap = QPixmap(size());
        dragPixmap.fill(Qt::transparent);
        QPainter painter(&dragPixmap);
        painter.setFont(font());
        painter.setPen(Qt::black);

        // 繪製置中的棋子符號
        QRect textRect = rect();
        painter.drawText(textRect, Qt::AlignCenter, m_draggedPieceText);
    }

    drag->setPixmap(dragPixmap);
    // 將拖曳圖示置於滑鼠游標中央
    drag->setHotSpot(QPoint(dragPixmap.width() / 2, dragPixmap.height() / 2));

    Qt::DropAction dropAction = drag->exec(Qt::MoveAction);

    // 拖曳結束後移除事件過濾器
    qApp->removeEventFilter(this);

    // 清除拖曳旗標和指標
    m_isDragging = false;
    m_currentDrag = nullptr;

    // 如果拖曳未被接受（取消或失敗），恢復棋子文字/圖示
    if (dropAction == Qt::IgnoreAction) {
        setText(m_draggedPieceText);
        // 通知父視窗拖曳已取消
        if (parent) {
            parent->onSquareDragCancelled(m_row, m_col);
        }
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

bool ChessSquare::eventFilter(QObject* watched, QEvent* event) {
    // 偵測拖曳期間的右鍵點擊以取消拖曳
    if (m_isDragging && m_currentDrag && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::RightButton) {
            // 恢復棋子到原始位置
            setText(m_draggedPieceText);
            m_draggedPieceText.clear();
            m_isDragging = false;
            
            // 通知父視窗取消拖曳
            myChess* parent = qobject_cast<myChess*>(window());
            if (parent) {
                parent->onSquareDragCancelled(m_row, m_col);
            }
            
            // 移除事件過濾器（在取消時）
            qApp->removeEventFilter(this);
            
            // 取消拖曳操作 - 透過關鍵事件模擬 Escape 鍵
            // 這會導致 drag->exec() 返回 Qt::IgnoreAction
            QKeyEvent escapeEvent(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
            QApplication::sendEvent(m_currentDrag, &escapeEvent);
            m_currentDrag = nullptr;
            
            return true;  // 事件已處理
        }
    }
    
    return QPushButton::eventFilter(watched, event);
}

// myChess 實作
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
    , m_viewingPosition(-1)
    , m_isViewingHistory(false)
    , m_timeControlEnabled(false)
    , m_timeControlMinutes(10)
    , m_incrementSeconds(0)
    , m_whiteTimeRemaining(0)
    , m_blackTimeRemaining(0)
    , m_isTimerRunning(false)
    , m_firstMoveMade(false)
{
    ui->setupUi(this);
    setWindowTitle(tr("Chess Game - Like Chess.com"));

    m_chessBoard = new ChessBoard();
    
    // 初始化計時器 - 使用 100ms 間隔以獲得更流暢的顯示
    m_gameTimer = new QTimer(this);
    m_gameTimer->setInterval(100);  // 100ms 週期
    connect(m_gameTimer, &QTimer::timeout, this, &myChess::onTimerTick);

    // 初始化音效
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
    
    // 初始化臨時檢視棋盤
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            m_tempViewBoard[i][j] = nullptr;
        }
    }
    
    // 啟動時顯示開始對話框
    QTimer::singleShot(100, this, &myChess::showStartDialog);
}

myChess::~myChess()
{
    clearTempViewBoard();
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
    // 建立中央小工具與佈局
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    // 使用小邊距，以便棋盤區域可以透過高度與寬度的關聯調整為正方形
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(6);

    // 狀態標籤
    QHBoxLayout* statusLayout = new QHBoxLayout();
    m_turnLabel = new QLabel(tr("Turn: White"), this);
    m_turnLabel->setFont(QFont("Arial", 14, QFont::Bold));
    m_statusLabel = new QLabel(tr("Game in progress"), this);
    m_statusLabel->setFont(QFont("Arial", 12));
    statusLayout->addWidget(m_turnLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(m_statusLabel);
    mainLayout->addLayout(statusLayout);
    
    // 時間控制標籤
    QHBoxLayout* timeLayout = new QHBoxLayout();
    m_whiteTimeLabel = new QLabel(tr("White: --:--"), this);
    m_whiteTimeLabel->setFont(QFont("Arial", 16, QFont::Bold));
    m_whiteTimeLabel->setAlignment(Qt::AlignLeft);
    m_blackTimeLabel = new QLabel(tr("Black: --:--"), this);
    m_blackTimeLabel->setFont(QFont("Arial", 16, QFont::Bold));
    m_blackTimeLabel->setAlignment(Qt::AlignRight);
    timeLayout->addWidget(m_whiteTimeLabel);
    timeLayout->addStretch();
    timeLayout->addWidget(m_blackTimeLabel);
    mainLayout->addLayout(timeLayout);

    // 西洋棋棋盤 - 使用相等的行列延展以進行成比例縮放
    QGridLayout* boardLayout = new QGridLayout();
    // 關鍵：沒有間距或邊距，讓網格單元可以精確劃分棋盤區域
    boardLayout->setSpacing(0);
    boardLayout->setContentsMargins(0, 0, 0, 0);

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            m_squares[row][col] = new ChessSquare(row, col, this);
            connect(m_squares[row][col], &QPushButton::clicked,
                    this, &myChess::onSquareClicked);
            boardLayout->addWidget(m_squares[row][col], row, col);
        }
        // 為所有行設定相等的延展以維持正方形
        boardLayout->setRowStretch(row, 1);
    }

    for (int col = 0; col < 8; ++col) {
        // 為所有列設定相等的延展以維持正方形
        boardLayout->setColumnStretch(col, 1);
    }

    // 將棋盤小工具建立為成員，以便 resizeEvent 可以控制其大小
    m_boardWidget = new SquareBoardWidget(this);
    m_boardWidget->setLayout(boardLayout);
    m_boardWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 將小工具置中添加
    mainLayout->addWidget(m_boardWidget, 1, Qt::AlignCenter);
    mainLayout->setAlignment(m_boardWidget, Qt::AlignCenter);

    // 用於查看歷史記錄的導航按鈕
    QHBoxLayout* navLayout = new QHBoxLayout();
    m_backToStartButton = new QPushButton(tr("<|回到一開始"), this);
    m_backToStartButton->setFont(QFont("Arial", 12));
    m_backToStartButton->setMinimumWidth(120);
    connect(m_backToStartButton, &QPushButton::clicked, this, &myChess::onBackToStart);

    m_previousMoveButton = new QPushButton(tr("<上一著"), this);
    m_previousMoveButton->setFont(QFont("Arial", 12));
    m_previousMoveButton->setMinimumWidth(120);
    connect(m_previousMoveButton, &QPushButton::clicked, this, &myChess::onPreviousMove);

    m_nextMoveButton = new QPushButton(tr(">下一著"), this);
    m_nextMoveButton->setFont(QFont("Arial", 12));
    m_nextMoveButton->setMinimumWidth(120);
    connect(m_nextMoveButton, &QPushButton::clicked, this, &myChess::onNextMove);

    m_backToCurrentButton = new QPushButton(tr(">|回到目前棋局"), this);
    m_backToCurrentButton->setFont(QFont("Arial", 12));
    m_backToCurrentButton->setMinimumWidth(120);
    connect(m_backToCurrentButton, &QPushButton::clicked, this, &myChess::onBackToCurrent);

    navLayout->addStretch();
    navLayout->addWidget(m_backToStartButton);
    navLayout->addWidget(m_previousMoveButton);
    navLayout->addWidget(m_nextMoveButton);
    navLayout->addWidget(m_backToCurrentButton);
    navLayout->addStretch();
    mainLayout->addLayout(navLayout);

    // 控制按鈕
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_newGameButton = new QPushButton(tr("New Game"), this);
    m_newGameButton->setFont(QFont("Arial", 12));
    m_newGameButton->setMinimumWidth(120);
    connect(m_newGameButton, &QPushButton::clicked, this, &myChess::onNewGame);

    m_undoButton = new QPushButton(tr("Undo"), this);
    m_undoButton->setFont(QFont("Arial", 12));
    m_undoButton->setMinimumWidth(120);
    connect(m_undoButton, &QPushButton::clicked, this, &myChess::onUndo);

    m_flipBoardButton = new QPushButton(tr("Flip Board"), this);
    m_flipBoardButton->setFont(QFont("Arial", 12));
    m_flipBoardButton->setMinimumWidth(120);
    connect(m_flipBoardButton, &QPushButton::clicked, this, &myChess::onFlipBoard);

    m_settingsButton = new QPushButton(tr("Settings"), this);
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
    // Show start dialog for new game
    showStartDialog();
}

void myChess::onUndo() {
    // 查看歷史記錄時無法撤銷
    if (m_isViewingHistory) {
        return;
    }
    
    // 嘗試撤銷上一步移動
    if (m_chessBoard->undo()) {
        // 清除任何選擇
        m_hasSelection = false;
        m_selectedSquare = QPoint(-1, -1);
        
        // 更新棋盤顯示
        updateBoard();
        clearHighlights();
        updateNavigationButtons();
        
        // 更新回合標籤
        QString turnText = (m_chessBoard->getCurrentTurn() == PieceColor::WHITE) ?
                               tr("Turn: White") : tr("Turn: Black");
        m_turnLabel->setText(turnText);
        
        // 更新狀態標籤
        m_statusLabel->setText(m_chessBoard->getGameStatus());
    } else {
        QMessageBox::information(this, tr("Undo"),
                                 tr("No moves to undo!"));
    }
}

void myChess::showGameOverDialog() {
    stopTimer();
    
    // 延遲顯示對話框，讓將死音效有時間播放完畢
    QTimer::singleShot(500, this, [this]() {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(tr("Game Over"));
        msgBox.setText(m_chessBoard->getGameStatus());
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        
        // 遊戲結束時重新啟用設定按鈕
        m_settingsButton->setEnabled(true);
    });
}

bool myChess::onSquareDragStarted(int row, int col) {
    // 查看歷史記錄時停用拖曳
    if (m_isViewingHistory || m_chessBoard->isGameOver()) {
        return false;
    }

    QPoint clickedPos(col, row);
    ChessPiece* piece = m_chessBoard->getPieceAt(clickedPos);

    // 只允許拖曳目前玩家的棋子
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
    if (m_isViewingHistory || m_chessBoard->isGameOver() || !m_hasSelection) {
        return;
    }

    QPoint targetPos(col, row);

    // 檢查目的地是否有棋子以播放吃子音效
    ChessPiece* targetPiece = m_chessBoard->getPieceAt(targetPos);
    bool isCapture = (targetPiece != nullptr);

    // 檢查是否會導致升變
    if (m_chessBoard->wouldBePromotion(m_selectedSquare, targetPos) && 
        m_chessBoard->canMove(m_selectedSquare, targetPos)) {
        // 顯示升變對話框
        ChessPiece* piece = m_chessBoard->getPieceAt(m_selectedSquare);
        PromotionDialog dialog(piece->getColor(), this);
        if (dialog.exec() == QDialog::Accepted) {
            m_chessBoard->setPromotionPieceType(dialog.getSelectedPieceType());
        } else {
            // 使用者取消升變對話框
            m_hasSelection = false;
            clearHighlights();
            updateBoard();
            return;
        }
    }

    // 嘗試進行移動
    bool moveSuccess = m_chessBoard->movePiece(m_selectedSquare, targetPos);
    m_hasSelection = false;
    clearHighlights();
    if (moveSuccess) {
        // 第一步移動時啟動計時器
        if (!m_firstMoveMade && m_timeControlEnabled) {
            startTimer();
            m_firstMoveMade = true;
        }
        
        // 為剛移動的玩家增加時間增量
        addIncrement();
        
        // 進行移動時重設檢視狀態
        m_viewingPosition = -1;
        m_isViewingHistory = false;
        
        // 判斷移動後的遊戲狀態以播放音效
        // 使用 ChessBoard API 方法而不是解析翻譯字串
        bool isCheckmate = m_chessBoard->isGameOver() && m_chessBoard->isKingInCheck(m_chessBoard->getCurrentTurn());
        bool isCheck = !isCheckmate && m_chessBoard->isKingInCheck(m_chessBoard->getCurrentTurn());
        
        // 檢查上一步移動是否為王車易位或吃過路兵
        bool isCastling = false;
        bool wasEnPassant = false;
        const QVector<Move>& history = m_chessBoard->getMoveHistory();
        if (!history.isEmpty()) {
            isCastling = history.last().wasCastling;
            wasEnPassant = history.last().wasEnPassant;
        }
        
        // 吃過路兵也是吃子
        if (wasEnPassant) {
            isCapture = true;
        }
        
        playMoveSound(isCapture, isCheck, isCheckmate, isCastling);
        
        // 第一步移動後停用設定
        if (m_chessBoard->getMoveHistory().size() == 1) {
            m_settingsButton->setEnabled(false);
        }
    }
    updateBoard();
    updateNavigationButtons();

    if (!moveSuccess) {
        // 無效移動 - 檢查是否選擇不同的棋子
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
    // 透過右鍵取消拖曳
    m_hasSelection = false;
    clearHighlights();
    updateBoard();
}

void myChess::playMoveSound(bool isCapture, bool isCheck, bool isCheckmate, bool isCastling) {
    // 優先順序：將死 > 將軍 > 王車易位 > 吃子 > 移動
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
                           tr("Turn: White") : tr("Turn: Black");
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
    // Disable piece selection/movement when viewing history
    if (m_isViewingHistory || m_chessBoard->isGameOver()) {
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
            // Check if there's a piece at the destination for capture sound
            ChessPiece* targetPiece = m_chessBoard->getPieceAt(clickedPos);
            bool isCapture = (targetPiece != nullptr);

            // 檢查是否會導致升變
            if (m_chessBoard->wouldBePromotion(m_selectedSquare, clickedPos) && 
                m_chessBoard->canMove(m_selectedSquare, clickedPos)) {
                // 顯示升變對話框
                ChessPiece* piece = m_chessBoard->getPieceAt(m_selectedSquare);
                PromotionDialog dialog(piece->getColor(), this);
                if (dialog.exec() == QDialog::Accepted) {
                    m_chessBoard->setPromotionPieceType(dialog.getSelectedPieceType());
                } else {
                    // 使用者取消升變對話框
                    m_hasSelection = false;
                    clearHighlights();
                    updateBoard();
                    return;
                }
            }

            // Try to make the move
            bool moveSuccess = m_chessBoard->movePiece(m_selectedSquare, clickedPos);
            m_hasSelection = false;
            clearHighlights();
            if (moveSuccess) {
                // Start timer on first move
                if (!m_firstMoveMade && m_timeControlEnabled) {
                    startTimer();
                    m_firstMoveMade = true;
                }
                
                // 為剛移動的玩家增加時間增量
                addIncrement();
                
                // Determine game state after move for sound
                // Use ChessBoard API methods instead of parsing translated strings
                bool isCheckmate = m_chessBoard->isGameOver() && m_chessBoard->isKingInCheck(m_chessBoard->getCurrentTurn());
                bool isCheck = !isCheckmate && m_chessBoard->isKingInCheck(m_chessBoard->getCurrentTurn());
                
                // Check if the last move was castling or en passant
                bool isCastling = false;
                bool wasEnPassant = false;
                const QVector<Move>& history = m_chessBoard->getMoveHistory();
                if (!history.isEmpty()) {
                    isCastling = history.last().wasCastling;
                    wasEnPassant = history.last().wasEnPassant;
                }
                
                // En passant is also a capture
                if (wasEnPassant) {
                    isCapture = true;
                }
                
                playMoveSound(isCapture, isCheck, isCheckmate, isCastling);
                
                // Reset viewing state when a move is made
                m_viewingPosition = -1;
                m_isViewingHistory = false;
                
                // Disable settings after first move
                if (m_chessBoard->getMoveHistory().size() == 1) {
                    m_settingsButton->setEnabled(false);
                }
            }
            updateBoard();
            updateNavigationButtons();

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
}

void myChess::applySettings() {
    // 套用悔棋按鈕可見性
    m_undoButton->setEnabled(m_undoEnabled);
    
    // 將顏色套用到所有格子
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            m_squares[row][col]->setColors(m_lightSquareColor, m_darkSquareColor);
        }
    }
    
    // 將音效設定為最大音量（100%）
    m_moveSound->setVolume(1.0);
    m_captureSound->setVolume(1.0);
    m_checkSound->setVolume(1.0);
    m_checkmateSound->setVolume(1.0);
    m_castlingSound->setVolume(1.0);
    
    // 套用時間控制設定
    resetTimers();
    updateTimeDisplay();
}

void myChess::showStartDialog() {
    StartDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        // 從對話框取得時間控制設定
        int timeSeconds = dialog.getTimeControlSeconds();  // 返回秒數，若無限制則為 0
        m_incrementSeconds = dialog.getIncrementSeconds();
        
        // 根據設定啟用或停用時間控制
        if (timeSeconds == 0) {
            // 無時間限制
            m_timeControlEnabled = false;
            m_timeControlMinutes = 0;
        } else {
            // 啟用時間控制
            m_timeControlEnabled = true;
            // 將秒數轉換為分鐘以與設定相容
            m_timeControlMinutes = timeSeconds / 60;
            if (m_timeControlMinutes == 0 && timeSeconds > 0) {
                m_timeControlMinutes = 1;  // 至少 1 分鐘以保持相容性
            }
        }
        
        // 以配置的時間初始化計時器（但尚未啟動）
        m_whiteTimeRemaining = timeSeconds * 1000;  // 轉換為毫秒
        m_blackTimeRemaining = timeSeconds * 1000;
        
        // 重設棋盤以開始新遊戲
        m_chessBoard->reset();
        m_hasSelection = false;
        clearHighlights();
        updateBoard();
        
        // 重設檢視狀態
        m_viewingPosition = -1;
        m_isViewingHistory = false;
        updateNavigationButtons();
        
        // 為新遊戲啟用設定按鈕
        m_settingsButton->setEnabled(true);
        
        // 重設第一步移動旗標 - 計時器將在第一步移動時啟動
        m_firstMoveMade = false;
        
        // 更新時間顯示但尚未啟動計時器
        updateTimeDisplay();
    }
}

void myChess::onPreviousMove() {
    int historySize = m_chessBoard->getMoveHistory().size();
    if (historySize == 0) return;
    
    // 為導航播放移動音效
    m_moveSound->play();
    
    // 查看歷史記錄時暫停計時器
    if (!m_isViewingHistory) {
        stopTimer();
    }
    
    if (m_isViewingHistory) {
        // 已在查看歷史記錄，向後移動一個位置
        if (m_viewingPosition > 0) {
            m_viewingPosition--;
            if (m_viewingPosition == 0) {
                // 顯示初始位置
                clearTempViewBoard();
                PieceColor turn;
                m_chessBoard->getBoardStateAtMove(-1, m_tempViewBoard, turn);
                for (int row = 0; row < 8; ++row) {
                    for (int col = 0; col < 8; ++col) {
                        m_squares[row][col]->setPiece(m_tempViewBoard[row][col]);
                        m_squares[row][col]->setInCheck(false);
                    }
                }
                m_turnLabel->setText(tr("Turn: White"));
                m_statusLabel->setText(tr("Viewing history - Initial position"));
            } else {
                displayBoardAtPosition(m_viewingPosition - 1);
            }
            updateNavigationButtons();
        }
    } else {
        // 尚未查看歷史記錄，從目前位置 - 1 開始
        m_viewingPosition = historySize;
        m_isViewingHistory = true;
        displayBoardAtPosition(historySize - 1);
        updateNavigationButtons();
    }
}

void myChess::onNextMove() {
    if (!m_isViewingHistory) return;
    
    // 為導航播放移動音效
    m_moveSound->play();
    
    int historySize = m_chessBoard->getMoveHistory().size();
    if (m_viewingPosition < historySize) {
        m_viewingPosition++;
        if (m_viewingPosition == historySize) {
            // 移動到目前位置
            onBackToCurrent();
        } else {
            displayBoardAtPosition(m_viewingPosition - 1);
            updateNavigationButtons();
        }
    }
}

void myChess::onBackToStart() {
    int historySize = m_chessBoard->getMoveHistory().size();
    if (historySize == 0) {
        // 尚無移動，無法顯示
        return;
    }
    
    // Pause timer when viewing history
    stopTimer();
    
    // Position -1 in viewing means we want to see the initial state (before any moves)
    // But we use 0 to mean the initial state for clarity
    m_viewingPosition = 0;
    m_isViewingHistory = true;
    
    // Clear temp board and display initial position
    clearTempViewBoard();
    
    // Create initial board state (before any moves)
    PieceColor turn;
    m_chessBoard->getBoardStateAtMove(-1, m_tempViewBoard, turn);
    
    // Update the display
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            m_squares[row][col]->setPiece(m_tempViewBoard[row][col]);
            m_squares[row][col]->setInCheck(false);
        }
    }
    
    m_turnLabel->setText(tr("Turn: White"));
    m_statusLabel->setText(tr("Viewing history - Initial position"));
    
    updateNavigationButtons();
}

void myChess::onBackToCurrent() {
    m_viewingPosition = -1;
    m_isViewingHistory = false;
    clearTempViewBoard();
    updateBoard();
    updateNavigationButtons();
    
    // 如果遊戲未結束則恢復計時器
    if (!m_chessBoard->isGameOver()) {
        startTimer();
    }
}

void myChess::updateNavigationButtons() {
    int historySize = m_chessBoard->getMoveHistory().size();
    
    if (m_isViewingHistory) {
        // Enable/disable based on position in history
        // Position 0 = initial, Position N = after N-th move
        m_backToStartButton->setEnabled(m_viewingPosition > 0);
        m_previousMoveButton->setEnabled(m_viewingPosition > 0);
        m_nextMoveButton->setEnabled(m_viewingPosition < historySize);
        m_backToCurrentButton->setEnabled(true);
        
        // Disable game actions while viewing history
        m_undoButton->setEnabled(false);
        
        // Visual indication that we're in viewing mode
        QString statusText;
        if (m_viewingPosition == 0) {
            statusText = tr("Viewing history - Initial position");
        } else {
            statusText = tr("Viewing history - Move %1 of %2")
                             .arg(m_viewingPosition)
                             .arg(historySize);
        }
        m_statusLabel->setText(statusText);
    } else {
        // At current position
        m_backToStartButton->setEnabled(historySize > 0);
        m_previousMoveButton->setEnabled(historySize > 0);
        m_nextMoveButton->setEnabled(false);
        m_backToCurrentButton->setEnabled(false);
        
        // Re-enable game actions
        m_undoButton->setEnabled(m_undoEnabled);
    }
}

void myChess::displayBoardAtPosition(int position) {
    // Clear any selection and highlights
    m_hasSelection = false;
    clearHighlights();
    
    // Get board state at the specified position
    if (position < 0 || position >= m_chessBoard->getMoveHistory().size()) {
        return;
    }
    
    // Clear any previous temp board
    clearTempViewBoard();
    
    // We need to reconstruct the board state at the given move
    // Position 0 means after the first move
    // We'll create a temporary board state by replaying moves
    
    PieceColor turn;
    m_chessBoard->getBoardStateAtMove(position, m_tempViewBoard, turn);
    
    // Update the display with this temporary state
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            m_squares[row][col]->setPiece(m_tempViewBoard[row][col]);
            m_squares[row][col]->setInCheck(false);
        }
    }
    
    // Update turn label for the position being viewed
    QString turnText = (turn == PieceColor::WHITE) ?
                           tr("Turn: White") : tr("Turn: Black");
    m_turnLabel->setText(turnText);
}

void myChess::clearTempViewBoard() {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (m_tempViewBoard[i][j] != nullptr) {
                delete m_tempViewBoard[i][j];
                m_tempViewBoard[i][j] = nullptr;
            }
        }
    }
}

void myChess::onTimerTick() {
    if (!m_isTimerRunning || m_chessBoard->isGameOver()) {
        return;
    }
    
    // 將目前玩家的時間減少 100ms
    if (m_chessBoard->getCurrentTurn() == PieceColor::WHITE) {
        m_whiteTimeRemaining -= 100;
        if (m_whiteTimeRemaining <= 0) {
            m_whiteTimeRemaining = 0;
            stopTimer();
            m_chessBoard->setGameOver(tr("Black wins by timeout"));
            QMessageBox::information(this, tr("Time Out"), 
                tr("White ran out of time! Black wins by timeout."));
            m_statusLabel->setText(tr("Black wins by timeout"));
            return;
        }
    } else {
        m_blackTimeRemaining -= 100;
        if (m_blackTimeRemaining <= 0) {
            m_blackTimeRemaining = 0;
            stopTimer();
            m_chessBoard->setGameOver(tr("White wins by timeout"));
            QMessageBox::information(this, tr("Time Out"), 
                tr("Black ran out of time! White wins by timeout."));
            m_statusLabel->setText(tr("White wins by timeout"));
            return;
        }
    }
    
    updateTimeDisplay();
}

void myChess::updateTimeDisplay() {
    if (m_timeControlEnabled) {
        m_whiteTimeLabel->setText(tr("White: %1").arg(formatTime(m_whiteTimeRemaining)));
        m_blackTimeLabel->setText(tr("Black: %1").arg(formatTime(m_blackTimeRemaining)));
        m_whiteTimeLabel->setVisible(true);
        m_blackTimeLabel->setVisible(true);
    } else {
        m_whiteTimeLabel->setVisible(false);
        m_blackTimeLabel->setVisible(false);
    }
}

void myChess::startTimer() {
    if (m_timeControlEnabled && !m_isTimerRunning) {
        m_isTimerRunning = true;
        m_gameTimer->start();  // Use the 100ms interval set in constructor
    }
}

void myChess::stopTimer() {
    m_isTimerRunning = false;
    m_gameTimer->stop();
}

void myChess::resetTimers() {
    stopTimer();
    m_whiteTimeRemaining = m_timeControlMinutes * 60 * 1000;  // Convert to milliseconds
    m_blackTimeRemaining = m_timeControlMinutes * 60 * 1000;
    updateTimeDisplay();
}

void myChess::addIncrement() {
    // 為剛移動的玩家增加時間增量 (i.e., not the current turn)
    if (m_incrementSeconds > 0 && m_timeControlEnabled) {
        int incrementMs = m_incrementSeconds * 1000;
        if (m_chessBoard->getCurrentTurn() == PieceColor::WHITE) {
            // Black just moved, add increment to black
            m_blackTimeRemaining += incrementMs;
        } else {
            // White just moved, add increment to white
            m_whiteTimeRemaining += incrementMs;
        }
        updateTimeDisplay();
    }
}

QString myChess::formatTime(int milliseconds) {
    int totalSeconds = milliseconds / 1000;
    int minutes = totalSeconds / 60;
    int secs = totalSeconds % 60;
    int deciseconds = (milliseconds % 1000) / 100;  // Get 1 decimal place
    
    // 時間少於 10 秒時顯示 1 位小數
    if (totalSeconds < 10) {
        return QString("%1.%2").arg(secs).arg(deciseconds);
    } else {
        // Don't pad minutes with leading zero for single-digit minutes
        return QString("%1:%2").arg(minutes).arg(secs, 2, 10, QChar('0'));
    }
}
