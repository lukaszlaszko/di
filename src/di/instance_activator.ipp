#pragma once

#include "instance_activator.hpp"
#include "activation_context.hpp"

#include <di/tools/cxxabi_utils.hpp>
#include <di/tools/traits/veriadic_traits.hpp>

#include <cassert>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <type_traits>


namespace di {

template <typename builder_type>
inline instance_activator::instance_activator(
        builder_type&& builder,
        bool trace_enabled)
    :
        definitions_(std::move(builder.definitions_)),
        interceptors_(std::move(builder.interceptors_)),
        decorators_(std::move(builder.decorators_)),
        modules_(std::move(builder.modules_)),
        trace_enabled_(trace_enabled)
{

}

template <typename T, typename... args_types>
inline std::unique_ptr<T> instance_activator::activate_unique(
        activation_context& context,
        args_types... args) const
{
    auto allocated = allocate<T, args_types...>(context, args...);
    return std::unique_ptr<T>(allocated.first);
}

template <typename T, typename... args_types>
inline std::shared_ptr<T> instance_activator::activate_shared(
        activation_context& context,
        args_types... args) const
{
    auto allocated = allocate<T, args_types...>(context, args...);
    return std::shared_ptr<T>(allocated.first);
}

template <typename T, typename... args_types>
inline T instance_activator::activate_raii(
        activation_context& context,
        args_types... args) const
{
    auto allocated = allocate<T, args_types...>(context, args...);
    auto deleter = allocated.second;

    auto raii_instance = std::move(*allocated.first);
    if (deleter)
        deleter(allocated.first);

    return std::move(raii_instance);
}

template <typename T, typename... args_types>
inline bool instance_activator::can_activate(
        const std::string& id) const
{
    auto definition_id = definition::make_id<T, args_types...>(id);
    auto search_result = definitions_.find(definition_id);

    return search_result != definitions_.end();
}

template <typename T, typename... args_types>
inline std::unique_ptr<T> instance_activator::activate_unique(
        const std::string& id,
        args_types... args) const
{
    activation_context context(id, *this);
    return activate_unique<T, args_types...>(context, args...);
}

template <typename T, typename... args_types>
inline std::shared_ptr<T> instance_activator::activate_shared(
        const std::string& id,
        args_types... args) const
{
    activation_context context(id, *this);
    return activate_shared<T, args_types...>(context, args...);
}

template <typename T, typename... args_types>
inline T instance_activator::activate_raii(
        const std::string& id,
        args_types... args) const
{
    activation_context context(id, *this);
    return activate_raii<T, args_types...>(context, args...);
}

template <typename T, typename... args_types>
inline std::unique_ptr<T> instance_activator::activate_unique(
        const std::string& id,
        annotations_map&& annotations,
        args_types... args) const
{
    activation_context context(id, *this, std::move(annotations));
    return activate_unique<T, args_types...>(context, args...);
}

template <typename T, typename... args_types>
inline std::shared_ptr<T> instance_activator::activate_shared(
        const std::string& id,
        annotations_map&& annotations,
        args_types... args) const
{
    activation_context context(id, *this, std::move(annotations));
    return activate_shared<T, args_types...>(context, args...);
}

template <typename T, typename... args_types>
inline T instance_activator::activate_raii(
        const std::string& id,
        annotations_map&& annotations,
        args_types... args) const
{
    activation_context context(id, *this, std::move(annotations));
    return activate_raii<T, args_types...>(context, args...);
}

template <typename T, typename... args_types>
inline bool instance_activator::can_activate_default() const
{
    return can_activate<T, args_types...>(definition::default_id);
}

template <typename T, typename... args_types>
inline std::unique_ptr<T> instance_activator::activate_default_unique(
        args_types... args) const
{
    return activate_unique<T, args_types...>(definition::default_id, args...);
}

template <typename T, typename... args_types>
inline std::shared_ptr<T> instance_activator::activate_default_shared(
        args_types... args) const
{
    return activate_shared<T, args_types...>(definition::default_id, args...);
}

template <typename T, typename... args_types>
inline T instance_activator::activate_default_raii(
        args_types... args) const
{
    return activate_raii<T, args_types...>(definition::default_id, args...);
}

template <typename T, typename... args_types>
inline std::unique_ptr<T> instance_activator::activate_default_unique(
        annotations_map&& annotations,
        args_types... args) const
{
    return activate_unique<T, args_types...>(definition::default_id, std::move(annotations), args...);
}

template <typename T, typename... args_types>
inline std::shared_ptr<T> instance_activator::activate_default_shared(
        annotations_map&& annotations,
        args_types... args) const
{
    return activate_shared<T, args_types...>(definition::default_id, std::move(annotations), args...);
}

template <typename T, typename... args_types>
inline T instance_activator::activate_default_raii(
        annotations_map&& annotations,
        args_types... args) const
{
    return activate_raii<T, args_types...>(definition::default_id, std::move(annotations), args...);
}

template <typename T, typename... args_types>
inline std::pair<T*, const std::function<void(T*)>&> instance_activator::allocate(
        activation_context& context,
        args_types... args) const
{
    using namespace std;
    using namespace tools;

    auto definition_id = definition::make_id<T, args_types...>(context.id());
    auto search_result = definitions_.find(definition_id);
    if (search_result == definitions_.end())
    {
        std::stringstream message;

        auto type_name = demangle(typeid(T).name());
        if (context.id() == definition::default_id)
            message << "No default definition for type: '" << type_name << "'";
        else
            message << "No named definition '" << context.id() << "' for type: '" << type_name << "'";

        auto args_count = sizeof...(args_types);
        if (args_count > 0u)
        {
            message << " with args: (";
            for (size_t i = 0u; i < args_count; i++)
            {
                auto arg_type_name = demangle(argument_types<args_types...>::name(i));
                if (i > 0)
                    message << ", " << arg_type_name;
                else
                    message << arg_type_name;
            }
            message << ")";
        }

        if (trace_enabled_)
        {
            message << std::endl;
            message << "definitions:" << endl;
            for (auto& definition : definitions_)
            {
                auto& definition_id = definition.first;
                message << definition_id.first << " " << demangle(definition_id.second.name()) << std::endl;
            }
        }

        throw invalid_argument(message.str());
    }

    auto& definition = search_result->second;
    auto& creator = definition.template creator<T, args_types...>();
    auto deleter = &definition.template deleter<T>();

    auto& annotations = const_cast<di::definition&>(definition).annotations();
    context.annotations_ << annotations;

    auto instance = creator(context, args...);

    auto interceptor_id = interceptor_definition::make_id<T, args_types...>();
    auto interceptor_range = interceptors_.equal_range(interceptor_id);
    for (auto iter = interceptor_range.first; iter != interceptor_range.second; ++iter)
    {
        auto& interceptor_definition = iter->second;
        auto& interceptor = interceptor_definition.template interceptor<T&, args_types...>();
        interceptor(*instance, context, args...);
    }

    auto decorator_id = decorator_definition::make_id<T>();
    auto decorator_range = decorators_.equal_range(decorator_id);
    for (auto iter = decorator_range.first; iter != decorator_range.second; ++iter)
    {
        auto& decorator_definition = iter->second;
        auto& decorator = decorator_definition.template decorator<T>();
        auto decorated = decorator(instance, context);

        instance = decorated;
        deleter = &decorator_definition.template deleter<T>();
    }

    return { instance, *deleter };
}

}
