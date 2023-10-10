#include <stdafx.h>
#include <cstdio>
#include <iphlpapi.h>
#include <dxgi.h>
#include <vector>
#include <windows.h>
#include <winuser.h>
#include <highlevelmonitorconfigurationapi.h>
#include <physicalmonitorenumerationapi.h>
#include <cfgmgr32.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <io.h>
#include <fcntl.h>
#include <chrono>
#include <iostream>
#include <string>

#include "SysInfoUtil.h"

using namespace std;

#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS

#include <tchar.h>
#include <initguid.h>
#include <wmistr.h>
DEFINE_GUID(WmiMonitorID_GUID, 0x671a8285, 0x4edb, 0x4cae, 0x99, 0xfe, 0x69, 0xa1, 0x5c, 0x48, 0xc0, 0xbc);
typedef struct WmiMonitorID {
    USHORT ProductCodeID[16];
    USHORT SerialNumberID[16];
    USHORT ManufacturerName[16];
    UCHAR WeekOfManufacture;
    USHORT YearOfManufacture;
    USHORT UserFriendlyNameLength;
    USHORT UserFriendlyName[1];
} WmiMonitorID, * PWmiMonitorID;
#define OFFSET_TO_PTR(Base, Offset) ((PBYTE)((PBYTE)Base + Offset))

typedef HRESULT(WINAPI* WOB) (IN LPGUID lpGUID, IN DWORD nAccess, OUT LONG*);
WOB WmiOpenBlock;
typedef HRESULT(WINAPI* WQAD) (IN LONG hWMIHandle, ULONG* nBufferSize, OUT UCHAR* pBuffer);
WQAD WmiQueryAllData;
typedef HRESULT(WINAPI* WCB) (IN LONG);
WCB WmiCloseBlock;

