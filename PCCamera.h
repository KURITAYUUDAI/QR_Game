#pragma once
#include "IFeature.h"
#include "KamataEngine.h"

#include <opencv2/opencv.hpp>
#include <ZXing/ReadBarcode.h>

class PCCamera : public IFeature 
{
private:
	void Initialize() override;

	// 更新
	void Update() override;

	// 停止
	void Shutdown() override;

public:
	// カメラ入力用
	cv::VideoCapture camera_;

	// フレーム格納用
	cv::Mat frame_;

	// ZXing ライブラリのオプション
	ZXing::ReaderOptions options_;

	// QRコードから読み取った文字列
	std::string qrCodeText_;
};
