#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QFont>
#include <QMouseEvent>
#include <QPixmap>
#include <QFileIconProvider>
#include <QStackedWidget>
#include <QTextEdit>
#include "SystemDataProvider.h"

class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(SystemDataProvider* data, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void selectMenu(int index);
    void setupUI();
    QWidget* createSidebar();
    QWidget* createContent();
    QWidget* createDashboardPanel();
    QWidget* createHardwarePanel();
    QWidget* createSoftwarePanel();
    QWidget* createLogsPanel();
    QWidget* createTitleBar();
    void updateData();

    SystemDataProvider* m_data;
    bool m_dragging;
    QPoint m_dragPos;
    int m_selectedMenu;
    QLabel* lblUsername;
    QLabel* lblOs;
    QLabel* lblCpuPercent;
    QLabel* lblMemoryPercent;
    QVBoxLayout* diskLayout;
    QLabel* lblCpuInfo;
    QLabel* lblGpuInfo;
    QLabel* lblSoftwareOs;
    QLabel* lblKernelInfo;
    QLabel* lblShellInfo;
    QLabel* lblUptime;
    QList<QLabel*> menuLabels;
    QStackedWidget* contentStack;
    QWidget* dashboardPanel;
    QWidget* hardwarePanel;
    QWidget* softwarePanel;
    QWidget* logsPanel;
    QTextEdit* logsEdit;
};

#endif
