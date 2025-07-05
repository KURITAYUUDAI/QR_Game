#include "GameScene.h"
#include "QRRead.h"
#include "QRExport.h"

using namespace KamataEngine;

GameScene::GameScene()
{ 
	ChangeMode(Mode::Wait);
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
	ChangeMode(Mode::Wait);

	if (current_) 
	{
		current_->Shutdown();
	}

	// インゲームの初期化処理を書く

}

void GameScene::Update()
{
	// インゲームの更新処理を書く
	
	static int mode = int(Mode::Wait);
	const char* items[] = { "Wait", "QR_Read", "QR_Export" };
	if (ImGui::Combo("ModeChange", &mode, items, IM_ARRAYSIZE(items)))
	{
		ChangeMode(Mode(mode));
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
		return;
	}
	current_->Initialize();
	mode_ = mode;
}