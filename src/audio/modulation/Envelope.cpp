// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/modulation/Envelope.hpp"
#include "audio/engine/AudioEngine.hpp"

Envelope::Envelope(DSP::Seconds atk, DSP::Seconds hld, DSP::Seconds dec, NormalizedFloat sus, DSP::Seconds rel) noexcept
    : attack(atk), hold(hld), decay(dec), sustain(sus), release(rel)
{}

float Envelope::GetNextSample() noexcept {
    float nextSample = GetNextSampleHelper();
    if (!m_hasBeenReleased) {
        m_lastValueBeforeRelease = nextSample;
    }
    return nextSample;
}

float Envelope::GetNextSampleHelper() noexcept {
    // Progress time
    m_timeSinceStart += DSP::Seconds(1.0f / SAMPLE_RATE);

    if (m_hasBeenReleased) {
        if (m_timeSinceStart < release) {
            // When released, interpolate from current amplitude down to zero
            return m_lastValueBeforeRelease * (1.0f - m_timeSinceStart.count() / release.count());
        }else {
            return 0.0f;
        }
    }

    if (m_timeSinceStart < attack) {
        return m_timeSinceStart.count() / attack.count();
    }
    if (m_timeSinceStart < attack + hold) {
        return 1.0f;
    }
    if (m_timeSinceStart < attack + hold + decay) {
        DSP::Seconds timeSinceDecStart = m_timeSinceStart - attack - hold;
        return 1.0f - (1.0f - sustain.get()) * timeSinceDecStart.count() / decay.count();
    }
    return sustain;
}

void Envelope::Restart() noexcept {
    m_timeSinceStart = DSP::Seconds(0.0f);
    m_hasBeenReleased = false;
}

void Envelope::Release() noexcept {
    if (!m_hasBeenReleased) {
        m_timeSinceStart = DSP::Seconds(0.0f);
        m_hasBeenReleased = true;
    }
}

bool Envelope::IsComplete() const noexcept {
    if (m_hasBeenReleased) {
        return m_timeSinceStart >= release;
    }
    return false;
}