#pragma once

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "wbemuuid.lib")

class SysInfoUtil
{
public:
    void GetMacByGetAdaptersInfo(char* outMAC) const;

    static int GetInfoByWMI();

    static void GetInfoByEnumDisplayMonitors();

    static int GetInfoByEnumDisplayDevices();

    static int GetInfoByQueryDisplayConfig();

    static void GetInfoByEdid();

    static int GetInfoByCfgmgr();





};
