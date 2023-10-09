#pragma once
#include <vector>
#include <map>

#include <Mmdeviceapi.h>

class ScreenManager
{
public:
	ScreenManager();
	~ScreenManager();
	void UpdateDisplayInfo();

private:
	void _UpdateDisplayDeviceList();
	void _UpdateMonitorInfoMap();
	static BOOL CALLBACK EnumMonitorsProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
	BOOL _EnumMonitorProc(HMONITOR hMonitor);

	std::vector<DISPLAY_DEVICE> _displayDeviceList;
	std::map<HMONITOR, MONITORINFOEX> _hMonitorInfoMap;
};
