// BORA_INCLUDED_IN_SDK
// ^^^ Never remove this! This indicates that this should be included in production builds of libbora
// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: ThreadLocalTemplate.h
 * Purpose: Multi purpose thread local template for thread_local for both WASM Guest and Native Host
 * Native uses the standard thread_local and is more of a wrapper.
 * WASM guest allocates memory to an accessible thread stack and stores data through there.
 *
 * Note: You should only use this in common code between the SDK and the runtime (global)
 * if you are writing code that is only compiled in native, feel free to use thread_local
*/
#pragma once
#ifndef __BORA__ // Native [Windows, Linux, etc] - Uses thread_local
#include <vector>
#include <atomic>
namespace bora
{
    template <typename T>
    class ThreadLocal {
    private:
        T default_value_;
        size_t instance_id_;

        // Thread-local storage container per native thread
        struct ThreadStorageNode {
            std::vector<T> values;
        };

        static std::atomic<size_t>& get_instance_counter() {
            static std::atomic<size_t> counter{0};
            return counter;
        }

        static ThreadStorageNode& get_thread_storage() {
            thread_local ThreadStorageNode node;
            return node;
        }

        void ensure_capacity() const {
            auto& storage = get_thread_storage();
            if (instance_id_ >= storage.values.size()) {
                storage.values.resize(instance_id_ + 1, default_value_);
            }
        }

    public:
        // Constructors
        explicit ThreadLocal(const T& defaultValue = T())
            : default_value_(defaultValue),
              instance_id_(get_instance_counter()++) {}

        // Disable copying to prevent instance ID misalignment
        ThreadLocal(const ThreadLocal&) = delete;
        ThreadLocal& operator=(const ThreadLocal&) = delete;

        // Implicit conversion to underlying type for the current native thread
        operator T&() {

            ensure_capacity();
            return get_thread_storage().values[instance_id_];
        }

        operator const T&() const {
            ensure_capacity();
            return get_thread_storage().values[instance_id_];
        }

        // Assignment operator
        ThreadLocal& operator=(const T& value) {
            ensure_capacity();
            get_thread_storage().values[instance_id_] = value;
            return *this;
        }

        // Compound assignment operators
        ThreadLocal& operator+=(const T& value) {
            ensure_capacity();
            get_thread_storage().values[instance_id_] += value;
            return *this;
        }

        ThreadLocal& operator-=(const T& value) {
            ensure_capacity();
            get_thread_storage().values[instance_id_] -= value;
            return *this;
        }

        // Pointer operators for class/struct types
        T* operator->() {
            ensure_capacity();
            return &get_thread_storage().values[instance_id_];
        }

        const T* operator->() const {
            ensure_capacity();
            return &get_thread_storage().values[instance_id_];
        }

        // Explicit accessor if pointer-to-data is needed
        T* ptr() {
            ensure_capacity();
            return &get_thread_storage().values[instance_id_];
        }
    };
} // bora
#else // WASM Guest
#include <vector>
#include <atomic>
#include "Mutex.h"
#include <cstdint>
#include <macros>

namespace bora {
template <typename T>
class ThreadLocal {
private:
    IMPORT_ATTR("bora::thread", "getThreadId")
    static size_t GetThreadID();

    T default_value_;
    size_t instance_id_;

    // Per-thread storage node containing values for all ThreadLocal instances of type T
    struct ThreadStorageNode {
        std::vector<T> values;
    };

    // Shared registry for instance ID recycling across all instances of ThreadLocal<T>
    struct Registry {
        std::atomic<size_t> counter{0};
        std::vector<size_t> free_list;
        bora::Mutex mtx;
    };

    static Registry& get_registry() {
        static Registry reg;
        return reg;
    }

    static size_t allocate_id() {
        auto& reg = get_registry();
        std::lock_guard lock(reg.mtx);
        if (!reg.free_list.empty()) {
            size_t id = reg.free_list.back();
            reg.free_list.pop_back();
            return id;
        }
        return reg.counter++;
    }

    static void deallocate_id(size_t id) {
        auto& reg = get_registry();
        std::lock_guard lock(reg.mtx);
        reg.free_list.push_back(id);
    }

    // Retrieve storage for the *current* WASM thread using the host-provided thread ID
    static ThreadStorageNode& get_thread_storage() {
        size_t thread_id = GetThreadID();

        // Support up to 128 concurrent max slots by default (can be adjusted)
        constexpr size_t kMaxWasmThreads = 128;
        static ThreadStorageNode thread_nodes[kMaxWasmThreads];

        if (thread_id >= kMaxWasmThreads) {
            // Safety fallback or trap if concurrency exceeds the fixed limit
            __builtin_trap();
        }

        return thread_nodes[thread_id];
    }

    void ensure_capacity() const {
        auto& storage = get_thread_storage();
        if (instance_id_ >= storage.values.size()) {
            storage.values.resize(instance_id_ + 1, default_value_);
        }
    }

public:
    // Constructors
    explicit ThreadLocal(const T& defaultValue = T())
        : default_value_(defaultValue),
          instance_id_(allocate_id()) {}

    ~ThreadLocal() {
        deallocate_id(instance_id_);
    }

    // Disable copying
    ThreadLocal(const ThreadLocal&) = delete;
    ThreadLocal& operator=(const ThreadLocal&) = delete;

    // Implicit conversion to underlying type for the current WASM thread
    operator T&() {
        ensure_capacity();
        return get_thread_storage().values[instance_id_];
    }

    operator const T&() const {
        ensure_capacity();
        return get_thread_storage().values[instance_id_];
    }

    // Assignment operator
    ThreadLocal& operator=(const T& value) {
        ensure_capacity();
        get_thread_storage().values[instance_id_] = value;
        return *this;
    }

    // Compound assignment operators
    ThreadLocal& operator+=(const T& value) {
        ensure_capacity();
        get_thread_storage().values[instance_id_] += value;
        return *this;
    }

    ThreadLocal& operator-=(const T& value) {
        ensure_capacity();
        get_thread_storage().values[instance_id_] -= value;
        return *this;
    }

    // Pointer operators for class/struct types
    T* operator->() {
        ensure_capacity();
        return &get_thread_storage().values[instance_id_];
    }

    const T* operator->() const {
        ensure_capacity();
        return &get_thread_storage().values[instance_id_];
    }

    // Explicit accessor if pointer-to-data is needed
    T* ptr() {
        ensure_capacity();
        return &get_thread_storage().values[instance_id_];
    }
};
} // namespace bora
#endif

