#pragma once
#include "IFeature.h"

#define NOMINMAX
#include "KamataEngine.h"

#include <opencv2/opencv.hpp>
#include <ZXing/MultiFormatWriter.h>
#include <ZXing/BitMatrix.h>

#include <cstdint>
#include <vector>

#include "Character.h"

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



	// バイナリ生成用
	cv::Mat generateBinaryQR(const std::vector<uint8_t>& data);

	// ユーティリティ
	template<typename T> void appendLE(std::vector<uint8_t>& buf, T value) 
	{
		for (size_t i = 0; i < sizeof(T); ++i)
			buf.push_back(static_cast<uint8_t>((value >> (8 * i)) & 0xFF));
	}
	
};
