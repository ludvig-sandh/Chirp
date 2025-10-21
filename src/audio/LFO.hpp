#pragma once

#include <functional>
#include <optional>

// Fwd dec.
class AudioProcessor;

class LFO {
public:
    virtual ~LFO() = default;
    virtual float GetNextSample() = 0;

    // LFO *: self, AudioProcessor *: the node modulated by the LFO
    std::optional<std::function<void(LFO *, AudioProcessor *)>> callback;
};