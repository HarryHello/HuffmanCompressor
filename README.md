## 程序功能
本程序实现了一个完整的基于Huffman编码的压缩程序，包含压缩、解压缩功能。

## 程序运行展示
参数不完整时的输出: 
```plaintext
Usage:
  .\Compressor.exe -c <input_file> [<output_file>]
  .\Compressor.exe -d <input_file>  <output_file>
```

正确参数: 
```plaintext
# 压缩
.\Compressor.exe -c cameraman.tif
Compressed 262750 bytes to 233595bytes.
Compression ratio: 88.9039%
Compress time: 8575us
Full time: 10ms

# 解压缩
.\Compressor.exe -d cameraman.tif.huff output.tif
Decompressed to 262750 bytes.
Decompress time: 8328us
Full time: 10ms
```

## 程序运行方式
该程序通过在命令行中用参数指定操作模式（压缩或解压缩）、输入文件路径和可选的输出文件路径来运行。
- 压缩模式：`.\Compressor.exe -c <input_file> [<output_file>]`
  - `<input_file>`：要压缩的输入文件路径。
  - `[<output_file>]`：可选的输出文件路径，默认输出为 `<input_file>.huff`。
- 解压缩模式：`.\Compressor.exe -d <input_file> <output_file>`
  - `<input_file>`：要解压缩的输入文件路径。
  - `<output_file>`：解压缩后的输出文件路径。