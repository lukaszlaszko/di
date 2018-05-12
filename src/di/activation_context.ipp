#pragma once

#include "instance_activator.hpp"
#include "activation_context.hpp"

#include <di/tools/cxxabi_utils.hpp>


namespace di {

template <typename T, typename... args_types>
template <typename D>
inline activation_context::activation<T, args_types...>::conversion<D>::conversion(activation&& original)
    : original_(std::move(original))
{

}

template <typename T, typename... args_types>
template <typename D>
inline activation_context::activation<T, args_types...>::conversion<D>::operator D() const
{
    T instance = original_;
    return D(std::move(instance));
}

template <typename T, typename... args_types>
template <typename D>
inline activation_context::activation<T, args_types...>::conversion<D>::operator std::unique_ptr<D>() const
{
    std::unique_ptr<T> instance = original_;
    return std::unique_ptr<D>(std::move(instance));
}

template <typename T, typename... args_types>
template <typename D>
inline activation_context::activation<T, args_types...>::conversion<D>::operator std::shared_ptr<D>() const
{
    std::shared_ptr<T> instance = original_;
    return std::shared_ptr<D>(std::move(instance));
}

template <typename T, typename... args_types>
inline activation_context::activation<T, args_types...>::activation(
        std::shared_ptr<activation_context> context,
        args_types... args)
    :
        context_(std::move(context)),
        args_(std::tuple<args_types...>(args...))
{
}

template <typename T, typename... args_types>
template <typename... extra_args_types>
inline activation_context::activation<T, args_types..., extra_args_types...> activation_context::activation<T, args_types...>::with(
        extra_args_types... extra_args) const
{
    return forward_with(std::index_sequence_for<args_types...>{}, extra_args...);
}

template <typename T, typename... args_types>
template <typename... extra_args_types>
inline activation_context::activation<T, args_types..., extra_args_types&...> activation_context::activation<T, args_types...>::with_reference(
        extra_args_types&... extra_args) const
{
    return forward_with_reference(std::index_sequence_for<args_types...>{}, extra_args...);
}

template <typename T, typename... args_types>
template <typename A>
inline activation_context::activation<T, args_types...>& activation_context::activation<T, args_types...>::with_annotation(const A& annotation)
{
    context_->annotations_.set(annotation);
    return *this;
}

template <typename T, typename... args_types>
template <typename A>
inline activation_context::activation<T, args_types...>& activation_context::activation<T, args_types...>::with_annotation(A&& annotation)
{
    context_->annotations_.set(annotation);
    return *this;
}

template <typename T, typename... args_types>
template <typename A>
inline activation_context::activation<T, args_types...>& activation_context::activation<T, args_types...>::
        with_optional_annotation(const A& optional_annotation)
{
    if (optional_annotation)
    {
        auto& annotation = optional_annotation.get();
        context_->annotations_.set(annotation);
    }
    return *this;
}

template <typename T, typename... args_types>
template <typename A>
inline activation_context::activation<T, args_types...>& activation_context::activation<T, args_types...>::
        with_optional_annotation(A&& optional_annotation)
{
    if (optional_annotation)
    {
        auto& annotation = optional_annotation.get();
        context_->annotations_.set(annotation);
    }
    return *this;
}

template <typename T, typename... args_types>
template <typename A, typename transformer_type>
inline activation_context::activation<T, args_types...>& activation_context::activation<T, args_types...>::
        with_optional_annotation(const A& optional_annotation, const transformer_type& transformer)
{
    if (optional_annotation)
    {
        auto& annotation = optional_annotation.get();
        context_->annotations_.set(transformer(annotation));
    }
    return *this;
}

template <typename T, typename... args_types>
template <typename A, typename transformer_type>
inline activation_context::activation<T, args_types...>& activation_context::activation<T, args_types...>::
        with_optional_annotation(A&& optional_annotation, const transformer_type& transformer)
{
    if (optional_annotation)
    {
        auto& annotation = optional_annotation.get();
        context_->annotations_.set(transformer(annotation));
    }
    return *this;
}

template <typename T, typename... args_types>
template <typename D>
inline activation_context::activation<T, args_types...>::conversion<D> activation_context::activation<T, args_types...>::as()
{
    return conversion<D>(std::move(*this));
}

template <typename T, typename... args_types>
inline activation_context::activation<T, args_types...>::operator T() const
{
    return forward_args_raii(std::index_sequence_for<args_types...>{});
}

template <typename T, typename... args_types>
inline activation_context::activation<T, args_types...>::operator std::unique_ptr<T>() const
{
    return forward_args_unique(std::index_sequence_for<args_types...>{});
}

template <typename T, typename... args_types>
inline activation_context::activation<T, args_types...>::operator std::shared_ptr<T>() const
{
    return forward_args_shared(std::index_sequence_for<args_types...>{});
}

template <typename T, typename... args_types>
template <typename... extra_args_types, size_t... args_count>
inline activation_context::activation<T, args_types..., extra_args_types...>
        activation_context::activation<T, args_types...>::forward_with(
                std::index_sequence<args_count...> args_sequence,
                extra_args_types... extra_args) const
{
    return activation<T, args_types..., extra_args_types...>(
            std::move(context_),
            std::get<args_count>(args_)...,
            extra_args...);
}

template <typename T, typename... args_types>
template <typename... extra_args_types, size_t... args_count>
inline activation_context::activation<T, args_types..., extra_args_types&...>
        activation_context::activation<T, args_types...>::forward_with_reference(
                std::index_sequence<args_count...> args_sequence,
                extra_args_types&... extra_args) const
{
    return activation<T, args_types..., extra_args_types&...>(
            std::move(context_),
            std::get<args_count>(args_)...,
            extra_args...);
}

template <typename T, typename... args_types>
template <size_t... args_count>
inline T activation_context::activation<T, args_types...>::forward_args_raii(
        std::index_sequence<args_count...>) const
{
    auto& activator = context_->activator_;
    return activator.template activate_raii<T, args_types...>(*context_, std::get<args_count>(args_)...);
}

template <typename T, typename... args_types>
template <size_t... args_count>
inline std::unique_ptr<T> activation_context::activation<T, args_types...>::forward_args_unique(
        std::index_sequence<args_count...>) const
{
    auto& activator = context_->activator_;
    return activator.template activate_unique<T, args_types...>(*context_, std::get<args_count>(args_)...);
}

template <typename T, typename... args_types>
template <size_t... args_count>
inline std::shared_ptr<T> activation_context::activation<T, args_types...>::forward_args_shared(
        std::index_sequence<args_count...>) const
{
    auto& activator = context_->activator_;
    auto& id = context_->id_;

    return activator.template activate_shared<T, args_types...>(id, std::get<args_count>(args_)...);
}

template <typename A>
inline bool activation_context::has_annotation() const
{
    return annotations_.contains<A>();
}

template <typename A>
inline const A& activation_context::annotation() const
{
    return annotations_.get<A>();
}

template <typename T, typename... args_types>
inline activation_context::activation<T, args_types...> activation_context::activate(
        const std::string& id,
        args_types... args) const
{
    return activate_with_description<T, args_types...>(id, tools::demangle(typeid(T).name()), args...);
}

template <typename T, typename... args_types>
inline activation_context::activation<T, args_types...> activation_context::activate_with_description(
        const std::string& id,
        const std::string& description,
        args_types... args) const
{
    std::unique_ptr<activation_context> child(new activation_context(id, description, *this));
    return activation<T, args_types...>(std::move(child), args...);
}

template <typename T, typename... args_types>
inline std::unique_ptr<T> activation_context::activate_unique(
        const std::string& id,
        args_types... args) const
{
    activation_context context(id, "", *this);
    return activator_.activate_unique<T, args_types...>(context, args...);
}

template <typename T, typename... args_types>
inline std::shared_ptr<T> activation_context::activate_shared(
        const std::string& id,
        args_types... args) const
{
    activation_context context(id, "", *this);
    return activator_.activate_shared<T, args_types...>(context, args...);
}

template <typename T, typename... args_types>
inline T activation_context::activate_raii(
        const std::string& id,
        args_types... args) const
{
    activation_context context(id, "", *this);
    return activator_.activate_raii<T, args_types...>(context, args...);
}

template <typename T, typename... args_types>
inline activation_context::activation<T, args_types...> activation_context::activate_default(
        args_types... args) const
{
    return activate<T, args_types...>(definition::default_id, args...);
}

template <typename T, typename... args_types>
inline activation_context::activation<T, args_types...> activation_context::activate_default_with_description(
        const std::string& description,
        args_types... args) const
{
    return activate_with_description<T, args_types...>(definition::default_id, description, args...);
}

template <typename T, typename... args_types>
inline std::unique_ptr<T> activation_context::activate_default_unique(
        args_types... args) const
{
    return activate_unique<T, args_types...>(definition::default_id, args...);
}

template <typename T, typename... args_types>
inline std::shared_ptr<T> activation_context::activate_default_shared(
        args_types... args) const
{
    return activate_shared<T, args_types...>(definition::default_id, args...);
}

template <typename T, typename... args_types>
inline T activation_context::activate_default_raii(
        args_types... args) const
{
    return activate_raii<T, args_types...>(definition::default_id, args...);
}

}

