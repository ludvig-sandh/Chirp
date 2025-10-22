#pragma once

namespace FeedbackDelayInfo {
    enum class Type {
        Mono,
        Stereo,
        PingPong
    };

    inline constexpr const char* Names[] = { "Mono", "Stereo", "Ping pong" };
}