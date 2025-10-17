// pacman.cpp
#include "pacman.h"
#include <QKeyEvent>
#include <QDebug>
#include "mainwindow.h"
#include <QTimer>
#include <QRect>
#include <QtMath>

Pacman::Pacman(QWidget *parent)
    : QWidget(parent), x(0), y(0), dx(0), dy(0), nextDx(0), nextDy(0), speed(2),
    mouthAngle(0), mouthOpening(true)
{
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, [this]() {
        // Update mouth angle (0-45 degrees)
        if (mouthOpening) {
            mouthAngle += 5;
            if (mouthAngle >= 45) {
                mouthOpening = false;
            }
        } else {
            mouthAngle -= 5;
            if (mouthAngle <= 0) {
                mouthOpening = true;
            }
        }
    });
    animationTimer->start(100); // Controls animation speed
}

Pacman::~Pacman() {
    if (animationTimer) {
        animationTimer->stop();
        delete animationTimer;
    }
}

void Pacman::setDirection(int key) {
    switch (key) {
    case Qt::Key_Up:
        nextDx = 0;
        nextDy = -1;
        break;
    case Qt::Key_Down:
        nextDx = 0;
        nextDy = 1;
        break;
    case Qt::Key_Left:
        nextDx = -1;
        nextDy = 0;
        break;
    case Qt::Key_Right:
        nextDx = 1;
        nextDy = 0;
        break;
    default:
        break;
    }
}

void Pacman::setNextDirection(int key) {
    setDirection(key);
}

bool Pacman::checkWallCollision(int targetX, int targetY, const MazeItem maze[][20],
                                int mazeWidth, int mazeHeight, int cellSize) const {
    QRectF pacmanRect(targetX - cellSize / 2.0, targetY - cellSize / 2.0, cellSize, cellSize);

    int startGridX = qFloor(pacmanRect.left() / cellSize);
    int endGridX = qFloor(pacmanRect.right() / cellSize);
    int startGridY = qFloor(pacmanRect.top() / cellSize);
    int endGridY = qFloor(pacmanRect.bottom() / cellSize);

    if (static_cast<int>(pacmanRect.width()) > 0 && static_cast<int>(pacmanRect.right()) % cellSize == 0) {
        endGridX--;
    }
    if (static_cast<int>(pacmanRect.height()) > 0 && static_cast<int>(pacmanRect.bottom()) % cellSize == 0) {
        endGridY--;
    }

    startGridX = qMax(0, startGridX);
    endGridX = qMin(mazeWidth - 1, endGridX);
    startGridY = qMax(0, startGridY);
    endGridY = qMin(mazeHeight - 1, endGridY);

    for (int i = startGridY; i <= endGridY; ++i) {
        for (int j = startGridX; j <= endGridX; ++j) {
            if (i >= 0 && i < mazeHeight && j >= 0 && j < mazeWidth) {
                if (maze[i][j] == MazeItem::Wall) {
                    return true;
                }
            } else {
                return true;
            }
        }
    }
    return false;
}

void Pacman::move(const MazeItem maze[][20], int mazeWidth, int mazeHeight, int cellSize) {
    int potentialNewX_next = x + nextDx * speed;
    int potentialNewY_next = y + nextDy * speed;

    int potentialNewX_current = x + dx * speed;
    int potentialNewY_current = y + dy * speed;

    bool canMoveInNextDirection = !checkWallCollision(potentialNewX_next, potentialNewY_next,
                                                      maze, mazeWidth, mazeHeight, cellSize);

    if (canMoveInNextDirection) {
        dx = nextDx;
        dy = nextDy;
        x = potentialNewX_next;
        y = potentialNewY_next;
    } else {
        bool canMoveInCurrentDirection = !checkWallCollision(potentialNewX_current, potentialNewY_current,
                                                             maze, mazeWidth, mazeHeight, cellSize);

        if (canMoveInCurrentDirection) {
            if (dx != nextDx || dy != nextDy) {
                x = potentialNewX_current;
                y = potentialNewY_current;
            } else {
                dx = 0;
                dy = 0;
            }
        } else {
            dx = 0;
            dy = 0;
        }
    }
}

void Pacman::draw(QPainter *painter, int cellSize) {
    painter->setBrush(Qt::yellow);
    painter->setPen(Qt::NoPen);

    int radius = cellSize / 2;

    if (dx != 0 || dy != 0) {
        // Correct mouth orientation based on movement direction
        int startAngle = 0;
        if (dx > 0) {       // Moving right
            startAngle = 45 * 16;    // Mouth opens right
        } else if (dx < 0) { // Moving left
            startAngle = 225 * 16;   // Mouth opens left
        } else if (dy > 0) { // Moving down
              startAngle = 315 * 16; // Mouth opens down
        } else if (dy < 0) { // Moving up
            startAngle = 135 * 16;   // Mouth opens up
        }

        // Draw Pacman as a pie slice
        painter->drawPie(QRectF(x - radius, y - radius, cellSize, cellSize),
                         startAngle - mouthAngle * 16,  // Mouth opens in movement direction
                         360 * 16 - 2 * mouthAngle * 16);
    } else {
        // Draw as full circle when not moving
        painter->drawEllipse(QRectF(x - radius, y - radius, cellSize, cellSize));
    }
}
