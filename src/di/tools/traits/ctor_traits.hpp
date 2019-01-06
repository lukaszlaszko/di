#pragma once

#include <array>
#include <utility>


namespace di { namespace tools {

/**
 * @brief Argument placeholder simulates presence of an argument to SFINE.
 * @details
 * @paragraph
 * Definition of conversion operators in a placeholder presents constraints on a form of argument passing formats allowed
 * for constructor arguments which can be resolves through automated type definition and constructor resolution.
 *
 * @paragraph
 * A valid constructor argument can be passed only through a value or rvalue. It is not allowed to pass autoresolved argument
 * through a reference or const reference. For this reason **operator U&() const** and **operator const U&() const** arent
 * specified.
 *
 * @pargraph
 * In order to avoid circular references, it is not allowed to define argument of same type as constructed type.
 *
 * @tparam T Type of constructed object this is an argument of.
 * @tparam index Argument index. (internal use only)
 */
template <class T, size_t index>
struct arg_placeholder
{
    template <class U=T, typename = typename std::enable_if_t<!std::is_same<U, T>::value && !std::is_reference<U>::value>>
    operator U();
};

template <typename T, size_t... indices>
constexpr bool try_construct(std::integer_sequence<size_t, indices...>);

template <typename T, size_t N>
constexpr bool has_constructor();

template <typename T, size_t N>
struct has_constructor_n;

template <typename T, size_t N>
struct has_constructor_n
{
    static constexpr bool value = has_constructor<T, N>();
};

/**
 * @briefs Produces an sequence with boolean values indicating if given type presents a non-copy,
 * non-move constructor with number of arguments corresposing to array index.
 * @return A sequence of constructors presence indicators.
 */
template <typename T, size_t... counts>
constexpr auto ctor_args_detect(std::integer_sequence<size_t, counts...>);

/**
 * @brief Retrieves smallest size of argument set required by a constructor of type T.
 * @tparam T Type which constructors should be evaluated.
 * @tparam max_args Maximum number of arguments to check.
 * @return Number of arguments of the smallest (the least arguments) constructor of the given type **T**.
 */
template <typename T, size_t max_args = 256ul>
constexpr size_t ctor_args_count();

template <typename T>
struct ctor_args_count_n
{
    static const bool value = ctor_args_count<T>();
};

/**
 * @brief Based on **ctor_args_detect**, counts number of positive elements of returned array.
 * @tparam T Type which constructors should be evaluated.
 * @tparam max_args Maximum number of arguments to consider.
 * @return Number of suitable constructors available.
 */
template <typename T, size_t max_args = 256ul>
constexpr size_t ctor_count();

template <typename T>
struct ctor_count_n
{
    static constexpr bool value = ctor_count<T>();
};

} }

#include "ctor_traits.ipp"
