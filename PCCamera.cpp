#include "PCCamera.h"

#include <opencv2/highgui.hpp>
#include "CameraSelector.h"

using namespace KamataEngine;

void PCCamera::Initialize()
{
	// カメラを接続
	int pcID = CameraSelector::FindDeviceByName(L"HD Webcam");
	camera_.open(pcID);
	if (!camera_.isOpened()) 
	{
		cv::destroyAllWindows();
		return;
	}

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