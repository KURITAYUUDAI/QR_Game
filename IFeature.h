#pragma once

class IFeature 
{
public:
	virtual ~IFeature() {}
	virtual void Initialize() = 0; // 起動時処理
	virtual void Update() = 0; // 毎フレーム更新（必要なら）
	virtual void Shutdown() = 0;   // 終了時処理
};