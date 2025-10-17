#ifndef PACMAN_H
#define PACMAN_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QRectF>
#include <QtMath>

enum class MazeItem;

class Pacman : public QWidget {
    Q_OBJECT
public:
    explicit Pacman(QWidget *parent = nullptr);
    ~Pacman();

    void move(const MazeItem maze[][20], int mazeWidth, int mazeHeight, int cellSize);
    void draw(QPainter *painter, int cellSize);
    void setDirection(int key);
    void setNextDirection(int key);
    int getX() const { return x; }
    int getY() const { return y; }
    void setX(int newX) { x = newX; }
    void setY(int newY) { y = newY; }
    void setSpeed(int newSpeed) { speed = newSpeed; }

private:
    int x, y;
    int dx, dy;
    int nextDx, nextDy;
    int speed;
    int mouthAngle;
    bool mouthOpening;
    QTimer *animationTimer;

    bool checkWallCollision(int targetX, int targetY, const MazeItem maze[][20], int mazeWidth, int mazeHeight, int cellSize) const;
};

#endif // PACMAN_H
