#include <Windows.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <map>
#include <filesystem>

void DeModifyCompressedFile(const std::string& filename) {
    std::filesystem::path compressed_file(filename);
    std::filesystem::path modified_bits_file("modified_bits.txt");
  // Open modified bits file
  std::ifstream txt_file((compressed_file.parent_path() / modified_bits_file).string());
  if (!txt_file.is_open()) {
    std::cout << "Failed to open modified bits file" << std::endl;
    return;
  }

  // Read modified bits
  std::map<int, std::uint8_t> modified_bits;
  std::string line;
  while (std::getline(txt_file, line)) {
    int pos;
    std::uint16_t value;
    std::istringstream iss(line);
    iss >> std::hex >> pos >> value;
    modified_bits[pos] = static_cast<std::uint8_t>(value);
  }
  txt_file.close();

  // Open compressed file
  std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary);
  if (!file.is_open()) {
    std::cout << "Failed to open file " << filename << std::endl;
    return;
  }

  // Get file size
  file.seekg(0, std::ios::end);
  const std::streamoff file_size = file.tellg();
  file.seekg(0, std::ios::beg);

  // Write modified bytes back to file
  for (const auto& it : modified_bits) {
    const int pos = it.first;
    const std::uint8_t value = it.second;
    if (pos >= 0 && pos < file_size) {
      file.seekp(pos);
      file.write(reinterpret_cast<const char*>(&value), 1);
    }
  }

  file.close();
}

bool DecompressWith7zip(const std::string& compressed_file_path, const std::string& password) {

    // 截取文件名
    std::string output_dir = compressed_file_path.substr(0, compressed_file_path.size() - 3);

    // 构造7zip命令行参数
    std::string cmdLine = "7z x \"" + compressed_file_path + "\" -mhe=on -t7z -m0=lzma2 -mx=9 -mfb=64 -md=32m -ms=on -p\"" + password + "\" -o\"" + output_dir + "\"";

    // 启动7zip进程
    STARTUPINFOA startupInfo = {0};
    PROCESS_INFORMATION processInfo = {0};
    if (!CreateProcessA(NULL, (LPSTR)cmdLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo))
    {
        std::cerr << "Error: Failed to create 7zip process. Error code: " << GetLastError() << std::endl;
        return false;
    }

    // 等待进程完成
    WaitForSingleObject(processInfo.hProcess, INFINITE);

    // 检查进程的退出代码
    DWORD exitCode = 0;
    if (!GetExitCodeProcess(processInfo.hProcess, &exitCode))
    {
        std::cerr << "Error: Failed to get exit code of 7zip process. Error code: " << GetLastError() << std::endl;
        return false;
    }

    // 清理进程句柄
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    // 检查进程的退出代码是否为0
    if (exitCode != 0)
    {
        std::cerr << "Error: 7zip process exited with non-zero exit code: " << exitCode << std::endl;
        return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    // 检查参数数量
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <7zfile>" << std::endl;
        return 1;
    }

    std::string file_path = argv[1];

    // 判断输入的文件名是否以".7z"结尾
    if (file_path.size() < 4 || file_path.substr(file_path.size() - 3) != ".7z")
    {
        std::cout << "The input file is not a .7z compressed file!" << std::endl;
        return 1;
    }

    // 弹出输入框要求输入密码
    std::string password;
    std::cout << "Please enter password for 7z archive: ";
    std::cin >> password;

    DeModifyCompressedFile(file_path);

    // 调用CompressWith7zip函数进行压缩
    if (DecompressWith7zip(file_path, password))
    {
        std::cout << "Decompression successful." << std::endl;
        return 0;
    }
    else
    {
        std::cerr << "Compression failed." << std::endl;
        return 1;
    }
}