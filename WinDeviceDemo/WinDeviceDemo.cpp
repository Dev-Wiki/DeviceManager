// WinDeviceDemo.cpp: 定义应用程序的入口点。
//

#include "WinDeviceDemo.h"
#include "Video/ScreenManager.h"

using namespace std;

int main()
{
	cout << "Hello CMake." << endl;

	ScreenManager screenManager;
	screenManager.UpdateDisplayInfo();
	return 0;
}
