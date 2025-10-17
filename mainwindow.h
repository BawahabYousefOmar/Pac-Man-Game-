#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <QPoint>
#include "pacman.h"
#include <QPushButton>

// Define different types of maze elements
enum class MazeItem {
    Wall,
    Path,
    Dot,
    PowerPellet
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override; // Handles keyboard input
    void paintEvent(QPaintEvent *event) override; // Responsible for drawing the game screen

private slots:
    void gameLoop(); // Runs every tick to update game logic
   // void endPowerUp();
    void resetGame(); // Function to reset the game
private:
    Pacman *pacman; // Pacman character instance
    QTimer *timer; // Timer to control game updates
    int score; // Player score
    // QVector<QPoint> pellets; // No longer directly storing pellet positions
    QVector<QPoint> powerPellets; // Store power pellet positions
    QVector<QPoint> dotPositions; // Store remaining dot positions
    // This NEW ADDED CODE
    int lives; // Track remaining lives
    bool gameEnded; // Track if game has ended
    QPushButton *tryAgainButton; // Button to restart game
    bool powerUpActive;
    QTimer *powerUpTimer;
    int originalPacmanSpeed;
    QColor originalEnemyColor; // Store original enemy color
    void startPowerUp();
    void endPowerUp();
    // THE NEW ADDED CODE ENDS HERE
    static const int mazeWidth = 20; // Width of the maze in cells
    static const int mazeHeight = 20; // Height of the maze in cells
    static const int cellSize = 20; // Size of each cell in pixels
    MazeItem maze[mazeHeight][mazeWidth]; // 2D array to represent the maze
    void initializeMaze(); // Initialize the maze layout
    // void initializePellets(); // No longer used directly
    // Enemy logic
    static const int enemyCount = 3; // Number of enemies
    QPoint enemyPos[enemyCount]; // Current position of the enemy
    int enemyDx[enemyCount];
    int enemyDy[enemyCount]; // Direction of enemy movement
    QColor enemyColors[enemyCount]; // Colors for enemies
    void moveEnemy(int index); // Update specific enemy position
    void changeEnemyDirection(int index); // Change direction for specific enemy
    QPoint getCellCenter(int row, int col) const; // Helper to get the center of a cell
    void resetPositions();
};

#endif // MAINWINDOW_H
