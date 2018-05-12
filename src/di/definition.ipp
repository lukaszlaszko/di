#pragma once

#include "definition.hpp"


namespace di {

template <typename creator_type, typename deleter_type>
inline definition::definition(creator_type&& creator, deleter_type&& deleter)
    :
        creator_(std::move(creator)),
        deleter_(std::move(deleter))
{

}

template <typename T, typename... args_types>
inline definition::id_type definition::make_id(const std::string& id)
{
    using signature_type = combined_identity<T, args_types...>;
    return std::make_pair(id, std::type_index(typeid(signature_type)));
}

template <typename return_type, typename... args_types>
inline const std::function<return_type*(const activation_context&, args_types...)>& definition::creator() const
{
    using creator_function_type = std::function<return_type*(const activation_context&, args_types...)>;
    return boost::any_cast<const creator_function_type&>(creator_);
}

template <typename T>
inline const std::function<void(T*)>& definition::deleter() const
{
    using deleter_function_type = std::function<void(T*)>;
    return boost::any_cast<const deleter_function_type&>(deleter_);
}

}

