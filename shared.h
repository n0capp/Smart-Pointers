#pragma once

#include "sw_fwd.h"

#include <cstddef>
#include <iostream>

class ControlBlockBase {
public:
    ControlBlockBase() : weak_cnt_(0), strong_cnt_(0) {
    }

    virtual void DecWeakCnt() {
    }

    virtual void IncWeakCnt() {
    }

    virtual void IncStrongCnt() {
    }

    virtual void DecStrongCnt() {
    }

    virtual ~ControlBlockBase() {
    }

    int weak_cnt_;
    int strong_cnt_;
};

class ESFTBase;

template<typename T>
class ControlBlockAsIs : public ControlBlockBase {
public:
    template<typename... Args>
    ControlBlockAsIs(Args &&... args) : ControlBlockBase() {
        new(GetPointer()) T(std::forward<Args>(args)...);
    }

    void DecStrongCnt() override {
        if (strong_cnt_ == 1) {
            reinterpret_cast<T *>(std::addressof(storage_))->~T();
            if (weak_cnt_ == 0) {
                delete this;
            } else {
                --strong_cnt_;
            }
        } else {
            --strong_cnt_;
        }
    }

    void IncStrongCnt() override {
        ++strong_cnt_;
    }

    void DecWeakCnt() override {
        --weak_cnt_;
        if (weak_cnt_ == 0 && strong_cnt_ == 0) {
            delete this;
        }
    }

    void IncWeakCnt() override {
        ++weak_cnt_;
    }

    ~ControlBlockAsIs() override {
    }

    T *GetPointer() {
        return reinterpret_cast<T *>(std::addressof(storage_));
    }

    T &Get() {
        return *GetPointer();
    }

    std::aligned_storage_t<sizeof(T), alignof(T)> storage_;
};

template<typename T>
class ControlBlockWithPointer : public ControlBlockBase {
public:
    ControlBlockWithPointer(T *obj_ptr) : ControlBlockBase(), obj_ptr_(obj_ptr) {
    }

    virtual ~ControlBlockWithPointer() {
    }

    T &Get() {
        return *obj_ptr_;
    }

    void DecStrongCnt() override {
        if (strong_cnt_ == 1) {
            delete obj_ptr_;
            if (weak_cnt_ == 0) {
                delete this;
            } else {
                --strong_cnt_;
            }
        } else {
            --strong_cnt_;
        }
    }

    void IncStrongCnt() override {
        ++strong_cnt_;
    }

    void DecWeakCnt() override {
        --weak_cnt_;
        if (weak_cnt_ == 0 && strong_cnt_ == 0) {
            delete this;
        }
    }

    void IncWeakCnt() override {
        ++weak_cnt_;
    }

    T *obj_ptr_;
};

template<typename T>
class SharedPtr {
public:
    template<typename U>
    friend
    class WeakPtr;

    friend class WeakPtr<T>;

    template<typename U>
    friend
    class SharedPtr;

    friend class WeakPtr<T>;

    template<typename U, typename... Args>
    friend SharedPtr<U> MakeShared(Args &&... args);

    SharedPtr() {
        block_ = nullptr;
        ptr_ = nullptr;
    };

    SharedPtr(std::nullptr_t) {
        block_ = nullptr;
        ptr_ = nullptr;
    };

    template<typename U>
    explicit SharedPtr(U *ptr) {
        block_ = new ControlBlockWithPointer<U>(ptr);
        block_->IncStrongCnt();
        ptr_ = ptr;
        if constexpr (std::is_convertible_v<T *, ESFTBase *>) {

            if (ptr_ != nullptr) {
                ptr_->weak_this_ = *this;
            }
        }
    }

