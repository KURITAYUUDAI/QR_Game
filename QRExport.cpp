#include "QRExport.h"

#include <ZXing/BarcodeFormat.h>
#include <filesystem>

using namespace KamataEngine;

// ウィンドウの存在／表示状態をチェックするヘルパー
static bool isWindowVisible(const std::string& winName) 
{
	// WND_PROP_VISIBLE: 1なら表示中、0なら非表示、負なら存在しない
	double prop = cv::getWindowProperty(winName, cv::WND_PROP_VISIBLE);
	return prop > 0;
}


QRExport::QRExport()
	: writer_(ZXing::BarcodeFormat::QRCode) 
{

}

void QRExport::Initialize()
{
	text_ = "Hello, QR_Export!";
	width_ = 250;
	height_ = 250;
	scale_ = 10;
	border_ = 4;
	
	savePath_ = "Output/qr_codes";
}

void QRExport::Update()
{
	// テキスト入力用に固定バッファを用意
	static char buf[512];
	strncpy_s(buf, text_.c_str(), sizeof(buf));
	ImGui::InputText("Text", buf, sizeof(buf));
	text_ = std::string(buf);

	ImGui::SliderInt("Width", &width_, 100, 1000);
	ImGui::SliderInt("Height", &height_, 100, 1000);
	ImGui::SliderInt("Scale", &scale_, 1, 20);
	ImGui::SliderInt("Border", &border_, 0, 10);

	if (ImGui::Button("Generate QR")) 
	{
		// QRコードエンコード
		ZXing::BitMatrix matrix = writer_.encode(text_, width_, height_);
		// BitMatrixをcv::Matに変換
		qrImage_ = cv::Mat(height_, width_, CV_8UC1);
		for (int y = 0; y < height_; ++y) {
			for (int x = 0; x < width_; ++x) {
				qrImage_.at<uchar>(y, x) = matrix.get(x, y) ? 0 : 255;
			}
		}
		// プレビューレンダリング
		if (!isWindowVisible("QR Preview"))
		{
			cv::namedWindow("QR Preview", cv::WINDOW_AUTOSIZE);
		}
		cv::imshow("QR Preview", qrImage_);
	}

	if (!qrImage_.empty()) 
	{
		if (ImGui::Button("Save QR Code")) 
		{
			// 保存
			std::string filename = "/" + text_ + ".png";
			std::string saveFilePath = savePath_ + filename;
			cv::imwrite(saveFilePath, qrImage_);
		}
	}
}

void QRExport::Shutdown() 
{
	// プレビューウィンドウを閉じる
	if (isWindowVisible("QR Preview"))
	{
		cv::destroyWindow("QR Preview");
	}
	qrImage_.release();
}


