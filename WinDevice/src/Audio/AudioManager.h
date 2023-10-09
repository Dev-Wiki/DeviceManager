#pragma once

#include <Mmdeviceapi.h>

class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

    HRESULT Init();
    HRESULT Uninit();

    IMMDeviceCollection* GetDeviceList(EDataFlow flow);
    IMMDevice* GetDefaultDevice(EDataFlow flow);

private:
    IMMDeviceEnumerator* pEnumerator = NULL;
    IMMDeviceCollection* pRenderCollection = NULL;
    IMMDeviceCollection* pCaptureCollection = NULL;
    IMMDevice* pDefaultRenderEndpoint = NULL;
    IMMDevice* pDefaultCaptureEndpoint = NULL;

    HRESULT _UpdateDeviceList(EDataFlow flow);
    HRESULT _UpdateDefaultDevice(EDataFlow flow);
};
