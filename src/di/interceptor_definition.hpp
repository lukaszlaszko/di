#pragma once

#include <boost/any.hpp>

#include <functional>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>


namespace di {

class activation_context;

class interceptor_definition
{
public:
    template <typename... types>
    struct combined_identity
    { };

    using id_type = std::type_index;
    using map_type = std::unordered_multimap<id_type, interceptor_definition>;

    template <typename interceptor_type>
    explicit interceptor_definition(interceptor_type&& interceptor);

    /**
     * @brief Non-copy constructable.
     */
    interceptor_definition(const interceptor_definition& other) = delete;
    /**
     * @brief Default move constructable.
     */
    interceptor_definition(interceptor_definition&& other) = default;
    /**
     * @brief Non-copy assignable.
     */
    interceptor_definition& operator=(const interceptor_definition& other) = delete;
    /**
     * @brief Default move assignable.
     */
    interceptor_definition& operator=(interceptor_definition&& other) = default;

    template <typename T, typename... args_types>
    static id_type make_id();

    template <typename T, typename... args_types>
    const std::function<void(T&, const activation_context&, args_types...)>& interceptor() const;

private:
    boost::any interceptor_;

};

}

#include "interceptor_definition.ipp"

