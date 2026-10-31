// BORA_INCLUDED_IN_SDK
// ^^^ Never remove this! This indicates that this should be included in production builds of libbora
// This header is apart of the BORA Source
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: CommandListContainers.h
 * Purpose: Contains all the global containers both the host and guest can use.
 */
#pragma once
#include <list>
#include <mutex>
#include <optional>
#include <vector>
#include <queue>

#include "Mutex.h"
#include "ThreadLocalTemplate.h"

template<typename C>
concept CommandListContainer =
    requires(C c, typename C::value_type v, const typename C::value_type* ptr, std::size_t count) {
    typename C::value_type;

    { c.push_back(std::move(v)) } -> std::same_as<void>;
    { c.push_back(v) } -> std::same_as<void>;
    { c.push_back(ptr, count) } -> std::same_as<void>;
    { c.pop_back() } -> std::same_as<std::optional<typename C::value_type>>;
    { c.access([](auto&&) {}) };

    { c.clear() } -> std::same_as<void>;
    { c.size() } -> std::convertible_to<std::size_t>;
    { c.data() } -> std::convertible_to<const typename C::value_type*>;
};

/// A Thread-Safe Vector that uses mutexes to ensure thread safety.
/// This is beneficial for command lists that don't need fast push rates in multiple threads.
/// \n However since this uses mutexes, many threads WILL wait for one thread to finish pushing to the vector, which can slow down performance
/// \n\n If you're looking for a vector solution that requires multiple threads to be able to access the vector all in once, use the \c ThreadLocalVector
/// @tparam T Type used for vector
template <typename T>
class MutexVector {
public:
    using value_type = T;

private:
    mutable bora::Mutex mutex_;
    std::vector<T> vec_;

public:
    MutexVector() = default;
    ~MutexVector() = default;

    // Mutexes are non-copyable, so delete copy operations
    MutexVector(const MutexVector&) = delete;
    MutexVector& operator=(const MutexVector&) = delete;

    // Allow moving if needed
    MutexVector(MutexVector&& other) noexcept {
        std::lock_guard<bora::Mutex> lock(other.mutex_);
        vec_ = std::move(other.vec_);
    }

    MutexVector& operator=(MutexVector&& other) noexcept {
        if (this != &other) {
            std::scoped_lock lock(mutex_, other.mutex_);
            vec_ = std::move(other.vec_);
        }
        return *this;
    }

    template<typename Func>
   void access(Func&& f) {
        std::lock_guard lock(mutex_);
        for (auto* item : vec_) {
                f(item);
        }
    }

    // 1. Mutation: push_back (const lvalue) -> void
    void push_back(const T& value) {
        std::lock_guard lock(mutex_);
        vec_.push_back(value);
    }

    // 2. Mutation: push_back (rvalue / move) -> void
    void push_back(T&& value) {
        std::lock_guard lock(mutex_);
        vec_.push_back(std::move(value));
    }

    void push_back(const T* dataPtr, std::size_t count) {
        if (!dataPtr || count == 0) return;
        std::lock_guard lock(mutex_);
        vec_.insert(vec_.end(), dataPtr, dataPtr + count);
    }

    // 3. Removal: pop_back -> std::optional<T>
    std::optional<T> pop_back() {
        std::lock_guard lock(mutex_);
        if (vec_.empty()) {
            return std::nullopt;
        }
        T val = std::move(vec_.back());
        vec_.pop_back();
        return val;
    }

    // Size: size -> convertible to std::size_t
    std::size_t size() const {
        std::lock_guard lock(mutex_);
        return vec_.size();
    }

    // Data: data -> convertible to const T*
    // NOTE: In a concurrent context, ensure pushes have stopped before calling data(),
    // as reallocation during concurrent writes can invalidate raw pointers.
    const T* data() const {
        std::lock_guard lock(mutex_);
        return vec_.data();
    }

    // Non-const overload of data() if needed by other systems
    T* data() {
        std::lock_guard lock(mutex_);
        return vec_.data();
    }

    // Optional convenience method to pre-allocate capacity and avoid reallocation issues
    void reserve(std::size_t capacity) {
        std::lock_guard lock(mutex_);
        vec_.reserve(capacity);
    }

