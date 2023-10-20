#pragma once

#include "sw_fwd.h"
#include "shared.h"

class ControlBlockBase;

template<typename T>
class WeakPtr {
public:
    template<typename U>
    friend
    class SharedPtr;

    template<typename U, typename... Args>
    friend SharedPtr<U> MakeShared(Args &&... args);

    WeakPtr() {
        block_ = nullptr;
        ptr_ = nullptr;
    };

    template<typename U>
    WeakPtr(const WeakPtr<U> &other) {
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->IncWeakCnt();
        }
        ptr_ = other.ptr_;
    };

    WeakPtr(ControlBlockBase *block, T *ptr) : block_(block), ptr_(ptr) {
        if constexpr (std::is_convertible_v<T *, ESFTBase *>) {
            if (ptr_ != nullptr) {
                ptr_->weak_this_ = *this;
            }
        }
        if (block_ != nullptr) {
            block_->IncWeakCnt();
        }
    }

    WeakPtr(const WeakPtr &other) {
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->IncWeakCnt();
        }
        ptr_ = other.ptr_;
    };

    WeakPtr(WeakPtr &&other) {
        block_ = other.block_;
        ptr_ = other.ptr_;
        other.block_ = nullptr;
        other.ptr_ = nullptr;
    };

    // Demote `SharedPtr`
    // #2 from https://en.cppreference.com/w/cpp/memory/weak_ptr/weak_ptr
    WeakPtr(const SharedPtr<T> &other) {
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->IncWeakCnt();
        }
        ptr_ = other.ptr_;
    };

    template<typename U>
    WeakPtr(const SharedPtr<U> &other) {
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->IncWeakCnt();
        }
        ptr_ = other.ptr_;
    };

    WeakPtr &operator=(const WeakPtr &other) {
        if (ptr_ == other.ptr_) {
            return *this;
        }
        if (block_ != nullptr) {
            block_->DecWeakCnt();
        }
        block_ = nullptr;
        ptr_ = nullptr;
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->IncWeakCnt();
        }
        ptr_ = other.ptr_;
        return *this;
    };

    WeakPtr &operator=(WeakPtr &&other) {
        if (ptr_ == other.ptr_) {
            return *this;
        }
        if (block_ != nullptr) {
            block_->DecWeakCnt();
        }
        block_ = nullptr;
        ptr_ = nullptr;
        block_ = other.block_;
        ptr_ = other.ptr_;
        other.block_ = nullptr;
        other.ptr_ = nullptr;
        return *this;
    };


    ~WeakPtr() {
        if (block_ != nullptr) {
            block_->DecWeakCnt();
        }
        block_ = nullptr;
        ptr_ = nullptr;
    };

    void Reset() {
        if (block_ != nullptr) {
            block_->DecWeakCnt();
        }
        block_ = nullptr;
        ptr_ = nullptr;
    };

    void Swap(WeakPtr &other) {
        std::swap(block_, other.block_);
        std::swap(ptr_, other.ptr_);
    };


    size_t UseCount() const {
        if (block_ == nullptr) {
            return 0;
        }
        return block_->strong_cnt_;
    };

    bool Expired() const {
        return UseCount() == 0;
    };

    SharedPtr<T> Lock() const {
        return Expired() ? SharedPtr<T>() : SharedPtr<T>(*this);
    };

    template<typename U>
    bool operator==(WeakPtr<U> &other) {
        return block_ == other.block_ && ptr_ == other.ptr_;
    }

    template<typename U>
    friend
    class EnableSharedFromThis;

private:
    template<typename U>
    friend
    class SharedPtr;

    ControlBlockBase *block_;
    T *ptr_;
};