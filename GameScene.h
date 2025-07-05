#pragma once
#include "KamataEngine.h"
#include <opencv2/opencv.hpp>
#include <ZXing/ReadBarcode.h>
#include <memory>
#include "IFeature.h"

// ゲームシーン
class GameScene 
{
public:
	enum Mode 
	{
		Wait,	// 待機モード
		QR_Read,	// QRコード読み取りモード
		QR_Export,	// QRコード書き出しモード
	};

	// コンストラクタ
	GameScene();

	// デストラクタ
	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// モード変更
	void ChangeMode(Mode mode);

	

private:

	Mode mode_;

	std::unique_ptr<IFeature> current_;

	// QRコードから読み取った文字列
	std::string qrCodeText_;
};