    // Optional clear method
    void clear() {
        std::lock_guard lock(mutex_);
        vec_.clear();
    }
};

/// A Thread safe vector that gives threads their own individual vectors and keeps track of all them.
/// \n This is very useful for command lists that may need high performance command lists that need to push information through threads simultaneously as theres no mutex while pushing
/// \n\n However, this is considered a bit overkill if you're using this in a command list that doesn't need performance, use \c MutexableVector for that case.
/// @tparam T Type used for vector
template<typename T>
class ThreadLocalVector {
public:
    // Internal wrapper to preserve temporal causality
    struct TimedEntry {
        uint64_t sequence_id;
        T value;

        // Sort comparator for chronological order
        bool operator<(const TimedEntry& other) const {
            return sequence_id < other.sequence_id;
        }
    };

    using value_type = T;

    struct ThreadChunk {
        std::vector<TimedEntry> items;
    };

    ThreadLocalVector() = default;
    ~ThreadLocalVector() {
        std::lock_guard<bora::Mutex> lock(registryMutex_);
        for (auto chunk : registeredChunks_) {
            delete chunk;
        }
        registeredChunks_.clear();
    }

    // Delete copy operations
    ThreadLocalVector(const ThreadLocalVector&) = delete;
    ThreadLocalVector& operator=(const ThreadLocalVector&) = delete;

    // Move constructor / assignment
    ThreadLocalVector(ThreadLocalVector&& other) noexcept {
        std::lock_guard<bora::Mutex> lock(other.registryMutex_);
        registeredChunks_ = std::move(other.registeredChunks_);
        mergedCacheValues_ = std::move(other.mergedCacheValues_);
        cacheDirty_ = other.cacheDirty_;
    }

    ThreadLocalVector& operator=(ThreadLocalVector&& other) noexcept {
        if (this != &other) {
            std::scoped_lock lock(registryMutex_, other.registryMutex_);
            registeredChunks_ = std::move(other.registeredChunks_);
            mergedCacheValues_ = std::move(other.mergedCacheValues_);
            cacheDirty_ = other.cacheDirty_;
        }
        return *this;
    }

    void MarkDirty() const {
        cacheDirty_ = true;
    }

    void push_back(const T& value) {
        MarkDirty();
        uint64_t seq = globalSequenceCounter_.fetch_add(1, std::memory_order_relaxed);
        GetLocalChunk().items.push_back(TimedEntry{seq, value});
    }

    void push_back(T&& value) {
        MarkDirty();
        uint64_t seq = globalSequenceCounter_.fetch_add(1, std::memory_order_relaxed);
        GetLocalChunk().items.push_back(TimedEntry{seq, std::move(value)});
    }

    void push_back(const T* dataPtr, std::size_t count) {
        if (!dataPtr || count == 0) return;
        MarkDirty();
        auto& items = GetLocalChunk().items;

        uint64_t base_seq = globalSequenceCounter_.fetch_add(count, std::memory_order_relaxed);
        for (std::size_t i = 0; i < count; ++i) {
            items.push_back(TimedEntry{base_seq + i, dataPtr[i]});
        }
    }

    std::optional<value_type> pop_back() {
        MarkDirty();
        auto& chunk = GetLocalChunk();
        if (chunk.items.empty()) return std::nullopt;

        value_type val = std::move(chunk.items.back().value);
        chunk.items.pop_back();
        return val;
    }

    std::size_t size() const {
        std::lock_guard<bora::Mutex> lock(registryMutex_);
        if (!cacheDirty_ && !mergedCacheValues_.empty()) {
            return mergedCacheValues_.size();
        }

        std::size_t total = 0;
        for (const auto* chunk : registeredChunks_) {
            if (chunk) total += chunk->items.size();
        }
        return total;
    }

