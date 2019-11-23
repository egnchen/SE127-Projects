#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "snakegame.h"
#include "leaderboardwindow.h"

class MainWindow: public QMainWindow
{
public:
    MainWindow();
    ~MainWindow() override;

private:
    Snake::SnakeGame *snakeGame;
    LeaderBoardWindow *leaderBoardWindow;
    void initMenu();
    void initStatusBar();

public slots:
    void showLeaderboard();
};

#endif // MAINWINDOW_H
