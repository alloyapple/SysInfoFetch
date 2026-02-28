#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QIcon>
#include "SystemDataProvider.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用程序图标（从资源文件加载）
    app.setWindowIcon(QIcon(":/logo.ico"));
    
    qDebug() << "Starting NeoFetch Pro...";
    
    SystemDataProvider systemData;
    qDebug() << "SystemDataProvider created";
    
    MainWindow window(&systemData);
    qDebug() << "MainWindow created";
    
    // 窗口图标（可选，因为应用程序图标已设置）
    window.setWindowIcon(QIcon(":/logo.ico"));
    window.show();
    qDebug() << "Window shown";
    
    qDebug() << "Entering exec...";
    return app.exec();
}
