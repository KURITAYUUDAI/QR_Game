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

	// バイナリ生成用
	cv::Mat GenerateBinaryQR(const std::vector<uint8_t>& data);

	// ユーティリティ
	template<typename T> 
	void appendLE(std::vector<uint8_t>& buf, T value) 
	{
		for (size_t i = 0; i < sizeof(T); ++i)
		{
			buf.push_back(static_cast<uint8_t>((value >> (8 * i)) & 0xFF));
		}
	}

	template<typename U> constexpr ImGuiDataType ImGuiDT();

	// uint8_t 用
	template<> constexpr ImGuiDataType ImGuiDT<uint8_t>() { return ImGuiDataType_U8; }

	// uint16_t 用
	template<> constexpr ImGuiDataType ImGuiDT<uint16_t>() { return ImGuiDataType_U16; }

	// uint32_t 用
	template<> constexpr ImGuiDataType ImGuiDT<uint32_t>() { return ImGuiDataType_U32; }
	
	// ImGui表示簡略化関数
	template<typename T> 
	void DataInputSlider(
		const char* label, Character::CharacterData& editData, 
		T Character::CharacterData::* member, T minVal, T maxVal);

	template<typename T> 
	void DataInputSlider(const char* label, T& value, T minVal, T maxVal);

	cv::Mat BitMatrixToMonoMat(const ZXing::BitMatrix& matrix);

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

	
	std::string redText_ = "RedQR";
	std::string greenText_ = "GreenQR";
	std::string blueText_ = "BlueQR";

	cv::Mat rgbQRImage_;
	
	static constexpr int MAX_TEXT_LENGTH = 64 * 1024; // テキスト入力の最大長
	char textBuf_[MAX_TEXT_LENGTH] = "";              // テキスト入力用バッファ
	


	// 設定用キャラデータ
	Character::CharacterData editData_;

};
