// huffman.cpp

#include "huffman.hpp"
#include <unordered_map>
#include <memory>
#include <array>
#include <queue>
#include <functional>
#include <stdexcept>
#include "bitWriter.hpp"
#include "bitReader.hpp"

// 哈夫曼树节点
struct Node
{
	uint8_t symbol;  // 该节点实际值
	uint32_t freq;   // 该值频数

	std::unique_ptr<Node> left;   // 左子节点
	std::unique_ptr<Node> right;  // 右子节点

	// 根据节点值和频数生成树节点
	Node(uint8_t sym, uint32_t fre) : symbol(sym), freq(fre) {}

	// 由低频数节点合成的新节点
	Node(std::unique_ptr<Node>&& _left, std::unique_ptr<Node>&& _right) : symbol(0), 
		freq(_left->freq + _right->freq), left(std::move(_left)), right(std::move(_right)) { }
};

// 编码结构体
struct Code
{
	uint32_t bits;   // 实际编码，从高位开始
	uint8_t length;  // 编码长度， 1~32
};

// 建立哈夫曼树
std::unique_ptr<Node> buildHuffmanTree(const std::array<uint32_t, 256>& freq);

// 由哈夫曼树建立编码表
std::array<Code, 256> buildCodeTable(const Node& root);

// 序列化文件头
std::vector<uint8_t> serializeHeader(size_t original_size, const Node& root);

// 序列化哈夫曼树以存储在文件头
void serialTree(std::vector<uint8_t>& header, const Node& root);

// 由编码表对文件编码
std::vector<uint8_t> encodeData(const std::vector<uint8_t>& input, const std::array<Code, 256>& code_table);

// 从文件头重建哈夫曼树，用于解压缩
std::unique_ptr<Node> deserializeTree(const uint8_t*& p, const uint8_t*& end);

// 从压缩文件中解压出一个字节
uint8_t decodeSymbol(const Node& root, BitReader& reader);

std::vector<uint8_t> HuffmanCodec::compress(const std::vector<uint8_t>& input)
{
	if (input.empty())
	{
		return {};
	}

	std::array<uint32_t, 256> freq{0};

	for (const uint8_t& byte : input)
	{
		++freq[byte];
	}

	auto root = buildHuffmanTree(freq);
	
	if (!root)
	{
		return {};
	}

	std::array<Code, 256> code_table = buildCodeTable(*root);
	std::vector<uint8_t> header = serializeHeader(input.size(), *root);
	std::vector<uint8_t> data = encodeData(input, code_table);

	std::vector<uint8_t> result;
	result.reserve(header.size() + data.size());
	result.insert(result.end(), header.begin(), header.end());
	result.insert(result.end(), data.begin(), data.end());

	return result;
}

std::vector<uint8_t> HuffmanCodec::decompress(const std::vector<uint8_t>& compressed)
{
	uint32_t header_size = static_cast<uint32_t>(compressed[0]) +
						  (static_cast<uint32_t>(compressed[1]) << 8) +
						  (static_cast<uint32_t>(compressed[2]) << 16) +
						  (static_cast<uint32_t>(compressed[3]) << 24);
	uint32_t original_size = static_cast<uint32_t>(compressed[4]) +
							(static_cast<uint32_t>(compressed[5]) << 8) +
							(static_cast<uint32_t>(compressed[6]) << 16) +
							(static_cast<uint32_t>(compressed[7]) << 24);

	const uint8_t* ptr = compressed.data() + 8;
	std::unique_ptr<Node> root = deserializeTree(ptr, compressed.data() + header_size);

	std::vector<uint8_t> res;
	res.reserve(original_size);

	BitReader reader(ptr, original_size);

	for (uint32_t i = 0; i < original_size; ++i)
	{
		res.push_back(decodeSymbol(*root, reader));
	}
	return res;
}


