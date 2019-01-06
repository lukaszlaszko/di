#pragma once

#include "ctor_traits.hpp"

#include <algorithm>
#include <type_traits>


namespace di { namespace tools {

template <typename T, size_t... indices>
inline constexpr bool try_construct(std::integer_sequence<size_t, indices...>)
{
    return std::is_constructible<T, arg_placeholder<T, indices>...>::value;
}

template <typename T, size_t N>
inline constexpr bool has_constructor()
{
    using sequence = std::make_integer_sequence<size_t, N>;
    return try_construct<T>(sequence());
}

template <typename T, size_t... counts>
inline constexpr auto ctor_args_detect(std::integer_sequence<size_t, counts...>)
{
    constexpr size_t count_size = sizeof...(counts);
    std::array<bool, count_size> tested { has_constructor_n<T, counts>::value... };

    return tested;
}

template <typename T, size_t... counts>
inline constexpr size_t ctor_args_count_detect(std::integer_sequence<size_t, counts...>)
{
    constexpr size_t count_size = sizeof...(counts);
    bool detected[count_size] { has_constructor_n<T, counts>::value... };

    for (auto i = 0ul; i < count_size; i++)
        if (detected[i])
            return i;

    return count_size;
}

template <typename T, size_t max_args>
inline constexpr size_t ctor_args_count()
{
    using sequence = std::make_integer_sequence<size_t, max_args>;
    return ctor_args_count_detect<T>(sequence());
}

template <typename T, size_t... counts>
inline constexpr size_t ctor_count_detect(std::integer_sequence<size_t, counts...>)
{
    constexpr size_t count_size = sizeof...(counts);
    bool detected[count_size] { has_constructor_n<T, counts>::value... };

    auto count = 0ul;
    for (auto i = 0ul; i < count_size; i++)
        if (detected[i])
            count++;

    return count;
}


template <typename T, size_t max_args>
inline constexpr size_t ctor_count()
{
    using sequence = std::make_integer_sequence<size_t, max_args>;
    return ctor_count_detect<T>(sequence());
}

} }