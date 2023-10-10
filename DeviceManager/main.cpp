#include "mainwindow.h"

#include <QApplication>
#include "Utils/CmdUtil.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CmdUtil::ExecuteCommand("");
    MainWindow w;
    w.show();
    return a.exec();
}
