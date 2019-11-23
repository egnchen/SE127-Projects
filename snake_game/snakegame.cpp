#include "snakegame.h"
#include "snake.h"
#include "helper.h"
#include <cstdlib>
#include <ctime>
#include <QMessageBox>
#include <QMenuBar>
#include <QTimer>
#include <QDebug>
#include <QInputDialog>
#include <QLineEdit>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

namespace Snake {

/*
 * init function
 * Initalize the snake game board
 */
SnakeGame::SnakeGame(unsigned int x, unsigned int y):
    gridCountX(x), gridCountY(y)
{
    // initialize grid
    grid = static_cast<gridPoint **>(calloc(x, sizeof(gridPoint *)));
    grid[0] = static_cast<gridPoint *>(calloc(x * y, sizeof(gridPoint)));
    assert(grid != nullptr);
    assert(grid[0] != nullptr);
    for(unsigned int i = 1; i < x; i++)
        grid[i] = grid[0] + i * y;

    resize(gridCountX * 30, gridCountY * 30);

    // set up theme
    theme[gridPoint::nothing] = Qt::NoBrush;
    theme[gridPoint::snake] = QColor(0, 0, 255);
    theme[gridPoint::wall] = QColor(0, 0, 0);
    theme[gridPoint::bonus] = QColor(0, 255, 0);
    theme[gridPoint::accel] = QColor(255, 0, 255);
    theme[gridPoint::breaker] = QColor(255, 0, 0);

    // set up timers
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(gameUpdate()));
    accelTimer = new QTimer(this);
    breakerTimer = new QTimer(this);
    accelTimer->setInterval(15000);
    breakerTimer->setInterval(10000);
    accelTimer->setSingleShot(true);
    breakerTimer->setSingleShot(true);
    connect(accelTimer, SIGNAL(timeout()), this, SLOT(accelUpdate()));
    connect(breakerTimer, SIGNAL(timeout()), this, SLOT(breakerUpdate()));

