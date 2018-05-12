#pragma once

#include "decorator_definition.hpp"
#include "definition.hpp"


namespace di {

template <typename decorator_type, typename deleter_type>
inline decorator_definition::decorator_definition(decorator_type&& decorator, deleter_type&& deleter)
    :
        decorator_(std::move(decorator)),
        deleter_(std::move(deleter))
{

}

template <typename T>
inline decorator_definition::id_type decorator_definition::make_id()
{
    return std::type_index(typeid(T));
}

template <typename T>
inline const std::function<T*(T*, const activation_context&)>& decorator_definition::decorator() const
{
    using decorator_function_type = std::function<T*(T*, const activation_context&)>;
    return boost::any_cast<const decorator_function_type&>(decorator_);
}

template <typename T>
inline const std::function<void(T*)>& decorator_definition::deleter() const
{
    using deleter_function_type = std::function<void(T*)>;
    return boost::any_cast<const deleter_function_type&>(deleter_);
}

}
