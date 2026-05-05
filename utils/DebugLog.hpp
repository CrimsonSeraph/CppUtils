/*
 * Copyright (c) 2026 CrimsonSeraph(ltyy.leoyu@gmail.com)
 * SPDX-License-Identifier: MIT
 */

#pragma once

// 标准库
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <regex>
#include <sstream>
#include <string>
#include <typeinfo>

#ifdef _WIN32
#include <windows.h>
#endif

// ============================================
// 日志等级枚举
// ============================================
enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARN = 2,
    LOG_ERROR = 3,
    LOG_NONE = 4
};

using LogSinkCallback = std::function<void(const std::string& module,
    const std::string& method,
    LogLevel level,
    const std::string& message)>;

struct LogSink {
    LogSinkCallback callback;   ///< 回调函数
    LogLevel min_level;         ///< 最小输出等级
};

// ============================================
// DebugLogUtil - 日志辅助工具命名空间
// ============================================
namespace DebugLogUtil {

    /// @brief 移除字符串中的换行符，并将连续多个空格压缩为一个
    /// @param str 输入字符串
    /// @return 处理后的字符串
    inline std::string remove_newline(const std::string& str) {
        std::string result;
        result.reserve(str.size());
        bool last_char_was_space = false;

        for (char ch : str) {
            if (ch == '\n' || ch == '\r') {
                continue;
            }
            if (ch == ' ') {
                if (!last_char_was_space) {
                    result.push_back(ch);
                    last_char_was_space = true;
                }
            }
            else {
                result.push_back(ch);
                last_char_was_space = false;
            }
        }
        return result;
    }

} // namespace DebugLogUtil

// ============================================
// DebugLog - 日志系统核心类（单例）
// ============================================
class DebugLog {
public:
    // -------------------- 单例 --------------------
    static DebugLog& get_instance();

    // 禁止拷贝
    DebugLog(const DebugLog&) = delete;
    DebugLog& operator=(const DebugLog&) = delete;

    // -------------------- 日志等级控制 --------------------
    /// @brief 设置所有模块的日志等级（枚举版本）
    void set_all_log_level(LogLevel level);

    /// @brief 设置所有模块的日志等级（整数版本: 0=DEBUG,1=INFO,2=WARN,3=ERROR,4=NONE）
    void set_all_log_level(int level);

    /// @brief 设置是否仅输出与模块等级匹配的日志（类型信息模式）
    void set_only_type_info(bool only_type_info);

    /// @brief 设置指定模块的日志等级
    void set_log_level(const std::string& module, LogLevel level);

    /// @brief 获取指定模块的日志等级
    LogLevel get_log_level(const std::string& module) const;

    /// @brief 设置默认日志等级（未被单独设置的模块使用）
    void set_default_log_level(LogLevel level);

    // -------------------- 日志输出 --------------------
    /// @brief 核心日志输出函数
    void log(const std::string& module, const std::string& method,
        LogLevel level, const std::string& message);

    // -------------------- Sink 管理 --------------------
    /// @brief 注册日志输出通道（Sink）
    void register_log_sink(const std::string& name, const LogSink& sink);

    /// @brief 注销日志输出通道
    void unregister_log_sink(const std::string& name);

    /// @brief 设置某个 Sink 的最小日志等级
    /// @return 成功返回 true，Sink 不存在或等级无效返回 false
    bool set_log_sink_level(const std::string& name, LogLevel level);

    // -------------------- 工具函数 --------------------
    /// @brief 将日志等级枚举转换为字符串
    const char* level_to_string(LogLevel level);

    /// @brief 将整数转换为日志等级枚举
    static LogLevel int_to_log_level(int level);

    /// @brief 检查是否处于“仅类型信息”模式
    bool is_only_type_info() const;

private:
    // -------------------- 构造/析构（单例私有）--------------------
    DebugLog() = default;
    ~DebugLog() = default;

    // -------------------- 成员变量 --------------------
    mutable std::mutex mutex_;                      ///< 保护 module_log_levels_ 等
    std::map<std::string, LogLevel> module_log_levels_; ///< 各模块日志等级
    LogLevel default_log_level_ = LOG_DEBUG;        ///< 默认日志等级
    bool is_only_type_info_ = false;                ///< 仅输出类型信息模式

    std::map<std::string, LogSink> log_sinks_;      ///< 注册的 Sink
    mutable std::mutex sinks_mutex_;                ///< 保护 log_sinks_
};

