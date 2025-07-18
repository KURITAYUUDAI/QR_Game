#include "QRRead.h"

#include "PCCamera.h"
#include "CellPhoneCamera.h"

#include <opencv2/highgui.hpp>

#include <ZXing/ReaderOptions.h>	// ReaderOptions
#include <ZXing/BarcodeFormat.h>	// BarcodeFormat::QrCode
#include <ZXing/ImageView.h>		// ImageView

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

	// ZXing のオプション設定
	options_.setTryHarder(true);
	options_.setFormats(ZXing::BarcodeFormat::QRCode);
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
		frame_ = currentCamera_->GetFrame();

		// グレースケール化
		cv::Mat gray;
		cv::cvtColor(frame_, gray, cv::COLOR_BGR2GRAY);

		// ZXing に投げるための ImageView を生成
		ZXing::ImageView iv(gray.data, gray.cols, gray.rows, ZXing::ImageFormat::Lum);

		// デコード実行
		ZXing::Result result = ZXing::ReadBarcode(iv, options_);
		if (result.isValid()) 
		{
			// 読み取った文字列を取得
			qrCodeText_ = result.text();

			// バウンディングポリゴンを描く
			auto pos = result.position(); // vector<PointF>
			for (size_t i = 0; i < pos.size(); ++i) 
			{
				const auto& p1 = pos[i];
				const auto& p2 = pos[(i + 1) % pos.size()];
				cv::line(frame_, cv::Point(int(p1.x), int(p1.y)), cv::Point(int(p2.x), int(p2.y)), cv::Scalar(255, 0, 0), 2);
			}
		}

		// OpenCV のウィンドウに表示
		cv::imshow("Preview", frame_);
		// ウィンドウのイベント処理＆キー入力待ち（1ms）
		if (Input::GetInstance()->PushKey(DIK_F1)) 
		{
			// ESCキーが押されたらウィンドウを閉じる
			cv::destroyAllWindows();
			return;
		}

		ImGui::Text("%s", qrCodeText_.c_str());
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