#include "SystemDataProvider.h"
#include <Windows.h>
#include <shlobj.h>
#include <QTimer>
#include <QTime>
#include <QSettings>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

SystemDataProvider::SystemDataProvider(QObject *parent)
    : QObject(parent), m_cpuPercent(0), m_memoryPercent(0)
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

    emit dataChanged();

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
    fetchCpuUsage();
    fetchMemoryUsage();
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
    
    wchar_t path[MAX_PATH];
    if (SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, path) == S_OK) {
        m_currentDir = QString::fromUtf16((const ushort*)path);
    } else {
        m_currentDir = "";
    }
    emit dataChanged();
}

void SystemDataProvider::fetchDiskInfo()
{
    m_diskInfo.clear();
    DWORD drives = GetLogicalDrives();
    
    for (int i = 0; i < 26; i++) {
        if (drives & (1 << i)) {
            QString drive = QString("%1:").arg(QChar('A' + i));
            wchar_t path[4];
            path[0] = L'A' + i;
            path[1] = L':';
            path[2] = L'\\';
            path[3] = L'\0';
            
            DWORD sectorsPerCluster, bytesPerSector, freeClusters, totalClusters;
            if (GetDiskFreeSpaceW(path, &sectorsPerCluster, &bytesPerSector, &freeClusters, &totalClusters)) {
                ULONGLONG totalBytes = (ULONGLONG)totalClusters * sectorsPerCluster * bytesPerSector;
                ULONGLONG freeBytes = (ULONGLONG)freeClusters * sectorsPerCluster * bytesPerSector;
                
                double totalGB = totalBytes / (1024.0 * 1024.0 * 1024.0);
                double freeGB = freeBytes / (1024.0 * 1024.0 * 1024.0);
                int percent = totalGB > 0 ? (int)((totalGB - freeGB) / totalGB * 100) : 0;
                
                QString fsType = "NTFS";
                wchar_t fsName[256];
                if (GetVolumeInformationW(path, nullptr, 0, nullptr, nullptr, nullptr, fsName, 256)) {
                    fsType = QString::fromUtf16((const ushort*)fsName);
                }
                
                QVariantMap d;
                d["drive"] = drive;
                d["total"] = QString::number(totalGB, 'f', 1);
                d["used"] = QString::number(totalGB - freeGB, 'f', 1);
                d["percent"] = percent;
                d["fstype"] = fsType;
                m_diskInfo.append(d);
            }
        }
    }
    emit dataChanged();
}

void SystemDataProvider::fetchCpuUsage()
{
    static FILETIME idleTime, kernelTime, userTime;
    static bool first = true;
    FILETIME idle, kernel, user;
    
    if (GetSystemTimes(&idle, &kernel, &user)) {
        if (!first) {
            ULONGLONG idle1 = ((ULONGLONG)idleTime.dwHighDateTime << 32) | idleTime.dwLowDateTime;
            ULONGLONG idle2 = ((ULONGLONG)idle.dwHighDateTime << 32) | idle.dwLowDateTime;
            ULONGLONG kernel1 = ((ULONGLONG)kernelTime.dwHighDateTime << 32) | kernelTime.dwLowDateTime;
            ULONGLONG kernel2 = ((ULONGLONG)kernel.dwHighDateTime << 32) | kernel.dwLowDateTime;
            ULONGLONG user1 = ((ULONGLONG)userTime.dwHighDateTime << 32) | userTime.dwLowDateTime;
            ULONGLONG user2 = ((ULONGLONG)user.dwHighDateTime << 32) | user.dwLowDateTime;
            
            ULONGLONG idleDiff = idle2 - idle1;
            ULONGLONG kernelDiff = kernel2 - kernel1;
            ULONGLONG userDiff = user2 - user1;
            ULONGLONG total = kernelDiff + userDiff;
            
            if (total > 0) {
                m_cpuPercent = (int)(100 - (idleDiff * 100 / total));
            }
        }
        first = false;
        idleTime = idle;
        kernelTime = kernel;
        userTime = user;
    }
}

void SystemDataProvider::fetchMemoryUsage()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memInfo)) {
        ULONGLONG totalMem = memInfo.ullTotalPhys;
        ULONGLONG availMem = memInfo.ullAvailPhys;
        ULONGLONG usedMem = totalMem - availMem;
        m_memoryPercent = (int)((usedMem * 100) / totalMem);
    }
}
