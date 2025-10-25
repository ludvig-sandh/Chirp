// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

struct AudioFrame {
    float left = 0.0f;
    float right = 0.0f;

    // Clamp the amplitude to avoid the possibility of going deaf
    void ClipToValidRange();

    // Blend between processed and unprocessed signals.
    // mix = 0 -> fully unprocessed
    // mix = 1 -> fully processed
    static AudioFrame Blend(const AudioFrame& processed, const AudioFrame& unprocessed, float mix) noexcept;

    constexpr AudioFrame operator+(const AudioFrame& other) const noexcept {
        return { left + other.left, right + other.right };
    }

    constexpr AudioFrame& operator+=(const AudioFrame& other) noexcept {
        left += other.left;
        right += other.right;
        return *this;
    }

    constexpr AudioFrame& operator+=(float term) noexcept {
        left += term;
        right += term;
        return *this;
    }

    constexpr AudioFrame& operator-=(float term) noexcept {
        left -= term;
        right -= term;
        return *this;
    }

    constexpr AudioFrame operator*(float scalar) const noexcept {
        return { left * scalar, right * scalar };
    }

    constexpr AudioFrame& operator*=(float scalar) noexcept {
        left *= scalar;
        right *= scalar;
        return *this;
    }

    constexpr AudioFrame operator/(float scalar) const noexcept {
        return { left / scalar, right / scalar };
    }

    constexpr AudioFrame& operator/=(float scalar) noexcept {
        left /= scalar;
        right /= scalar;
        return *this;
    }
};

// --- Global operators (for commutativity) ---
constexpr inline AudioFrame operator*(float scalar, const AudioFrame& frame) noexcept {
    return frame * scalar;
}