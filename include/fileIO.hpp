// fileIO.hpp

#pragma once

#include <fstream>
#include <vector>

// 用于处理文件读写的辅助命名空间
namespace fileIO
{
	// 按位写入文件
	void write(const std::vector<uint8_t>& data, const std::string& filename)
	{
		std::ofstream file(filename, std::ios::binary);
		if (!file)
		{
			throw std::runtime_error("Fail to open file: " + filename);
		}

		file.write(reinterpret_cast<const char*>(data.data()), data.size());

		if (!file)
		{
			throw std::runtime_error("Fail to write into file: " + filename);
		}
	}

	// 按位读取文件
	std::vector<uint8_t> read(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::binary | std::ios::ate);
		if (!file)
		{
			throw std::runtime_error("Fail to open file: " + filename);
		}

		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<uint8_t> buffer(size);
		if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
		{
			throw std::runtime_error("Fail to read file: " + filename);
		}

		return buffer;
	}

} // namespace fileIO