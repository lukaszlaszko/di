#pragma once

#include "definition.hpp"
#include "decorator_definition.hpp"
#include "interceptor_definition.hpp"

#include <functional>
#include <list>
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <utility>


namespace di {

/**
 * @brief A dependency injection activator.
 *
 * @details
 * An activator collects definitions of how to activate types in form of callbacks. Multiple definitions of the same type are
 * allowed, each definition is identified by it's unique id.
 *
 */
class instance_activator final
{
public:
    template <typename builder_type>
    explicit instance_activator(builder_type&& builder, bool trace_enabled = false);

    /**
     * @brief Non-default constructable.
     */
    instance_activator() = delete;
    /**
     * @brief Non-copy constructable.
     */
    instance_activator(const instance_activator& other) = delete;
    /**
     * @brief Default move constructable.
     */
    instance_activator(instance_activator&& other) = default;

    /**
     * @brief Non-copy assignable.
     */
    instance_activator& operator=(const instance_activator& other) = delete;

    /**
     * @brief Default move assignable.
     */
    instance_activator& operator=(instance_activator&& other) = default;

    template <typename T, typename... args_types>
    std::unique_ptr<T> activate_unique(activation_context& context, args_types... args) const;

    template <typename T, typename... args_types>
    std::shared_ptr<T> activate_shared(activation_context& context, args_types... args) const;

    template <typename T, typename... args_types>
    T activate_raii(activation_context& context, args_types... args) const;

    template <typename T, typename... args_types>
    bool can_activate(const std::string& id) const;

    template <typename T, typename... args_types>
    std::unique_ptr<T> activate_unique(const std::string& id, args_types... args) const;

    template <typename T, typename... args_types>
    std::shared_ptr<T> activate_shared(const std::string& id, args_types... args) const;

    template <typename T, typename... args_types>
    T activate_raii(const std::string& id, args_types... args) const;

    template <typename T, typename... args_types>
    std::unique_ptr<T> activate_unique(
            const std::string& id,
            annotations_map&& annotations,
            args_types... args) const;

    template <typename T, typename... args_types>
    std::shared_ptr<T> activate_shared(
            const std::string& id,
            annotations_map&& annotations,
            args_types... args) const;

    template <typename T, typename... args_types>
    T activate_raii(
            const std::string& id,
            annotations_map&& annotations,
            args_types... args) const;

    template <typename T, typename... args_types>
    bool can_activate_default() const;

    template <typename T, typename... args_types>
    std::unique_ptr<T> activate_default_unique(args_types... args) const;

    template <typename T, typename... args_types>
    std::shared_ptr<T> activate_default_shared(args_types... args) const;

    template <typename T, typename... args_types>
    T activate_default_raii(args_types... args) const;

    template <typename T, typename... args_types>
    std::unique_ptr<T> activate_default_unique(
            annotations_map&& annotations,
            args_types... args) const;

    template <typename T, typename... args_types>
    std::shared_ptr<T> activate_default_shared(
            annotations_map&& annotations,
            args_types... args) const;

    template <typename T, typename... args_types>
    T activate_default_raii(
            annotations_map&& annotations,
            args_types... args) const;

private:
    template <typename T, typename... args_types>
    std::pair<T*, const std::function<void(T*)>&> allocate(
            activation_context& context,
            args_types... args) const;

    definition::map_type definitions_;
    interceptor_definition::map_type interceptors_;
    decorator_definition::map_type decorators_;
    std::list<boost::any> modules_;

    bool trace_enabled_;

};

}

#include "instance_activator.ipp"
