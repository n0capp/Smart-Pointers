#pragma once

#include "compressed_pair.h"

#include <cstddef>

struct Slug {
    template<typename T>
    void operator()(T *) {
    }
};


template<typename T, typename Deleter = Slug>
class UniquePtr {
public:
    UniquePtr(T *ptr = nullptr) {
        data_.GetFirst() = ptr;
    };

    UniquePtr(T *ptr, Deleter deleter)
            : data_(ptr, std::move(deleter)) {

    };

    template<typename A, typename B>
    UniquePtr(UniquePtr<A, B> &&other) noexcept {
        data_.GetSecond() = std::move(other.GetDeleter());
        data_.GetFirst() = other.Release();
    };

    template<typename A, typename B>
    UniquePtr(UniquePtr<A, B> &other) = delete;

    template<typename A, typename B>
    UniquePtr &operator=(UniquePtr<A, B> &other) = delete;

    template<typename A, typename B>
    UniquePtr &operator=(UniquePtr<A, B> &&other) noexcept {
        if (data_.GetFirst() == other.Get()) {
            return *this;
        }
        Clear();
        data_.GetFirst() = other.Release();
        data_.GetSecond() = std::move(other.GetDeleter());
        return *this;
    };

    UniquePtr &operator=(std::nullptr_t) {
        Clear();
        data_.GetFirst() = nullptr;
        return *this;
    };

    ~UniquePtr() {
        Clear();
    };

    T *Release() {
        T *ptr = data_.GetFirst();
        data_.GetFirst() = nullptr;
        return ptr;
    };

    void Reset(T *ptr = nullptr) {
        auto old_ptr = data_.GetFirst();
        data_.GetFirst() = ptr;
        if (std::is_same_v<Deleter, Slug>) {
            delete old_ptr;
        } else {
            data_.GetSecond()(old_ptr);
        }
    };

    void Swap(UniquePtr &other) {
        T *t = other.data_.GetFirst();
        other.data_.GetFirst() = data_.GetFirst();
        data_.GetFirst() = t;
        std::swap(data_.GetSecond(), other.data_.GetSecond());
    };

    T *Get() const {
        return data_.GetFirst();
    };

    Deleter &GetDeleter() {
        return data_.GetSecond();
    };

    const Deleter &GetDeleter() const {
        return data_.GetSecond();
    };

    explicit operator bool() const {
        return data_.GetFirst() != nullptr;
    };


    std::add_lvalue_reference_t<T> operator*() const {
        return *data_.GetFirst();
    };

    T *operator->() const {
        return data_.GetFirst();
    };

private:
    CompressedPair<T *, Deleter> data_;

    void Clear() {
        if (std::is_same_v<Deleter, Slug>) {
            delete data_.GetFirst();
        } else {
            data_.GetSecond()(data_.GetFirst());
        }
    }
};

// Specialization for arrays
template<typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    UniquePtr(T *ptr = nullptr) {
        data_.GetFirst() = ptr;
    };

    UniquePtr(T *ptr, Deleter deleter)
            : data_(ptr, std::move(deleter)) {

    };

    template<typename A, typename B>
    UniquePtr(UniquePtr<A, B> &&other) noexcept {
        data_.GetSecond() = std::move(other.GetDeleter());
        data_.GetFirst() = other.Release();
    };

    template<typename A, typename B>
    UniquePtr(UniquePtr<A, B> &other) = delete;

    template<typename A, typename B>
    UniquePtr &operator=(UniquePtr<A, B> &other) = delete;

    template<typename A, typename B>
    UniquePtr &operator=(UniquePtr<A, B> &&other) noexcept {
        if (data_.GetFirst() == other.Get()) {
            return *this;
        }
        Clear();
        data_.GetFirst() = other.Release();
        data_.GetSecond() = std::move(other.GetDeleter());
        return *this;
    };

    T &operator[](size_t index) {
        return *(data_.GetFirst() + index);
    }

    UniquePtr &operator=(std::nullptr_t) {
        Clear();
        data_.GetFirst() = nullptr;
        return *this;
    };

    ~UniquePtr() {
        Clear();
    };

    T *Release() {
        T *ptr = data_.GetFirst();
        data_.GetFirst() = nullptr;
        return ptr;
    };

    void Reset(T *ptr = nullptr) {
        auto old_ptr = data_.GetFirst();
        data_.GetFirst() = ptr;
        if (std::is_same_v<Deleter, Slug>) {
            delete[] old_ptr;
        } else {
            data_.GetSecond()(old_ptr);
        }
    };

    void Swap(UniquePtr &other) {
        T *t = other.data_.GetFirst();
        other.data_.GetFirst() = data_.GetFirst();
        data_.GetFirst() = t;
        std::swap(data_.GetSecond(), other.data_.GetSecond());
    };

    T *Get() const {
        return data_.GetFirst();
    };

    Deleter &GetDeleter() {
        return data_.GetSecond();
    };

    const Deleter &GetDeleter() const {
        return data_.GetSecond();
    };

    explicit operator bool() const {
        return data_.GetFirst() != nullptr;
    };


    T &operator*() const {
        return *data_.GetFirst();
    };

    T *operator->() const {
        return data_.GetFirst();
    };

private:
    CompressedPair<T *, Deleter> data_;

    void Clear() {
        if (std::is_same_v<Deleter, Slug>) {
            delete[] data_.GetFirst();
        } else {
            data_.GetSecond()(data_.GetFirst());
        }
    }
};