    // Returns raw T pointer representing the chronologically sorted commands
   const value_type* data() const {
    std::lock_guard lock(registryMutex_);
    if (!cacheDirty_ && !mergedCacheValues_.empty()) {
        return mergedCacheValues_.data();
    }

    // 1. Filter out empty chunks and calculate total size
    std::vector<const std::vector<TimedEntry>*> activeChunks;
    activeChunks.reserve(registeredChunks_.size());

    std::size_t totalSize = 0;
    for (const auto* chunk : registeredChunks_) {
        if (chunk && !chunk->items.empty()) {
            activeChunks.push_back(&chunk->items);
            totalSize += chunk->items.size();
        }
    }

    // Pre-allocate the final vector size ONCE upfront to eliminate push_back overhead
    mergedCacheValues_.resize(totalSize);
    if (totalSize == 0) {
        cacheDirty_ = false;
        return mergedCacheValues_.data();
    }

    // Fast path: If only one chunk has data, just copy values directly
    if (activeChunks.size() == 1) {
        std::size_t writeIdx = 0;
        for (const auto& entry : *activeChunks[0]) {
            if constexpr (std::is_copy_constructible_v<value_type>) {
                mergedCacheValues_[writeIdx++] = entry.value;
            } else {
                mergedCacheValues_[writeIdx++] = std::move(const_cast<value_type&>(entry.value));
            }
        }
        cacheDirty_ = false;
        return mergedCacheValues_.data();
    }


    // 2. K-Way Merge using a Min-Heap (Priority Queue)
    struct HeapNode {
        std::vector<TimedEntry>::const_iterator current;
        std::vector<TimedEntry>::const_iterator end;
        bool operator>(const HeapNode& other) const {
            return current->sequence_id > other.current->sequence_id;
        }
    };

    std::priority_queue<HeapNode, std::vector<HeapNode>, std::greater<HeapNode>> minHeap;
    for (const auto* itemsPtr : activeChunks) {
        minHeap.push(HeapNode{itemsPtr->begin(), itemsPtr->end()});
    }

    std::size_t writeIdx = 0;
    while (!minHeap.empty()) {
        HeapNode node = minHeap.top();
        minHeap.pop();

        // Direct index writing instead of push_back()
        if constexpr (std::is_copy_constructible_v<value_type>) {
            mergedCacheValues_[writeIdx++] = node.current->value;
        } else {
            mergedCacheValues_[writeIdx++] = std::move(const_cast<value_type&>(node.current->value));
        }

        // Advance the iterator for that chunk and push back if elements remain
        auto nextIt = node.current + 1;
        if (nextIt != node.end) {
            minHeap.push(HeapNode{nextIt, node.end});
        }
    }

    cacheDirty_ = false;
    return mergedCacheValues_.data();
    }

    value_type* data() {
        return const_cast<value_type*>(std::as_const(*this).data());
    }

    void clear() {
        std::lock_guard<bora::Mutex> lock(registryMutex_);
        for (auto* chunk : registeredChunks_) {
            if (chunk) chunk->items.clear();
        }
        cacheDirty_ = true;
    }

    template<typename Func>
    void access(Func&& f) {
        data(); // Ensures cache is fresh and sorted
        if (!mergedCacheValues_.empty()) {
            f(mergedCacheValues_);
        }
    }

private:
    inline static std::atomic<uint64_t> globalSequenceCounter_{0};

    struct ThreadRegistration {
        std::list<ThreadChunk*> chunks;
        ThreadLocalVector* owner;

        void registerThread(ThreadLocalVector* parent) {
            owner = parent;
            registerChunk();
        }

        void registerChunk()
        {
            std::lock_guard<bora::Mutex> lock(owner->registryMutex_);
            auto chunk = new ThreadChunk();
            chunk->items.reserve(2048);
            chunks.push_back(chunk);
            owner->registeredChunks_.push_back(chunk);
        }

        ThreadChunk* GetCurrentChunk()
        {
            return chunks.back();
        }

        ThreadRegistration() = default;
        ThreadRegistration(ThreadLocalVector* parent) {
            registerThread(parent);
        }
    };

    bora::ThreadLocal<ThreadRegistration> localReg;
    bora::ThreadLocal<int> isRegistered{0};

    ThreadChunk& GetLocalChunk() {
        if (isRegistered == 0) {
            localReg->registerThread(this);
            isRegistered = 1;
        }
        auto chunk = localReg->GetCurrentChunk();
        if (chunk->items.size() >= 2048)
        {
            localReg->registerChunk();
        }
        return *localReg->GetCurrentChunk();
    }

    mutable bora::Mutex registryMutex_;
    std::list<ThreadChunk*> registeredChunks_;
    mutable std::vector<value_type> mergedCacheValues_;
    mutable bool cacheDirty_ = true;
};


