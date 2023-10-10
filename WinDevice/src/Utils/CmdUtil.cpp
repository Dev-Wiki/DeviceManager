#include "CmdUtil.h"

#include <iostream>
#include <Windows.h>
#include <string>
#include "spdlog/spdlog.h"

using namespace std;

std::string CmdUtil::ExecuteCommand(const std::string& command)
{
    spdlog::info("ExecuteCommand command:{0}", command);
    auto cmd_result = std::string();
    char psBuffer[0x2000] = { 0 };
    FILE* pPipe;

    /* Run DIR so that it writes its output to a pipe. Open this
     * pipe with read text attribute so that we can read it
     * like a text file.
     */

    if ((pPipe = _popen(command.c_str(), "rt")) == NULL)
    {
        exit(1);
    }

    /* Read pipe until end of file, or an error occurs. */

    while (fgets(psBuffer, 0x2000, pPipe))
    {
        cmd_result += psBuffer;
        memset(psBuffer, 0, 0x2000);
    }

    int endOfFileVal = feof(pPipe);
    int closeReturnVal = _pclose(pPipe);

    if (endOfFileVal)
    {
        spdlog::info("ExecuteCommand, Process returned {0}", closeReturnVal);
    }
    else
    {
        spdlog::error("ExecuteCommand Error: Failed to read the pipe to the end");
    }
    spdlog::info("ExecuteCommand result: {0}", cmd_result);
    return cmd_result;
}
