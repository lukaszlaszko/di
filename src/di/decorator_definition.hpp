#pragma once

#include <di/tools/hash.hpp>

#include <boost/any.hpp>

#include <functional>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>


namespace di {

class activation_context;

class decorator_definition
{
public:
    template <typename... types>
    struct combined_identity
    { };

    using id_type = std::type_index;
    using map_type = std::unordered_multimap<id_type, decorator_definition>;

    template <typename decorator_type, typename deleter_type>
    explicit decorator_definition(decorator_type&& decorator, deleter_type&& deleter);

    /**
     * @brief Non-copy constructable.
     */
    decorator_definition(const decorator_definition& other) = delete;
    /**
     * @brief Default move constructable.
     */
    decorator_definition(decorator_definition&& other) = default;
    /**
     * @brief Non-copy assignable.
     */
    decorator_definition& operator=(const decorator_definition& other) = delete;
    /**
     * @brief Default move assignable.
     */
    decorator_definition& operator=(decorator_definition&& other) = default;

    template <typename T>
    static id_type make_id();

    template <typename T>
    const std::function<T*(T*, const activation_context&)>& decorator() const;

    /**
     * @brief Gets delete method defining a way how decorator should be deallocated.
     * @tparam T A type of decorator to delete.
     * @return A reference to deleter function.
     */
    template <typename T>
    const std::function<void(T*)>& deleter() const;

private:
    boost::any decorator_;
    boost::any deleter_;

};

}

#include "decorator_definition.ipp"

