// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#include "audio/modulation/Envelope.hpp"
#include "audio/engine/AudioEngine.hpp"

Envelope::Envelope(float atk, float hld, float dec, NormalizedFloat sus, float rel) noexcept
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
    m_timeSinceStart += 1.0f / SAMPLE_RATE;

    if (m_hasBeenReleased) {
        if (m_timeSinceStart < release) {
            // When released, interpolate from current amplitude down to zero
            return m_lastValueBeforeRelease * (1.0f - m_timeSinceStart / release);
        }else {
            return 0.0f;
        }
    }

    if (m_timeSinceStart < attack) {
        return m_timeSinceStart / attack;
    }
    if (m_timeSinceStart < attack + hold) {
        return 1.0f;
    }
    if (m_timeSinceStart < attack + hold + decay) {
        float timeSinceDecStart = m_timeSinceStart - attack - hold;
        return 1.0f - (1.0f - sustain.get()) * timeSinceDecStart / decay;
    }
    return sustain;
}

void Envelope::Restart() noexcept {
    m_timeSinceStart = 0.0f;
    m_hasBeenReleased = false;
}

void Envelope::Release() noexcept {
    if (!m_hasBeenReleased) {
        m_timeSinceStart = 0.0f;
        m_hasBeenReleased = true;
    }
}

bool Envelope::IsComplete() const noexcept {
    if (m_hasBeenReleased) {
        return m_timeSinceStart >= release;
    }
    return false;
}