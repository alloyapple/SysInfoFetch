#include "SystemDataProvider.h"
#include <Windows.h>
#include <shlobj.h>
#include <iphlpapi.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <QTimer>
#include <QTime>
#include <QSettings>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

SystemDataProvider::SystemDataProvider(QObject *parent)
    : QObject(parent), m_cpuPercent(0), m_memoryPercent(0), m_memoryTotal(0), m_memoryUsed(0)
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
    m_memoryInfo = "Loading...";
    m_diskHardwareInfo = "Loading...";
    m_networkInfo = "Loading...";

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
    fetchMemoryUsage();
    fetchMemoryInfo();
    fetchDiskHardwareInfo();
    fetchNetworkInfo();
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
    fetchMemoryInfo();
    fetchDiskHardwareInfo();
    fetchNetworkInfo();
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
        m_memoryTotal = memInfo.ullTotalPhys;
        ULONGLONG availMem = memInfo.ullAvailPhys;
        m_memoryUsed = m_memoryTotal - availMem;
        m_memoryPercent = (int)((m_memoryUsed * 100) / m_memoryTotal);
    }
}

void SystemDataProvider::fetchMemoryInfo()
{
    if (m_memoryTotal > 0) {
        qulonglong totalGB = m_memoryTotal / (1024 * 1024 * 1024);
        qulonglong usedGB = m_memoryUsed / (1024 * 1024 * 1024);
        m_memoryInfo = QString("%1 GiB RAM").arg(totalGB);
    } else {
        m_memoryInfo = "Unknown";
    }
    emit dataChanged();
}

void SystemDataProvider::fetchDiskHardwareInfo()
{
    m_diskHardwareInfo = "";
    
    // 初始化 COM
    HRESULT hres = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hres) && hres != RPC_E_CHANGED_MODE) {
        m_diskHardwareInfo = "WMI init failed";
        emit dataChanged();
        return;
    }
    
    // 设置安全级别
    hres = CoInitializeSecurity(
        nullptr, -1, nullptr, nullptr,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr, EOAC_NONE, nullptr
    );
    
    IWbemLocator* pLoc = nullptr;
    hres = CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER,
                            IID_IWbemLocator, (LPVOID*)&pLoc);
    
    if (SUCCEEDED(hres)) {
        IWbemServices* pSvc = nullptr;
        BSTR wmiNamespace = SysAllocString(L"ROOT\\CIMV2");
        hres = pLoc->ConnectServer(
            wmiNamespace,
            nullptr, nullptr, nullptr, 0, nullptr, nullptr, &pSvc
        );
        SysFreeString(wmiNamespace);
        
        if (SUCCEEDED(hres)) {
            // 设置安全
            hres = CoSetProxyBlanket(
                pSvc,
                RPC_C_AUTHN_WINNT,
                RPC_C_AUTHZ_NONE,
                nullptr,
                RPC_C_AUTHN_LEVEL_CALL,
                RPC_C_IMP_LEVEL_IMPERSONATE,
                nullptr,
                EOAC_NONE
            );
            
            if (SUCCEEDED(hres)) {
                IEnumWbemClassObject* pEnumerator = nullptr;
                BSTR query = SysAllocString(L"SELECT * FROM Win32_DiskDrive");
                BSTR wql = SysAllocString(L"WQL");
                hres = pSvc->ExecQuery(
                    wql,
                    query,
                    WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                    nullptr,
                    &pEnumerator
                );
                SysFreeString(query);
                SysFreeString(wql);
                
                if (SUCCEEDED(hres)) {
                    IWbemClassObject* pclsObj = nullptr;
                    ULONG uReturn = 0;
                    QStringList disks;
                    
                    while (pEnumerator) {
                        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
                        if (uReturn == 0) break;
                        
                        VARIANT vtProp;
                        
                        // 获取型号
                        hr = pclsObj->Get(L"Model", 0, &vtProp, nullptr, nullptr);
                        QString model = (vtProp.vt == VT_BSTR) ? QString::fromWCharArray(vtProp.bstrVal) : "Unknown";
                        VariantClear(&vtProp);
                        
                        // 获取大小
                        hr = pclsObj->Get(L"Size", 0, &vtProp, nullptr, nullptr);
                        QString size = "Unknown";
                        if (vtProp.vt == VT_BSTR) {
                            bool ok;
                            qint64 bytes = QString::fromWCharArray(vtProp.bstrVal).toLongLong(&ok);
                            if (ok && bytes > 0) {
                                double gb = bytes / (1024.0 * 1024.0 * 1024.0);
                                size = QString("%1 GB").arg(gb, 0, 'f', 1);
                            }
                        }
                        VariantClear(&vtProp);
                        
                        // 获取接口类型
                        hr = pclsObj->Get(L"InterfaceType", 0, &vtProp, nullptr, nullptr);
                        QString interfaceType = (vtProp.vt == VT_BSTR) ? QString::fromWCharArray(vtProp.bstrVal) : "Unknown";
                        VariantClear(&vtProp);
                        
                        // 获取序列号
                        hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, nullptr, nullptr);
                        QString serial = (vtProp.vt == VT_BSTR) ? QString::fromWCharArray(vtProp.bstrVal) : "Unknown";
                        VariantClear(&vtProp);
                        
                        // 格式化输出
                        QString diskInfo = QString("%1 - %2, %3, S/N: %4")
                            .arg(model).arg(size).arg(interfaceType).arg(serial.left(20));
                        disks.append(diskInfo);
                        
                        pclsObj->Release();
                    }
                    
                    if (disks.isEmpty()) {
                        m_diskHardwareInfo = "No disks found";
                    } else {
                        m_diskHardwareInfo = disks.join("\n");
                    }
                    
                    pEnumerator->Release();
                }
                pSvc->Release();
            }
        }
        pLoc->Release();
    }
    
    CoUninitialize();
    emit dataChanged();
}

void SystemDataProvider::fetchNetworkInfo()
{
    // 获取网络适配器信息
    ULONG bufferSize = 0;
    GetAdaptersInfo(nullptr, &bufferSize);
    if (bufferSize == 0) {
        m_networkInfo = "No adapters";
        return;
    }
    
    IP_ADAPTER_INFO* adapterInfo = (IP_ADAPTER_INFO*)malloc(bufferSize);
    if (adapterInfo && GetAdaptersInfo(adapterInfo, &bufferSize) == ERROR_SUCCESS) {
        QStringList adapters;
        for (IP_ADAPTER_INFO* adapter = adapterInfo; adapter; adapter = adapter->Next) {
            if (adapter->Type == MIB_IF_TYPE_ETHERNET || adapter->Type == IF_TYPE_IEEE80211) {
                QString name = QString::fromLocal8Bit(adapter->Description);
                if (!name.isEmpty()) {
                    adapters.append(name);
                }
            }
        }
        m_networkInfo = adapters.isEmpty() ? "No adapters" : adapters.join(", ");
    } else {
        m_networkInfo = "Unknown";
    }
    
    if (adapterInfo) free(adapterInfo);
    emit dataChanged();
}
