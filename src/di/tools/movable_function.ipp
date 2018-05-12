#pragma once

#include "movable_function.hpp"

#include <type_traits>
#include <typeindex>

namespace di { namespace tools {

template <typename callable_type>
std::type_index target_type(const callable_type& callable)
{
    return std::type_index(typeid(callable_type));
}

template <typename return_type, typename...args_types>
std::type_index target_type(movable_function<return_type(args_types...)>& function)
{
    return function.target_type();
};

template <typename return_type, typename...args_types>
std::type_index target_type(const movable_function<return_type(args_types...)>& function)
{
    return function.target_type();
};

} }
