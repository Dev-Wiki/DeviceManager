#include "mainwindow.h"

#include <QApplication>
#include "spdlog/spdlog.h"
#include "Video/ScreenManager.h"
#include "Utils/CmdUtil.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
//    ScreenManager screen_manager;
//    screen_manager.UpdateDisplayInfo();
    CmdUtil::ExecuteCommand("ls");
    spdlog::info("000");
    return a.exec();
}