void SysInfoUtil::GetInfoByEdid()
{
    HRESULT hr = E_FAIL;
    LONG hWmiHandle;
    PWmiMonitorID MonitorID;
    HINSTANCE hDLL = LoadLibrary(L"Advapi32.dll");
    WmiOpenBlock = (WOB)GetProcAddress(hDLL, "WmiOpenBlock");
    WmiQueryAllData = (WQAD)GetProcAddress(hDLL, "WmiQueryAllDataW");
    WmiCloseBlock = (WCB)GetProcAddress(hDLL, "WmiCloseBlock");
    if (WmiOpenBlock != NULL && WmiQueryAllData && WmiCloseBlock)
    {
        WCHAR pszDeviceId[256] = L"";
        hr = WmiOpenBlock((LPGUID)&WmiMonitorID_GUID, GENERIC_READ, &hWmiHandle);
        if (hr == ERROR_SUCCESS)
        {
            ULONG nBufferSize = 0;
            UCHAR* pAllDataBuffer = 0;
            PWNODE_ALL_DATA pWmiAllData;
            hr = WmiQueryAllData(hWmiHandle, &nBufferSize, 0);
            if (hr == ERROR_INSUFFICIENT_BUFFER)
            {
                pAllDataBuffer = (UCHAR*)malloc(nBufferSize);
                hr = WmiQueryAllData(hWmiHandle, &nBufferSize, pAllDataBuffer);
                if (hr == ERROR_SUCCESS)
                {
                    while (1)
                    {
                        pWmiAllData = (PWNODE_ALL_DATA)pAllDataBuffer;
                        if (pWmiAllData->WnodeHeader.Flags & WNODE_FLAG_FIXED_INSTANCE_SIZE)
                            MonitorID = (PWmiMonitorID)&pAllDataBuffer[pWmiAllData->DataBlockOffset];
                        else
                            MonitorID = (PWmiMonitorID)&pAllDataBuffer[pWmiAllData->OffsetInstanceDataAndLength[0].OffsetInstanceData];

                        ULONG nOffset = 0;
                        WCHAR* pwsInstanceName = 0;
                        nOffset = (ULONG)pAllDataBuffer[pWmiAllData->OffsetInstanceNameOffsets];
                        pwsInstanceName = (WCHAR*)OFFSET_TO_PTR(pWmiAllData, nOffset + sizeof(USHORT));
                        WCHAR wsText[255] = L"";
                        wcout << L"Instance =" << pwsInstanceName << endl;
                        // swprintf(wsText, L"Instance Name = %s\r\n", pwsInstanceName);
                        OutputDebugString(wsText);

                        WCHAR* pwsUserFriendlyName;
                        pwsUserFriendlyName = (WCHAR*)MonitorID->UserFriendlyName;
                        wcout << L"UserFriendlyName =" << pwsUserFriendlyName << endl;
                        // swprintf(wsText, L"User Friendly Name = %s\r\n", pwsUserFriendlyName);
                        OutputDebugString(wsText);

                        WCHAR* pwsManufacturerName;
                        pwsManufacturerName = (WCHAR*)MonitorID->ManufacturerName;
                        wcout << L"ManufacturerName =" << pwsManufacturerName << endl;
                        // swprintf(wsText, L"Manufacturer Name = %s\r\n", pwsManufacturerName);
                        OutputDebugString(wsText);

                        WCHAR* pwsProductCodeID;
                        pwsProductCodeID = (WCHAR*)MonitorID->ProductCodeID;
                        wcout << L"ProductCodeID =" << pwsProductCodeID << endl;
                        // swprintf(wsText, L"Product Code ID = %s\r\n", pwsProductCodeID);
                        OutputDebugString(wsText);

                        WCHAR* pwsSerialNumberID;
                        pwsSerialNumberID = (WCHAR*)MonitorID->SerialNumberID;
                        wcout << L"SerialNumberID =" << pwsSerialNumberID << endl;
                        // swprintf(wsText, L"Serial Number ID = %s\r\n", pwsSerialNumberID);
                        OutputDebugString(wsText);
                        
                        if (!pWmiAllData->WnodeHeader.Linkage)
                            break;
                        pAllDataBuffer += pWmiAllData->WnodeHeader.Linkage;
                    }
                    free(pAllDataBuffer);
                }
            }
            WmiCloseBlock(hWmiHandle);
        }
    }
}

void SysInfoUtil::GetMacByGetAdaptersInfo(char* outMAC) const
{
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    PIP_ADAPTER_INFO pAdapter = NULL;
    PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);

    // Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS)
    {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
    }

    //char MACAddress[32];
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR)
    {
        pAdapter = pAdapterInfo;
        while (pAdapter)
        {
            if (pAdapter->AddressLength == 6 && pAdapter->Type == MIB_IF_TYPE_ETHERNET)
            {
                sprintf_s(outMAC, 18, "%02X-%02X-%02X-%02X-%02X-%02X",
                    int(pAdapter->Address[0]),
                    int(pAdapter->Address[1]),
                    int(pAdapter->Address[2]),
                    int(pAdapter->Address[3]),
                    int(pAdapter->Address[4]),
                    int(pAdapter->Address[5]));
                break;
            }
            pAdapter = pAdapter->Next;
        }
    }
    free(pAdapterInfo);
}

