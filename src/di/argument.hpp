#pragma once

#include "definition.hpp"

#include <cstdlib>
#include <utility>


namespace di {

/**
 * @brief Stores information about requested argument in explicit type definition.
 * @details
 * @paragraph
 * When a type is explicitely defined with **definition_builder::define_explicit_type** a list of constructor argument types
 * can be passed to explicitely select constructor of defined type which should be used for given definition. This type
 * can be used instead of actual constructor argument type providing a mean of passing not only type of requested argument, but
 * also identifier of desired reigstration of that type within dependency injection container.
 *
 * @paragraph
 * Consider following fragment of code:
 * @code
 *
 * [...]
 *
 * char tag_1[] = "tag_1";
 * char tag_2[] = "tag_2";
 *
 * [...]
 *
 * struct component
 * {
 *      component(int field) : field_(field)
 *      {
 *      }
 *
 *      int field_;
 *  };
 *
 *  definition_builder builder;
 *  builder.define_explicit_type<component, argument<int, tag_1>>();
 *  builder.define_instance<int>(tag_1, 5);
 *  builder.define_instance<int>(tag_2, 11);
 *
 *  instance_activator activator(std::move(builder));
 *  auto created = activator.activate_default_raii<component>();
 *
 *  [...]
 *
 * @tparam T Type of argument.
 * @tparam argument_id Registration id for requested type which should be used for this argument.
 */
template <typename T, const char* argument_id>
struct argument
{
    using type = T;
    static constexpr auto id = argument_id;
};

template <typename T>
struct argument_id
{
    static constexpr auto value = definition::default_id;
};

template <typename T, const char* id>
struct argument_id<argument<T, id>>
{
    static constexpr auto value = argument<T, id>::id;
};

template <typename T>
struct argument_type
{
    using type = T;
};

template <typename T, const char* id>
struct argument_type<argument<T, id>>
{
    using type = T;
};

}

