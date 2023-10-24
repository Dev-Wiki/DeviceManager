#include "mainwindow.h"

#include <QApplication>
#include "Utils/LogManager.h"
#include "Video/ScreenManager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qInstallMessageHandler(LogManager::CustomMessageHandler);

    ScreenManager screenManager;
    screenManager.UpdateDisplayInfo();

    HRESULT hr = S_OK;
    for (size_t i = 0; i < screenManager._displayAdapterList.size(); i++) {
        DXGI_ADAPTER_DESC adapterDesc;
        hr = screenManager._displayAdapterList[i]->GetDesc(&adapterDesc);
        if (SUCCEEDED(hr)) {
            qDebug("Adapter: %s", adapterDesc.Description);
        }
    }

    MainWindow w;
    w.show();
    return a.exec();
}