std::unique_ptr<Node> buildHuffmanTree(const std::array<uint32_t, 256>& freq)
{
	auto cmp = [](const std::unique_ptr<Node>& a, const std::unique_ptr<Node>& b) {
		return a->freq > b->freq;
		};

	std::priority_queue<
						std::unique_ptr<Node>,
						std::vector<std::unique_ptr<Node>>,
						decltype(cmp)
					   > node_pq;

	for (int i = 0; i < 256; ++i)
	{
		if (freq[i] > 0)
		{
			node_pq.push(std::make_unique<Node>(i, freq[i]));
		}
	}

	while (node_pq.size() > 1)
	{
		std::unique_ptr<Node> left = std::move(const_cast<std::unique_ptr<Node>&>(node_pq.top()));
		node_pq.pop();
		std::unique_ptr<Node> right = std::move(const_cast<std::unique_ptr<Node>&>(node_pq.top()));
		node_pq.pop();
		node_pq.push(std::make_unique<Node>(std::move(left), std::move(right)));
	}

	std::unique_ptr<Node> root = std::move(const_cast<std::unique_ptr<Node>&>(node_pq.top()));
	return root;
}

std::array<Code, 256> buildCodeTable(const Node& root)
{
	std::array<Code, 256> code_table{0};

	// 特判：若哈夫曼树中只包含根节点
	if (root.left == nullptr && root.right == nullptr)
	{
		code_table[root.symbol] = Code{ 0, 1 };
		return std::array<Code, 256>();
	}
	
	// 深度优先遍历建立编码表
	std::function<void(uint32_t, uint8_t, const Node&)> dfsBuild = 
	[&code_table, &dfsBuild](uint32_t _cur_code, uint8_t _cur_length, const Node& _node)
	{
		if (_node.left == nullptr && _node.right == nullptr)
		{
			code_table[_node.symbol] = Code{_cur_code, _cur_length};
			return;
		}

		if (_cur_length >= 32)
		{
			throw std::runtime_error("Huffman code too long!");
		}

		if (_node.left)
		{
			dfsBuild(_cur_code, _cur_length + 1, *_node.left);
		}

		if (_node.right)
		{
			dfsBuild(_cur_code | (0x80000000U >> _cur_length), _cur_length + 1, *_node.right);
		}
	};

	dfsBuild(0x0000, 0x00, root);
	return code_table;
}

std::vector<uint8_t> serializeHeader(const size_t _original_size, const Node& root)
{
	std::vector<uint8_t> header;
	// 4 bytes : header_size
	// 4 bytes : original_size
	// variable: huffmantree

	// 为header_size预留位置
	header.resize(8, 0);

	// 原本文件的大小
	uint32_t original_size = _original_size;
	header[4] = original_size & 0xFF;
	header[5] = (original_size >> 8) & 0xFF;
	header[6] = (original_size >> 16) & 0xFF;
	header[7] = (original_size >> 24) & 0xFF;

	serialTree(header, root);

	// header的实际大小
	uint32_t header_size = header.size();
	header[0] = header_size & 0xFF;
	header[1] = (header_size >> 8) & 0xFF;
	header[2] = (header_size >> 16) & 0xFF;
	header[3] = (header_size >> 24) & 0xFF;

	return header;
}

void serialTree(std::vector<uint8_t>& header, const Node& root)
{
	if (root.left == nullptr && root.right == nullptr)
	{
		header.push_back(0x01);
		header.push_back(root.symbol);
		return;
	}

	// 哈夫曼树的非叶子节点一定同时拥有左右子节点，因此无需判断
	header.push_back(0x00);
	serialTree(header, *root.left);
	serialTree(header, *root.right);
}

std::unique_ptr<Node> deserializeTree(const uint8_t*& p, const uint8_t*& end)
{
	uint8_t flag = *p++;

	// 超出边界仍未结束，可以认为不是该程序压缩产生的文件
	if (p > end)
	{
		throw std::runtime_error("Incorrect format of file!");
	}

	// 叶子节点
	if (flag == 0x01)
	{
		uint8_t symbol = *p++;
		return std::make_unique<Node>(symbol, 0); // freq 任意
	}
	// 内部节点
	else
	{
		auto left = deserializeTree(p, end);
		auto right = deserializeTree(p, end);
		return std::make_unique<Node>(std::move(left), std::move(right));
	}
}

std::vector<uint8_t> encodeData(const std::vector<uint8_t>& input, const std::array<Code, 256>& code_table)
{
	BitWriter writer;

	// 按位写入
	for (const uint8_t& sym : input)
	{
		const Code& code = code_table[sym];
		writer.writeBits(code.bits, code.length);
	}

	return writer.finish();
}

uint8_t decodeSymbol(const Node& root, BitReader& reader)
{
	const Node* current = &root;
	while (current->left || current->right)
	{
		bool bit = reader.readBit();
		current = bit ? current->right.get() : current->left.get();
	}
	return current->symbol;
}