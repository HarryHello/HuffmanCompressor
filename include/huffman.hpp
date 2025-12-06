// huffman.hpp

#pragma once

#include <vector>
#include <cstdint>

// ¹ş·òÂü±àÂëÀà
class HuffmanCodec
{
public:
	// Ñ¹Ëõº¯Êı
	static std::vector<uint8_t> compress(const std::vector<uint8_t>& input);

	// ½âÑ¹Ëõº¯Êı
	static std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressed);
};