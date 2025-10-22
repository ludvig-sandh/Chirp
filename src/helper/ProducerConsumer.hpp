// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ludvig Sandh

#pragma once

#include <memory>
#include <optional>
#include <mutex>
#include <condition_variable>

// A bounded buffer but for just one item rather than a buffer.
template <typename T>
class ProducerConsumer {
public:
    void Produce(std::unique_ptr<T> item);
    std::unique_ptr<T> Consume();

    // Notifies that one side has closed, so no need to wait indefinitely
    // Extra note: a condition variable's predicate is always checked before waiting,
    // so we won't ever get stuck waiting for a closed producer/consumer.
    void Close();
private:
    bool m_closed = false;
    std::unique_ptr<T> m_heldItem;

    std::mutex m_mtx;
    std::condition_variable m_waitEmpty;
    std::condition_variable m_waitFull;
};

template <typename T>
void ProducerConsumer<T>::Produce(std::unique_ptr<T> item) {
    std::unique_lock<std::mutex> lock(m_mtx);
    m_waitEmpty.wait(lock, [this]{ return m_heldItem == nullptr || m_closed; });
    if (m_closed) {
        // Ignore production
        return;
    }
    m_heldItem = std::move(item);
    m_waitFull.notify_one();
}

template <typename T>
void ProducerConsumer<T>::Close() {
    std::unique_lock<std::mutex> lock(m_mtx);
    m_closed = true;
    m_waitFull.notify_all();
    m_waitEmpty.notify_all();
}

template <typename T>
std::unique_ptr<T> ProducerConsumer<T>::Consume() {
    std::unique_lock<std::mutex> lock(m_mtx);
    m_waitFull.wait(lock, [this]{ return m_heldItem != nullptr || m_closed; });
    if (m_heldItem == nullptr) {
        // Ignore consumption
        return nullptr;
    }
    std::unique_ptr<T> temp = std::move(m_heldItem);
    m_waitEmpty.notify_one();
    return temp;
}