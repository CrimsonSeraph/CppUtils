/*
 * Copyright (c) 2026 CrimsonSeraph(ltyy.leoyu@gmail.com)
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

/// @brief 任意精度大整数类，基数为 10^9
class BigInt {
public:
    /// @brief 默认构造，值为 0
    BigInt() : big_n_{ 0 } {}

    /// @brief 从 long long 构造
    /// @param value 初始值（非负）
    explicit BigInt(long long value) {
        transform_from_ll(value);
    }

    /// @brief 拷贝构造
    BigInt(const BigInt& other) = default;

    /// @brief 析构函数
    ~BigInt() = default;

    /// @brief 获取内部表示的常量引用（只读）
    /// @return 存储数字的 vector，低位在前，每位范围 [0, kBase)
    const std::vector<long long>& get_big_n() const {
        return big_n_;
    }

    /// @brief 加法运算符重载
    BigInt operator+(const BigInt& other) const {
        BigInt result;
        const size_t max_size = std::max(big_n_.size(), other.big_n_.size());
        long long carry = 0;

        for (size_t i = 0; i < max_size || carry; ++i) {
            long long sum = carry;
            if (i < big_n_.size()) sum += big_n_[i];
            if (i < other.big_n_.size()) sum += other.big_n_[i];

            result.big_n_.push_back(sum % kBase);
            carry = sum / kBase;
        }

        normalize(result.big_n_);
        return result;
    }

    /// @brief 乘法运算符重载（自动选择普通乘法或 Karatsuba）
    BigInt operator*(const BigInt& other) const {
        const size_t threshold = 32;  // 位数超过此值使用 Karatsuba

        if (big_n_.size() < threshold || other.big_n_.size() < threshold) {
            BigInt result;
            result.big_n_ = multiply_naive(big_n_, other.big_n_);
            return result;
        }

        BigInt result;
        result.big_n_ = karatsuba(big_n_, other.big_n_);
        return result;
    }

    /// @brief 输出流运算符
    friend std::ostream& operator<<(std::ostream& os, const BigInt& num) {
        if (num.big_n_.empty()) {
            os << '0';
            return os;
        }

        // 最高位直接输出，不补零
        os << num.big_n_.back();
        for (int i = static_cast<int>(num.big_n_.size()) - 2; i >= 0; --i) {
            os << std::setw(9) << std::setfill('0') << num.big_n_[i];
        }
        return os;
    }

    /// @brief 输入流运算符（仅支持非负整数）
    friend std::istream& operator>>(std::istream& is, BigInt& num) {
        long long value;
        is >> value;

        if (value < 0) {
            std::cerr << "错误：BigInt 不支持负数，已重置为 0\n";
            num.big_n_.clear();
            num.big_n_.push_back(0);
            return is;
        }

        num.transform_from_ll(value);
        return is;
    }

private:
    // 常量
    static constexpr long long kBase = 1000000000;  // 10^9

    // 成员变量
    std::vector<long long> big_n_;  // 低位在前，每个元素 < kBase

    // ---------- 私有辅助函数 ----------
    void transform_from_ll(long long value) {
        big_n_.clear();
        if (value == 0) {
            big_n_.push_back(0);
            return;
        }
        while (value > 0) {
            big_n_.push_back(value % kBase);
            value /= kBase;
        }
    }

    // 普通 O(n^2) 乘法
    std::vector<long long> multiply_naive(const std::vector<long long>& a,
        const std::vector<long long>& b) const {
        std::vector<long long> result(a.size() + b.size(), 0);

        for (size_t i = 0; i < a.size(); ++i) {
            long long carry = 0;
            for (size_t j = 0; j < b.size() || carry; ++j) {
                long long prod = result[i + j] + carry;
                if (j < b.size()) {
                    prod += a[i] * b[j];
                }
                result[i + j] = prod % kBase;
                carry = prod / kBase;
            }
        }

        normalize(result);
        return result;
    }

    // Karatsuba 乘法（递归）
    std::vector<long long> karatsuba(const std::vector<long long>& a,
        const std::vector<long long>& b) {
        // 规模较小时回退到普通乘法
        if (a.empty() || b.empty()) return {};
        if (a.size() <= 32 || b.size() <= 32) {
            return multiply_naive(a, b);
        }

        // 取较大长度的一半作为分割点
        const size_t m = std::max(a.size(), b.size()) / 2;

        // 分割 A = A1 * base^m + A0
        std::vector<long long> a0(a.begin(), a.begin() + std::min(m, a.size()));
        std::vector<long long> a1(a.begin() + std::min(m, a.size()), a.end());

        // 分割 B = B1 * base^m + B0
        std::vector<long long> b0(b.begin(), b.begin() + std::min(m, b.size()));
        std::vector<long long> b1(b.begin() + std::min(m, b.size()), b.end());

        // 递归计算
        std::vector<long long> z0 = karatsuba(a0, b0);
        std::vector<long long> z2 = karatsuba(a1, b1);

        // 计算 a0+a1 和 b0+b1
        std::vector<long long> a0_plus_a1 = add_vectors(a0, a1);
        std::vector<long long> b0_plus_b1 = add_vectors(b0, b1);
        std::vector<long long> z1 = karatsuba(a0_plus_a1, b0_plus_b1);
        // z1 = (A0+A1)*(B0+B1) - z2 - z0
        z1 = subtract_vectors(z1, z2);
        z1 = subtract_vectors(z1, z0);

        // 结果 = z2 * base^(2m) + z1 * base^m + z0
        std::vector<long long> result = z0;
        add_shifted(result, z1, m);
        add_shifted(result, z2, 2 * m);

        normalize(result);
        return result;
    }

    // ---------- Karatsuba 辅助运算（静态，避免 ODR） ----------
    static std::vector<long long> add_vectors(const std::vector<long long>& a,
        const std::vector<long long>& b) {
        std::vector<long long> result;
        const size_t max_size = std::max(a.size(), b.size());
        long long carry = 0;
        for (size_t i = 0; i < max_size || carry; ++i) {
            long long sum = carry;
            if (i < a.size()) sum += a[i];
            if (i < b.size()) sum += b[i];
            result.push_back(sum % kBase);
            carry = sum / kBase;
        }
        return result;
    }

    static std::vector<long long> subtract_vectors(const std::vector<long long>& a,
        const std::vector<long long>& b) {
        std::vector<long long> result = a;
        long long borrow = 0;
        for (size_t i = 0; i < result.size(); ++i) {
            long long sub = borrow;
            if (i < b.size()) sub += b[i];
            if (result[i] < sub) {
                result[i] += kBase - sub;
                borrow = 1;
            }
            else {
                result[i] -= sub;
                borrow = 0;
            }
        }
        while (result.size() > 1 && result.back() == 0) result.pop_back();
        return result;
    }

    static void add_shifted(std::vector<long long>& dest,
        const std::vector<long long>& src,
        size_t shift) {
        if (src.empty()) return;
        if (dest.size() < shift + src.size()) dest.resize(shift + src.size(), 0);
        long long carry = 0;
        for (size_t i = 0; i < src.size() || carry; ++i) {
            if (i < src.size()) {
                dest[shift + i] += src[i] + carry;
            }
            else {
                dest[shift + i] += carry;
            }
            carry = dest[shift + i] / kBase;
            dest[shift + i] %= kBase;
        }
    }

    static void normalize(std::vector<long long>& num) {
        while (num.size() > 1 && num.back() == 0) {
            num.pop_back();
        }
        if (num.empty()) num.push_back(0);
    }
};
