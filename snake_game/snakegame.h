#ifndef SNAKEWINDOW_H
#define SNAKEWINDOW_H
#include <QKeyEvent>
#include "snake.h"
#include <vector>
#include <deque>
#include <queue>
#include <QMenu>
#include <QMenuBar>
#include <QPainter>
#include <QTimer>

namespace Snake {
class SnakeGame: public QWidget
{
    Q_OBJECT
public:
    SnakeGame(unsigned int x = 30, unsigned int y = 30);
    ~SnakeGame() override;

    void getSize(unsigned int &x, unsigned int &y) const { x = gridCountX; y = gridCountY; }
    bool isHosting() const { return hosting; }
    bool isAccel() const { return accelTimer ? accelTimer->isActive() : false; }
    bool isBreaker() const { return consumeWalls; }

private:
    static const int intervalHuman = 80;
    static const int intervalComputer = 40;
    int interval;
    unsigned int gridCountX, gridCountY;
    QTimer *timer = nullptr;
    QTimer *accelTimer = nullptr, *breakerTimer = nullptr;

    bool consumeWalls = false;
    bool hosting = false;
    gridPoint **grid;
    Point lastBonus;
    QColor theme[6];

    std::vector<Snake> snakes;

    void init(bool multiplayer);
    void genBonus();
    void genBlock();
    Point genPoint() const;
    Point getDirection(const Point src, const Point dest) const;
    void updateHosting();
    void recordScore();

public slots:
    void gameUpdate();
    void accelUpdate();
    void breakerUpdate();
    void start();
    void startMP();
    void pause();
    void resume();
    void toggleHosting();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

};
}
#endif // SNAKEWINDOW_H