int SysInfoUtil::GetInfoByQueryDisplayConfig()
{
    wcout << L"=====GetInfoByQueryDisplayConfig start=====" << endl;
    std::vector<DISPLAYCONFIG_PATH_INFO> paths;
    std::vector<DISPLAYCONFIG_MODE_INFO> modes;
    UINT32 flags = QDC_ONLY_ACTIVE_PATHS;
    LONG isError = ERROR_INSUFFICIENT_BUFFER;

    UINT32 pathCount, modeCount;
    isError = GetDisplayConfigBufferSizes(flags, &pathCount, &modeCount);
    if (isError)
    {
        return 0;
    }

    // Allocate the path and mode arrays
    paths.resize(pathCount);
    modes.resize(modeCount);
    // Get all active paths and their modes
    isError = QueryDisplayConfig(flags, &pathCount, paths.data(), &modeCount, modes.data(), nullptr);
    // The function may have returned fewer paths/modes than estimated
    paths.resize(pathCount);
    modes.resize(modeCount);
    if (isError)
    {
        return 0;
    }
    // For each active path
    int len = paths.size();
    for (int i = 0; i < len; i++)
    {
        // Find the target (monitor) friendly name
        DISPLAYCONFIG_TARGET_DEVICE_NAME targetName = {};
        targetName.header.adapterId = paths[i].targetInfo.adapterId;
        targetName.header.id = paths[i].targetInfo.id;
        targetName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        targetName.header.size = sizeof(targetName);
        isError = DisplayConfigGetDeviceInfo(&targetName.header);

        if (isError)
        {
            return 0;
        }
        wcout << "monitorFriendlyDeviceName:" << targetName.monitorFriendlyDeviceName << endl;
        wcout << "monitorDevicePath:" << targetName.monitorDevicePath << endl;
    }
    wcout << "=====GetInfoByQueryDisplayConfig end=====" << endl;
}
int SysInfoUtil::GetInfoByCfgmgr()
{
    wcout << "GetInfoByCfgmgr start" << endl;
    DEVINST devInstRoot;
    CONFIGRET cr = CM_Locate_DevNode(&devInstRoot, NULL, 0);
    if (cr != CR_SUCCESS) {
        // 处理错误
        wcout << "GetInfoByCfgmgr error" << cr << endl;
        return 0;
    }
    DEVINST devInstMonitor;
    cr = CM_Get_Child(&devInstMonitor, devInstRoot, 0);
    while (cr == CR_SUCCESS) {
        // 在这里处理监视器设备
        WCHAR displayName[1024];
        ULONG displayNameSize = sizeof(displayName);

        cr = CM_Get_DevNode_Registry_Property(devInstMonitor, CM_DRP_FRIENDLYNAME, NULL, displayName, &displayNameSize, 0);
        if (cr == CR_SUCCESS) {
            // displayName 包含显示名称
            wcout << "displayName:" << displayName << endl;
        }

        WCHAR hardwareId[1024];
        ULONG hardwareIdSize = sizeof(hardwareId);

        cr = CM_Get_DevNode_Registry_Property(devInstMonitor, CM_DRP_HARDWAREID, NULL, hardwareId, &hardwareIdSize, 0);
        if (cr == CR_SUCCESS) {
            // hardwareId 包含硬件ID
            wcout << "hardwareId:" << hardwareId << endl;
        }
        // 继续遍历下一个设备
        cr = CM_Get_Sibling(&devInstMonitor, devInstMonitor, 0);
    }
    wcout << "GetInfoByCfgmgr end" << endl;
}

int SysInfoUtil::GetInfoByEnumDisplayDevices()
{
    wcout << "=====GetInfoByEnumDisplayDevices start=====" << endl;
    DISPLAY_DEVICE displayDevice;
    displayDevice.cb = sizeof(DISPLAY_DEVICE);
    DWORD deviceIndex = 0;

    while (EnumDisplayDevices(NULL, deviceIndex, &displayDevice, 0))
    {
        std::wcout << "Display Device Name: " << displayDevice.DeviceName << std::endl;
        std::wcout << "Display Device String: " << displayDevice.DeviceString << std::endl;
        std::wcout << "Display Device ID: " << displayDevice.DeviceID << std::endl;
        std::wcout << "Display Device Key: " << displayDevice.DeviceKey << std::endl;
        deviceIndex++;
    }

    wcout << "=====GetInfoByEnumDisplayDevices end=====" << endl;
    return 0;
}

