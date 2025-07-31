#define NOMINMAX
#include "Character.h"

std::vector<uint8_t> Character::SerializeCharacterData(const CharacterData& data) 
{
	std::vector<uint8_t> buf;
	// あらかじめ容量を確保しておくと realloc が減り高速化
	buf.reserve(
		  sizeof(data.characterId) 
		+ sizeof(data.level) 
		+ sizeof(data.hp) 
		+ sizeof(data.attack) 
		+ sizeof(data.defense) 
		+ sizeof(data.numSkills) 
		+ data.skillIds.size() * sizeof(uint16_t)
	);

	// 1. characterId (4 バイト)
	appendLE<uint32_t>(buf, data.characterId);

	// 2. level (1 バイト)
	appendLE<uint8_t>(buf, data.level);

	// 3. hp (2 バイト)
	appendLE<uint16_t>(buf, data.hp);

	// 4. attack (4 バイト)
	appendLE<uint32_t>(buf, data.attack);

	// 5. defense (4 バイト)
	appendLE<uint32_t>(buf, data.defense);

	// 6. numSkills (1 バイト)
	buf.push_back(data.numSkills);

	// 7. skillIds（固定長 kMaxSkills、未使用分は0）
	for (size_t i = 0; i < CharacterData::kMaxSkills; ++i) 
	{
		uint16_t id = (i < data.numSkills) ? data.skillIds[i] : 0;
		appendLE<uint16_t>(buf, id);
	}

	return buf;
}

Character::CharacterData Character::DeserializeCharacterData(const std::vector<uint8_t>& buf) 
{
	CharacterData data;
	size_t idx = 0;
	// characterId
	data.characterId = uint32_t(buf[idx]) 
					| (uint32_t(buf[idx + 1]) << 8) 
					| (uint32_t(buf[idx + 2]) << 16) 
					| (uint32_t(buf[idx + 3]) << 24);
	idx += 4;

	// level
	data.level = uint16_t(buf[idx]);
	idx++;

	// hp
	data.hp = uint16_t(buf[idx])
	       | (uint16_t(buf[idx + 1]) << 8);
	idx += 2;

	// attack
	data.attack = uint32_t(buf[idx]) 
			   | (uint32_t(buf[idx + 1]) << 8) 
			   | (uint32_t(buf[idx + 2]) << 16) 
			   | (uint32_t(buf[idx + 3]) << 24);
	idx += 4;

	// defense
	data.defense = uint32_t(buf[idx]) 
				| (uint32_t(buf[idx + 1]) << 8) 
				| (uint32_t(buf[idx + 2]) << 16) 
				| (uint32_t(buf[idx + 3]) << 24);
	idx += 4;

	// numSkills
	data.numSkills = buf[idx++];

	 // 7. skillIds（常に kMaxSkills 個読み込む）
	for (size_t i = 0; i < CharacterData::kMaxSkills; ++i) 
	{
		uint16_t id = uint16_t(buf[idx]) | (uint16_t(buf[idx + 1]) << 8);
		data.skillIds[i] = id;
		idx += 2;
	}

	return data;
}

std::vector<Character::Chunk> Character::MakeChunks(const std::vector<uint8_t>& data, size_t maxPayload)
{
	// 全チャンク数を計算
	size_t totalChunks = (data.size() + maxPayload - 1) / maxPayload;
	std::vector<Chunk> chunks;
	// 事前にデータ容量を確保しておく
	chunks.reserve(totalChunks);

	for (size_t i = 0; i < totalChunks; ++i) 
	{
		Chunk c;
		c.index = static_cast<uint8_t>(i);
		c.total = static_cast<uint8_t>(totalChunks);

		// データの切り出し位置と長さ
		size_t offset = i * maxPayload;
		size_t len = std::min(maxPayload, data.size() - offset);

		// ペイロードを設定
		c.payload.insert(c.payload.end(), data.begin() + offset, data.begin() + offset + len);

		chunks.push_back(std::move(c));
	}

	return chunks;
}