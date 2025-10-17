#include "mainwindow.h"
#include <QKeyEvent>
#include <QPainter>
#include <QtMath>
#include <QPixmap>
#include <QDebug>
#include <QRandomGenerator>
#include <QPushButton>

bool checkWallCollision(int x, int y, int width, int height, const MazeItem maze[][20], int mazeWidth, int mazeHeight, int cellSize) {
    QRectF rect(x, y, width, height);
    int startX = qFloor(rect.left() / cellSize);
    int endX = qFloor(rect.right() / cellSize);
    int startY = qFloor(rect.top() / cellSize);
    int endY = qFloor(rect.bottom() / cellSize);

    if (static_cast<int>(rect.width()) > 0 && static_cast<int>(rect.right()) % cellSize == 0)
        endX--;
    if (static_cast<int>(rect.height()) > 0 && static_cast<int>(rect.bottom()) % cellSize == 0)
        endY--;

    startX = qMax(0, startX);
    endX = qMin(mazeWidth - 1, endX);
    startY = qMax(0, startY);
    endY = qMin(mazeHeight - 1, endY);

    for (int i = startY; i <= endY; ++i) {
        for (int j = startX; j <= endX; ++j) {
            if (i >= 0 && i < mazeHeight && j >= 0 && j < mazeWidth && maze[i][j] == MazeItem::Wall) {
                return true;
            }
        }
    }
    return false;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), pacman(new Pacman(this)), timer(new QTimer(this)),
    score(0), lives(3), gameEnded(false),
    powerUpActive(false), powerUpTimer(new QTimer(this))
{
    setFixedSize(mazeWidth * cellSize, mazeHeight * cellSize);
    initializeMaze();
    connect(timer, &QTimer::timeout, this, &MainWindow::gameLoop);
    timer->start(100);

    pacman->setX(cellSize * 1 + cellSize / 2);
    pacman->setY(cellSize * 1 + cellSize / 2);
    setFocusPolicy(Qt::StrongFocus);

    connect(powerUpTimer, &QTimer::timeout, this, &MainWindow::endPowerUp);
    originalPacmanSpeed = 2;

    enemyColors[0] = Qt::red;
    enemyColors[1] = Qt::cyan;
    enemyColors[2] = Qt::magenta;

    tryAgainButton = new QPushButton("Try Again", this);
    tryAgainButton->setGeometry(width()/2 - 50, height()/2 + 40, 100, 30);
    tryAgainButton->hide();
    connect(tryAgainButton, &QPushButton::clicked, this, &MainWindow::resetGame);
}

MainWindow::~MainWindow() {
    delete pacman;
    delete timer;
    delete powerUpTimer;
    delete tryAgainButton;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    pacman->setNextDirection(event->key());
}

void MainWindow::resetPositions() {
    // Reset Pacman
    pacman->setX(cellSize * 1 + cellSize / 2);
    pacman->setY(cellSize * 1 + cellSize / 2);
    pacman->setNextDirection(Qt::Key_Right);

    // Reset enemies to their initial positions
    enemyPos[0].setX(cellSize * 9 + cellSize / 2);
    enemyPos[0].setY(cellSize * 9 + cellSize / 2);
    enemyPos[1].setX(cellSize * 9 + cellSize / 2);
    enemyPos[1].setY(cellSize * 9 + cellSize / 2);
    enemyPos[2].setX(cellSize * 15 + cellSize / 2);
    enemyPos[2].setY(cellSize * 14 + cellSize / 2);

    for (int i = 0; i < enemyCount; i++) {
        changeEnemyDirection(i);
    }
}

void MainWindow::resetGame() {
    lives = 3;
    score = 0;
    gameEnded = false;
    tryAgainButton->hide();
    initializeMaze();
    resetPositions();
    timer->start(100);
    update();
}

