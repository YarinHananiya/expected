/*******************************************************************************
 * helper class for expected, to distinguish between Expected and Exception
 * when they are the same type.
 * Author: Yarin Hananiya
 * ****************************************************************************/
#ifndef UNEXPECTED_HPP
#define UNEXPECTED_HPP

#include <functional> // std::reference_wrapper

// clang-format off

template<typename Exception>
class unexpected {
public:
    /* non-explicit */ unexpected(const Exception& e);
    unexpected(const unexpected&) = default;
    unexpected(unexpected&&) = default;
    unexpected& operator=(const unexpected&) = default;
    unexpected& operator=(unexpected&&) = default;
    ~unexpected() = default;

    template<typename OtherException>
    unexpected(const unexpected<OtherException>& other);
    template<typename OtherException>
    unexpected(unexpected<OtherException>&& other);

    auto get() & noexcept -> Exception&;
    auto get() const & noexcept -> const Exception&;
    auto get() && noexcept -> Exception&&;
    auto get() const && noexcept -> const Exception&&;

private:
    Exception m_e;

    template<typename OtherException>
    friend class unexpected;
};

template<typename Exception>
unexpected<Exception>::unexpected(const Exception& e) : m_e(e) {
}

template<typename Exception>
template<typename OtherException>
unexpected<Exception>::unexpected(const unexpected<OtherException>& other)
  : m_e(other.m_e) {
}

template<typename Exception>
template<typename OtherException>
unexpected<Exception>::unexpected(unexpected<OtherException>&& other)
  : m_e(std::move(other.m_e)) {
}

template<typename Exception>
auto unexpected<Exception>::get() & noexcept -> Exception& {
    return m_e;
}

template<typename Exception>
auto unexpected<Exception>::get() const & noexcept -> const Exception& {
    return (const_cast<unexpected&>(*this)).get();
}

template<typename Exception>
auto unexpected<Exception>::get() && noexcept -> Exception&& {
    return std::move(m_e);
}

template<typename Exception>
auto unexpected<Exception>::get() const && noexcept -> const Exception&& {
    return (const_cast<unexpected&&>(*this)).get();
}

// clang-format on

#endif
