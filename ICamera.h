#pragma once
#include <opencv2/opencv.hpp>

class ICamera
{
public:
	virtual ~ICamera() {}
	virtual void Initialize() = 0; // 起動時処理
	virtual void Update() = 0;     // 毎フレーム更新（必要なら）
	virtual void Shutdown() = 0;   // 終了時処理

	virtual cv::Mat GetFrame() const = 0;

};