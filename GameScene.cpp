#include "GameScene.h"
#include "QRRead.h"
#include "QRExport.h"

#include "CameraSelector.h"

using namespace KamataEngine;

GameScene::GameScene()
{ 
	modeIndex_ = int(Mode::Wait);
	ChangeMode(Mode(modeIndex_));
}

GameScene::~GameScene() 
{
	if (current_) 
	{
		current_->Shutdown();
	}
}

void GameScene::Initialize()
{
	// メンバ変数への代入処理
	modeIndex_ = int(Mode::Wait);
	ChangeMode(Mode(modeIndex_));

	if (current_) 
	{
		current_->Shutdown();
	}

	// インゲームの初期化処理を書く

}

void GameScene::Update()
{
	// インゲームの更新処理を書く
	//for (int i = 0; i < 5; ++i) {
	//	cv::VideoCapture cap(i);
	//	if (!cap.isOpened()) {
	//		continue;
	//	}

	//	std::cout << "Device ID " << i << " is available. Press [Esc] to skip." << std::endl;

	//	// フレーム表示して確認
	//	while (true) {
	//		cv::Mat frame;
	//		cap >> frame;
	//		if (frame.empty())
	//			break;

	//		cv::imshow("Camera Preview", frame);

	//		int key = cv::waitKey(1);
	//		if (key == 27)
	//			break; // Escで次のデバイスへ
	//	}

	//	cap.release();
	//	cv::destroyAllWindows();
	//}

	// 開けない時はここでブレークポイントを使うと良し
	/*auto devices = CameraSelector::EnumerateDevices();
	std::wcout << L"検出されたカメラ一覧：" << std::endl;
	for (auto& d : devices) {
		std::wcout << L"  ID=" << d.id << L", Name=\"" << d.name << L"\"" << std::endl;
	}*/


	const char* items[] = { "Wait", "QR_Read", "QR_Export" };
	if (ImGui::Combo("ModeChange", &modeIndex_, items, IM_ARRAYSIZE(items)))
	{
		ChangeMode(Mode(modeIndex_));
	}

	// 現在機能の更新処理
	if (current_) 
	{
		current_->Update();
	}

}

void GameScene::Draw()
{ 
	
}

void GameScene::ChangeMode(Mode mode) 
{
	// 現在のモードを終了し内部をリセットする
	if (current_) 
	{
		current_->Shutdown();
		current_.reset();
	}

	// 選択されたモードをcurrent_に代入する
	switch (mode)
	{ 
	case Wait:
		break;

	case QR_Read:
		current_ = std::make_unique<QRRead>();
		break;

	case QR_Export:
		current_ = std::make_unique<QRExport>();
		break;
	}

	// current_がnullptrでない場合は初期化を行う
	if (!current_) 
	{
		mode_ = mode;
		return;
	}
	current_->Initialize();
	mode_ = mode;
}