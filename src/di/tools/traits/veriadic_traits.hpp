#pragma once

#include <string>
#include <tuple>
#include <utility>
#include <vector>


namespace di { namespace tools {

template <typename... args_types>
struct argument_types
{
    static constexpr size_t count = sizeof...(args_types);

    static std::string name(size_t index);

    template <std::size_t N>
    struct at
    {
        using type = typename std::tuple_element<N, std::tuple<args_types...>>::type;
    };
};

} }

#include "veriadic_traits.ipp"

