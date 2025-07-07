#include "PCCamera.h"
#include <opencv2/highgui.hpp>

#include <ZXing/ReaderOptions.h>	// ReaderOptions
#include <ZXing/BarcodeFormat.h>	// BarcodeFormat::QrCode
#include <ZXing/ImageView.h>		// ImageView

#include "CameraSelector.h"

using namespace KamataEngine;

void PCCamera::Initialize()
{
	//// カメラを起動
	int pcID = CameraSelector::FindDeviceByName(L"HD Webcam");
	camera_.open(pcID);
	if (!camera_.isOpened()) 
	{
		cv::destroyAllWindows();
		return;
	}

	// ZXing のオプション設定
	options_.setTryHarder(true);
	options_.setFormats(ZXing::BarcodeFormat::QRCode);

	// 解像度を設定（必要に応じて）
	camera_.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	camera_.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

	// ウィンドウを作成
	cv::namedWindow("PC_Camera Preview", cv::WINDOW_AUTOSIZE);
}

void PCCamera::Update()
{
	if (!camera_.isOpened()) 
	{
		// カメラが開けなかった場合はログにエラーメッセージを表示
		ImGui::Text("Error: Could not open camera.");

		return;
	}
	// カメラから１フレーム取得
	camera_ >> frame_;
	if (frame_.empty())
	{
		// フレーム取得失敗時は何もしない
		return;
	}

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
	cv::imshow("PC_Camera Preview", frame_);
	// ウィンドウのイベント処理＆キー入力待ち（1ms）
	if (Input::GetInstance()->PushKey(DIK_F1)) 
	{
		// ESCキーが押されたらウィンドウを閉じる
		cv::destroyAllWindows();
		return;
	}

	ImGui::Text("%s", qrCodeText_.c_str());
}

void PCCamera::Shutdown()
{
	// カメラを閉じる
	if (camera_.isOpened()) 
	{
		camera_.release();
	}
	// OpenCVのウィンドウを閉じる
	cv::destroyAllWindows();
}