// ============================================
// 单例实现（内联）
// ============================================
inline DebugLog& DebugLog::get_instance() {
    static DebugLog instance;
    static std::once_flag flag;
    std::call_once(flag, [&]() {
        LogSink console_sink;
        console_sink.callback = [](const std::string& module,
            const std::string& method,
            LogLevel level,
            const std::string& message) {
                std::string tag1 = "[" + module + "]";
                std::string tag2 = "<" + method + ">";
                std::string tag3 = "(" + std::string(DebugLog::get_instance().level_to_string(level)) + ")";

                const int TAG1_WIDTH = 30;
                const int TAG2_WIDTH = 35;
                const int TAG3_WIDTH = 10;

                auto pad_right = [](const std::string& s, int width) {
                    if (s.length() >= width) {
                        return s.substr(0, width);
                    }
                    return s + std::string(width - s.length(), ' ');
                    };

                std::string formatted = pad_right(tag1, TAG1_WIDTH) + " "
                    + pad_right(tag2, TAG2_WIDTH) + " "
                    + pad_right(tag3, TAG3_WIDTH) + ": "
                    + message;

#ifdef _WIN32
                HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
                if (h_console != INVALID_HANDLE_VALUE) {
                    DWORD written;
                    WriteConsoleA(h_console, formatted.c_str(), formatted.size(), &written, nullptr);
                    WriteConsoleA(h_console, "\n", 1, &written, nullptr);
                }
                else {
                    std::cerr << formatted << std::endl;
                }
#else
                std::cerr << formatted << std::endl;
#endif
            };
        console_sink.min_level = LOG_DEBUG;
        instance.register_log_sink("console", console_sink);
        });
    return instance;
}

// ============================================
// 日志等级控制实现（内联）
// ============================================
inline void DebugLog::set_all_log_level(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    default_log_level_ = level;
    for (auto& pair : module_log_levels_) {
        pair.second = level;
    }
}

inline void DebugLog::set_all_log_level(int level) {
    switch (level) {
    case 0: set_all_log_level(LOG_DEBUG); break;
    case 1: set_all_log_level(LOG_INFO);  break;
    case 2: set_all_log_level(LOG_WARN);  break;
    case 3: set_all_log_level(LOG_ERROR); break;
    case 4: set_all_log_level(LOG_NONE);  break;
    default: set_all_log_level(LOG_DEBUG); break;
    }
}

inline void DebugLog::set_only_type_info(bool only_type_info) {
    std::lock_guard<std::mutex> lock(mutex_);
    is_only_type_info_ = only_type_info;
}

inline void DebugLog::set_log_level(const std::string& module, LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    module_log_levels_[module] = level;
}

inline LogLevel DebugLog::get_log_level(const std::string& module) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = module_log_levels_.find(module);
    if (it != module_log_levels_.end()) {
        return it->second;
    }
    return default_log_level_;
}

inline void DebugLog::set_default_log_level(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    default_log_level_ = level;
}

// ============================================
// 日志输出实现（内联）
// ============================================
inline void DebugLog::log(const std::string& module, const std::string& method,
    LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(sinks_mutex_);
    for (const auto& pair : log_sinks_) {
        const auto& sink = pair.second;
        if (level >= sink.min_level) {
            sink.callback(module, method, level, message);
        }
    }
}

// ============================================
// Sink 管理实现（内联）
// ============================================
inline void DebugLog::register_log_sink(const std::string& name, const LogSink& sink) {
    std::lock_guard<std::mutex> lock(sinks_mutex_);
    log_sinks_[name] = sink;
}

inline void DebugLog::unregister_log_sink(const std::string& name) {
    std::lock_guard<std::mutex> lock(sinks_mutex_);
    log_sinks_.erase(name);
}

inline bool DebugLog::set_log_sink_level(const std::string& name, LogLevel level) {
    std::lock_guard<std::mutex> lock(sinks_mutex_);
    if (log_sinks_.find(name) == log_sinks_.end()) {
        return false;
    }
    if (level < LOG_DEBUG || level > LOG_NONE) {
        return false;
    }
    auto it = log_sinks_.find(name);
    if (it != log_sinks_.end()) {
        it->second.min_level = level;
    }
    return true;
}

// ============================================
// 工具函数实现（内联）
// ============================================
inline const char* DebugLog::level_to_string(LogLevel level) {
    switch (level) {
    case LOG_DEBUG: return "DEBUG";
    case LOG_INFO:  return "INFO";
    case LOG_WARN:  return "WARN";
    case LOG_ERROR: return "ERROR";
    default:        return "UNKNOWN";
    }
}

inline LogLevel DebugLog::int_to_log_level(int level) {
    switch (level) {
    case 0: return LOG_DEBUG;
    case 1: return LOG_INFO;
    case 2: return LOG_WARN;
    case 3: return LOG_ERROR;
    case 4: return LOG_NONE;
    default: return LOG_DEBUG;
    }
}

inline bool DebugLog::is_only_type_info() const {
    return is_only_type_info_;
}

// ============================================
// 日志宏（便捷调用）
// ============================================
#define LOG_MODULE(module, method, level, ...)                                 \
    do {                                                                       \
        LogLevel moduleLevel = DebugLog::get_instance().get_log_level(module); \
        bool shouldLog = DebugLog::get_instance().is_only_type_info()          \
                         ? (level == moduleLevel)                              \
                         : (level >= moduleLevel);                             \
        if (shouldLog) {                                                       \
            std::ostringstream oss;                                            \
            oss << __VA_ARGS__;                                                \
            DebugLog::get_instance().log(module, method, level, oss.str());    \
        }                                                                      \
    } while (0)
