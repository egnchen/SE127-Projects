#include "leaderboardwindow.h"
#include "helper.h"
#include <QTableWidget>

LeaderBoardWindow::LeaderBoardWindow()
{
    setWindowTitle("单人模式排行榜");
    tbl = new QTableWidget();
    QStringList headers;
    headers << "名字" << "分数";
    tbl->setColumnCount(2);
    tbl->setHorizontalHeaderLabels(headers);
    setCentralWidget(tbl);
}

LeaderBoardWindow::~LeaderBoardWindow() { delete tbl; }

void LeaderBoardWindow::refreshData() {
    QList<snake::Helper::entry> entries = snake::Helper::getSortedRecords();
    tbl->setRowCount(entries.length() >= 10 ? 10 : entries.length());
    for(int i = 0; i < tbl->rowCount(); i++) {
        tbl->setItem(i, 0, new QTableWidgetItem(entries[i].name));
        tbl->setItem(i, 1, new QTableWidgetItem(QString(std::to_string(entries[i].score).c_str())));
    }
    resize(400, (tbl->rowCount() + 2) * 30);
}

void LeaderBoardWindow::showEvent(QShowEvent *event) {
    this->refreshData();
}
