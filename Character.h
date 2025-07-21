#pragma once
#define NOMINMAX
#include "KamataEngine.h"

class Character
{
public:

	struct CharacterData
	{
		uint32_t characterId;		// キャラクターID
		uint8_t level;				// レベル
		uint16_t hp;				// 体力
		uint32_t attack;			// 攻撃力
		uint32_t defense;			// 防御力
		// 習得技IDリスト（可変長は末尾に長さ＋ID列を入れる）
		uint8_t numSkills;
		std::vector<uint16_t> skillIds;
	};

	struct Chunk 
	{
		uint8_t index;
		uint8_t total;
		std::vector<uint8_t> payload;
	};

	// リトルエンディアンで整数を buf に追加するユーティリティ
	template<typename T>
	static void appendLE(std::vector<uint8_t>& buf, T value) 
	{
		for (size_t i = 0; i < sizeof(T); ++i) 
		{
			buf.push_back(static_cast<uint8_t>((value >> (8 * i)) & 0xFF));
		}
	}

	static std::vector<uint8_t> SerializeCharacterData(const CharacterData& data);

	static CharacterData DeserializeCharacterData(const std::vector<uint8_t>& buf);

	static std::vector<Chunk> MakeChunks(const std::vector<uint8_t>& data, size_t maxPayload);
};