    template<typename U>
    SharedPtr(const SharedPtr<U> &other) {
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->IncStrongCnt();
        }
        ptr_ = other.ptr_;
    };

    SharedPtr(const SharedPtr &other) {
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->IncStrongCnt();
        }
        ptr_ = other.ptr_;
    };

    SharedPtr(ControlBlockBase *block, T *ptr) : block_(block), ptr_(ptr) {
        if constexpr (std::is_convertible_v<T *, ESFTBase *>) {
            if (ptr_ != nullptr) {
                ptr_->weak_this_ = *this;
            }
        }
        if (block_ != nullptr) {
            block_->IncStrongCnt();
        }
    }

    template<typename U>
    SharedPtr(SharedPtr<U> &&other) {
        block_ = other.block_;
        ptr_ = other.ptr_;
        other.block_ = nullptr;
        other.ptr_ = nullptr;
    };

    template<typename Y>
    SharedPtr(const SharedPtr<Y> &other, T *ptr) {
        ptr_ = ptr;
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->IncStrongCnt();
        }
    };

    explicit SharedPtr(const WeakPtr<T> &other) {
        if (other.Expired()) {
            throw BadWeakPtr();
        }
        block_ = other.block_;
        ptr_ = other.ptr_;
        if (block_ != nullptr) {
            block_->IncStrongCnt();
        }
    };

    template<typename U>
    SharedPtr &operator=(const SharedPtr<U> &other) {
        if (ptr_ == other.ptr_) {
            return *this;
        }
        if (block_ != nullptr) {
            block_->DecStrongCnt();
        }
        block_ = nullptr;
        ptr_ = nullptr;
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->IncStrongCnt();
        }
        ptr_ = other.ptr_;
        return *this;
    };

    SharedPtr &operator=(const SharedPtr &other) {
        if (ptr_ == other.ptr_) {
            return *this;
        }
        if (block_ != nullptr) {
            block_->DecStrongCnt();
        }
        block_ = nullptr;
        ptr_ = nullptr;
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->IncStrongCnt();
        }
        ptr_ = other.ptr_;
        return *this;
    };

    template<typename U>
    SharedPtr &operator=(SharedPtr<U> &&other) {
        if (ptr_ == other.ptr_) {
            return *this;
        }
        if (block_ != nullptr) {
            block_->DecStrongCnt();
        }
        block_ = nullptr;
        ptr_ = nullptr;
        block_ = other.block_;
        ptr_ = other.ptr_;
        other.block_ = nullptr;
        other.ptr_ = nullptr;
        return *this;
    };


    ~SharedPtr() {
        if (block_ != nullptr) {
            block_->DecStrongCnt();
        }
        block_ = nullptr;
        ptr_ = nullptr;
    };


    void Reset() {
        if (block_ != nullptr) {
            block_->DecStrongCnt();
        }
        block_ = nullptr;
        ptr_ = nullptr;
    };

    template<typename U>
    void Reset(U *ptr) {
        if (block_ != nullptr) {
            block_->DecStrongCnt();
        }
        block_ = nullptr;
        ptr_ = nullptr;
        block_ = new ControlBlockWithPointer<U>(ptr);
        block_->IncStrongCnt();
        ptr_ = ptr;
    };

    void Swap(SharedPtr &other) {
        std::swap(block_, other.block_);
        std::swap(ptr_, other.ptr_);
    };


    T *Get() const {
        return ptr_;
    };

    T &operator*() const {
        return *ptr_;
    };

    T *operator->() const {
        return ptr_;
    };

    size_t UseCount() const {
        if (block_ == nullptr) {
            return 0;
        }
        return block_->strong_cnt_;
    };

    explicit operator bool() const {
        return ptr_ != nullptr;
    };

private:
    template<typename U>
    friend
    class WeakPtr;

    friend class WeakPtr<T>;

    template<typename S, typename V>
    friend inline bool operator==(const SharedPtr<S> &left, const SharedPtr<V> &right);

    ControlBlockBase *block_;
    T *ptr_;
};

template<typename S, typename V>
inline bool operator==(const SharedPtr<S> &left, const SharedPtr<V> &right) {
    return left.ptr_ == right.ptr_;
};

template<typename U, typename... Args>
SharedPtr<U> MakeShared(Args &&... args) {
    SharedPtr<U> sp;
    auto block = new ControlBlockAsIs<U>(std::forward<Args>(args)...);
    sp.block_ = block;
    sp.ptr_ = block->GetPointer();
    block->IncStrongCnt();
    if constexpr (std::is_convertible_v<U *, ESFTBase *>) {
        if (sp.ptr_ != nullptr) {
            sp.ptr_->weak_this_ = sp;
        }
    }
    return sp;
};

class ESFTBase {
};

template<typename T>
class EnableSharedFromThis : public ESFTBase {
public:
    SharedPtr<T> SharedFromThis() {
        return SharedPtr<T>(weak_this_.block_, weak_this_.ptr_);
    };

    SharedPtr<const T> SharedFromThis() const {
        return SharedPtr<const T>(weak_this_.block_, weak_this_.ptr_);
    };

    WeakPtr<T> WeakFromThis() noexcept {
        return WeakPtr<T>(weak_this_.block_, weak_this_.ptr_);
    };

    WeakPtr<const T> WeakFromThis() const noexcept {
        return WeakPtr<const T>(weak_this_.block_, weak_this_.ptr_);
    };

    template<typename U>
    friend
    class WeakPtr;

    template<typename U>
    friend
    class SharedPtr;

    template<typename U>
    friend
    class EnableSharedFromThis;

    template<typename U, typename... Args>
    friend SharedPtr<U> MakeShared(Args &&... args);

protected:
    WeakPtr<T> weak_this_;
};
