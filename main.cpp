#include <windows.h>
#include <iostream>
#include <vector>

bool CompressWith7zip(std::vector<std::string> filenames, std::string password)
{
    // 构造7zip命令行参数
    std::string cmdLine = "7z a -p" + password + " -mhe=on -t7z -m0=lzma2 -mx=9 -mfb=64 -md=32m -ms=on archive.7z";

    // 添加所有文件名到命令行参数中
    for (const auto& filename : filenames)
    {
        cmdLine += " \"" + filename + "\"";
    }

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
        std::cout << "Usage: " << argv[0] << " <file1> <file2> ... <fileN>" << std::endl;
        return 1;
    }

    // 读取文件名并保存到vector中
    std::vector<std::string> filenames;
    for (int i = 1; i < argc; ++i)
    {
        filenames.push_back(argv[i]);
    }

    // 弹出输入框要求输入密码
    std::string password;
    std::cout << "Please enter password for 7z archive: ";
    std::cin >> password;

    // 调用CompressWith7zip函数进行压缩
    if (CompressWith7zip(filenames, password))
    {
        std::cout << "Compression successful." << std::endl;
        return 0;
    }
    else
    {
        std::cerr << "Compression failed." << std::endl;
        return 1;
    }
}
