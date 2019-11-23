#ifndef LEADERBOARDWINDOW_H
#define LEADERBOARDWINDOW_H

#include <QMainWindow>
#include <QTableWidget>

class LeaderBoardWindow: public QMainWindow
{
public:
    LeaderBoardWindow();
    ~LeaderBoardWindow() override;
    void refreshData();
private:
    QTableWidget *tbl;
protected:
    void showEvent(QShowEvent *event) override;
};

#endif // LEADERBOARDWINDOW_H
