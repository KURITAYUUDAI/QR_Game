#pragma once
#include "IFeature.h"

#include "KamataEngine.h"

#include <opencv2/opencv.hpp>
#include <ZXing/ReadBarcode.h>

class QRRead : public IFeature
{
public:
	
	enum CameraMode 
	{
		Wait,				// 待機モード
		PC_Camera,			// PCカメラモード
		CellPhone_Camera,	// スマホカメラモード
	};

	QRRead();

	// 初期化
	void Initialize() override;

	// 更新
	void Update() override;

	// 停止
	void Shutdown() override;

	// モード変更
	void ChangeCameraMode(CameraMode mode);

private:

	// QRコードから読み取った文字列
	std::string qrCodeText_;

	// カメラ変更用変数
	CameraMode mode_;

	// ImGui::Combo用のインデックス
	int cameraModeIndex_;

	// 現在のカメラクラスへのポインタ
	std::unique_ptr<IFeature> currentCamera_;

};


