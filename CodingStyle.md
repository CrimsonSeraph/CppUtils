# CppUtils CodingStyle

基于 C++20 的工具、算法实现仓库。

本规范旨在保证代码的可读性与一致性，所有贡献者应遵循。

## 目录结构

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

## 编码规范

### 1. 文件编码与换行

- **字符编码**: UTF-8 without BOM
- **换行符**: CRLF（Windows 风格）
- **文件末尾**: 必须保留一个空行

### 2. 缩进与空白

- **缩进方式**: 4 个空格（不使用 Tab）
> 若使用 Tab，必须设置为等同于 4 个空格且最后文件保存时转换为 4 个空格
- **行最大长度**: 暂不作严格限制，但建议不超过 120 字符
- **括号风格**: 左括号与语句同行（K&R 风格）

```cpp
// 正确示例
if (condition) {
    do_something();
}
```

### 3. 命名规则

| 元素 | 风格 | 示例 |
| - | - | - |
| 类名 | 双驼峰（PascalCase） | `GameCore`, `PluginLoader` |
| 变量名 | 小写 + 下划线（snake_case） | `player_health`, `max_level` |
| 函数名 | 小写 + 下划线（snake_case） | `load_plugin()`, `register_level()` |
| 常量/宏 | 全大写 + 下划线 | `MAX_BUFFER_SIZE` |
| 接口类（抽象基类） | 前缀 `I` + PascalCase | `ILevel`, `IBuff` |

> **注意**: 重写标准库或第三方库的虚函数时，保持原有命名风格。

### 4. 导入顺序（C++ `#include`）

- **顺序规则**:
  1. 自定义头文件（`include/` 下的文件）
  2. 第三方库（如 nlohmann/json）
  3. 标准库

- **内部排序**: 每组内按字母顺序排列

```cpp
// 自定义头文件
#include "GameCore.h"
#include "PluginLoader.h"

// 第三方库
#include <nlohmann/json.hpp>

// 标准库
#include <fstream>
#include <memory>
#include <vector>
```

### 5. 头文件中的各项定义顺序

类声明中各分区必须按以下顺序排列（若某分区为空，可省略）:

```
public: → protected: → private:
```

#### 5.1 各分区内部成员排列顺序

- **`public` 分区**
  1. 构造函数 / 析构函数
  2. 公共接口（普通成员函数）
  3. 公共成员变量（极少使用）

- **`protected` 分区**
  1. 重写的虚函数（`override`）
  2. 其他受保护方法
  3. 受保护成员变量

- **`private` 分区**
  1. 常量（`static constexpr` / `const`）
  2. 成员变量
  3. 私有辅助函数

#### 5.2 特殊成员的放置位置（按优先级）

- **友元声明**: 紧接在 `private:` 标签之后的第一行
- **静态成员变量**: 放在 `private:` 区的最开头（常量之前）
- **静态成员函数**: 放在所属分区（`public` / `private`）的普通成员函数之前

#### 5.3 模板函数与静态工具函数

- **模板函数**: 仅声明在头文件中，定义放入同名的 `类名_impl.hpp` 文件（同一目录）。
- **静态工具函数**: 不要放在类声明内，应独立到 `_utils.hpp` / `_utils.cpp` 文件中。

### 6. 源文件组织规范

- 函数定义的顺序**必须**与头文件中各分区的声明顺序严格一致。
- 每个分区内，函数定义的顺序也必须与头文件中声明的顺序相同。
- 若头文件有调整（如成员函数重排），源文件需同步调整定义顺序。

### 7. 函数注释规范

为每个函数添加说明注释，推荐使用 Doxygen 风格:

- 复杂函数使用以下格式:

```cpp
/// @brief 简短功能描述
/// @param param1 参数说明
/// @param param2 参数说明
/// @return 返回值说明
/// @note 注意事项（可选）
int calculate(int param1, double param2);
```

- 简单函数可在函数前一行使用 `//` 进行简短描述:

```cpp
// 检查游戏是否正在进行
bool is_game_running();
```

- 若无参数、返回值或特殊说明，对应标签可省略。

### 8. 日志规范

若使用 `DebugLog.hpp` 使用自定义日志宏（如 `LOG_DEBUG`、`LOG_INFO`、`LOG_WARN`、`LOG_ERROR` 等），遵循以下原则:

#### 8.1 添加调试信息的位置

- 函数入口/出口（尤其是复杂逻辑、状态改变、资源分配释放等）
- 关键分支（`if`/`else`、`switch`）和循环开始/结束
- 错误处理路径、异常路径
- 重要变量值的变更前后
- 外部调用（系统API、插件加载）前后

#### 8.2 减少不必要的调试信息

- 过于频繁调用的函数（如主循环中每帧调用的函数）中避免输出每次调用，可改为采样或条件输出
- 移除明显冗余的日志（例如连续输出相同信息，或与上下文重复的信息）
- 避免在热点路径（高频循环）中输出日志，以免影响性能

#### 8.3 日志级别使用

| 级别 | 用途 |
| - | - |
| `LOG_ERROR` | 严重错误，影响功能（如无法加载插件、配置文件损坏） |
| `LOG_WARN`  | 可恢复的异常情况（如某层级资源缺失、插件接口版本不匹配） |
| `LOG_INFO`  | 重要的状态变化、模块初始化/销毁、用户操作等 |
| `LOG_DEBUG` | 详细调试信息，默认可能在 Release 版本禁用 |

> 根据实际情况选择合适的级别，不要滥用 `LOG_INFO` 和 `LOG_ERROR`。

#### 8.4 目标

调试信息充分但不过量，能帮助定位问题，不影响性能或淹没关键信息。

### 9. 注释规范（基本）

- **单行注释（独占一行）**: 放在代码块前面，与代码块开头保持相同缩进级别。
  `//` 后跟一个空格，再写注释内容。

```cpp
// 检查玩家是否在层内
if (current_level != nullptr) {
    current_level->update();
}
```

- **行尾注释**: 与代码之间**间隔一个 Tab**（不是多个空格），`//` 后跟一个空格。

```cpp
int retry_count = 0;  // 当前重试次数
bool game_over = false;  // 游戏结束标志
```

- **其他注释形式**（块注释、文档注释等）不作强制要求，可自由使用。

### 10. 其他约定

- 头文件使用 `#pragma once` 进行防止重复包含。
- 未在本规范中明确提及的内容（如 switch 语句格式、异常处理方式等），可保持个人习惯，后续逐步补充。

## 开发环境

- **编译器**: 支持 C++20 的 MSVC 2022 / GCC 11+ / Clang 14+
- **构建工具**: CMake 3.16+

## 参考

- 项目贡献前请阅读 [CONTRIBUTING.md](CONTRIBUTING.md)
- 本规范会随项目发展逐步完善，欢迎讨论并提出改进建议。
