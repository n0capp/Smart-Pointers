#pragma once

#include <memory>
#include <type_traits>

template <typename F, typename S>
static constexpr bool kUseEbo =
    std::is_empty_v<F> && !std::is_final_v<F> && !std::is_base_of_v<F, S>;
template <typename F, typename S, bool gf = kUseEbo<F, S>, bool gs = kUseEbo<S, F>>
struct CompressedPair;

template <typename F, typename S>
struct CompressedPair<F, S, false, false> {
    CompressedPair() : f_(F()), s_(S()) {
    }

    CompressedPair(F&& f, S&& s) : f_(std::move(f)), s_(std::move(s)) {
    }

    CompressedPair(const F& f, const S& s) : f_(f), s_(s) {
    }

    CompressedPair(F&& f, const S& s) : f_(std::move(f)), s_(s) {
    }

    CompressedPair(const F& f, S&& s) : f_(f), s_(std::move(s)) {
    }

    CompressedPair& operator=(CompressedPair& other) {
        f_ = other.f_;
        s_ = other.s_;
    }

    CompressedPair& operator=(CompressedPair&& other) {
        f_ = std::move(other.f_);
        s_ = std::move(other.s_);
    }
    F& GetFirst() {
        return f_;
    }
    S& GetSecond() {
        return s_;
    };

    const F& GetFirst() const {
        return f_;
    }

    const S& GetSecond() const {
        return s_;
    };

private:
    F f_;
    S s_;
};

template <typename F, typename S>
struct CompressedPair<F, S, false, true> : S {
    CompressedPair() : f_(F()) {
    }

    CompressedPair(F&& f, S&& s) : f_(std::move(f)) {
    }

    CompressedPair(const F& f, const S& s) : f_(f) {
    }

    CompressedPair(F&& f, const S& s) : f_(std::move(f)) {
    }

    CompressedPair(const F& f, S&& s) : f_(f) {
    }

    CompressedPair& operator=(CompressedPair& other) {
        f_ = other.f_;
    }

    CompressedPair& operator=(CompressedPair&& other) {
        f_ = std::move(other.f_);
    }

    F& GetFirst() {
        return f_;
    }
    S& GetSecond() {
        return *this;
    };

    const F& GetFirst() const {
        return f_;
    }

    const S& GetSecond() const {
        return *this;
    };

private:
    F f_;
};

template <typename F, typename S>
struct CompressedPair<F, S, true, false> : F {
    CompressedPair() : s_(S()) {
    }

    CompressedPair(F&& f, S&& s) : s_(std::move(s)) {
    }

    CompressedPair(const F& f, const S& s) : s_(s) {
    }

    CompressedPair(F&& f, const S& s) : s_(s) {
    }

    CompressedPair(const F& f, S&& s) : s_(std::move(s)) {
    }

    CompressedPair& operator=(CompressedPair& other) {
        s_ = other.s_;
    }

    CompressedPair& operator=(CompressedPair&& other) {
        s_ = std::move(other.s_);
    }
    F& GetFirst() {
        return *this;
    }
    S& GetSecond() {
        return s_;
    };

    const F& GetFirst() const {
        return *this;
    }

    const S& GetSecond() const {
        return s_;
    };

private:
    S s_;
};

template <typename F, typename S>
struct CompressedPair<F, S, true, true> : F, S {
    CompressedPair() {
    }

    CompressedPair(F&& f, S&& s) {
    }

    CompressedPair(const F& f, const S& s) {
    }

    CompressedPair(F&& f, const S& s) {
    }

    CompressedPair(const F& f, S&& s) {
    }

    CompressedPair& operator=(CompressedPair& other) {
    }

    CompressedPair& operator=(CompressedPair&& other) {
    }
    F& GetFirst() {
        return *this;
    }
    S& GetSecond() {
        return *this;
    };

    const F& GetFirst() const {
        return *this;
    }

    const S& GetSecond() const {
        return *this;
    };
};