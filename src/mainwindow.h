#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QFont>
#include <QPalette>
#include <QColor>
#include <QTimer>
#include <QScrollArea>
#include <QProgressBar>
#include "SystemDataProvider.h"

class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(SystemDataProvider* data, QWidget *parent = nullptr) 
        : QWidget(parent), m_data(data) 
    {
        setupUI();
        connect(m_data, &SystemDataProvider::dataChanged, this, &MainWindow::updateData);
        connect(m_data, &SystemDataProvider::timeChanged, this, &MainWindow::updateTime);
    }

private:
    SystemDataProvider* m_data;
    QLabel* lblUsername;
    QLabel* lblTime;
    QLabel* lblCpu;
    QLabel* lblGpu;
    QLabel* lblDisplay;
    QLabel* lblOs;
    QLabel* lblKernel;
    QLabel* lblShell;
    QLabel* lblUptime;
    QVBoxLayout* diskLayout;

    void setupUI() {
        setWindowTitle("NeoFetch Pro");
        setFixedSize(900, 650);
        setWindowFlags(Qt::FramelessWindowHint);
        
        QPalette pal;
        pal.setColor(QPalette::Window, QColor("#1A1A22"));
        setPalette(pal);

        QHBoxLayout* mainLayout = new QHBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        QWidget* sidebar = createSidebar();
        mainLayout->addWidget(sidebar);

        QWidget* content = createContent();
        mainLayout->addWidget(content, 1);
    }

    QWidget* createSidebar() {
        QWidget* side = new QWidget(this);
        side->setFixedWidth(180);
        side->setStyleSheet("background-color: #1A1A22;");

        QVBoxLayout* layout = new QVBoxLayout(side);
        layout->setContentsMargins(10, 20, 10, 0);
        layout->setSpacing(8);

        QLabel* logo = new QLabel("⎋", side);
        logo->setFont(QFont("Segoe UI Symbol", 42));
        logo->setStyleSheet("color: #6C7086;");
        logo->setAlignment(Qt::AlignCenter);
        layout->addWidget(logo, 0, Qt::AlignHCenter);

        QLabel* title = new QLabel("NeoFetch Pro", side);
        title->setFont(QFont("Consolas", 11));
        title->setStyleSheet("color: #6C7086;");
        title->setAlignment(Qt::AlignCenter);
        layout->addWidget(title, 0, Qt::AlignHCenter);

        QFrame* line = new QFrame(side);
        line->setFrameShape(QFrame::HLine);
        line->setStyleSheet("color: #313244;");
        layout->addWidget(line);

        QString menuItems[] = {"Dashboard", "Hardware", "Software", "Logs"};
        for (int i = 0; i < 4; i++) {
            QLabel* menu = new QLabel(menuItems[i], side);
            menu->setFont(QFont("Consolas", 12));
            menu->setStyleSheet("color: #CDD6F4; padding: 8px;");
            if (i == 0) menu->setStyleSheet(menu->styleSheet() + "border-left: 2px solid #A6E3A1;");
            layout->addWidget(menu);
        }

        layout->addStretch();

        QWidget* rainbowBar = new QWidget(side);
        rainbowBar->setFixedHeight(40);
        rainbowBar->setStyleSheet("background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #F38BA8, stop:0.2 #F9E2AF, stop:0.4 #A6E3A1, stop:0.6 #89DCEB, stop:0.8 #89B4FA, stop:1 #CBA6F7);");
        
        QVBoxLayout* rainbowLayout = new QVBoxLayout(rainbowBar);
        rainbowLayout->setContentsMargins(0, 0, 0, 0);
        
        lblUsername = new QLabel(side);
        lblUsername->setFont(QFont("Consolas", 10));
        lblUsername->setStyleSheet("color: #1A1A22; font-weight: bold;");
        lblUsername->setAlignment(Qt::AlignCenter);
        rainbowLayout->addWidget(lblUsername, 0, Qt::AlignCenter);

        layout->addWidget(rainbowBar);

        updateTime();
        lblUsername->setText(QString("[%1]: ~/%2 @ [%3]")
            .arg(m_data->username())
            .arg(m_data->currentDir())
            .arg(m_data->time()));

        return side;
    }

    QWidget* createContent() {
        QWidget* content = new QWidget(this);
        content->setStyleSheet("background-color: #1A1A22;");

        QVBoxLayout* layout = new QVBoxLayout(content);
        layout->setContentsMargins(0, 0, 0, 0);

        QWidget* titleBar = createTitleBar();
        layout->addWidget(titleBar);

        QScrollArea* scroll = new QScrollArea(content);
        scroll->setWidgetResizable(true);
        scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scroll->setStyleSheet("QScrollBar:vertical { width: 4px; } QScrollBar::handle:vertical { background: #6C7086; border-radius: 2px; }");

        QWidget* scrollContent = new QWidget();
        QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent);
        scrollLayout->setContentsMargins(20, 20, 20, 20);
        scrollLayout->setSpacing(16);

        scrollLayout->addWidget(createHardwareCard());
        scrollLayout->addWidget(createSoftwareCard());
        scrollLayout->addWidget(createUptimeCard());
        scrollLayout->addWidget(createUXCard());

        scroll->setWidget(scrollContent);
        layout->addWidget(scroll);

        return content;
    }

    QWidget* createTitleBar() {
        QWidget* bar = new QWidget(this);
        bar->setFixedHeight(36);
        bar->setStyleSheet("background-color: #1A1A22;");

        QHBoxLayout* layout = new QHBoxLayout(bar);
        layout->setContentsMargins(16, 0, 0, 0);

        QLabel* title = new QLabel("NeoFetch Pro 1.0.0", bar);
        title->setFont(QFont("Consolas", 12));
        title->setStyleSheet("color: #CDD6F4;");
        layout->addWidget(title);

        layout->addStretch();

        QPushButton* minBtn = new QPushButton("─", bar);
        minBtn->setFixedSize(46, 36);
        minBtn->setStyleSheet("QPushButton { background: transparent; color: #CDD6F4; border: none; } QPushButton:hover { background: #313244; }");
        connect(minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
        layout->addWidget(minBtn);

        QPushButton* maxBtn = new QPushButton("□", bar);
        maxBtn->setFixedSize(46, 36);
        maxBtn->setStyleSheet("QPushButton { background: transparent; color: #CDD6F4; border: none; } QPushButton:hover { background: #313244; }");
        connect(maxBtn, &QPushButton::clicked, [this]() {
            if (isMaximized()) showNormal(); else showMaximized();
        });
        layout->addWidget(maxBtn);

        QPushButton* closeBtn = new QPushButton("✕", bar);
        closeBtn->setFixedSize(46, 36);
        closeBtn->setStyleSheet("QPushButton { background: transparent; color: #CDD6F4; border: none; } QPushButton:hover { background: #F38BA8; }");
        connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);
        layout->addWidget(closeBtn);

        return bar;
    }

    QFrame* createCard(QString title, QString color) {
        QFrame* card = new QFrame();
        card->setFrameStyle(QFrame::Box);
        card->setStyleSheet(QString("QFrame { border: 1px solid #313244; border-radius: 8px; background: transparent; }"));

        QVBoxLayout* layout = new QVBoxLayout(card);
        layout->setContentsMargins(16, 16, 16, 16);
        layout->setSpacing(8);

        QLabel* titleLabel = new QLabel(title);
        titleLabel->setFont(QFont("Consolas", 12));
        titleLabel->setStyleSheet(QString("color: %1; font-weight: bold;").arg(color));
        layout->addWidget(titleLabel);

        return card;
    }

    QFrame* createHardwareCard() {
        QFrame* card = createCard("[--- Hardware ---]", "#A6E3A1");
        QVBoxLayout* layout = static_cast<QVBoxLayout*>(card->layout());

        addLabel(layout, "CPU", "#6C7086", 11);
        lblCpu = addLabel(layout, "", "#CDD6F4", 11);
        
        addLabel(layout, "GPU", "#6C7086", 11);
        lblGpu = addLabel(layout, "", "#CDD6F4", 11);

        addLabel(layout, "Display", "#6C7086", 11);
        lblDisplay = addLabel(layout, "", "#CDD6F4", 11);

        addLabel(layout, "Disk", "#6C7086", 11);
        
        diskLayout = new QVBoxLayout();
        diskLayout->setSpacing(4);
        layout->addLayout(diskLayout);

        return card;
    }

    QFrame* createSoftwareCard() {
        QFrame* card = createCard("[--- Software ---]", "#F9E2AF");
        QVBoxLayout* layout = static_cast<QVBoxLayout*>(card->layout());

        addLabel(layout, "OS", "#6C7086", 11);
        lblOs = addLabel(layout, "", "#CDD6F4", 11);

        addLabel(layout, "Kernel", "#6C7086", 11);
        lblKernel = addLabel(layout, "", "#CDD6F4", 11);

        addLabel(layout, "Shell", "#6C7086", 11);
        lblShell = addLabel(layout, "", "#CDD6F4", 11);

        return card;
    }

    QFrame* createUptimeCard() {
        QFrame* card = createCard("[--- Uptime ---]", "#F38BA8");
        QVBoxLayout* layout = static_cast<QVBoxLayout*>(card->layout());

        lblUptime = new QLabel(card);
        lblUptime->setFont(QFont("Consolas", 16));
        lblUptime->setStyleSheet("color: #CDD6F4; font-weight: bold;");
        layout->addWidget(lblUptime);

        return card;
    }

    QFrame* createUXCard() {
        QFrame* card = createCard("[--- UI/UX ---]", "#89B4FA");
        QVBoxLayout* layout = static_cast<QVBoxLayout*>(card->layout());

        addLabel(layout, "DE", "#6C7086", 11);
        addLabel(layout, "Windows Explorer (WinUI 3)", "#CDD6F4", 11);

        addLabel(layout, "WM", "#6C7086", 11);
        addLabel(layout, "DWM (Desktop Window Manager)", "#CDD6F4", 11);

        addLabel(layout, "Font", "#6C7086", 11);
        addLabel(layout, "Consolas / JetBrains Mono", "#CDD6F4", 11);

        addLabel(layout, "App", "#6C7086", 11);
        addLabel(layout, "neofetch-pro 1.0.0", "#CDD6F4", 11);

        return card;
    }

    QLabel* addLabel(QLayout* parent, QString text, QString color, int size) {
        QLabel* label = new QLabel(text);
        label->setFont(QFont("Consolas", size));
        label->setStyleSheet(QString("color: %1;").arg(color));
        parent->addWidget(label);
        return label;
    }

    void updateData() {
        lblCpu->setText(m_data->cpuInfo());
        lblGpu->setText(m_data->gpuInfo());
        lblDisplay->setText(m_data->displayInfo() + " [External]");
        lblOs->setText(m_data->osInfo());
        lblKernel->setText(m_data->kernelInfo());
        lblShell->setText(m_data->shellInfo());
        lblUptime->setText(m_data->uptime());

        while (diskLayout->count()) {
            QLayoutItem* item = diskLayout->takeAt(0);
            delete item->widget();
            delete item;
        }

        for (auto& disk : m_data->getDiskInfo()) {
            QVariantMap d = disk.toMap();
            QString info = QString("%1: %2 GiB / %3 GiB (%4%)")
                .arg(d["drive"].toString())
                .arg(d["used"].toString())
                .arg(d["total"].toString())
                .arg(d["percent"].toInt());
            
            QProgressBar* bar = new QProgressBar();
            bar->setValue(d["percent"].toInt());
            bar->setFixedHeight(6);
            bar->setTextVisible(false);
            bar->setStyleSheet("QProgressBar { border: none; background: #313244; border-radius: 3px; } QProgressBar::chunk { background: #A6E3A1; border-radius: 3px; }");
            diskLayout->addWidget(bar);

            QLabel* label = new QLabel(info);
            label->setFont(QFont("Consolas", 9));
            label->setStyleSheet("color: #6C7086;");
            diskLayout->addWidget(label);
        }
    }

    void updateTime() {
        lblUsername->setText(QString("[%1]: ~/%2 @ [%3]")
            .arg(m_data->username())
            .arg(m_data->currentDir())
            .arg(m_data->time()));
    }
};

#endif
