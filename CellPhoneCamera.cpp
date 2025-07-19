#include "CellPhoneCamera.h"

#include <opencv2/highgui.hpp>
#include "CameraSelector.h"

using namespace KamataEngine;

void CellPhoneCamera::Initialize()
{
	// カメラを接続
	int phoneID = CameraSelector::FindDeviceByName(L"iVCam");
	camera_.open(phoneID);
	if (!camera_.isOpened()) 
	{
		cv::destroyAllWindows();
		return;
	}

	//// ウィンドウを作成
	//cv::namedWindow("CellPhone_Camera Preview", cv::WINDOW_AUTOSIZE);
}

void CellPhoneCamera::Update()
{
	if (!camera_.isOpened()) 
	{
		// カメラが開けなかった場合はログにエラーメッセージを表示
		ImGui::Text("Error: Could not open IP camera.");
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

void CellPhoneCamera::Shutdown() 
{
	// カメラを閉じる
	if (camera_.isOpened()) 
	{
		camera_.release();
	}
	// OpenCVのウィンドウを閉じる
	cv::destroyAllWindows();
}

int CellPhoneCamera::SetCameraDeviceID()
{ 
	return 0;
}