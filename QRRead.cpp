#include "QRRead.h"

#include "PCCamera.h"
#include "CellPhoneCamera.h"


#include <opencv2/highgui.hpp>

#include <ZXing/ReaderOptions.h>	// ReaderOptions
#include <ZXing/BarcodeFormat.h>	// BarcodeFormat::QrCode
#include <ZXing/ImageView.h>		// ImageView

using namespace KamataEngine;

QRRead::QRRead()
{ 
	ChangeCameraMode(CaptureMode::Wait); 
}

void QRRead::Initialize() 
{
	cameraModeIndex_ = int(CaptureMode::Wait);
	ChangeCameraMode(CaptureMode(cameraModeIndex_));

	if (currentCamera_) 
	{
		currentCamera_->Shutdown();
	}

	// ZXing のオプション設定
	options_.setTryHarder(true);
	options_.setFormats(ZXing::BarcodeFormat::QRCode);
}

void QRRead::Update() 
{
	// 現在機能の更新処理

	// テキストQR / バイナリQR モード切替フラグ
	if (ImGui::Button(binaryMode_ ? "Switch Text QR mode" : "Switch Binary QR mode")) 
	{
		binaryMode_ = !binaryMode_;
		// バッファリセット
		chunkBuffer_.clear();
		expectedTotal_ = 0;
	}
	ImGui::Separator();

	// 動作モード選択（待機／キャプチャ／ファイル）
	const char* opItems[] = {"Wait", "Capture", "LoadFile"};
	const char* capItems[] = {"Wait", "PC_Camera", "CellPhone_Camera"};
	int opIndex = static_cast<int>(operationMode_);
	int capIndex = static_cast<int>(captureMode_);
	if (ImGui::Combo("Mode", &opIndex, opItems, IM_ARRAYSIZE(opItems))) 
	{
		operationMode_ = static_cast<OperationMode>(opIndex);
		// モード切り替え時にはバッファ初期化
		chunkBuffer_.clear();
		expectedTotal_ = 0;
		if (operationMode_ == OperationMode::Capture) 
		{
			// キャプチャモードに切り替えたらカメラモードも初期化
			captureMode_ = CaptureMode::PC_Camera;
			capIndex = static_cast<int>(captureMode_);
			cameraModeIndex_ = static_cast<int>(captureMode_);
			ChangeCameraMode(captureMode_);
		} 
		else
		{
			captureMode_ = CaptureMode::Wait;
			cameraModeIndex_ = static_cast<int>(captureMode_);
			ChangeCameraMode(captureMode_);
		}
	}

	if (ImGui::Checkbox("Enable RGB QR mode", &isRGB_)) 
	{
		qrCodeText_.clear();
		rgbCodeText_ = {"", "", ""};
	}
	ImGui::Separator();


	switch (operationMode_)
	{
	case OperationMode::Wait:
		
		break;

	case OperationMode::Capture:
		
		
		if (ImGui::Combo("Camera Type", &capIndex, capItems, IM_ARRAYSIZE(capItems))) 
		{
			captureMode_ = static_cast<CaptureMode>(capIndex);
			ChangeCameraMode(captureMode_);
		}
		if (currentCamera_)
		{
			// カメラ更新＆フレーム取得
			currentCamera_->Update();
			frame_ = currentCamera_->GetFrame();
		}
		
		break;

	case OperationMode::LoadFile:

		static char buf[256] = "";
		ImGui::InputText("File Path", buf, sizeof(buf));
		ImGui::SameLine();
		if (ImGui::Button("Load")) {
			filePath_ = buf;
			frame_ = cv::imread(filePath_);
		}

		break;

	}

	// フレームがあればデコードに回す
	if (frame_.empty())
	{
		return;
	}

	ZXing::Barcode result;
	std::vector<ZXing::Barcode> rgbResults_(3);

	if (isRGB_) 
	{
		std::vector<cv::Mat> channels;
		cv::split(frame_, channels); // [B, G, R]

		for (int i = 0; i < 3; ++i) 
		{
			ZXing::ImageView iv(channels[i].data, channels[i].cols, channels[i].rows, ZXing::ImageFormat::Lum);
			rgbResults_[i] = ZXing::ReadBarcode(iv, options_);
			
			if (rgbResults_[i].isValid())
			{
				rgbCodeText_[i] = rgbResults_[i].text();
			}
		}
	} 
	else 
	{
		// 通常のグレースケール処理
		cv::Mat gray;
		cv::cvtColor(frame_, gray, cv::COLOR_BGR2GRAY);
		if (binaryMode_) 
		{
			cv::bitwise_not(gray, gray);
		}

		ZXing::ImageView iv(gray.data, gray.cols, gray.rows, ZXing::ImageFormat::Lum);
		result = ZXing::ReadBarcode(iv, options_);

		if (result.isValid()) 
		{
			if (binaryMode_) 
			{
				BinaryResult(result);
			} 
			else 
			{
				qrCodeText_ = result.text();
			}
		}

		ImGui::Separator();
	}

	// バイナリモードでは白黒反転
	// ZXing の仕様に合わせて、白背景のQRコードを黒背景に変換
	// これにより、QRコードのデコードが正しく行えるようになる
	// (ZXingは白背景のQRコードを想定しているため)
	ImGui::Separator();

	if (isRGB_) 
	{
		ImGui::Text("Red QR   : %s", rgbCodeText_[2].c_str());
		ImGui::Text("Green QR : %s", rgbCodeText_[1].c_str());
		ImGui::Text("Blue QR  : %s", rgbCodeText_[0].c_str());

		if (ImGui::Button("Reconstruct Base64"))
		{
			// チャネル順： 
			// parts[0]=R → rgbCodeText_[2], 
			// parts[1]=G → rgbCodeText_[1], 
			// parts[2]=B → rgbCodeText_[0]
			std::string concat = rgbCodeText_[2] + rgbCodeText_[1] + rgbCodeText_[0];
			
			// バッファにコピー（安全にトランケート）
			strncpy_s(base64Buf_, concat.c_str(), sizeof(base64Buf_));
		}

		// 読み取り専用で長文表示
		ImGui::InputTextMultiline("Base64 Output", base64Buf_, sizeof(base64Buf_), 
								  ImVec2(-1, 120), 
								  ImGuiInputTextFlags_ReadOnly);
	} 
	else 
	{
		if (!binaryMode_) 
		{
			ImGui::Text("%s", qrCodeText_.c_str());
		} 
		else 
		{
			ImGui::Text("=== Character Data ===");
			ImGui::Text("ID        : %u", lastData_.characterId);
			ImGui::Text("Level     : %u", lastData_.level);
			ImGui::Text("HP        : %u", lastData_.hp);
			ImGui::Text("Attack    : %u", lastData_.attack);
			ImGui::Text("Defense   : %u", lastData_.defense);
			ImGui::Text("NumSkills : %u", lastData_.numSkills);
		}
		if (lastData_.numSkills > 0) 
		{
			ImGui::Text("Skill IDs :");
			ImGui::Indent();
			for (size_t i = 0; i < lastData_.skillIds.size(); ++i) 
			{
				ImGui::BulletText("%u", lastData_.skillIds[i]);
			}
			ImGui::Unindent();
		}
	}

	// キャプチャ使用時にフレームがあれば描画する
	if (operationMode_ == OperationMode::Capture && currentCamera_) 
	{
		if (isRGB_)
		{
			for (int i = 0; i < 3; ++i) 
			{
				if (rgbResults_[i].isValid())
				{
					if (operationMode_ == OperationMode::Capture) 
					{
						// カメラデバイスのシャットダウン
						if (currentCamera_) 
						{
							currentCamera_->Shutdown();
							currentCamera_.reset(); // 所有権を破棄
						}
						// UI上も「待機モード」に戻す
						captureMode_ = CaptureMode::Wait;
						cv::destroyAllWindows();
					}
				}
			}
		} 
		else
		{
			if (result.isValid()) {
				//// バウンディングポリゴンを描く
				// auto pos = result.position(); // vector<PointF>
				// for (size_t i = 0; i < pos.size(); ++i)
				//{
				//	const auto& p1 = pos[i];
				//	const auto& p2 = pos[(i + 1) % pos.size()];
				//	cv::line(frame_,
				//		cv::Point(int(p1.x), int(p1.y)),
				//		cv::Point(int(p2.x), int(p2.y)),
				//		cv::Scalar(255, 0, 0), 2);
				// }

				if (operationMode_ == OperationMode::Capture) {
					// カメラデバイスのシャットダウン
					if (currentCamera_) {
						currentCamera_->Shutdown();
						currentCamera_.reset(); // 所有権を破棄
					}
					// UI上も「待機モード」に戻す
					captureMode_ = CaptureMode::Wait;
					cv::destroyAllWindows();
				}
			}
		}

		if (captureMode_ == CaptureMode::Wait)
		{
			return;
		}

		// OpenCV のウィンドウに表示
		cv::imshow("Preview", frame_);
		// ウィンドウのイベント処理＆キー入力待ち（1ms）
		if (Input::GetInstance()->PushKey(DIK_F1)) 
		{
			// ESCキーが押されたらウィンドウを閉じる
			cv::destroyAllWindows();
			return;
		}
	}
}