    // set focus
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

SnakeGame::~SnakeGame() {
    free(grid[0]);
    free(grid);
}

/*
 * helper function
 * generate a random point on the board
 */
Point SnakeGame::genPoint() const{
    Point pt;
    bool flag;
    do {
        flag = true;
        pt = Point(rand() % gridCountX, rand() % gridCountY);
        if(grid[pt.x][pt.y] != gridPoint::nothing) flag = false;
        if(pt.x > 0 && grid[pt.x - 1][pt.y] == gridPoint::wall) flag = false;
        if(pt.x < (gridCountX - 1) && grid[pt.x + 1][pt.y] == gridPoint::wall) flag = false;
        if(pt.y > 0 && grid[pt.x][pt.y - 1] == gridPoint::wall) flag = false;
        if(pt.y < (gridCountY - 1) && grid[pt.x][pt.y + 1] == gridPoint::wall) flag = false;
    } while (!flag);
    return pt;
}

/*
 * generate bonus point
 * randonly on the board
 */
void SnakeGame::genBonus() {
    Point pt = genPoint();
    grid[pt.x][pt.y] = gridPoint::bonus;
    lastBonus = pt;
    if((rand() & 0xf) == 0) {
        // generate acceleration or breaker
        // have a chance of 1/16
        pt = genPoint();
        if(rand() & 1) grid[pt.x][pt.y] = gridPoint::accel;
        else grid[pt.x][pt.y] = gridPoint::breaker;
    }
}

/*
 * generate a random block on the board
 */
void SnakeGame::genBlock() {
    Point pt = genPoint();
    grid[pt.x][pt.y] = gridPoint::wall;
}

/*
 * initialize a new game
 * this function will wipe everything and generate a new board
 * @param   multiplayer     indicate if multiplayer mode is on.
 *                          Will generate two snakes in multiplayer mode
 */
void SnakeGame::init(bool multiplayer) {
    // clear everything
    memset(grid[0], 0, sizeof(gridPoint) * gridCountX * gridCountY);
    timer->stop();
    accelTimer->stop();
    breakerTimer->stop();
    hosting = false;
    consumeWalls = false;
    snakes = std::vector<Snake>();

    // reinitialize rand seed
    srand(static_cast<unsigned>(time(nullptr)));

    // generate two bonus points
    genBonus();
    genBonus();
    // generate lots of blocks
    for(int i = 0; i < 10; i++)
        genBlock();

    snakes.push_back(Snake(gridCountX, gridCountY));
    if(multiplayer)
        snakes.push_back(Snake(gridCountX, gridCountY));

    interval = intervalHuman;
    timer->setInterval(interval);
    update();
}

/*
 * slot functions for start singleplayer & start multiplayer.
 */
void SnakeGame::start() { init(false); }
void SnakeGame::startMP() { init(true); }

/*
 * pause and resume slots.
 */
void SnakeGame::pause() {
    timer->stop();
    update();
}

void SnakeGame::resume() {
    timer->start();
    update();
}

/*
 * handle user input.
 * Player No.1 use ↑↓←→
 * Player No.2 use WSAD
 */
void SnakeGame::keyPressEvent(QKeyEvent *event) {
    int key = event->key();
    switch(key) {
    case Qt::Key_Down:  snakes[0].updateDirection(1, 0); break;
    case Qt::Key_Up:    snakes[0].updateDirection(-1,0); break;
    case Qt::Key_Left:  snakes[0].updateDirection(0,-1); break;
    case Qt::Key_Right: snakes[0].updateDirection(0, 1); break;
    default:
        if(snakes.size() > 1) // multiplyer
            switch(key) {
            case Qt::Key_S: snakes[1].updateDirection(1, 0); break;
            case Qt::Key_W: snakes[1].updateDirection(-1,0); break;
            case Qt::Key_A: snakes[1].updateDirection(0,-1); break;
            case Qt::Key_D: snakes[1].updateDirection(0, 1); break;
            }
    }
}

/*
 * Self update when computer is hosting the game
 */
void SnakeGame::updateHosting() {
    for(std::vector<Snake>::iterator ps = snakes.begin(); ps != snakes.end(); ++ps) {
        // first, find a direction using the naive approach
        Point dirToBonus = getDirection(ps->getHead(), lastBonus);
        if(dirToBonus != Point(0, 0)) {
            // determine whether the snake would be cornered
            // after making this move
            ps->updateDirection(dirToBonus.x, dirToBonus.y);
            gridPoint prevHead;
            if(ps->fakeUpdate(grid, prevHead)) {
                Point fakeHead = ps->getNextHead(),
                        fakeTail = ps->getNextTail(grid);
                if(fakeHead == fakeTail || getDirection(fakeHead, fakeTail) != Point(0,0)) {
                    // checked, use this direction
                    ps->reverse(grid, prevHead);
                    continue;
                }
                ps->reverse(grid, prevHead);
            }
            qDebug() << "Will get cornered, using other directions...";
        }
        // if naive approach fails,
        // choose whichever valid direction that won't make the snake lose its tail
        Point directions[4] = { Point(0,1), Point(0,-1), Point(1,0), Point(-1,0) };
        unsigned char rank[4] = {0};
        dirToBonus = lastBonus - ps->getHead();
        for(int i = 0; i < 4; i++) {
            // try all four directions and check if the snake loses its tail
            ps->updateDirection(directions[i].x, directions[i].y);
            gridPoint prevHead;
            if(ps->fakeUpdate(grid, prevHead)) {
                Point fakeHead = ps->getNextHead(),
                        fakeTail = ps->getNextTail(grid);
                if(getDirection(fakeHead, fakeTail) != Point(0,0))
                    rank[i] += 4;
                // the direction that put the snake head from the bonus point
                // is considered better
                if(Point::mahattanDistance(fakeHead, lastBonus) > Point::mahattanDistance(ps->getHead(), lastBonus))
                    rank[i] += Point::mahattanDistance(fakeHead, lastBonus) - Point::mahattanDistance(ps->getHead(), lastBonus);
                ps->reverse(grid, prevHead);
            }
        }
        qDebug() << rank[0] << rank[1] << rank[2] << rank[3];
        // choose the best one
        const Point &choice = directions[std::distance(rank, std::max_element(rank, rank + 4))];
        ps->updateDirection(choice.x, choice.y);
    }
}

/*
 * update & repaint game
 */
void SnakeGame::gameUpdate() {
    int consumedBonus = 0;

    // hosted
    if(hosting)
        updateHosting();
    // update all snakes and grid accordingly
    for(std::vector<Snake>::iterator ps = snakes.begin(); ps != snakes.end(); ++ps) {
        if(!ps->update(grid)) {
            // this snake is dead
            Point pt = ps->getNextHead();

            timer->stop();
            QString prompt;
            if(!ps->withinBound(pt))
                prompt = "You just went outside the game area.";
            else if(grid[pt.x][pt.y] == gridPoint::snake)
                prompt = "You just ate yourself(or another snake).";
            else if(grid[pt.x][pt.y] == gridPoint::wall) {
                if(consumeWalls) {
                    // not dead if consume wall is on
                    grid[pt.x][pt.y] = gridPoint::nothing;
                    ps->update(grid);
                    continue;
                }
                prompt = "You bumped into the wall.";
            }

            QMessageBox::information(nullptr, "Failed", prompt);
            recordScore();
            this->init(false);
            break;
        } else {
            if(!ps->haveUpdatedTail()) {
                consumedBonus++;
            } else if(ps->getHeadPrev() == gridPoint::accel) {
                // enable acceleration
                qDebug() << "acceleration on";
                timer->setInterval(interval / 2);
                accelTimer->start();
            } else if(ps->getHeadPrev() == gridPoint::breaker) {
                // enable breaker
                qDebug() << "breaker on";
                consumeWalls = true;
                breakerTimer->start();
            }
        }
    }
    if(timer->isActive() == false)
        return;
    // regenerate all bonus points
    while(consumedBonus--) genBonus();
    // repaint the whole picture
    update();
}

void SnakeGame::recordScore() {
    if(snakes.size() > 1) return;
    QList<snake::Helper::entry> entris = snake::Helper::getSortedRecords();
    if(entris.length() >= 10 && entris.last().score > snakes[0].getLength())
        return;

    bool ok;
    QString name = QInputDialog::getText(this, "大侠留下尊姓大名", "Enter your name here, we'll record your score:",
                                         QLineEdit::Normal, QString(), &ok);
    if(ok && name.length() > 0) {
        snake::Helper::addRecord(name, snakes[0].getLength());
    }
}

// get direction to given point
Point SnakeGame::getDirection(const Point src, const Point dest) const {
    const Point directions[4] = { Point(0,1), Point(1,0), Point(0,-1), Point(-1,0) };
    // initialize array
    static Point **pathGrid = static_cast<Point **>(calloc(gridCountX, sizeof(Point *)));
    if(pathGrid[0] == nullptr) {
        pathGrid[0] = static_cast<Point *>(calloc(gridCountX * gridCountY, sizeof(Point)));
        for(int i = 1; i < gridCountX; i++)
            pathGrid[i] = pathGrid[0] + i * gridCountY;
    } else {
        memset(pathGrid[0], 0, sizeof(Point) * gridCountX * gridCountY);
    }

    std::queue<Point> q = std::queue<Point>();
    q.push(src);
    while(!q.empty()) {
        Point cur = q.front();
        q.pop();
        for(int i = 0; i < 4; i++) {
            Point next = cur + directions[i];
            if(next.x < 0 || next.x >= int(gridCountX) || next.y < 0 || next.y >= int(gridCountY))
                continue;
            if(next == dest) {
                // reached, walk back
                while(!(cur - pathGrid[cur.x][cur.y] == src))
                    cur -= pathGrid[cur.x][cur.y];
                return cur == src ? directions[i] : cur - src;
            }
            if(pathGrid[next.x][next.y] == Point(0,0) &&
                    grid[next.x][next.y] != gridPoint::wall && grid[next.x][next.y] != gridPoint::snake) {
                // can walk here
                pathGrid[next.x][next.y] = directions[i];
                q.push(next);
            }
        }
    }

    // loop exited, path not found
    return Point(0,0);
}

void SnakeGame::accelUpdate() {
    // slot to disable accleration
    timer->setInterval(interval);
}

void SnakeGame::breakerUpdate() {
    // slot to disable breaker
    consumeWalls = false;
}

void SnakeGame::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::RenderHint::Antialiasing);
    // draw out the grid
    painter.setPen(QColor(64, 64, 64));
    painter.setBrush(theme[gridPoint::nothing]);
    float gridWidth = float(width()) / gridCountX,
            gridHeight = float(height()) / gridCountY;
    for(unsigned int i = 0; i < gridCountX; i++)
        painter.drawLine(0, gridHeight * i, width(), gridHeight * i);
    for(unsigned int i = 0; i < gridCountY; i++)
        painter.drawLine(gridWidth * i, 0, gridWidth * i, height());

    if(snakes.size()) {
        // draw out the content
        painter.save();
        painter.setPen(QColor(128, 128, 128));
        for(int i = 0; i < gridCountX; i++)
            for(int j = 0; j < gridCountY; j++)
                if(grid[i][j] != gridPoint::nothing) {
                    painter.setBrush(theme[grid[i][j]]);
                    painter.drawRoundedRect(j * gridWidth, i * gridHeight, gridWidth, gridHeight, gridWidth * 0.2, gridHeight * 0.2);
                }
        painter.restore();


        // draw out hosting prompt
        QString prompt;
        // score information
        prompt.append("Score: ");
        for(Snake &s: snakes) {
            prompt.append(std::to_string(s.getLength()).c_str());
            prompt.append(" ");
        }
        prompt.append("\n");
        // status information
        if(hosting)
            prompt += "Hosting...\n";
        if(breakerTimer->isActive())
            prompt += QString("Breaker on [%1s]\n").arg(breakerTimer->remainingTime() / 1000);
        if(accelTimer->isActive())
            prompt += QString("Acceleration on [%1s]\n").arg(accelTimer->remainingTime() / 1000);
        // draw out prompt
        painter.setFont(QFont("Sans Serif", 15, 20));
        painter.setPen(QColor("white"));
        painter.drawText(rect(), Qt::AlignTop | Qt::AlignLeft, prompt);
    }
    if(timer && timer->isActive() == false) {
        // draw out prompt right in the middle
        QFont font = QFont("Sans Serif", 20, 40);
        painter.setFont(font);
        painter.setPen(QColor("white"));
        QString prompt;
        if(snakes.size() == 1) {
            prompt += "Singleplayer\n";
            prompt += "Control: ↑↓←→\n";
        }
        else {
            prompt += "Multiplayer\n";
            prompt += "Control: ↑↓←→ WSAD\n";
        }
        painter.drawText(rect(), Qt::AlignCenter, prompt + "Use Ctrl+R to start.");
    }
}

void SnakeGame::toggleHosting() {
    hosting = !hosting;
    interval = hosting ? intervalComputer : intervalHuman;
    timer->setInterval(interval);
}
}
