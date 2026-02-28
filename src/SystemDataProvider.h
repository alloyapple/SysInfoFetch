#ifndef SYSTEMDATAPROVIDER_H
#define SYSTEMDATAPROVIDER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QTimer>

class SystemDataProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString cpuInfo READ cpuInfo NOTIFY dataChanged)
    Q_PROPERTY(QString gpuInfo READ gpuInfo NOTIFY dataChanged)
    Q_PROPERTY(QString displayInfo READ displayInfo NOTIFY dataChanged)
    Q_PROPERTY(QString memoryInfo READ memoryInfo NOTIFY dataChanged)
    Q_PROPERTY(QString networkInfo READ networkInfo NOTIFY dataChanged)
    Q_PROPERTY(QString diskHardwareInfo READ diskHardwareInfo NOTIFY dataChanged)
    Q_PROPERTY(QString osInfo READ osInfo NOTIFY dataChanged)
    Q_PROPERTY(QString kernelInfo READ kernelInfo NOTIFY dataChanged)
    Q_PROPERTY(QString shellInfo READ shellInfo NOTIFY dataChanged)
    Q_PROPERTY(QString uptime READ uptime NOTIFY dataChanged)
    Q_PROPERTY(QString username READ username NOTIFY dataChanged)
    Q_PROPERTY(QString currentDir READ currentDir NOTIFY dataChanged)
    Q_PROPERTY(int cpuPercent READ cpuPercent NOTIFY dataChanged)
    Q_PROPERTY(int memoryPercent READ memoryPercent NOTIFY dataChanged)
    Q_PROPERTY(qulonglong memoryTotal READ memoryTotal NOTIFY dataChanged)
    Q_PROPERTY(qulonglong memoryUsed READ memoryUsed NOTIFY dataChanged)
    Q_PROPERTY(QString time READ time NOTIFY timeChanged)

public:
    explicit SystemDataProvider(QObject *parent = nullptr);

    QString cpuInfo() const { return m_cpuInfo; }
    QString gpuInfo() const { return m_gpuInfo; }
    QString displayInfo() const { return m_displayInfo; }
    QString osInfo() const { return m_osInfo; }
    QString kernelInfo() const { return m_kernelInfo; }
    QString shellInfo() const { return m_shellInfo; }
    QString uptime() const { return m_uptime; }
    QString username() const { return m_username; }
    QString currentDir() const { return m_currentDir; }
    int cpuPercent() const { return m_cpuPercent; }
    int memoryPercent() const { return m_memoryPercent; }
    QString time() const { return m_time; }
    qulonglong memoryTotal() const { return m_memoryTotal; }
    qulonglong memoryUsed() const { return m_memoryUsed; }
    QString memoryInfo() const { return m_memoryInfo; }
    QString diskHardwareInfo() const { return m_diskHardwareInfo; }
    QString networkInfo() const { return m_networkInfo; }

    Q_INVOKABLE QVariantList getDiskInfo() const { return m_diskInfo; }

signals:
    void dataChanged();
    void timeChanged();

private slots:
    void fetchAllData();
    void updateSystemData();
    void updateTime();

private:
    void fetchCpuInfo();
    void fetchGpuInfo();
    void fetchDisplayInfo();
    void fetchOsInfo();
    void fetchKernelInfo();
    void fetchShellInfo();
    void fetchUptime();
    void fetchUserInfo();
    void fetchDiskInfo();
    void fetchMemoryInfo();
    void fetchDiskHardwareInfo();
    void fetchNetworkInfo();
    void fetchCpuUsage();
    void fetchMemoryUsage();

    QString m_cpuInfo;
    QString m_gpuInfo;
    QString m_displayInfo;
    QString m_osInfo;
    QString m_kernelInfo;
    QString m_shellInfo;
    QString m_uptime;
    QString m_username;
    QString m_currentDir;
    int m_cpuPercent;
    int m_memoryPercent;
    QString m_time;
    QVariantList m_diskInfo;
    qulonglong m_memoryTotal;
    qulonglong m_memoryUsed;
    QString m_memoryInfo;
    QString m_diskHardwareInfo;
    QString m_networkInfo;

    QTimer *m_updateTimer;
    QTimer *m_timeTimer;
};

#endif