void QRRead::Shutdown() 
{
	// メンバ変数への代入処理
	ChangeCameraMode(CaptureMode::Wait);

	if (currentCamera_) 
	{
		currentCamera_->Shutdown();
	}
}

void QRRead::ChangeCameraMode(CaptureMode mode)
{
	// 現在のモードを終了し内部をリセットする
	if (currentCamera_) 
	{
		currentCamera_->Shutdown();
		currentCamera_.reset();
	}

	// 選択されたモードをcurrent_に代入する
	switch (mode) 
	{

	case CaptureMode::Wait:
		currentCamera_ = nullptr; // 何もしない
		break;

	case CaptureMode::PC_Camera:
		currentCamera_ = std::make_unique<PCCamera>();
		break;

	case CaptureMode::CellPhone_Camera:
		currentCamera_ = std::make_unique<CellPhoneCamera>();
		break;
	}

	// current_がnullptrでない場合は初期化を行う
	if (!currentCamera_) 
	{
		captureMode_ = mode;
		return;
	}
	currentCamera_->Initialize();
	captureMode_ = mode;
}

void QRRead::BinaryResult(const ZXing::Result& result)
{
	ZXing::ByteArray raw = result.bytes(); // ZXing::ByteArray
	if (raw.empty())
		return; // バイトモードでない場合はスキップ

	// バイト列を vector<uint8_t> にコピー
	std::vector<uint8_t> data(raw.begin(), raw.end());

	// チャンクヘッダ取得
	uint8_t idx = data[0];
	uint8_t total = data[1];
	if (!expectedTotal_)
	{
		expectedTotal_ = total;
	}

	// ペイロード部分だけを蓄積
	std::vector<uint8_t> payload(data.begin() + 2, data.end());
	chunkBuffer_[idx] = std::move(payload);

	// すべて揃ったら再構築 & デシリアライズ
	if (chunkBuffer_.size() == expectedTotal_) 
	{
		std::vector<uint8_t> fullData;
		fullData.reserve(expectedTotal_ * 200); // maxPayload 分

		for (uint8_t i = 0; i < expectedTotal_; ++i) 
		{
			auto& chunk = chunkBuffer_[i];
			fullData.insert(fullData.end(), chunk.begin(), chunk.end());
		}

		// 読み取ったデータを復元して受け渡し
		lastData_ = Character::DeserializeCharacterData(fullData);
		hasData_ = true; // ここで表示許可

		// バッファリセット（次の読み取りに備えて）
		chunkBuffer_.clear();
		expectedTotal_ = 0;
	}
}