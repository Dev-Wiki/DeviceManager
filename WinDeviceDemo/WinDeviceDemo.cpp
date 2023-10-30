// WinDeviceDemo.cpp: 定义应用程序的入口点。
//

#include "WinDeviceDemo.h"
#include "Video/ScreenManager.h"

using namespace std;

int main()
{
	HRESULT hr = S_OK;
	hr = CoInitialize(nullptr);
	if (FAILED(hr)) {
		return 0;
	}
	cout << "Hello CMake." << endl;

	ScreenManager screenManager;
	screenManager.UpdateDisplayInfo();
	CoUninitialize();
	return 0;
}
