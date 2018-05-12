#pragma once

#include <string>


namespace di {

/**
 * @brief Tagged annotation.
 * @details
 * Tagged annotation template generates a wrapper type uniquely separates different annotaitons of the same type within
 * activation context. For example:
 * @code
 * [...]
 *
 * static const auto name_tag = 1u;
 * static const auto surname_tag = 2u;
 *
 * definition_builder builder;
 * builder.define_default<TestObject_1>([](const activation_context& context) ->TestObject_1
 * {
 *    auto& name = context.annotation<annotation<string, name_tag>::type>();
 *    auto& surname = context.annotation<annotation<string, surname_tag>::type>();
 *
 *    return {  name.value + surname.value };
 * });
 *
 * instance_activator activator(std::move(builder));
 *
 * annotations_map annotations(
 *       make_annotation<name_tag>(string("John")),
 *       make_annotation<surname_tag>(string("Smith")));
 *
 * auto instance = activator.activate_default_raii<TestObject_1>(std::move(annotations));
 *
 * [...]
 * @endcode
 *
 * @tparam T Annotation type.
 * @tparam tag Tag identifier. Number.
 */
template <typename T, size_t tag = 0u>
struct annotation
{
    using type = annotation<T, tag>;

    T value;
};

/**
 * @brief Creates an instance of **annotation** with default annotation tag.
 * @tparam T Annotating type to wrap.
 * @param value Value of wrapped annotation.
 * @return An instance of default tagged **annotation** wrapping annotated value.
 */
template <typename T>
annotation<T> make_annotation(T&& value);

/**
 * @brief Creates an instance of **annotation** with seected annotation tag.
 * @tparam tag Tag to use when wrappign the annotation.
 * @tparam T Annotating type to wrap.
 * @param value Value of wrapped annotation.
 * @return An instance of custom tagged **annotation** wrapping annotated value.
 */
template <size_t tag, typename T>
annotation<T, tag> make_annotation(T&& value);

}

#include "annotation.ipp"

