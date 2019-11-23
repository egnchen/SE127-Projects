#include "snake.h"
#include <ctime>
#include <cstdlib>
#include <cassert>
#include <QtCore>

namespace Snake {
/*
 * Snake - abstrated version of snake
 * This class is a platform & framework independent
 * logic layer implementation of a snake
 */
Snake::Snake(int boundaryX, int boundaryY):
    boundary(Point(boundaryX, boundaryY))
{
    // init startup position
    body = std::deque<Point>();
    body.push_front(
                Point(boundaryX / 4 + rand() % (boundaryX / 2),
                      boundaryY / 4 + rand() % (boundaryY / 2)));

    // init startup direction
    int rand = random() & 3;
    direction = Point( ((rand & 2) ? (-1) : 1) * (rand & 1) ? 0 : 1,
                       ((rand & 2) ? (-1) : 1) * (rand & 1) ? 1 : 0 );
}

bool Snake::update(gridPoint **grid) {
    // update through direction
    Point newPoint = getNextHead();

    if(!withinBound(newPoint) ||
            grid[newPoint.x][newPoint.y] == gridPoint::wall ||
            grid[newPoint.x][newPoint.y] == gridPoint::snake)
        // dead
        return false;

    // update tail
    if(grid[newPoint.x][newPoint.y] == gridPoint::bonus) {
        // got bonus, don't have to update
        _haveUpdatedTail = false;
    } else {
        _haveUpdatedTail = true;
        prevTail = body.back();
        body.pop_back();
        grid[prevTail.x][prevTail.y] = gridPoint::nothing;
    }
    // update head
    body.push_front(newPoint);
    prevHead = grid[newPoint.x][newPoint.y];
    grid[newPoint.x][newPoint.y] = gridPoint::snake;
    _haveUpdatedHead = true;
    return true;
}

// fake update for automated playing
// this function won't update the exposed states stored in snake object.
bool Snake::fakeUpdate(gridPoint **grid, gridPoint &prevHead) const {
    Point newPoint = getNextHead();
    if(!withinBound(newPoint))
        return false;
    if(grid[newPoint.x][newPoint.y] == gridPoint::wall ||
            grid[newPoint.x][newPoint.y] == gridPoint::snake)
        return false;

    // update tail
    if(grid[newPoint.x][newPoint.y] != gridPoint::bonus)
        grid[getTail().x][getTail().y] = gridPoint::nothing;
    // update head
    prevHead = grid[newPoint.x][newPoint.y];
    grid[newPoint.x][newPoint.y] = gridPoint::snake;
    return true;
}

void Snake::reverse(gridPoint **grid, gridPoint prevHead) const {
    Point newPoint = getNextHead();
    grid[newPoint.x][newPoint.y] = prevHead;
    if(prevHead != gridPoint::bonus)
        // restore tail
        grid[body.back().x][body.back().y] = gridPoint::snake;
}

void Snake::updateDirection(int directionX, int directionY) {
    assert((directionX == 0) ^ (directionY == 0));
    assert(abs(directionX) <= 1 && abs(directionY) <= 1);
    direction = Point(directionX, directionY);
}

Point Snake::getNextTail(gridPoint **grid) const {
    Point head = getNextHead();
    if(grid[head.x][head.y] == gridPoint::bonus)
        return getTail();
    else
        return body.size() == 1 ? head : body.rbegin()[1];
}
}
