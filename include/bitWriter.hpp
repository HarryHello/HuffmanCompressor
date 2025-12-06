// bitWriter.hpp

#pragma once

#include <vector>
#include <cstdint>

// 用于按位写入的辅助类
class BitWriter
{
    std::vector<uint8_t> buffer_;
    uint8_t current_byte_ = 0;
    int bits_in_byte_ = 0;  // 0 ~ 7

public:
    // 写入单个比特（0 或 1）
    void writeBit(bool bit)
    {
        if (bit)
        {
            // 从高位开始：第 0 位（MSB）是 0x80 (10000000)
            current_byte_ |= (0x80 >> bits_in_byte_);
        }
        ++bits_in_byte_;
        if (bits_in_byte_ == 8)
        {
            flush();
        }
    }

    // 写入一个哈夫曼编码（高位对齐，已知长度）
    void writeBits(uint32_t code, uint8_t len)
    {
        for (uint8_t i = 0; i < len; ++i)
        {
            // 从最高位（bit 31）开始取
            bool bit = (code >> (31 - i)) & 1;
            writeBit(bit);
        }
    }

    // 完成写入，处理末尾不足 8 位的情况
    std::vector<uint8_t> finish()
    {
        if (bits_in_byte_ > 0)
        {
            // 末尾填充 0
            buffer_.push_back(current_byte_);
        }
        return std::move(buffer_);
    }

private:
    // 完成一个字节
    void flush()
    {
        buffer_.push_back(current_byte_);
        current_byte_ = 0;
        bits_in_byte_ = 0;
    }
};