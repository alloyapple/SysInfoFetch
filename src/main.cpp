#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include "SystemDataProvider.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "Starting NeoFetch Pro...";
    
    SystemDataProvider systemData;
    qDebug() << "SystemDataProvider created";
    
    MainWindow window(&systemData);
    qDebug() << "MainWindow created";
    
    window.show();
    qDebug() << "Window shown";
    
    qDebug() << "Entering exec...";
    return app.exec();
}
