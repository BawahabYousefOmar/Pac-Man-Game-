#include <QApplication>
#include "mainwindow.h"
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window; // Create main window for the game
    window.show(); // Show the window
    return app.exec(); // Start the Qt application loop
}


/*

*/
