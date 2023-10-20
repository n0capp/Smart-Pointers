#pragma once

#include <cstddef>
#include <utility>
#include <iostream>

class SimpleCounter {
public:
    size_t IncRef() {
        ++count_;
        return count_;
    };

    size_t DecRef() {
        --count_;
        return count_;
    };

    size_t RefCount() const {
        return count_;
    };

    size_t count_ = 0;
};

struct DefaultDelete {
    template<typename T>
    static void Destroy(T *object) {
        delete object;
    }
};

template<typename Derived, typename Counter, typename Deleter>
class RefCounted {
public:
    void IncRef() {
        counter_.IncRef();
    };

    void DecRef() {
        counter_.DecRef();
        if (counter_.RefCount() == 0) {
            Deleter::Destroy(static_cast<Derived *>(this));
        }
    };

    size_t RefCount() const {
        return counter_.RefCount();
    };

    RefCounted &operator=(RefCounted &other) {
        return *this;
    }

    RefCounted &operator=(RefCounted &&other) {
        return *this;
    }

private:
    Counter counter_;
};

template<typename Derived, typename D = DefaultDelete>
using SimpleRefCounted = RefCounted<Derived, SimpleCounter, D>;

template<typename T>
class IntrusivePtr {
    template<typename Y>
    friend
    class IntrusivePtr;

public:
    IntrusivePtr() {
        ptr_ = nullptr;
    };

    IntrusivePtr(std::nullptr_t) {
        ptr_ = nullptr;
    };

    explicit IntrusivePtr(T *ptr) {
        ptr_ = ptr;
        if (ptr_ != nullptr) {
            ptr_->IncRef();
        }
    };

    template<typename Y>
    IntrusivePtr(const IntrusivePtr<Y> &other) {
        ptr_ = other.ptr_;
        if (ptr_ != nullptr) {
            ptr_->IncRef();
        }
    };

    template<typename Y>
    IntrusivePtr(IntrusivePtr<Y> &&other) {
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
    };

    IntrusivePtr(const IntrusivePtr &other) {
        ptr_ = other.ptr_;
        if (ptr_ != nullptr) {
            ptr_->IncRef();
        }
    };

    IntrusivePtr(IntrusivePtr &&other) {
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
    };

    template<typename U>
    IntrusivePtr &operator=(const IntrusivePtr<U> &other) {
        if (ptr_ == other.ptr_) {
            return *this;
        }
        if (ptr_ != nullptr) {
            ptr_->DecRef();
            ptr_ = nullptr;
        }
        ptr_ = other.ptr_;
        if (ptr_ != nullptr) {
            ptr_->IncRef();
        }
        return *this;
    };

    IntrusivePtr &operator=(const IntrusivePtr &other) {
        if (ptr_ == other.ptr_) {
            return *this;
        }
        if (ptr_ != nullptr) {
            ptr_->DecRef();
            ptr_ = nullptr;
        }
        ptr_ = other.ptr_;
        if (ptr_ != nullptr) {
            ptr_->IncRef();
        }
        return *this;
    };

    template<typename U>
    IntrusivePtr &operator=(IntrusivePtr<U> &&other) {
        if (ptr_ == other.ptr_) {
            return *this;
        }
        if (ptr_ != nullptr) {
            ptr_->DecRef();
            ptr_ = nullptr;
        }
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
        return *this;
    };

    IntrusivePtr &operator=(IntrusivePtr &&other) {
        if (ptr_ == other.ptr_) {
            return *this;
        }
        if (ptr_ != nullptr) {
            ptr_->DecRef();
            ptr_ = nullptr;
        }
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
        return *this;
    };

    ~IntrusivePtr() {
        if (ptr_ != nullptr) {
            ptr_->DecRef();
        }
        ptr_ = nullptr;
    };

    void Reset() {
        if (ptr_ != nullptr) {
            ptr_->DecRef();
        }
        ptr_ = nullptr;
    };

    template<typename U>
    void Reset(U *ptr) {
        if (ptr_ != nullptr) {
            ptr_->DecRef();
        }
        ptr_ = nullptr;
        ptr_ = ptr;
        if (ptr_ != nullptr) {
            ptr_->IncRef();
        }
    };

    void Swap(IntrusivePtr &other) {
        std::swap(ptr_, other.ptr_);
    };

    // Observers
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
        if (ptr_ == nullptr) {
            return 0;
        }
        return ptr_->RefCount();
    };

    explicit operator bool() const {
        return ptr_ != nullptr;
    };

private:
    T *ptr_;
};

template<typename T, typename... Args>
IntrusivePtr<T> MakeIntrusive(Args &&... args) {
    return IntrusivePtr<T>(new T(std::forward<Args>(args)...));
};
