#include "AudioManager.h"
#include <iostream>


AudioManager::AudioManager()
{
    // 创建设备枚举器
}

AudioManager::~AudioManager()
{
}

HRESULT AudioManager::Init()
{
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&pEnumerator));
    if (FAILED(hr))
    {
        std::cerr << "Error: CoCreateInstance failed with hr = " << hr << std::endl;
    }
    hr = _UpdateDeviceList(eCapture);
    if (FAILED(hr))
    {
        std::cerr << "Error: Update Capture Device failed with hr = " << hr << std::endl;
    }
    hr = _UpdateDeviceList(eRender);
    if (FAILED(hr))
    {
        std::cerr << "Error: Update Render Device failed with hr = " << hr << std::endl;
    }
    return hr;
}

HRESULT AudioManager::Uninit()
{
    pEnumerator = nullptr;
    pCaptureCollection = nullptr;
    pRenderCollection = nullptr;
    pDefaultCaptureEndpoint = nullptr;
    pDefaultRenderEndpoint = nullptr;
    return S_OK;
}

IMMDeviceCollection* AudioManager::GetDeviceList(EDataFlow flow)
{
    if (flow == eCapture)
    {
        UINT count;
        pCaptureCollection->GetCount(&count);
        if (count == 0)
        {
            _UpdateDeviceList(eCapture);
        }
        return pCaptureCollection;
    }
    if (flow == eRender)
    {
        UINT count;
        pRenderCollection->GetCount(&count);
        if (count == 0)
        {
            _UpdateDeviceList(eRender);
        }
        return pRenderCollection;
    }
    return nullptr;
}

IMMDevice* AudioManager::GetDefaultDevice(EDataFlow flow)
{
    if (flow == eCapture)
    {
        return pDefaultCaptureEndpoint;
    }
    if (flow == eRender)
    {
        return pDefaultRenderEndpoint;
    }
    return nullptr;
}

HRESULT AudioManager::_UpdateDeviceList(EDataFlow flow)
{
    HRESULT hr = S_OK;
    if (flow == eCapture)
    {
        hr = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pCaptureCollection);
    }
    if (flow == eRender)
    {
        hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pRenderCollection);
    }
    return hr;
}

HRESULT AudioManager::_UpdateDefaultDevice(EDataFlow flow)
{
    HRESULT hr = S_OK;
    if (flow == eCapture)
    {
        hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDefaultCaptureEndpoint);
    }
    if (flow == eRender)
    {
        hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDefaultRenderEndpoint);
    }
    return hr;
}
