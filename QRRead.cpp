#include "QRRead.h"

#include <opencv2/highgui.hpp>

#include "PCCamera.h"
#include "CellPhoneCamera.h"

using namespace KamataEngine;

QRRead::QRRead()
{ 
	ChangeCameraMode(CameraMode::Wait); 
}

void QRRead::Initialize() 
{
	cameraModeIndex_ = int(CameraMode::Wait);
	ChangeCameraMode(CameraMode(cameraModeIndex_));

	if (currentCamera_) 
	{
		currentCamera_->Shutdown();
	}
}

void QRRead::Update() 
{
	const char* items[] = {"Wait", "PC_Camera", "CellPhone_Camera"};
	if (ImGui::Combo("CameraChange", &cameraModeIndex_, items, IM_ARRAYSIZE(items))) 
	{
		ChangeCameraMode(CameraMode(cameraModeIndex_));
	}

	// 現在機能の更新処理
	if (currentCamera_) 
	{
		currentCamera_->Update();
	}
}

void QRRead::Shutdown() 
{
	// メンバ変数への代入処理
	ChangeCameraMode(CameraMode::Wait);

	if (currentCamera_) 
	{
		currentCamera_->Shutdown();
	}
}

void QRRead::ChangeCameraMode(CameraMode mode)
{
	// 現在のモードを終了し内部をリセットする
	if (currentCamera_) 
	{
		currentCamera_->Shutdown();
		currentCamera_.reset();
	}

	// 選択されたモードをcurrent_に代入する
	switch (mode) 
	{
	case Wait:
		break;

	case PC_Camera:
		currentCamera_ = std::make_unique<PCCamera>();
		break;

	case CellPhone_Camera:
		currentCamera_ = std::make_unique<CellPhoneCamera>();
		break;
	}

	// current_がnullptrでない場合は初期化を行う
	if (!currentCamera_) 
	{
		mode_ = mode;
		return;
	}
	currentCamera_->Initialize();
	mode_ = mode;
}