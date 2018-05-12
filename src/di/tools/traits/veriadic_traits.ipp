#pragma once

#include <stdexcept>
#include <tuple>
#include <typeinfo>

#include "veriadic_traits.hpp"


namespace di { namespace tools {

template <typename... args_types>
inline std::string argument_types<args_types...>::name(size_t index)
{
    std::vector<std::string> names {
        typeid(args_types).name()...
    };

    return names.at(index);
}

} }

