// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <algorithm>

namespace Util {

// Represents a float in the range [0.0f, 1.0f]. Any other value gets clamped into that range.
class NormalizedFloat {
public:
    constexpr NormalizedFloat(float value) noexcept
        : m_value(std::clamp(value, 0.0f, 1.0f)) {}

    // Accessor
    constexpr float get() const noexcept { return m_value; }

    // Handy implicit conversion to float
    constexpr operator float() const noexcept { return m_value; }

     // Compound assignment operators
    constexpr NormalizedFloat& operator+=(NormalizedFloat delta) noexcept {
        m_value = std::clamp(m_value + delta.get(), 0.0f, 1.0f);
        return *this;
    }

    constexpr NormalizedFloat& operator-=(NormalizedFloat delta) noexcept {
        m_value = std::clamp(m_value - delta.get(), 0.0f, 1.0f);
        return *this;
    }

    constexpr NormalizedFloat& operator*=(float scale) noexcept {
        m_value = std::clamp(m_value * scale, 0.0f, 1.0f);
        return *this;
    }

    constexpr NormalizedFloat& operator/=(float scale) noexcept {
        if (scale != 0.0f)
            m_value = std::clamp(m_value / scale, 0.0f, 1.0f);
        return *this;
    }

    // Binary operators (non-mutating)
    friend constexpr NormalizedFloat operator+(NormalizedFloat lhs, NormalizedFloat rhs) noexcept {
        lhs += rhs;
        return lhs;
    }

    friend constexpr NormalizedFloat operator-(NormalizedFloat lhs, NormalizedFloat rhs) noexcept {
        lhs -= rhs;
        return lhs;
    }

    friend constexpr NormalizedFloat operator*(NormalizedFloat lhs, NormalizedFloat rhs) noexcept {
        lhs *= rhs;
        return lhs;
    }

    friend constexpr NormalizedFloat operator/(NormalizedFloat lhs, NormalizedFloat rhs) noexcept {
        lhs /= rhs;
        return lhs;
    }

    // Comparisons
    friend constexpr bool operator==(NormalizedFloat lhs, NormalizedFloat rhs) noexcept {
        return lhs.m_value == rhs.m_value;
    }

    friend constexpr auto operator<=>(NormalizedFloat lhs, NormalizedFloat rhs) noexcept = default;

private:
    float m_value;
};

} // namespace Util
