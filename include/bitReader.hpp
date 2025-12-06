// bitReader.hpp

#pragma once

#include <cstdint>

// 用于按位读取的辅助类
class BitReader
{
    const uint8_t* data_;  // 数据起始指针
    size_t size_;          // 数据总共字节数
    size_t byte_pos_ = 0;  // 当前已读字节数
    uint8_t bit_pos_ = 0;  // 当前字节内已读位数，0~7

public:
    BitReader(const uint8_t* data, size_t size) : data_(data), size_(size) {}

    // 读取一个字节
    bool readBit()
    {
        if (byte_pos_ >= size_) throw std::runtime_error("EOF");
        bool bit = (data_[byte_pos_] >> (7 - bit_pos_)) & 1;
        if (++bit_pos_ == 8)
        {
            bit_pos_ = 0;
            ++byte_pos_;
        }
        return bit;
    }
};