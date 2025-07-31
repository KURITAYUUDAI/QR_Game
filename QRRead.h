#pragma once
#include "IFeature.h"
#include "ICamera.h"

#define NOMINMAX
#include "KamataEngine.h"

#include "Character.h"

#include <opencv2/opencv.hpp>
#include <ZXing/ReadBarcode.h>

class QRRead : public IFeature
{
public:
	
	enum class OperationMode 
	{ 
		Wait, 
		Capture, 
		LoadFile 
	};
	// キャプチャ方法
	enum class CaptureMode 
	{ 
		Wait,
		PC_Camera, 
		CellPhone_Camera 
	};

	QRRead();

	// 初期化
	void Initialize() override;

	// 更新
	void Update() override;

	// 停止
	void Shutdown() override;

	// モード変更
	void ChangeCameraMode(CaptureMode mode);

	// 新規：バイナリ処理
	void BinaryResult(const ZXing::Result& result);

private:

	// カメラ変更用変数

	OperationMode operationMode_ = OperationMode::Wait;

	CaptureMode captureMode_ = CaptureMode::PC_Camera;

	// ファイル読込用
	std::string filePath_;

	// ImGui::Combo用のインデックス
	int cameraModeIndex_;

	// 現在のカメラクラスへのポインタ
	std::unique_ptr<ICamera> currentCamera_;

	// フレーム格納用
	cv::Mat frame_;

	// ZXing ライブラリのオプション
	ZXing::ReaderOptions options_;

	// QRコードから読み取った文字列
	std::string qrCodeText_;

	// バイナリモードフラグ
	bool binaryMode_ = false;                            

	// チャンク蓄積
	std::map<uint8_t, std::vector<uint8_t>> chunkBuffer_;  
	
	// 全チャンク数
	uint8_t expectedTotal_ = 0;   

	// 読み取ったキャラデータ
	Character::CharacterData lastData_;
	
	// データ読み取り済みフラグ
	bool hasData_ = false;

	bool isRGB_ = false;

	std::vector<bool> isReadRGB_; // RGB各チャンネルの読み取り済みフラグ

	std::array<std::string, 3> rgbCodeText_; // R, G, B（index = 2, 1, 0）

	static constexpr int MAX_BASE64_LEN = 64 * 1024;

	 char base64Buf_[MAX_BASE64_LEN] = {0};
};


