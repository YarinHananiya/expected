/*******************************************************************************
 * expected<Expected Type, Exception Type>
 * can used as a return type from a function that can fail, instead of throwing
 * an exception.
 * Author: Yarin Hananiya
 * ****************************************************************************/
#ifndef EXPECTED_HPP
#define EXPECTED_HPP

#include <cassert>     // assert
#include <type_traits> // std::enable_if_t
#include <variant>     // std::variant

#include "unexpected.hpp" // unexpected

// clang-format off

template<typename T, typename... Args>
struct First { using type = T; };

template<typename T, typename E>
class [[nodiscard]] expected {
public:
    template<typename = std::enable_if_t<std::is_default_constructible_v<T>>>
    explicit expected();
    explicit expected(const T& t);
    explicit expected(T&& t);
    explicit expected(const unexpected<E>& e);
    explicit expected(unexpected<E>&& e);
    template<typename... Args, 
             typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    explicit expected(Args&& ... args);
    expected(const expected&) = default;
    expected(expected&&) = default;
    expected& operator=(const expected&) = default;
    expected& operator=(expected&&) = default;
    ~expected() = default;

    template<typename U>
    expected(const expected<T, U>& other);
    template<typename U>
    expected(expected<T, U>&& other);

    auto has_value() const noexcept -> bool;
    explicit operator bool() const noexcept;
    auto swap(expected & other) noexcept -> void;

    // these functions throw E if has_value == false
    auto value() & -> T&;
    auto value() const & -> const T&;
    auto value() && -> T&&;
    auto value() const && -> const T&&;

    // undefined behavior if has_value == true
    auto error() & -> E&;
    auto error() const & -> const E&;
    auto error() && -> E&&;
    auto error() const && -> const E&&;

private:
    std::variant<T, unexpected<E>> m_value_or_error;

    template<typename T_, typename U>
    friend class expected;
};

template<typename T, typename E>
template<typename>
expected<T, E>::expected() : m_value_or_error() {
}

template<typename T, typename E>
expected<T, E>::expected(const T& t) : m_value_or_error(t) {
}

template<typename T, typename E>
expected<T, E>::expected(T&& t) : m_value_or_error(std::move(t)) {
}

template<typename T, typename E>
expected<T, E>::expected(const unexpected<E>& e) : m_value_or_error(e) {
}

template<typename T, typename E>
expected<T, E>::expected(unexpected<E>&& e) : m_value_or_error(std::move(e)) {
}

template<typename T, typename E>
template<typename... Args, typename>
expected<T, E>::expected(Args&&... args)
  : m_value_or_error(std::in_place_type_t<T>(), std::forward<Args>(args)...) {
}

template<typename T, typename E>
template<typename U>
expected<T, E>::expected(const expected<T, U>& other) {
    if (other.has_value()) {
        std::get<T>(m_value_or_error) = std::get<T>(other.m_value_or_error);
    } else {
        m_value_or_error = std::get<unexpected<U>>(other.m_value_or_error);
    }
}

template<typename T, typename E>
template<typename U>
expected<T, E>::expected(expected<T, U>&& other) {
    if (other.has_value()) {
        std::get<T>(m_value_or_error) = std::get<T>(std::move(other.m_value_or_error));
    } else {
        m_value_or_error = std::get<unexpected<U>>(std::move(other.m_value_or_error));
    }
}


template<typename T, typename E>
auto expected<T, E>::has_value() const noexcept -> bool {
    return std::holds_alternative<T>(m_value_or_error);
}

template<typename T, typename E>
expected<T, E>::operator bool() const noexcept {
    return has_value();
}

template<typename T, typename E>
auto expected<T, E>::value() & -> T& {
    if (!has_value()) {
        throw std::get<unexpected<E>>(m_value_or_error).get();
    }

    return std::get<T>(m_value_or_error);
}

template<typename T, typename E>
auto expected<T, E>::value() const& -> const T& {
    return (const_cast<expected&>(*this)).value();
}

template<typename T, typename E>
auto expected<T, E>::value() && -> T&& {
    if (!has_value()) {
        throw std::get<unexpected<E>>(std::move(m_value_or_error)).get();
    }

    return std::get<T>(std::move(m_value_or_error));
}

template<typename T, typename E>
auto expected<T, E>::value() const&& -> const T&& {
    return (const_cast<expected&&>(*this)).value();
}

template<typename T, typename E>
auto expected<T, E>::error() & -> E& {
    assert(!has_value());
    return std::get<unexpected<E>>(m_value_or_error).get();
}

template<typename T, typename E>
auto expected<T, E>::error() const & -> const E& {
    return (const_cast<expected&>(*this)).error();
}

template<typename T, typename E>
auto expected<T, E>::error() && -> E&& {
    assert(!has_value());
    return std::get<unexpected<E>>(std::move(m_value_or_error)).get();
}

template<typename T, typename E>
auto expected<T, E>::error() const && -> const E&& {
    return (const_cast<expected&&>(*this)).error();
}

template<typename T, typename E>
auto expected<T, E>::swap(expected& other) noexcept -> void {
    m_value_or_error.swap(other.m_value_or_error);
}

// clang-format on

#endif