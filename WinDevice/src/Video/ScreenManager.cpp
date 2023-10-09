#include "ScreenManager.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "Utils/StringUtil.h"


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
			Wstring2String(displayDevice.DeviceName), Wstring2String(displayDevice.DeviceString),
			Wstring2String(displayDevice.DeviceID), Wstring2String(displayDevice.DeviceKey));
        _displayDeviceList.push_back(displayDevice);
        deviceIndex++;
    }

    spdlog::info("=====GetInfoByEnumDisplayDevices end=====");
}

BOOL ScreenManager::_EnumMonitorProc(HMONITOR hMonitor)
{
	MONITORINFOEX monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFOEX);
	if (GetMonitorInfo(hMonitor, &monitorInfo))
	{
		// ����Ѻ�����
		spdlog::info("_UpdateMonitorInfoMap, szDevice:{0}, right:{1}, bottom:{2}", Wchar2String(monitorInfo.szDevice),
			monitorInfo.rcMonitor.right, monitorInfo.rcMonitor.bottom);
	}
	auto it = ScreenManager::_hMonitorInfoMap.find(hMonitor);
	if (it != ScreenManager::_hMonitorInfoMap.end())
	{
		// ���Ѿ����ڣ���ʾ�����ظ�
		// �����ﴦ���ظ������
	}
	else
	{
		ScreenManager::_hMonitorInfoMap.insert(std::make_pair(hMonitor, monitorInfo));
		// �������ڣ���ʾû���ظ�
		// �����ﴦ����ظ������
	}

	DISPLAYCONFIG_TARGET_DEVICE_NAME targetDeviceName = {};
	targetDeviceName.header.size = sizeof(targetDeviceName);

	// ��ȡָ�� HMONITOR ��Ŀ���豸������Ϣ
	if (DisplayConfigGetDeviceInfo(&targetDeviceName.header) == ERROR_SUCCESS)
	{
		spdlog::info("_UpdateMonitorInfoMap, monitorDevicePath:{0}, monitorFriendlyDeviceName:{1}",
			Wchar2String(targetDeviceName.monitorDevicePath), Wchar2String(targetDeviceName.monitorFriendlyDeviceName));

		// ��ȡָ�� HMONITOR �ķֱ�����Ϣ
		DEVMODE dm;
		dm.dmSize = sizeof(DEVMODE);
		dm.dmDriverExtra = 0;

		if (EnumDisplaySettings(targetDeviceName.monitorDevicePath, ENUM_CURRENT_SETTINGS, &dm) != 0)
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
	// ö����ʾ��
	EnumDisplayMonitors(nullptr, nullptr, EnumMonitorsProc, reinterpret_cast<LPARAM>(this));
	spdlog::info("=====GetInfoByEnumDisplayMonitors end=====");
}
