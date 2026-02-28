#include "SystemDataProvider.h"
#include <Windows.h>
#include <QTimer>
#include <QTime>
#include <QSettings>

SystemDataProvider::SystemDataProvider(QObject *parent)
    : QObject(parent)
{
    m_cpuInfo = "Loading...";
    m_gpuInfo = "Loading...";
    m_displayInfo = "Loading...";
    m_osInfo = "Loading...";
    m_kernelInfo = "Loading...";
    m_shellInfo = "Loading...";
    m_uptime = "Loading...";
    m_username = "user";
    m_currentDir = "~";
    m_time = "00:00";

    QTimer::singleShot(500, this, &SystemDataProvider::fetchAllData);
}

void SystemDataProvider::fetchAllData()
{
    fetchCpuInfo();
    fetchGpuInfo();
    fetchDisplayInfo();
    fetchOsInfo();
    fetchShellInfo();
    fetchUptime();
    fetchUserInfo();
    fetchDiskInfo();
    updateTime();

    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &SystemDataProvider::updateSystemData);
    m_updateTimer->start(5000);

    m_timeTimer = new QTimer(this);
    connect(m_timeTimer, &QTimer::timeout, this, &SystemDataProvider::updateTime);
    m_timeTimer->start(1000);
}

void SystemDataProvider::updateSystemData()
{
    fetchDiskInfo();
    emit dataChanged();
}

void SystemDataProvider::updateTime()
{
    m_time = QTime::currentTime().toString("HH:mm");
    emit timeChanged();
}

void SystemDataProvider::fetchCpuInfo()
{
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        wchar_t cpuName[256];
        DWORD size = sizeof(cpuName);
        DWORD type;
        if (RegQueryValueExW(hKey, L"ProcessorNameString", nullptr, &type, (LPBYTE)cpuName, &size) == ERROR_SUCCESS) {
            m_cpuInfo = QString::fromUtf16((const ushort*)cpuName);
        }
        RegCloseKey(hKey);
    } else {
        m_cpuInfo = "Unknown CPU";
    }
    emit dataChanged();
}

void SystemDataProvider::fetchGpuInfo()
{
    m_gpuInfo = "Generic GPU";
    emit dataChanged();
}

void SystemDataProvider::fetchDisplayInfo()
{
    DEVMODEW dm = {};
    dm.dmSize = sizeof(dm);
    if (EnumDisplaySettingsW(nullptr, ENUM_CURRENT_SETTINGS, &dm)) {
        m_displayInfo = QString("%1x%2 @ %3 Hz").arg(dm.dmPelsWidth).arg(dm.dmPelsHeight).arg(dm.dmDisplayFrequency);
    } else {
        m_displayInfo = "Unknown";
    }
    emit dataChanged();
}

void SystemDataProvider::fetchOsInfo()
{
    QSettings reg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", QSettings::NativeFormat);
    m_osInfo = reg.value("ProductName", "Windows").toString();
    emit dataChanged();
}

void SystemDataProvider::fetchKernelInfo()
{
    m_kernelInfo = "NT 10.0";
    emit dataChanged();
}

void SystemDataProvider::fetchShellInfo()
{
    m_shellInfo = "PowerShell";
    emit dataChanged();
}

void SystemDataProvider::fetchUptime()
{
    ULONGLONG ticks = GetTickCount64();
    qint64 days = (ticks / 60000) / 1440;
    qint64 mins = (ticks / 60000) % 1440;
    m_uptime = QString("Clock %1 days, %2 mins").arg(days).arg(mins);
    emit dataChanged();
}

void SystemDataProvider::fetchUserInfo()
{
    wchar_t name[256];
    DWORD size = sizeof(name);
    if (GetUserNameW(name, &size)) {
        m_username = QString::fromUtf16((const ushort*)name);
    }
    m_currentDir = "~";
    emit dataChanged();
}

void SystemDataProvider::fetchDiskInfo()
{
    m_diskInfo.clear();
    DWORD drives = GetLogicalDrives();
    for (int i = 0; i < 26; i++) {
        if (drives & (1 << i)) {
            QString drive = QString("%1:").arg('A' + i);
            ULARGE_INTEGER freeBytes, totalBytes;
            if (GetDiskFreeSpaceExW(QString(drive + "\\").toStdWString().c_str(), &freeBytes, &totalBytes, nullptr)) {
                double totalGB = totalBytes.QuadPart / (1024.0 * 1024.0 * 1024.0);
                double freeGB = freeBytes.QuadPart / (1024.0 * 1024.0 * 1024.0);
                int percent = totalGB > 0 ? (int)((totalGB - freeGB) / totalGB * 100) : 0;
                QVariantMap d;
                d["drive"] = drive;
                d["total"] = QString::number(totalGB, 'f', 1);
                d["used"] = QString::number(totalGB - freeGB, 'f', 1);
                d["percent"] = percent;
                m_diskInfo.append(d);
            }
        }
    }
}
