#pragma once

#include <dshow.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#pragma comment(lib, "Strmiids.lib")

// CameraSelector: Device name-based camera selection for PC camera and smartphone virtual camera
class CameraSelector 
{
public:
	struct DeviceInfo 
	{
		int id;            // VideoCapture デバイスID
		std::wstring name; // DirectShow FriendlyName
	};

	// 全ビデオ入力デバイスを列挙
	static std::vector<DeviceInfo> EnumerateDevices() 
	{
		std::vector<DeviceInfo> devices;
		CoInitialize(nullptr);

		ICreateDevEnum* pDevEnum = nullptr;
		IEnumMoniker* pEnum = nullptr;
		if (SUCCEEDED(CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum)))) {

			if (pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0) == S_OK) 
			{

				IMoniker* pMoniker = nullptr;
				int index = 0;
				while (pEnum->Next(1, &pMoniker, nullptr) == S_OK) 
				{
					IPropertyBag* pPropBag = nullptr;
					if (SUCCEEDED(pMoniker->BindToStorage(nullptr, nullptr, IID_PPV_ARGS(&pPropBag))))
					{
						VARIANT varName;
						VariantInit(&varName);
						if (SUCCEEDED(pPropBag->Read(L"FriendlyName", &varName, nullptr))) 
						{
							devices.push_back({index, varName.bstrVal});
						}
						VariantClear(&varName);
						pPropBag->Release();
					}
					pMoniker->Release();
					++index;
				}
				pEnum->Release();
			}
			pDevEnum->Release();
		}

		CoUninitialize();
		return devices;
	}

	// デバイス名に指定した文字列を含むものを検索（最初に見つかった ID を返す、なければ -1）
	static int FindDeviceByName(const std::wstring& substring) 
	{
		auto devices = EnumerateDevices();
		for (const auto& d : devices) {
			if (d.name.find(substring) != std::wstring::npos) 
			{
				return d.id;
			}
		}
		return -1;
	}
};