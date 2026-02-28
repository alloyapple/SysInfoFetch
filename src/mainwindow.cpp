#include "mainwindow.h"
#include <QDebug>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

MainWindow::MainWindow(SystemDataProvider* data, QWidget *parent) 
    : QWidget(parent), m_data(data), m_dragging(false), m_selectedMenu(0),
      lblUsername(nullptr), lblOs(nullptr), lblCpuPercent(nullptr), 
      lblMemoryPercent(nullptr), diskLayout(nullptr),
      lblCpuInfo(nullptr), lblGpuInfo(nullptr), lblSoftwareOs(nullptr),
      lblKernelInfo(nullptr), lblShellInfo(nullptr), lblUptime(nullptr),
      logsEdit(nullptr), menuLabels(), contentStack(nullptr),
      dashboardPanel(nullptr), hardwarePanel(nullptr), softwarePanel(nullptr),
      logsPanel(nullptr)
{
    setupUI();
    connect(m_data, &SystemDataProvider::dataChanged, this, &MainWindow::updateData);
    qDebug() << "MainWindow created";
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->pos().y() < 36) {
        m_dragging = true;
        m_dragPos = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (m_dragging) {
        move(event->globalPos() - m_dragPos);
        event->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    m_dragging = false;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease) {
        QLabel* label = qobject_cast<QLabel*>(obj);
        if (label && label->property("menuIndex").isValid()) {
            int index = label->property("menuIndex").toInt();
            if (event->type() == QEvent::MouseButtonRelease) {
                selectMenu(index);
            }
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void MainWindow::selectMenu(int index) {
    if (index == m_selectedMenu) return;
    m_selectedMenu = index;
    for (int i = 0; i < menuLabels.size(); i++) {
        if (i == index) {
            menuLabels[i]->setStyleSheet("color: #CDD6F4; padding: 8px; border-left: 2px solid #A6E3A1;");
        } else {
            menuLabels[i]->setStyleSheet("color: #6C7086; padding: 8px;");
        }
    }
    if (contentStack) {
        contentStack->setCurrentIndex(index);
    }
}

void MainWindow::setupUI() {
    setWindowTitle("NeoFetch Pro");
    setFixedSize(1000, 700);
    setWindowFlags(Qt::FramelessWindowHint);
    
    QPalette pal;
    pal.setColor(QPalette::Window, QColor("#0F0F14"));
    setPalette(pal);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QWidget* sidebar = createSidebar();
    mainLayout->addWidget(sidebar);

    QWidget* content = createContent();
    mainLayout->addWidget(content, 1);
}

QWidget* MainWindow::createSidebar() {
    QWidget* side = new QWidget(this);
    side->setFixedWidth(220);
    side->setStyleSheet("background-color: #0F0F14;");

    QVBoxLayout* layout = new QVBoxLayout(side);
    layout->setContentsMargins(10, 20, 10, 0);
    layout->setSpacing(8);

    QLabel* logo = new QLabel(side);
    QFileIconProvider iconProvider;
    QIcon icon = iconProvider.icon(QFileIconProvider::Computer);
    QPixmap pixmap = icon.pixmap(36, 36);
    if (!pixmap.isNull()) {
        logo->setPixmap(pixmap.scaled(36, 36, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        logo->setText("⚡");
        logo->setFont(QFont("Consolas", 20));
        logo->setStyleSheet("color: #6C7086;");
    }
    logo->setAlignment(Qt::AlignCenter);
    layout->addWidget(logo, 0, Qt::AlignHCenter);

    QLabel* title = new QLabel("NeoFetch Pro", side);
    title->setFont(QFont("Consolas", 10));
    title->setStyleSheet("color: #6C7086;");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title, 0, Qt::AlignHCenter);

    QFrame* line = new QFrame(side);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #1E1E28;");
    layout->addWidget(line);

    QString menuItems[] = {"Dashboard", "Hardware", "Software", "Logs"};
    menuLabels.clear();
    for (int i = 0; i < 4; i++) {
        QLabel* menu = new QLabel(menuItems[i], side);
        menu->setFont(QFont("Consolas", 11));
        menu->setStyleSheet("color: #CDD6F4; padding: 8px;");
        menu->setCursor(Qt::PointingHandCursor);
        menu->installEventFilter(this);
        menu->setProperty("menuIndex", i);
        menuLabels.append(menu);
        if (i == 0) menu->setStyleSheet(menu->styleSheet() + "border-left: 2px solid #A6E3A1;");
        layout->addWidget(menu);
    }

    layout->addStretch();

    lblUsername = new QLabel(side);
    lblUsername->setFont(QFont("Consolas", 10));
    lblUsername->setStyleSheet("color: #A6E3A1; font-weight: bold;");
    lblUsername->setAlignment(Qt::AlignCenter);
    layout->addWidget(lblUsername, 0, Qt::AlignHCenter);

    return side;
}

QWidget* MainWindow::createContent() {
    QWidget* content = new QWidget(this);
    content->setStyleSheet("background-color: #0F0F14;");

    QVBoxLayout* layout = new QVBoxLayout(content);
    layout->setContentsMargins(0, 0, 0, 0);

    QWidget* titleBar = createTitleBar();
    layout->addWidget(titleBar);

    contentStack = new QStackedWidget(content);
    contentStack->setStyleSheet("background-color: transparent;");

    dashboardPanel = createDashboardPanel();
    hardwarePanel = createHardwarePanel();
    softwarePanel = createSoftwarePanel();
    logsPanel = createLogsPanel();

    contentStack->addWidget(dashboardPanel);
    contentStack->addWidget(hardwarePanel);
    contentStack->addWidget(softwarePanel);
    contentStack->addWidget(logsPanel);

    layout->addWidget(contentStack);

    return content;
}

QWidget* MainWindow::createDashboardPanel() {
    QWidget* container = new QWidget();
    container->setStyleSheet("background-color: transparent;");

    QHBoxLayout* hLayout = new QHBoxLayout(container);
    hLayout->setContentsMargins(20, 20, 20, 20);
    hLayout->setSpacing(0);

    QFrame* card = new QFrame(container);
    card->setFrameStyle(QFrame::Box);
    card->setStyleSheet("QFrame { background-color: #18181F; border: 1px solid #1E1E28; border-radius: 12px; }");

    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(32, 24, 32, 24);
    cardLayout->setSpacing(12);

    QLabel* logo = new QLabel(card);
    QFileIconProvider iconProvider;
    QIcon icon = iconProvider.icon(QFileIconProvider::Computer);
    QPixmap pixmap = icon.pixmap(28, 28);
    if (!pixmap.isNull()) {
        logo->setPixmap(pixmap.scaled(28, 28, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        logo->setText("⚡");
        logo->setFont(QFont("Consolas", 20));
        logo->setStyleSheet("color: #CDD6F4;");
    }
    logo->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(logo);

    QLabel* appName = new QLabel("NeoFetch Pro", card);
    appName->setFont(QFont("Consolas", 10));
    appName->setStyleSheet("color: #6C7086; margin-top: 4px;");
    appName->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(appName);

    cardLayout->addSpacing(8);

    QFrame* line1 = new QFrame(card);
    line1->setFrameShape(QFrame::HLine);
    line1->setStyleSheet("color: #1E1E28;");
    cardLayout->addWidget(line1);

    cardLayout->addSpacing(8);

    lblUsername = new QLabel(card);
    lblUsername->setFont(QFont("Consolas", 11));
    lblUsername->setStyleSheet("color: #A6E3A1; font-weight: bold;");
    cardLayout->addWidget(lblUsername);

    cardLayout->addSpacing(4);

    QFrame* line2 = new QFrame(card);
    line2->setFrameShape(QFrame::HLine);
    line2->setStyleSheet("color: #1E1E28;");
    cardLayout->addWidget(line2);

    cardLayout->addSpacing(8);

    lblOs = new QLabel(card);
    lblOs->setFont(QFont("Consolas", 11));
    lblOs->setStyleSheet("color: #CDD6F4;");
    cardLayout->addWidget(lblOs);

    cardLayout->addSpacing(8);

    QFrame* line3 = new QFrame(card);
    line3->setFrameShape(QFrame::HLine);
    line3->setStyleSheet("color: #1E1E28;");
    cardLayout->addWidget(line3);

    cardLayout->addSpacing(8);

    lblCpuPercent = new QLabel(card);
    lblCpuPercent->setFont(QFont("Consolas", 11));
    lblCpuPercent->setStyleSheet("color: #89B4FA;");
    cardLayout->addWidget(lblCpuPercent);

    cardLayout->addSpacing(4);

    lblMemoryPercent = new QLabel(card);
    lblMemoryPercent->setFont(QFont("Consolas", 11));
    lblMemoryPercent->setStyleSheet("color: #F5C2E7;");
    cardLayout->addWidget(lblMemoryPercent);

    cardLayout->addSpacing(8);

    QFrame* line4 = new QFrame(card);
    line4->setFrameShape(QFrame::HLine);
    line4->setStyleSheet("color: #1E1E28;");
    cardLayout->addWidget(line4);

    cardLayout->addSpacing(8);

    diskLayout = new QVBoxLayout();
    diskLayout->setSpacing(2);
    cardLayout->addLayout(diskLayout);

    cardLayout->addSpacing(8);

    lblUptime = new QLabel(card);
    lblUptime->setFont(QFont("Consolas", 10));
    lblUptime->setStyleSheet("color: #6C7086;");
    cardLayout->addWidget(lblUptime);

    hLayout->addWidget(card, 1, Qt::AlignCenter);

    return container;
}

QWidget* MainWindow::createHardwarePanel() {
    QWidget* container = new QWidget();
    container->setStyleSheet("background-color: transparent;");

    QHBoxLayout* hLayout = new QHBoxLayout(container);
    hLayout->setContentsMargins(20, 20, 20, 20);
    hLayout->setSpacing(0);

    QFrame* card = new QFrame(container);
    card->setFrameStyle(QFrame::Box);
    card->setStyleSheet("QFrame { background-color: #18181F; border: 1px solid #1E1E28; border-radius: 12px; }");

    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(32, 24, 32, 24);
    cardLayout->setSpacing(0);

    QLabel* logo = new QLabel(card);
    logo->setText("🖥");
    logo->setFont(QFont("Consolas", 20));
    logo->setStyleSheet("color: #CDD6F4;");
    logo->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(logo);
    cardLayout->setSpacing(0);

    QLabel* panelTitle = new QLabel("Hardware", card);
    panelTitle->setFont(QFont("Consolas", 10));
    panelTitle->setStyleSheet("color: #6C7086; margin-top: 4px;");
    panelTitle->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(panelTitle);

    cardLayout->addSpacing(20);

    QVBoxLayout* infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(10);

    lblCpuInfo = new QLabel(card);
    lblCpuInfo->setFont(QFont("Consolas", 11));
    lblCpuInfo->setStyleSheet("color: #CDD6F4;");
    infoLayout->addWidget(lblCpuInfo);

    lblGpuInfo = new QLabel(card);
    lblGpuInfo->setFont(QFont("Consolas", 11));
    lblGpuInfo->setStyleSheet("color: #CDD6F4;");
    infoLayout->addWidget(lblGpuInfo);

    cardLayout->addLayout(infoLayout);

    hLayout->addWidget(card, 1, Qt::AlignCenter);

    return container;
}

QWidget* MainWindow::createSoftwarePanel() {
    QWidget* container = new QWidget();
    container->setStyleSheet("background-color: transparent;");

    QHBoxLayout* hLayout = new QHBoxLayout(container);
    hLayout->setContentsMargins(20, 20, 20, 20);
    hLayout->setSpacing(0);

    QFrame* card = new QFrame(container);
    card->setFrameStyle(QFrame::Box);
    card->setStyleSheet("QFrame { background-color: #18181F; border: 1px solid #1E1E28; border-radius: 12px; }");

    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(32, 24, 32, 24);
    cardLayout->setSpacing(0);

    QLabel* logo = new QLabel(card);
    logo->setText("📦");
    logo->setFont(QFont("Consolas", 20));
    logo->setStyleSheet("color: #CDD6F4;");
    logo->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(logo);
    cardLayout->setSpacing(0);

    QLabel* panelTitle = new QLabel("Software", card);
    panelTitle->setFont(QFont("Consolas", 10));
    panelTitle->setStyleSheet("color: #6C7086; margin-top: 4px;");
    panelTitle->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(panelTitle);

    cardLayout->addSpacing(20);

    QVBoxLayout* infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(10);

    lblSoftwareOs = new QLabel(card);
    lblSoftwareOs->setFont(QFont("Consolas", 11));
    lblSoftwareOs->setStyleSheet("color: #CDD6F4;");
    infoLayout->addWidget(lblSoftwareOs);

    lblKernelInfo = new QLabel(card);
    lblKernelInfo->setFont(QFont("Consolas", 11));
    lblKernelInfo->setStyleSheet("color: #CDD6F4;");
    infoLayout->addWidget(lblKernelInfo);

    lblShellInfo = new QLabel(card);
    lblShellInfo->setFont(QFont("Consolas", 11));
    lblShellInfo->setStyleSheet("color: #CDD6F4;");
    infoLayout->addWidget(lblShellInfo);

    cardLayout->addLayout(infoLayout);

    hLayout->addWidget(card, 1, Qt::AlignCenter);

    return container;
}

QWidget* MainWindow::createLogsPanel() {
    QWidget* container = new QWidget();
    container->setStyleSheet("background-color: transparent;");

    QHBoxLayout* hLayout = new QHBoxLayout(container);
    hLayout->setContentsMargins(20, 20, 20, 20);
    hLayout->setSpacing(0);

    QFrame* card = new QFrame(container);
    card->setFrameStyle(QFrame::Box);
    card->setStyleSheet("QFrame { background-color: #18181F; border: 1px solid #1E1E28; border-radius: 12px; }");

    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(32, 24, 32, 24);
    cardLayout->setSpacing(0);

    QLabel* logo = new QLabel(card);
    logo->setText("📋");
    logo->setFont(QFont("Consolas", 20));
    logo->setStyleSheet("color: #CDD6F4;");
    logo->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(logo);
    cardLayout->setSpacing(0);

    QLabel* panelTitle = new QLabel("Logs", card);
    panelTitle->setFont(QFont("Consolas", 10));
    panelTitle->setStyleSheet("color: #6C7086; margin-top: 4px;");
    panelTitle->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(panelTitle);

    cardLayout->addSpacing(20);

    QTextEdit* edit = new QTextEdit(card);
    edit->setReadOnly(true);
    edit->setStyleSheet("color: #CDD6F4; background-color: transparent; border: none; font-family: Consolas; font-size: 10px;");
    edit->setFont(QFont("Consolas", 10));
    cardLayout->addWidget(edit);

    hLayout->addWidget(card, 1, Qt::AlignCenter);

    logsEdit = edit;
    return container;
}

QWidget* MainWindow::createTitleBar() {
    QWidget* bar = new QWidget(this);
    bar->setFixedHeight(36);
    bar->setStyleSheet("background-color: #0F0F14;");

    QHBoxLayout* layout = new QHBoxLayout(bar);
    layout->setContentsMargins(16, 0, 0, 0);

    QLabel* title = new QLabel("NeoFetch Pro 1.0.0", bar);
    title->setFont(QFont("Consolas", 11));
    title->setStyleSheet("color: #6C7086;");
    layout->addWidget(title);

    layout->addStretch();

    QPushButton* minBtn = new QPushButton("─", bar);
    minBtn->setFixedSize(46, 36);
    minBtn->setStyleSheet("QPushButton { background: transparent; color: #6C7086; border: none; } QPushButton:hover { background: #1E1E28; }");
    connect(minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
    layout->addWidget(minBtn);

    QPushButton* maxBtn = new QPushButton("□", bar);
    maxBtn->setFixedSize(46, 36);
    maxBtn->setStyleSheet("QPushButton { background: transparent; color: #6C7086; border: none; } QPushButton:hover { background: #1E1E28; }");
    connect(maxBtn, &QPushButton::clicked, [this]() {
        if (isMaximized()) showNormal(); else showMaximized();
    });
    layout->addWidget(maxBtn);

    QPushButton* closeBtn = new QPushButton("✕", bar);
    closeBtn->setFixedSize(46, 36);
    closeBtn->setStyleSheet("QPushButton { background: transparent; color: #6C7086; border: none; } QPushButton:hover { background: #F38BA8; color: white; }");
    connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);
    layout->addWidget(closeBtn);

    return bar;
}

void MainWindow::updateData() {
    // 调试日志
    QString logPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/neofetch_ui_debug.txt";
    QFile logFile(logPath);
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);
        out << "updateData called, disk count: " << m_data->getDiskInfo().size() << endl;
        logFile.close();
    }
    
    // 获取磁盘数据（一次获取，多处使用）
    QList<QVariant> disks = m_data->getDiskInfo();
    
    lblUsername->setText(QString("%1@%2")
        .arg(m_data->username())
        .arg(m_data->currentDir()));
    
    lblOs->setText(m_data->osInfo());
    lblCpuPercent->setText(QString("CPU: %1%").arg(m_data->cpuPercent()));
    lblMemoryPercent->setText(QString("Memory: %1%").arg(m_data->memoryPercent()));

    lblCpuInfo->setText(QString("CPU: %1").arg(m_data->cpuInfo()));
    lblGpuInfo->setText(QString("GPU: %1").arg(m_data->gpuInfo()));

    lblSoftwareOs->setText(m_data->osInfo());
    lblKernelInfo->setText(QString("Kernel: %1").arg(m_data->kernelInfo()));
    lblShellInfo->setText(QString("Shell: %1").arg(m_data->shellInfo()));

    // 更新磁盘信息 - 显示所有磁盘
    if (diskLayout) {
        // 清空现有标签
        while (diskLayout->count()) {
            QLayoutItem* item = diskLayout->takeAt(0);
            if (item->widget()) delete item->widget();
            delete item;
        }
        
        for (const auto& disk : disks) {
            QVariantMap d = disk.toMap();
            QString drive = d["drive"].toString().trimmed();
            QString fsType = d["fstype"].toString();
            QString used = d["used"].toString();
            QString total = d["total"].toString();
            int percent = d["percent"].toInt();
            
            QString diskText = QString("%1 %2 %3 GiB / %4 GiB (%5%)")
                .arg(drive).arg(fsType).arg(used).arg(total).arg(percent);
            
            QLabel* lbl = new QLabel();
            lbl->setFont(QFont("Consolas", 10));
            lbl->setStyleSheet("color: #F9E2AF;");
            lbl->setText(diskText);
            diskLayout->addWidget(lbl);
        }
    }

    lblUptime->setText(QString("Uptime: %1").arg(m_data->uptime()));

    if (logsEdit) {
        QString logs = "=== System Information ===\n\n";
        logs += QString("CPU: %1\n").arg(m_data->cpuInfo());
        logs += QString("GPU: %1\n").arg(m_data->gpuInfo());
        logs += QString("OS: %1\n").arg(m_data->osInfo());
        logs += QString("Kernel: %1\n").arg(m_data->kernelInfo());
        logs += QString("Shell: %1\n").arg(m_data->shellInfo());
        logs += QString("Uptime: %1\n\n").arg(m_data->uptime());
        
        logs += "=== Disk Usage ===\n\n";
        for (auto& disk : disks) {
            QVariantMap d = disk.toMap();
            logs += QString("%1 (%2): %3 GiB / %4 GiB (%5%)\n")
                .arg(d["drive"].toString())
                .arg(d["fstype"].toString())
                .arg(d["used"].toString())
                .arg(d["total"].toString())
                .arg(d["percent"].toInt());
        }
        
        logsEdit->setText(logs);
    }
}
