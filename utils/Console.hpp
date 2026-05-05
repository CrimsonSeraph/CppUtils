/*
 * Copyright (c) 2026 CrimsonSeraph(ltyy.leoyu@gmail.com)
 * SPDX-License-Identifier: MIT
 */

#pragma once

 // 标准库
#include <iostream>

#ifdef _WIN32
#include <cstdio>
#include <cwchar>
#include <windows.h>
#endif

// ============================================
// Console - 控制台管理类
// ============================================
class Console {
public:
    // -------------------- 单例 --------------------
    /// @brief 获取单例实例
    static Console& get_instance();

    // -------------------- 公共接口 --------------------
    /// @brief 创建控制台（Windows 上实际创建，其他平台返回 false）
    /// @return 成功返回 true
    bool create();

    /// @brief 销毁控制台（Windows 上释放，其他平台无操作）
    void destroy();

    /// @brief 检查控制台是否已创建
    bool is_created() const;

    // 禁止拷贝
    Console(const Console&) = delete;
    Console& operator=(const Console&) = delete;

private:
    // -------------------- 构造/析构（单例私有）--------------------
    Console() = default;
    ~Console() { destroy(); }

    // -------------------- 成员变量 --------------------
    bool is_created_ = false;   ///< 控制台是否已创建

#ifdef _WIN32
    // -------------------- 私有辅助函数（Windows 专用）--------------------
    /// @brief Windows 平台实际创建调试控制台
    /// @return 成功返回 true
    bool create_debug_console();
#endif
};

// ============================================
// 单例实现
// ============================================
inline Console& Console::get_instance() {
    static Console instance;
    return instance;
}

// ============================================
// 公共接口实现
// ============================================
inline bool Console::create() {
    if (is_created_) {
        return true;
    }

#ifdef _WIN32
    is_created_ = create_debug_console();
#else
    std::cerr << "[Console] Create [WARN] 当前操作系统不支持控制台输出！" << std::endl;
    is_created_ = false;
#endif
    return is_created_;
}

inline void Console::destroy() {
#ifdef _WIN32
    if (is_created_) {
        FreeConsole();
        is_created_ = false;
    }
#else
    is_created_ = false;
#endif
}

inline bool Console::is_created() const {
    return is_created_;
}

// ============================================
// 私有辅助函数实现
// ============================================
#ifdef _WIN32
inline bool Console::create_debug_console() {
    // 分配控制台
    if (!AllocConsole()) {
        DWORD error = GetLastError();
        // 如果已经分配了控制台，这也是成功的
        if (error == ERROR_ACCESS_DENIED) {
            if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
                return false;
            }
        }
        else {
            return false;
        }
    }

    // 获取标准输出句柄
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hError = GetStdHandle(STD_ERROR_HANDLE);

    if (hOutput == INVALID_HANDLE_VALUE ||
        hInput == INVALID_HANDLE_VALUE ||
        hError == INVALID_HANDLE_VALUE) {
        return false;
    }

    // 设置控制台代码页为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // 重定向标准流到控制台
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);

    // 同步 C++ 标准流与 C 流
    std::ios::sync_with_stdio(true);

    // 清除流状态
    std::cout.clear();
    std::cerr.clear();
    std::cin.clear();

    // 设置控制台字体
    CONSOLE_FONT_INFOEX cf = { 0 };
    cf.cbSize = sizeof(cf);
    cf.dwFontSize.Y = 14;

    const wchar_t* fontNames[] = {
        L"Consolas",
        L"Lucida Console",
        L"DejaVu Sans Mono",
        L"MS Gothic"
    };

    for (const wchar_t* fontName : fontNames) {
        wcscpy_s(cf.FaceName, fontName);
        if (SetCurrentConsoleFontEx(hOutput, FALSE, &cf)) {
            break;
        }
    }

    // 启用虚拟终端处理
    DWORD dwMode = 0;
    if (GetConsoleMode(hOutput, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOutput, dwMode);
    }

    SetConsoleTitleW(L"Debug Console");
    return true;
}
#endif  // _WIN32