void MainWindow::initializeMaze() {
    const char initialMaze[mazeHeight][mazeWidth] = {
        {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
        {'W', 'S', 'D', 'N', 'D', 'D', 'D', 'N', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'W'},
        {'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'D', 'D', 'W'},
        {'W', 'D', 'W', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'D', 'N', 'W', 'D', 'D', 'D', 'W', 'D', 'W', 'W'},
        {'W', 'D', 'W', 'N', 'W', 'W', 'W', 'D', 'W', 'D', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'D', 'W', 'W'},
        {'W', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'W', 'W'},
        {'W', 'W', 'W', 'D', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'W', 'D', 'W', 'W', 'W'},
        {'W', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'W'},
        {'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W'},
        {'W', 'D', 'W', 'D', 'D', 'E', 'W', 'D', 'E', 'E', 'D', 'D', 'W', 'D', 'D', 'D', 'W', 'D', 'W', 'W'},
        {'W', 'D', 'D', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'D', 'W', 'W'},
        {'W', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'W', 'W'},
        {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'W'},
        {'W', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'W'},
        {'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'E', 'D', 'W'},
        {'W', 'N', 'W', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'W', 'N', 'W', 'W'},
        {'W', 'D', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'W', 'W', 'D', 'W', 'D', 'W', 'W'},
        {'W', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'W', 'D', 'W', 'D', 'D', 'D', 'W', 'D', 'D', 'D', 'W', 'W'},
        {'W', 'D', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'D', 'D', 'W'},
        {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'}
    };

    dotPositions.clear();
    powerPellets.clear();

    for (int i = 0; i < mazeHeight; ++i) {
        for (int j = 0; j < mazeWidth; ++j) {
            switch (initialMaze[i][j]) {
            case 'W':
                maze[i][j] = MazeItem::Wall;
                break;
            case 'D':
                maze[i][j] = MazeItem::Dot;
                dotPositions.append(getCellCenter(i, j));
                break;
            case 'P':
                maze[i][j] = MazeItem::Path;
                break;
            case 'N':
                maze[i][j] = MazeItem::PowerPellet;
                powerPellets.append(getCellCenter(i, j));
                break;
            case 'S':
                maze[i][j] = MazeItem::Path;
                pacman->setX(j * cellSize + cellSize / 2);
                pacman->setY(i * cellSize + cellSize / 2);
                break;
            case 'E':
                maze[i][j] = MazeItem::Path;
                for (int e = 0; e < enemyCount; e++) {
                    if (enemyPos[e].isNull()) {
                        enemyPos[e].setX(j * cellSize + cellSize / 2);
                        enemyPos[e].setY(i * cellSize + cellSize / 2);
                        enemyDx[e] = (e % 2 == 0) ? 2 : -2;
                        enemyDy[e] = 0;
                        changeEnemyDirection(e);
                        break;
                    }
                }
                break;
            default:
                maze[i][j] = MazeItem::Path;
                break;
            }
        }
    }
}

QPoint MainWindow::getCellCenter(int row, int col) const {
    return QPoint(col * cellSize + cellSize / 2, row * cellSize + cellSize / 2);
}

void MainWindow::changeEnemyDirection(int index) {
    QVector<QPoint> possibleDirections;
    int enemyWidth = 16;
    int enemyHeight = 16;
    int targetX, targetY;

    // Check right
    targetX = enemyPos[index].x() + cellSize;
    targetY = enemyPos[index].y();
    if (!checkWallCollision(targetX - enemyWidth / 2, targetY - enemyHeight / 2, enemyWidth, enemyHeight, maze, mazeWidth, mazeHeight, cellSize))
        possibleDirections.append(QPoint(cellSize, 0));

    // Check left
    targetX = enemyPos[index].x() - cellSize;
    targetY = enemyPos[index].y();
    if (!checkWallCollision(targetX - enemyWidth / 2, targetY - enemyHeight / 2, enemyWidth, enemyHeight, maze, mazeWidth, mazeHeight, cellSize))
        possibleDirections.append(QPoint(-cellSize, 0));

    // Check down
    targetX = enemyPos[index].x();
    targetY = enemyPos[index].y() + cellSize;
    if (!checkWallCollision(targetX - enemyWidth / 2, targetY - enemyHeight / 2, enemyWidth, enemyHeight, maze, mazeWidth, mazeHeight, cellSize))
        possibleDirections.append(QPoint(0, cellSize));

    // Check up
    targetX = enemyPos[index].x();
    targetY = enemyPos[index].y() - cellSize;
    if (!checkWallCollision(targetX - enemyWidth / 2, targetY - enemyHeight / 2, enemyWidth, enemyHeight, maze, mazeWidth, mazeHeight, cellSize))
        possibleDirections.append(QPoint(0, -cellSize));

    if (!possibleDirections.isEmpty()) {
        int randomIndex = QRandomGenerator::global()->bounded(possibleDirections.size());
        QPoint newDirection = possibleDirections[randomIndex];
        enemyDx[index] = newDirection.x();
        enemyDy[index] = newDirection.y();
    }
}

void MainWindow::moveEnemy(int index) {
    int targetX = enemyPos[index].x() + enemyDx[index];
    int targetY = enemyPos[index].y() + enemyDy[index];
    int enemyWidth = 16;
    int enemyHeight = 16;

    if (!checkWallCollision(targetX - enemyWidth / 2, targetY - enemyHeight / 2, enemyWidth, enemyHeight, maze, mazeWidth, mazeHeight, cellSize)) {
        enemyPos[index].rx() = targetX;
        enemyPos[index].ry() = targetY;
        enemyPos[index].rx() = (enemyPos[index].x() / cellSize) * cellSize + cellSize / 2;
        enemyPos[index].ry() = (enemyPos[index].y() / cellSize) * cellSize + cellSize / 2;
    } else {
        changeEnemyDirection(index);
    }
}

void MainWindow::gameLoop() {
    pacman->move(maze, mazeWidth, mazeHeight, cellSize);
    for (int i = 0; i < enemyCount; i++) {
        moveEnemy(i);
    }

    QRect pacmanRect(pacman->getX() - cellSize / 2, pacman->getY() - cellSize / 2, cellSize, cellSize);
    QPoint pacmanCenter(pacman->getX(), pacman->getY());

    // Check collision with dots
    for (int i = 0; i < dotPositions.size(); ++i) {
        if (QLineF(pacmanCenter, dotPositions[i]).length() < cellSize / 2.0) {
            int row = dotPositions[i].y() / cellSize;
            int col = dotPositions[i].x() / cellSize;
            if (row >= 0 && row < mazeHeight && col >= 0 && col < mazeWidth) {
                maze[row][col] = MazeItem::Path;
            }
            dotPositions.remove(i);
            score += 10;
            break;
        }
    }

    // Check collision with power pellets
    for (int i = 0; i < powerPellets.size(); ++i) {
        if (QLineF(pacmanCenter, powerPellets[i]).length() < cellSize / 2.0) {
            int row = powerPellets[i].y() / cellSize;
            int col = powerPellets[i].x() / cellSize;
            if (row >= 0 && row < mazeHeight && col >= 0 && col < mazeWidth) {
                maze[row][col] = MazeItem::Path;
            }
            powerPellets.remove(i);
            score += 50;
            startPowerUp();
            break;
        }
    }

    for (int i = 0; i < enemyCount; i++) {
        QRect enemyRect(enemyPos[i].x() - 15, enemyPos[i].y() - 15, 30, 30);
        if (pacmanRect.intersects(enemyRect)) {
            if (powerUpActive) {
                enemyPos[i].setX(cellSize * 10 + cellSize / 2);
                enemyPos[i].setY(cellSize * 10 + cellSize / 2);
                changeEnemyDirection(i);
            } else {
                lives--;
                if (lives <= 0) {
                    timer->stop();
                    gameEnded = true;
                } else {
                    resetPositions();
                }
                break;
            }
        }
    }
    update();
}

void MainWindow::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::black);

    // Draw maze
    for (int i = 0; i < mazeHeight; ++i) {
        for (int j = 0; j < mazeWidth; ++j) {
            QRect cellRect(j * cellSize, i * cellSize, cellSize, cellSize);
            if (maze[i][j] == MazeItem::Wall) {
                painter.fillRect(cellRect, Qt::blue);
            } else if (maze[i][j] == MazeItem::Dot) {
                painter.setBrush(Qt::yellow);
                painter.drawEllipse(getCellCenter(i, j), 3, 3);
            } else if (maze[i][j] == MazeItem::PowerPellet) {
                painter.setBrush(Qt::white);
                painter.drawEllipse(getCellCenter(i, j), 7, 7);
            }
        }
    }

    pacman->draw(&painter, cellSize);

    // Draw enemies
    for (int i = 0; i < enemyCount; i++) {
        QPixmap enemyPixmap(":/images/enemy.png");
        QColor enemyColor = powerUpActive ? Qt::blue : enemyColors[i];
        if (enemyPixmap.isNull()) {
            painter.setBrush(enemyColor);
            painter.drawEllipse(enemyPos[i], 10, 10);
        } else {
            painter.drawPixmap(enemyPos[i].x() - 15, enemyPos[i].y() - 15,
                               enemyPixmap.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    // Draw score and lives
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14));
    painter.drawText(10, 20, QString("Score: %1").arg(score));
    painter.drawText(width() - 100, 20, QString("Lives: %1").arg(lives));

    if (gameEnded) {
        painter.setPen(Qt::red);
        painter.setFont(QFont("Arial", 24, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "GAME OVER!");
        tryAgainButton->show();
    } else if (!timer->isActive() && dotPositions.isEmpty() && powerPellets.isEmpty()) {
        painter.setPen(Qt::green);
        painter.setFont(QFont("Arial", 24, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "YOU WIN!");
    }
}

void MainWindow::startPowerUp() {
    powerUpActive = true;
    pacman->setSpeed(originalPacmanSpeed * 2);
    powerUpTimer->start(10000);
}

void MainWindow::endPowerUp() {
    powerUpActive = false;
    pacman->setSpeed(originalPacmanSpeed);
    powerUpTimer->stop();
}
