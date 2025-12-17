# Huffman Compressor
这是一个课程项目，实现了基于Huffman编码的文件压缩与解压缩程序。

## 程序功能
本程序实现了一个完整的基于Huffman编码的压缩程序，包含压缩、解压缩功能。

## 程序运行展示
参数不完整时的输出: 
```plaintext
Usage:
  .\Compressor.exe -c <input_file> [<output_file>]
  .\Compressor.exe -d <input_file>  <output_file>
```

正确参数示例: 
```plaintext
# 压缩示例
.\Compressor.exe -c cameraman.tif
Compressed 262750 bytes to 233595 bytes.
Compression ratio: 88.9039%
Compress time: 8575us
Full time: 10ms

# 解压缩示例
.\Compressor.exe -d cameraman.tif.huff output.tif
Decompressed to 262750 bytes.
Decompress time: 8328us
Full time: 10ms
```

## 程序运行方式
该程序通过在命令行中用参数指定操作模式（压缩或解压缩）、输入文件路径和可选的输出文件路径来运行。

### 支持的模式选项
- **压缩模式**：`-c`, `-compress`, `--compress`
- **解压缩模式**：`-d`, `-decompress`, `--decompress`

### 命令行用法
- **压缩**：`.\Compressor.exe -c <input_file> [<output_file>]`
  - `<input_file>`：要压缩的输入文件路径。
  - `[<output_file>]`：可选的输出文件路径，默认输出为 `<input_file>.huff`。

- **解压缩**：`.\Compressor.exe -d <input_file> <output_file>`
  - `<input_file>`：要解压缩的输入文件路径。
  - `<output_file>`：解压缩后的输出文件路径（必须指定）。

### 输出信息说明
- 压缩前后的字节数
- 压缩率（压缩后大小占压缩前大小的百分比）
- 压缩/解压缩操作所用时间（微秒）
- 程序总运行时间（毫秒）

## 代码结构
```
HuffmanCompressor/
├── include/           # 头文件目录
│   ├── bitReader.hpp  # 位读取工具类，用于从文件中读取位流
│   ├── bitWriter.hpp  # 位写入工具类，用于向文件中写入位流
│   ├── fileIO.hpp     # 文件读写工具类，提供文件的读写操作
│   └── huffman.hpp    # Huffman编码核心类，定义压缩和解压缩接口
├── src/               # 源代码目录
│   ├── huffman.cpp    # Huffman编码实现，包含压缩和解压缩的具体算法
│   └── main.cpp       # 程序入口和命令行处理，解析参数并调用相应功能
├── CMakeLists.txt     # CMake构建配置文件
├── .gitignore         # Git忽略文件配置
└── README.md          # 项目说明文档
```

## 技术特点
- 基于Huffman编码算法实现高效压缩
- 支持位级别的读写操作，提高压缩效率
- 实时显示压缩率和处理时间
- 完善的错误处理机制

这是一个课程项目，用于学习和实践数据压缩算法。