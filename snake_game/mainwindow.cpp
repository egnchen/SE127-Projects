#include "mainwindow.h"
#include "snakegame.h"
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QStatusBar>

MainWindow::MainWindow()
{
    snakeGame = new Snake::SnakeGame();
    leaderBoardWindow = new LeaderBoardWindow();
    setCentralWidget(snakeGame);
    initMenu();
    resize(800, 800);
    setWindowTitle("贪吃蛇");
    snakeGame->start();
}

void MainWindow::initMenu() {
    QMenu *gameMenu = menuBar()->addMenu(tr("&Game"));
    QMenu *advancedMenu = menuBar()->addMenu(tr("&advanced"));
    QAction *newGameAction = new QAction(tr("&New game"), this),
            *startAction = new QAction(tr("&Start"), this),
            *pauseAction = new QAction(tr("&Pause"), this),
            *exitAction = new QAction(tr("&exit"), this),
            *startMPAction = new QAction(tr("&Multiplayer"), this),
            *hostAction = new QAction(tr("&Host"), this);
    QAction *showRankAction = new QAction(tr("&Leaderboard"), this);

    newGameAction->setShortcut(QKeySequence("Ctrl+N"));
    connect(newGameAction, SIGNAL(triggered()), snakeGame, SLOT(start()));
    pauseAction->setShortcut(QKeySequence("Ctrl+P"));
    connect(pauseAction, SIGNAL(triggered()), snakeGame, SLOT(pause()));
    startAction->setShortcut(QKeySequence("Ctrl+R"));
    connect(startAction, SIGNAL(triggered()), snakeGame, SLOT(resume()));
    startMPAction->setShortcut(QKeySequence("Ctrl+M"));
    exitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(startMPAction, SIGNAL(triggered()), snakeGame, SLOT(startMP()));
    hostAction->setShortcut(QKeySequence("Ctrl+H"));
    connect(hostAction, SIGNAL(triggered()), snakeGame, SLOT(toggleHosting()));
    showRankAction->setShortcut(QKeySequence("Ctrl+L"));
    connect(showRankAction, SIGNAL(triggered()), leaderBoardWindow, SLOT(show()));

    gameMenu->addAction(newGameAction);
    gameMenu->addAction(startAction);
    gameMenu->addAction(pauseAction);
    gameMenu->addSeparator();
    gameMenu->addAction(exitAction);

    advancedMenu->addAction(startMPAction);
    advancedMenu->addAction(hostAction);
    advancedMenu->addSeparator();
    advancedMenu->addAction(showRankAction);
}

void MainWindow::initStatusBar() {
    statusBar()->show();
}

MainWindow::~MainWindow() {
    delete leaderBoardWindow;
    delete snakeGame;
    // do nothing
}
