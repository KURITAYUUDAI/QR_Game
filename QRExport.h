#pragma once
#include "IFeature.h"

#include "KamataEngine.h"

#include <opencv2/opencv.hpp>
#include <ZXing/MultiFormatWriter.h>
#include <ZXing/BitMatrix.h>

class QRExport : public IFeature 
{
public:

	QRExport();

	void Initialize() override;

	// 更新
	void Update() override;

	// 停止
	void Shutdown() override;

private:

	// エクスポートする文字列
	std::string text_;
	
	// 画像サイズ
	int width_;
	int height_;
	
	// モジュールあたりのスケールとマージン
	int scale_;
	int border_;
	
	// 生成したQRコード画像
	cv::Mat qrImage_;
	
	// ZXingライター
	ZXing::MultiFormatWriter writer_;

	// 生成したQRコードの保存先パス
	std::string savePath_;

};
