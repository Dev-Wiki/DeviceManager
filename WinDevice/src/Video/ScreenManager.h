#pragma once
#include <vector>
#include <map>
#include <dxgi.h>

class ScreenManager
{
public:
	ScreenManager();
	~ScreenManager();
    void UpdateDisplayInfo();

    std::vector<DISPLAY_DEVICE> _displayDeviceList;
    std::map<HMONITOR, MONITORINFOEX> _hMonitorInfoMap;
    std::vector<IDXGIAdapter*> _displayAdapterList;

private:
	void _UpdateDisplayDeviceList();
	void _UpdateMonitorInfoMap();
    void _UpdateDisplayAdapterList();
	static BOOL CALLBACK EnumMonitorsProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
	BOOL _EnumMonitorProc(HMONITOR hMonitor);
};
