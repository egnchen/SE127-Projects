#ifndef SNAKE_H
#define SNAKE_H
#include <deque>
#include <vector>
#include <iostream>

namespace Snake {

struct Point {
    int x;
    int y;
    Point(int x = 0, int y = 0):x(x),y(y){}
    Point &operator+=(const Point rhs) { x += rhs.x; y += rhs.y; return *this; }
    Point operator+(const Point rhs) const { return Point(this->x + rhs.x, this->y + rhs.y); }
    Point &operator-(void) { this->x *= -1; this->y *= -1; return *this; }
    Point &operator-=(const Point rhs) { x -= rhs.x; y -= rhs.y; return *this; }
    Point operator-(const Point rhs) const { return Point(this->x - rhs.x, this->y - rhs.y); }
    bool operator<(const Point rhs) const { return x < rhs.x && y < rhs.y; }
    bool operator>(const Point rhs) const { return x > rhs.x && y > rhs.y; }
    bool operator==(const Point rhs) const { return x == rhs.x && y == rhs.y; }
    bool operator!=(const Point rhs) const { return !(*this == rhs); }
    std::ostream &operator<<(std::ostream& out) const { return out << this->x << ',' << this->y; }
    static int mahattanDistance(const Point a, const Point b) { return abs(a.x - b.x) + abs(a.y - b.y); }
};

enum gridPoint {
    nothing = 0,
    snake   = 1,
    wall    = 2,
    bonus   = 3,
    accel   = 4,
    breaker = 5
};

class Snake
{
private:
    Point boundary;
    Point direction;
    std::deque<Point> body;
    Point prevTail;
    gridPoint prevHead;
    bool _haveUpdatedHead = false;
    bool _haveUpdatedTail = false;

public:
    Snake(int boundaryX, int boundaryY);

    const std::deque<Point> &getBody() const { return body; }
    unsigned int getLength() const { return static_cast<unsigned int>(body.size()); }
    const Point &getHead() const { return body.front(); }
    const Point &getTail() const { return body.back(); }
    const Point getPrevTail() const { return prevTail; }
    const gridPoint &getHeadPrev() const { return prevHead; }

    Point getNextHead() const { return body.front() + direction; }
    Point getNextTail(gridPoint **grid) const;
    bool haveUpdatedHead() const { return _haveUpdatedHead; }
    bool haveUpdatedTail() const { return _haveUpdatedTail; }
    bool colliate(Point pt) const { return getHead() == pt; }
    bool withinBound(const Point p) const { return p < boundary && p > Point(-1, -1); }
    bool withinBound() const { return withinBound(getHead()); }

    bool update(gridPoint **grid);
    void updateDirection(int directionX, int directionY);
    bool fakeUpdate(gridPoint **grid, gridPoint &prevHead) const;
    void reverse(gridPoint **grid, gridPoint prevHead) const;
};

}
#endif // SNAKE_H
