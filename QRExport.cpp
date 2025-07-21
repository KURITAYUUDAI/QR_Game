#include "QRExport.h"

#include <ZXing/BarcodeFormat.h>
#include <filesystem>

#include <opencv2/imgcodecs.hpp>
#include <iostream>

#include <ZXing/qrcode/QRErrorCorrectionLevel.h>

using namespace KamataEngine;

// ここからバイナリかテキストでクラス分けするか、変数で分岐する形にするか
// Exportで全てやってしまうのではなく、CharacterDataの生成とチャンク化は別クラスで行うべきかもしれない

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

	editData_ = 
	{
	    1001, 10, 500, 75, 30, 2, {11, 22}
    };
}

void QRExport::Update()
{
	// テキストQR / バイナリQR モード切替フラグ
	static bool binaryMode = false;
	if (ImGui::Button(binaryMode ? "Switch Text QR mode" : "Switch Binary QR mode")) 
	{
		binaryMode = !binaryMode;
	}
	ImGui::Separator();

	if (!binaryMode) 
	{
		// --- テキスト QR ---

		// テキスト入力用に固定バッファを用意
		static char buf[512];
		strncpy_s(buf, text_.c_str(), sizeof(buf));
		ImGui::InputText("Text", buf, sizeof(buf));
		text_ = std::string(buf);

		ImGui::SliderInt("Width", &width_, 100, 1000);
		ImGui::SliderInt("Height", &height_, 100, 1000);
		ImGui::SliderInt("Scale", &scale_, 1, 20);
		ImGui::SliderInt("Border", &border_, 0, 10);

		if (ImGui::Button("Generate Text QR")) 
		{
			// QRコードエンコード
			ZXing::BitMatrix matrix = writer_.encode(text_, width_, height_);
			// BitMatrixをcv::Matに変換
			qrImage_ = cv::Mat(height_, width_, CV_8UC1);
			for (int y = 0; y < height_; ++y) 
			{
				for (int x = 0; x < width_; ++x) 
				{
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
			if (ImGui::Button("Save Text QR")) 
			{
				// 保存
				std::filesystem::create_directories(savePath_);
				std::string filename = "/" + text_ + ".png";
				std::string saveFilePath = savePath_ + filename;
				cv::imwrite(saveFilePath, qrImage_);
			}
		}
	} 
	else
	{
		// --- バイナリ QR ---
		
		// uint32_t の範囲
		static uint32_t minU32 = std::numeric_limits<uint32_t>::min();
		static uint32_t maxU32 = std::numeric_limits<uint32_t>::max();
		
		// uint8_t の範囲
		


		// 範囲定義
		static uint32_t minCharId = 0,	maxCharId = 10000;
		static uint8_t  minU8 = 0,		maxU8 = std::numeric_limits<uint8_t>::max();
		static uint16_t minU16 = 0,		maxU16 = std::numeric_limits<uint16_t>::max();
		static uint32_t minAtk = 0,		maxAtk = 1000; 
		static uint8_t  minSkills = 0,	maxSkills = 16;
		static uint16_t minSkillId = 0, maxSkillId = 32;
		
		// CharacterData の編集パネル
		// QRExport::Update() 内の編集パネル部分
		ImGui::Begin("Character Editor");
		
		DataInputSlider("Character ID (0 ~ 10000)", editData_, 
			&Character::CharacterData::characterId, minCharId, maxCharId);

		DataInputSlider("Level (0 ~ 255)", editData_, 
			&Character::CharacterData::level, minU8, maxU8);

		DataInputSlider("HP (0 ~ 65535)", editData_, 
			&Character::CharacterData::hp, minU16, maxU16);

		DataInputSlider("Attack (0 ~ 1000)", editData_, 
			&Character::CharacterData::attack, minAtk, maxAtk);

		DataInputSlider("Defense (0 ~ 1000)", editData_, 
			&Character::CharacterData::defense, minAtk, maxAtk);

		DataInputSlider("Num Skills (0 ~ 16)", editData_, 
			&Character::CharacterData::numSkills, minSkills, maxSkills);

		// numSkills が変わったら必ずリサイズ
		if (editData_.skillIds.size() != editData_.numSkills) 
		{
			editData_.skillIds.resize(editData_.numSkills);
		}

		// Skill ID 列 (uint16_t)
		if (editData_.numSkills > 0) 
		{
			ImGui::Indent();
			for (int i = 0; i < editData_.numSkills; ++i) 
			{
				ImGui::PushID(i);
				
				DataInputSlider(("Skill ID " + std::to_string(i + 1)).c_str(), 
					editData_.skillIds[i], minSkillId, maxSkillId);

				ImGui::PopID();
			}
			ImGui::Unindent();
		}

		ImGui::End();

		

		if (ImGui::Button("Generate Binary QR for CharacterData")) 
		{
			
			auto buf = Character::SerializeCharacterData(editData_);
			auto chunks = Character::MakeChunks(buf, /*maxPayload*/ 200);
			for (auto& c : chunks) 
			{
				qrImage_ = generateBinaryQR(c.payload);
				// 保存やプレビューは generateBinaryQR 内で実施
			}

			cv::imshow("Binary QR", qrImage_);
		}

		if (!qrImage_.empty()) 
		{
			if (ImGui::Button("Save Binary QR")) 
			{
				// 保存
				std::filesystem::create_directories(savePath_);
				std::string filename = std::string("/") + "sampleCharacterData" + ".png";
				std::string saveFilePath = savePath_ + filename;
				cv::imwrite(saveFilePath, qrImage_);
			}
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

cv::Mat QRExport::generateBinaryQR(const std::vector<uint8_t>& data) 
{
	// ヘッダ + payload をまとめる
	std::vector<uint8_t> buf;
	buf.reserve(2 + data.size());
	// index/total は payload 前提、ここ簡易サンプルなら固定 0/1
	buf.push_back(0);
	buf.push_back(1);
	buf.insert(buf.end(), data.begin(), data.end());

	 // ──── 2) 生バイト列を std::wstring に変換 ────
	//    (MultiFormatWriter の .encode() は std::wstring 版を持っています)
	std::wstring wraw;
	wraw.reserve(buf.size());
	for (auto b : buf)
		wraw.push_back(static_cast<wchar_t>(b));

	 // ──── 3) ライターにオプションをセット ────
	// border_ は GUI で設定した余白
	writer_.setMargin(border_);
	// 誤り訂正レベル（例として L）
	writer_.setEccLevel(ZXing::QRCode::BitsFromECLevel(ZXing::QRCode::ErrorCorrectionLevel::Low));
	// バイトモードで埋め込み
	writer_.setEncoding(ZXing::CharacterSet::BINARY);

	// ──── 4) エンコード（幅・高さだけを渡す） ────
	auto matrix = writer_.encode(wraw, width_, height_);

	// ──── 5) BitMatrix → OpenCV Mat ────
	cv::Mat img(height_, width_, CV_8UC1);
	for (int y = 0; y < height_; ++y)
		for (int x = 0; x < width_; ++x)
			img.at<uchar>(y, x) = matrix.get(x, y) ? 0 : 255;
	
	return img;
}

template<typename T> 
void QRExport::DataInputSlider(
	const char* label, Character::CharacterData& editData, 
	T Character::CharacterData::* member, T minVal, T maxVal)
{
	// メンバーアクセスして参照版にフォワード
	DataInputSlider(label, editData.*member, minVal, maxVal);
}


template<typename T>
void QRExport::DataInputSlider(const char* label, T& value, T minVal, T maxVal) 
{
	std::string sliderId = std::string("##") + label + "Slider";
	std::string inputId = std::string("##") + label + "Input";

	ImGui::Text("%s", label);
	ImGui::SliderScalar(sliderId.c_str(), ImGuiDT<T>(), &value, &minVal, &maxVal, "%u");
	ImGui::SameLine();
	ImGui::PushItemWidth(60);
	if (ImGui::InputScalar(inputId.c_str(), ImGuiDT<T>(), 
		&value, nullptr, nullptr, "%u", ImGuiInputTextFlags_CharsDecimal)) 
	{
		value = value < minVal ? minVal 
			: value > maxVal ? maxVal 
			: value;
	}
	ImGui::PopItemWidth();
}