#pragma once

#include <type_traits>

template<class T>
struct Q
{
    constexpr Q() noexcept(std::is_nothrow_copy_constructible_v<T>) = default;
};

