#include "ScreenManager.h"
#include "spdlog/spdlog.h"
#include "Utils/StringUtil.h"
#include "Utils/Log.h"
#include <dxgi.h>

ScreenManager::ScreenManager()
{
    
}

ScreenManager::~ScreenManager()
{
	
}

void ScreenManager::UpdateDisplayInfo()
{
	_UpdateDisplayDeviceList();
	_UpdateMonitorInfoMap();
	_UpdateDisplayAdapterList();
}

void ScreenManager::_UpdateDisplayDeviceList()
{
    spdlog::info("=====GetInfoByEnumDisplayDevices start=====");
    DISPLAY_DEVICE displayDevice;
    displayDevice.cb = sizeof(DISPLAY_DEVICE);
    DWORD deviceIndex = 0;

    while (EnumDisplayDevices(nullptr, deviceIndex, &displayDevice, 0))
    {
        spdlog::info("Display Device DeviceName:{0}, DeviceString:{1}, DeviceID:{2}, DeviceKey:{3}.", 
			displayDevice.DeviceName, displayDevice.DeviceString, displayDevice.DeviceID, displayDevice.DeviceKey);
        _displayDeviceList.push_back(displayDevice);
        deviceIndex++;
    }

    spdlog::info("=====GetInfoByEnumDisplayDevices end=====");
}

void ScreenManager::_UpdateDisplayAdapterList()
{
	 LOG_FUNC_START();
     HRESULT hr = S_OK;
     IDXGIFactory* pFactory = nullptr;
     hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));
     if (FAILED(hr)) {
         spdlog::error("CreateDXGIFactory failed");
         return;
     }

	 IDXGIAdapter* pAdapter = nullptr;
	 for (UINT adapterIndex = 0; pFactory->EnumAdapters(adapterIndex, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++adapterIndex)
	 {
	 	DXGI_ADAPTER_DESC adapterDesc;
	 	pAdapter->GetDesc(&adapterDesc);
	 	spdlog::info("Adapter Index:{0}, Description:{1}, DeviceId:{2}, VendorId:{3}, SubSysId:{4}, Revision:{5}, AdapterLuid(H-L):{6}-{7} ",
	 		adapterIndex, Wchar2String(adapterDesc.Description), adapterDesc.DeviceId, adapterDesc.VendorId, adapterDesc.SubSysId,
	 		adapterDesc.Revision, adapterDesc.AdapterLuid.HighPart, adapterDesc.AdapterLuid.LowPart);

	 	// print adapter output info
	 	IDXGIOutput* pOutput;
	 	int outputCount = 0;
	 	for (UINT j = 0; pAdapter->EnumOutputs(j, &pOutput) != DXGI_ERROR_NOT_FOUND; ++j) {
	 		DXGI_OUTPUT_DESC outputDesc;
	 		pOutput->GetDesc(&outputDesc);
            
             MONITORINFOEX monitorInfo;
             monitorInfo.cbSize = sizeof(MONITORINFOEX);
             if (GetMonitorInfo(outputDesc.Monitor, &monitorInfo))
             {
                 // 输出友好名称
                 spdlog::info("Adapter Output Index:{0}, DeviceName:{1}, szDevice:{2}, right:{3}, bottom:{4}",
                              j, Wchar2String(outputDesc.DeviceName), monitorInfo.szDevice, monitorInfo.rcMonitor.right, monitorInfo.rcMonitor.bottom);
             }
	 	}
	 }
	 pFactory->Release();
	 LOG_FUNC_END();
}

BOOL ScreenManager::_EnumMonitorProc(HMONITOR hMonitor)
{
	MONITORINFOEX monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFOEX);
	if (GetMonitorInfo(hMonitor, &monitorInfo))
	{
		// 输出友好名称
		spdlog::info("_UpdateMonitorInfoMap, szDevice:{0}, right:{1}, bottom:{2}", monitorInfo.szDevice,
			monitorInfo.rcMonitor.right, monitorInfo.rcMonitor.bottom);
	}
	auto it = ScreenManager::_hMonitorInfoMap.find(hMonitor);
	if (it != ScreenManager::_hMonitorInfoMap.end())
	{
		// 键已经存在，表示存在重复
		// 在这里处理重复的情况
	}
	else
	{
		ScreenManager::_hMonitorInfoMap.insert(std::make_pair(hMonitor, monitorInfo));
		// 键不存在，表示没有重复
		// 在这里处理非重复的情况
	}

	DISPLAYCONFIG_TARGET_DEVICE_NAME targetDeviceName = {};
	targetDeviceName.header.size = sizeof(targetDeviceName);

	// 获取指定 HMONITOR 的目标设备名称信息
	if (DisplayConfigGetDeviceInfo(&targetDeviceName.header) == ERROR_SUCCESS)
	{
		spdlog::info("_UpdateMonitorInfoMap, monitorDevicePath:{0}, monitorFriendlyDeviceName:{1}",
			Wchar2String(targetDeviceName.monitorDevicePath), Wchar2String(targetDeviceName.monitorFriendlyDeviceName));

		// 获取指定 HMONITOR 的分辨率信息
		DEVMODE dm;
		dm.dmSize = sizeof(DEVMODE);
		dm.dmDriverExtra = 0;

		if (EnumDisplaySettingsA((LPCSTR)targetDeviceName.monitorDevicePath, ENUM_CURRENT_SETTINGS, &dm) != 0)
		{
			spdlog::info("Resolution:{0}x{1}", dm.dmPelsWidth, dm.dmPelsHeight);
		}
		else
		{
			spdlog::info("Failed to get display resolution.");
		}
	}
	else
	{
		spdlog::info("Failed to get display device info.");
	}
	return TRUE;
}

BOOL CALLBACK ScreenManager::EnumMonitorsProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	return ((ScreenManager*)dwData)->_EnumMonitorProc(hMonitor);
}

void ScreenManager::_UpdateMonitorInfoMap()
{
	spdlog::info("=====GetInfoByEnumDisplayMonitors start=====");
	// 枚举显示器
	EnumDisplayMonitors(nullptr, nullptr, EnumMonitorsProc, reinterpret_cast<LPARAM>(this));
	spdlog::info("=====GetInfoByEnumDisplayMonitors end=====");
}