void GetInfoByEnumDisplayDevicesA()
{
    DISPLAY_DEVICEA dd;
    DEVMODEA dm;
    for (int i = 0;; i++) {
        ZeroMemory(&dd, sizeof(dd));
        dd.cb = sizeof(dd);
        BOOL ret = (EnumDisplayDevicesA(NULL, i, &dd, 0));
        if (ret == FALSE)
            break;
        ZeroMemory(&dm, sizeof(dm));
        dm.dmSize = sizeof(dm);
        if (EnumDisplaySettingsA(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm)) {
            std::cout << "Device #" << i << " Information:" << std::endl;
            std::cout << "Device Name: " << dd.DeviceName << std::endl;
            std::cout << "Device String: " << dd.DeviceString << std::endl;
            std::cout << "State: " << (dd.StateFlags & DISPLAY_DEVICE_ACTIVE ? "Active" : "Inactive") << std::endl;
            std::cout << "Device ID: " << dd.DeviceID << std::endl;
            std::cout << "Device Key: " << dd.DeviceKey << std::endl;
            std::cout << std::endl;
        }
    }
}
void SysInfoUtil::GetInfoByEnumDisplayMonitors()
{
    wcout << "=====GetInfoByEnumDisplayMonitors start=====" << endl;
    // 枚举显示器
    EnumDisplayMonitors(NULL, NULL, [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) -> BOOL
    {
	    MONITORINFOEX monitorInfo;
	    monitorInfo.cbSize = sizeof(MONITORINFOEX);
	    if (GetMonitorInfo(hMonitor, &monitorInfo))
	    {
		    // 输出友好名称
		    wcout << "szDevice:" << monitorInfo.szDevice << endl;
		    wcout << "right:" << monitorInfo.rcMonitor.right << endl;
		    wcout << "bottom:" << monitorInfo.rcMonitor.bottom << endl;
	    }
        DISPLAYCONFIG_TARGET_DEVICE_NAME targetDeviceName = {};
	    targetDeviceName.header.size = sizeof(targetDeviceName);

	    // 获取指定 HMONITOR 的目标设备名称信息
	    if (DisplayConfigGetDeviceInfo(&targetDeviceName.header) == ERROR_SUCCESS)
	    {
		    wcout << L"Device Name: " << targetDeviceName.monitorFriendlyDeviceName << endl;
		    wcout << L"Source Name: " << targetDeviceName.monitorDevicePath << endl;

		    // 获取指定 HMONITOR 的分辨率信息
		    DEVMODE dm;
		    dm.dmSize = sizeof(DEVMODE);
		    dm.dmDriverExtra = 0;

		    if (EnumDisplaySettings(targetDeviceName.monitorDevicePath, ENUM_CURRENT_SETTINGS, &dm) != 0)
		    {
			    wcout << L"Resolution: " << dm.dmPelsWidth << L" x " << dm.dmPelsHeight << endl;
		    }
		    else
		    {
			    wcout << L"Failed to get display resolution." << endl;
		    }

		    wcout << L"------------------------------------------" << endl;
	    }
	    else
	    {
		    wcout << L"Failed to get display device info." << endl;
	    }
	    return TRUE;
    }, 0);
    wcout << "=====GetInfoByEnumDisplayMonitors end=====" << endl;
}

