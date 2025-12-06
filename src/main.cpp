#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include "huffman.hpp"
#include "fileIO.hpp"

int main(int argc, char* argv[])
{
	// 若参数过长或过短则输出用法
	if (argc != 3 && argc != 4)
	{
		std::cerr << "Usage:\n"
				  << "  " << argv[0] << " -c <input_file> [<output_file>]\n"
			      << "  " << argv[0] << " -d <input_file>  <output_file> \n";
		return 1;
	}

	// 计算完整程序运行时间
	auto start_time = std::chrono::steady_clock::now();
	auto end_time = start_time;

	// 单独计算压缩/解压缩时间
	auto compress_start_time = start_time;
	auto compress_end_time = start_time;

	// 模式 -c -d
	std::string mode(argv[1]);
	// 输入文件
	std::string input_file(argv[2]);
	// 输出文件，若为压缩则可以自动添加后缀
	std::string output_file = argc == 4 ? argv[3] : (input_file + ".huff");

	try
	{
		// 压缩模式
		if (mode == "-c" || mode == "-compress" || mode == "--compress")
		{
			// 按字节读取任意文件
			std::vector<uint8_t> input = fileIO::read(input_file);

			compress_start_time = std::chrono::steady_clock::now();
			// 按字节压缩任意文件
			std::vector<uint8_t> compressed = HuffmanCodec::compress(input);
			compress_end_time = std::chrono::steady_clock::now();

			// 将压缩结果写入文件
			fileIO::write(compressed, output_file);

			// 输出压缩前后字节和压缩率
			std::cout << "Compressed " << input.size() << " bytes to "
				<< compressed.size() << "bytes.\n"
				<< "Compression ratio: "
				<< 100.0 * (static_cast<double>(compressed.size()) / input.size())
				<< "%\n";

			// 压缩时间，单位为微秒
			auto us = std::chrono::duration_cast<std::chrono::microseconds> (compress_end_time - compress_start_time);
			std::cout << "Compress time: " << us.count() << "us\n";
		}
		// 解压缩模式
		else if (mode == "-d" || mode == "-decompress" || mode == "--decompress")
		{
			// 需要显式地给出完整文件名，否则无法解析文件类型生成扩展名
			// 事实上文件类型并不影响解压缩，但解压后无法判断文件类型
			// 可以根据解压后的文件头来判断，没有实现
			if (argc != 4)
			{
				std::cerr << "You need to give the output filename explicitly.\n";
				return 1;
			}

			// 按字节读取压缩文件
			std::vector<uint8_t> compressed = fileIO::read(input_file);

			compress_start_time = std::chrono::steady_clock::now();
			// 解压文件
			std::vector<uint8_t> output = HuffmanCodec::decompress(compressed);
			compress_end_time = std::chrono::steady_clock::now();

			// 将解压结果写入文件
			fileIO::write(output, output_file);
			// 输出解压后的文件大小
			std::cout << "Decompressed to " << output.size() << " bytes.\n";

			// 输出解压缩花费的时间，单位为微秒
			auto us = std::chrono::duration_cast<std::chrono::microseconds> (compress_end_time - compress_start_time);
			std::cout << "Decompress time: " << us.count() << "us\n";
		}
		// 未知模式
		else
		{
			std::cerr << "Error: Unknown mode \'" << mode << "\' Use -c or -d.\n";
			return 1;
		}
		// 程序运行结束时间
		end_time = std::chrono::steady_clock::now();
		// 输出完整运行时间，单位为毫秒
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
		std::cout << "Full time: " << ms.count() << "ms\n";
	}
	// 运行时错误
	catch (const std::runtime_error& e)
	{
		std::cerr << "Runtime error: " << e.what() << "\n";
		return 1;
	}
	// 其他错误类型
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << "\n";
		return 1;
	}
	// 未知错误
	catch (...)
	{
		std::cerr << "Meet unknown error!\n";
		return 1;
	}
}
