// BORA_INCLUDED_IN_SDK
// ^^^ Never remove this! This indicates that this should be included in production builds of libbora
// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: Mutex.h
 * Purpose: ?
*/
#pragma once

namespace bora
{
#ifndef __BORA__
#include <mutex>
using Mutex = std::mutex;
#else
#include <atomic>

class Mutex {
    std::atomic<bool> flag_{false};

public:
    Mutex() = default;
    ~Mutex() = default;

    // Prevent copying
    Mutex(const Mutex&) = delete;
    Mutex& operator=(const Mutex&) = delete;

    void lock() {
        // Spin until we successfully acquire the lock
        while (flag_.exchange(true, std::memory_order_acquire)) {
            // Optional: emit a hint if available, or keep spinning
        }
    }

    bool try_lock() {
        return !flag_.exchange(true, std::memory_order_acquire);
    }

    void unlock() {
        flag_.store(false, std::memory_order_release);
    }
};
#endif
}