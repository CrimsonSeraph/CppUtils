# CppUtils

基于 C++20 的工具、算法实现仓库。

> **目录**
> - [一、项目简介](#一项目简介)
> - [二、功能特性](#二功能特性)
> - [三、使用说明](#三使用说明)
> - [四、项目结构](#四项目结构)
> - [五、截图](#五截图)
> - [六、编码规范](#六编码规范)
> - [七、FAQ](#七faq)
> - [八、贡献指南](#八贡献指南)
> - [九、许可证](#九许可证)
> - [十、联系方式](#十联系方式)

---

## 一、项目简介

**CppUtils** 是一个轻量级、跨平台的 C++ 工具库，聚焦于日常开发中常用的辅助组件和经典算法实现。
项目完全采用 **C++20** 标准，旨在提供：

- 开箱即用的实用工具类（控制台增强、日志系统等）
- 清晰高效的算法示例与封装
- 统一的代码风格和现代 C++ 最佳实践

无论你是希望快速集成日志模块，还是学习 C++20 的新特性，这个仓库都能为你提供参考。

---

## 二、功能特性

**工具目录 `utils/`**

| 组件 | 描述 | 状态 |
| - | - | - |
| `Console.hpp` | 控制台辅助类：支持彩色输出、光标控制、用户输入封装，基于 ANSI 转义码 | ✅ 已完成 |
| `DebugLog.hpp` | 轻量级日志系统：支持多级别（DEBUG/INFO/WARN/ERROR）、输出到控制台/文件、自动时间戳、线程安全 | ✅ 已完成 |

> 更多实用工具和算法将陆续加入，欢迎贡献！

---

## 三、使用说明

### 环境要求

- 支持 C++20 的编译器（GCC 10+ / Clang 12+ / MSVC 2022 17.0+）
- CMake 3.15+（推荐，但非强制）

### 集成方式

#### 1. 直接包含头文件

将 `utils/` 目录复制到你的项目中，然后包含所需头文件：

```cpp
#include "utils/Console.hpp"
#include "utils/DebugLog.hpp"

int main() {
    Console::PrintColor("Hello, CppUtils!", Console::Color::Green);
    DebugLog::Init("app.log", DebugLog::Level::Info);
    LOG_INFO("Application started");
    return 0;
}
```

#### 2. CMake 子目录集成

在你的 `CMakeLists.txt` 中添加：

```cmake
add_subdirectory(path/to/CppUtils)
target_link_libraries(your_target PRIVATE CppUtils)
```

> 注意：仓库本身不提供 `CMakeLists.txt`？实际目录未包含，你可以自行添加或直接使用头文件。

### 基本示例

- **控制台彩色输出**：`Console::PrintColor("text", Console::Color::Red);`
- **日志记录**：`LOG_DEBUG("Value = %d", 42);`

详细 API 请参阅各头文件中的注释。

---

## 四、项目结构

```text
CppUtils/
├── screenshot/                         # 截屏资源文件
│   └── README.md                       # screenshot 目录说明
├── utils/                              # 工具
│   ├── Console.hpp                     # 控制台类
│   ├── DebugLog.hpp                    # 日志系统
│   └── README.md                       # utils 目录说明
├── .editorconfig                       # 编辑器代码风格配置
├── .gitattributes                      # Git 属性配置（换行符等）
├── .gitignore                          # Git 忽略文件规则
├── CONTRIBUTING.md                     # 贡献指南
├── CodingStyle.md                      # 代码规范文档
├── LICENSE                             # MIT 许可证
└── README.md                           # 仓库说明文档
```

---

## 五、截图

暂时留空

更多截图请查看 [`screenshot/`](screenshot/README.md) 目录。

---

## 六、编码规范

本项目严格遵循 `CodingStyle.md` 中定义的规范，并由 `.editorconfig` 辅助保持格式统一。核心要点：

- **命名约定**：
  - 类型/类名：大驼峰 `MyClass`
  - 函数/变量：小驼峰 `getValue()` / 下划线可选但推荐统一
  - 宏：全大写 `LOG_INFO`
- **格式**：缩进 4 空格，行宽 100 字符，使用 clang-format 同步
- **现代 C++ 特性**：优先使用 `std::span`、`std::string_view`、concepts、lambda 等
- **错误处理**：推荐 `std::expected` (C++23) 或异常，但保持接口简单

提交代码前请确保通过 `.editorconfig` 对齐风格。

---

## 七、FAQ

暂时留空

---

## 八、贡献指南

欢迎提交 Issue 和 Pull Request。贡献前请确保：

- 代码遵循 [CodingStyle.md](CodingStyle.md) 规范。
- 新功能或 bug 修复经过本地测试。
- 更新相关文档（如 README、CHANGELOG）。
- 对于较大改动，建议先开 Issue 讨论设计。

详细流程请参考 [CONTRIBUTING.md](CONTRIBUTING.md)。

---

## 九、许可证

本项目使用 **MIT 许可证**。详情请参阅项目根目录下的 [LICENSE](LICENSE.txt) 文件。

所依赖的第三方组件适用各自的许可证（如 nlohmann/json 使用 MIT）。

---

## 十、联系方式

- 作者: [CrimsonSeraph]
- BiliBili: [浪天幽影(UID: 1741002917)](https://space.bilibili.com/1741002917?spm_id_from=333.1007.0.0)
- X: [𝒞𝓇𝒾𝓂𝓈𝑜𝓃𝒮𝑒𝓇𝒶𝓅𝒽✟升天✟(@CrimSeraph_QwQ)](https://x.com/CrimSeraph_QwQ)
- 项目主页: [https://github.com/CrimsonSeraph/DG-LAB-Client](https://github.com/CrimsonSeraph/DG-LAB-Client)

欢迎 star 和 fork，让更多人受益！
