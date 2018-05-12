#pragma once

#include "interceptor_definition.hpp"
#include "definition.hpp"


namespace di {

template <typename interceptor_type>
inline interceptor_definition::interceptor_definition(interceptor_type&& interceptor)
    :
        interceptor_(std::move(interceptor))
{

}

template <typename T, typename... args_types>
inline interceptor_definition::id_type interceptor_definition::make_id()
{
    using signature_type = combined_identity<T, args_types...>;
    return std::type_index(typeid(signature_type));
}

template <typename T, typename... args_types>
inline const std::function<void(T&, const activation_context&, args_types...)>& interceptor_definition::interceptor() const
{
    using interceptor_function_type = std::function<void(T&, const activation_context&, args_types...)>;
    return boost::any_cast<const interceptor_function_type&>(interceptor_);
}

}