int SysInfoUtil::GetInfoByWMI()
{
    wcout << "=====GetInfoByWMI start=====" << endl;
    _setmode(_fileno(stdout), _O_U16TEXT);
    HRESULT hr;

    hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        std::wcerr << L"COM 初始化失败！" << std::endl;
        return 1;
    }

    hr = CoInitializeSecurity(
        nullptr,
        -1,
        nullptr,
        nullptr,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr,
        EOAC_NONE,
        nullptr);

    if (FAILED(hr)) {
        CoUninitialize();
        std::wcerr << L"CoInitializeSecurity 失败！" << std::endl;
        return 1;
    }

    IWbemLocator* pLocator = nullptr;
    hr = CoCreateInstance(
        CLSID_WbemLocator,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator,
        reinterpret_cast<void**>(&pLocator));

    if (FAILED(hr)) {
        CoUninitialize();
        std::wcerr << L"CoCreateInstance 失败！" << std::endl;
        return 1;
    }

    IWbemServices* pServices = nullptr;
    hr = pLocator->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"),
        nullptr,
        nullptr,
        0,
        0,
        0,
        0,
        &pServices);

    pLocator->Release();

    if (FAILED(hr)) {
        CoUninitialize();
        std::wcerr << L"ConnectServer 失败！" << std::endl;
        return 1;
    }

    hr = CoSetProxyBlanket(
        pServices,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        nullptr,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr,
        EOAC_NONE);

    if (FAILED(hr)) {
        pServices->Release();
        CoUninitialize();
        std::wcerr << L"CoSetProxyBlanket 失败！" << std::endl;
        return 1;
    }

    IEnumWbemClassObject* pPnPEntity = nullptr;
    hr = pServices->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_PnPEntity where service=\"monitor\""),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr,
        &pPnPEntity);

    IEnumWbemClassObject* pDesktopMonitor = nullptr;
    hr = pServices->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_DesktopMonitor"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr,
        &pDesktopMonitor);

    if (FAILED(hr)) {
        pServices->Release();
        CoUninitialize();
        std::wcerr << L"ExecQuery 失败！" << std::endl;
        return 1;
    }

    IWbemClassObject* pPnPEntityClassObject = nullptr;
    ULONG uReturn = 0;

    while (pPnPEntity->Next(WBEM_INFINITE, 1, &pPnPEntityClassObject, &uReturn) == 0) {
        VARIANT vtDeviceID;
        VARIANT vtPNPDeviceID;

        hr = pPnPEntityClassObject->Get(L"DeviceID", 0, &vtDeviceID, 0, 0);
        hr = pPnPEntityClassObject->Get(L"PNPDeviceID", 0, &vtPNPDeviceID, 0, 0);

        if (SUCCEEDED(hr) && vtDeviceID.vt == VT_BSTR && vtPNPDeviceID.vt == VT_BSTR) {
            std::wcout << L"DeviceID: " << vtDeviceID.bstrVal << std::endl;
            std::wcout << L"PNPDeviceID: " << vtPNPDeviceID.bstrVal << std::endl;
        }

        pPnPEntityClassObject->Release();
    }

    IWbemClassObject* pDesktopMonitorClassObject = nullptr;
    ULONG uDeskReturn = 0;
    while (pDesktopMonitor->Next(WBEM_INFINITE, 1, &pDesktopMonitorClassObject, &uDeskReturn) == 0) {
        VARIANT vtDeviceID;
        VARIANT vtScreenWidth;
        VARIANT vtScreenHeight;
        VARIANT vtPNPDeviceID;

        hr = pDesktopMonitorClassObject->Get(L"DeviceID", 0, &vtDeviceID, 0, 0);
        hr = pDesktopMonitorClassObject->Get(L"ScreenWidth", 0, &vtScreenWidth, 0, 0);
        hr = pDesktopMonitorClassObject->Get(L"ScreenHeight", 0, &vtScreenHeight, 0, 0);
        hr = pDesktopMonitorClassObject->Get(L"PNPDeviceID", 0, &vtPNPDeviceID, 0, 0);

        if (SUCCEEDED(hr) && vtDeviceID.vt == VT_BSTR && vtScreenWidth.vt == VT_I4 && vtScreenHeight.vt == VT_I4 ) {
            std::wcout << L"DeviceID: " << vtDeviceID.bstrVal << std::endl;
            std::wcout << L"size: " << vtScreenWidth.lVal << L"x" << vtScreenHeight.lVal << std::endl;
        }

        pDesktopMonitorClassObject->Release();
    }
    pPnPEntity->Release();
    pDesktopMonitor->Release();
    pServices->Release();
    CoUninitialize();
    wcout << L"=====GetInfoByWMI end=====" << endl;
    return 0;
}
