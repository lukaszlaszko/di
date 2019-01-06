#pragma once

#include "definition_builder.hpp"

#include <di/tools/traits/ctor_traits.hpp>

#include <sstream>
#include <stdexcept>
#include <type_traits>


namespace di {

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...>::registration(
        const std::string& id,
        definition& definition,
        definition_builder& builder)
    :
        id_(id),
        definition_(definition),
        builder_(builder)
{

}

template <typename T, typename... args_types>
inline const std::string& definition_builder::registration<T, args_types...>::id() const
{
    return id_;
}

/**
 * @brief Registers a definition of base type D deriving from current registration.
 * @details
 * @paragraph
 * This method adds current registration as a registration of base type D. As a result dictionary of definitions will
 * contain both registrations for types D and T with the same id and list of arguments.
 *
 * @paragraph
 * This method will be only unfolded through SFINE for base types for T.
 *
 * @tparam T A type which is a subject of the initial registration.
 * @tparam args_types List of argument types required by the base registration.
 * @tparam D A base type of T for which the base registration will be extended.
 * @return A registration object for applied registration.
 */
template <typename T, typename... args_types>
template <typename D>
inline typename std::enable_if_t<std::is_base_of<D, T>::value, definition_builder::registration<D, args_types...>>
        definition_builder::registration<T, args_types...>::as()
{
    return builder_.try_define<D, args_types...>(
            id_,
            [&definition = definition_](const activation_context& context, args_types... args) -> D*
            {
                // it's save to keep the reference, but how about move into activator?
                // https://stackoverflow.com/questions/11337494/c-stl-unordered-map-implementation-reference-validity
                auto& creator = definition.template creator<T, args_types...>();

                auto instance = creator(context, args...);
                return dynamic_cast<D*>(instance);
            },
            [&definition = definition_](D* pointer)
            {
                auto& deleter = definition.template deleter<T>();

                if (deleter)
                    deleter(reinterpret_cast<T*>(pointer));
            });
}

/**
 * @brief Registers a definition of wrapping type for the current registration output.
 * @details
 * This method registers a wrapper type constructable by moving a rvalue of type T as the only argument of type D constructor.
 *
 * @tparam T The base type registered with this registration.
 * @tparam args_types Registration arguments.
 * @tparam D The wrapper type.
 * @return A registration object for wrapper definition.
 */
template <typename T, typename... args_types>
template <typename D>
inline typename std::enable_if_t<!std::is_base_of<D, T>::value, definition_builder::registration<D, args_types...>>
        definition_builder::registration<T, args_types...>::as()
{
    return builder_.try_define<D, args_types...>(
            id_,
            [id = id_](const activation_context& context, args_types... args) -> D*
            {
                return new D(context.activate_raii<T, args_types...>(id, args...));
            },
            [&definition = definition_](D* pointer)
            {
                auto& deleter = definition.template deleter<T>();

                if (deleter)
                    deleter(reinterpret_cast<T*>(pointer));
            });
}

template <typename T, typename... args_types>
template <typename... annotation_types>
inline definition_builder::registration<T, args_types...>&
        definition_builder::registration<T, args_types...>::annotate(const annotation_types&... annotations)
{
    auto& annotations_ref = definition_.annotations();
    annotations_ref.set(annotations...);

    return *this;
}

template <typename T, typename... args_types>
template <typename... annotation_types>
inline definition_builder::registration<T, args_types...>&
        definition_builder::registration<T, args_types...>::annotate(annotation_types&&... annotations)
{
    auto& annotations_ref = definition_.annotations();
    annotations_ref.set(std::move(annotations)...);

    return *this;
}

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...>::operator definition&()
{
    return definition_;
}

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define(
        const std::string& id,
        typename identity<std::function<T(const activation_context&, args_types...)>>::type&& factory)
{
    static_assert(
            std::is_move_constructible<T>::value,
            "T has to be movable!");

    return try_define<T, args_types...>(
            id,
            [factory = std::move(factory)](const activation_context& context, args_types... args) -> T*
            {
                auto created = factory(context, args...);
                return new T(std::move(created));
            },
            {});
}

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define(
        const std::string& id,
        typename identity<std::function<T(args_types...)>>::type&& factory)
{
    return define<T, args_types...>(id, [factory = std::move(factory)](const activation_context& context, args_types... args) -> T
    {
        return factory(args...);
    });
}

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define(
        const std::string& id,
        typename identity<std::function<T*(const activation_context&, args_types...)>>::type&& factory,
        typename identity<std::function<void(T*)>>::type&& deleter)
{
    static_assert(
            std::is_move_constructible<T>::value,
            "T has to be movable!");

    return try_define<T, args_types...>(
            id,
            [factory = std::move(factory)](const activation_context& context, args_types... args) -> T*
            {
                return factory(context, args...);
            },
            [deleter = std::move(deleter)](void* pointer)
            {
                auto t_pointer = reinterpret_cast<T*>(pointer);
                if (deleter)
                    deleter(t_pointer);
                else
                    delete t_pointer;
            });
}

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define(
        const std::string& id,
        typename identity<std::function<T*(args_types...)>>::type&& factory,
        typename identity<std::function<void(T*)>>::type&& deleter)
{
    return define<T, args_types...>(id, [factory = std::move(factory)](const activation_context& context, args_types... args) -> T*
    {
        return factory(args...);
    },
    std::move(deleter));
}

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define(
        const std::string& id,
        typename identity<std::function<std::unique_ptr<T>(const activation_context&, args_types...)>>::type&& factory)
{
    return try_define<T, args_types...>(
            id,
            [factory = std::move(factory)](const activation_context& context, args_types... args) -> T*
            {
                auto created = factory(context, args...);
                return created.release();
            },
            {});
}

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define(
        const std::string& id,
        typename identity<std::function<std::unique_ptr<T>(args_types...)>>::type&& factory)
{
    return define<T, args_types...>(id, [factory = std::move(factory)](const activation_context& context, args_types... args) -> std::unique_ptr<T>
    {
        return factory(args...);
    });
}

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define_default(
        typename identity<std::function<T(const activation_context&, args_types...)>>::type&& factory)
{
    return define<T, args_types...>(definition::default_id, std::move(factory));
}

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define_default(
        typename identity<std::function<T(args_types...)>>::type&& factory)
{
    return define<T, args_types...>(definition::default_id, std::move(factory));
}

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define_default(
        typename identity<std::function<T*(const activation_context&, args_types...)>>::type&& factory,
        typename identity<std::function<void(T*)>>::type&& deleter)
{
    return define<T, args_types...>(
            definition::default_id,
            std::move(factory),
            std::move(deleter));
}

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define_default(
        typename identity<std::function<T*(args_types...)>>::type&& factory,
        typename identity<std::function<void(T*)>>::type&& deleter)
{
    return define<T, args_types...>(
            definition::default_id,
            std::move(factory),
            std::move(deleter));
}

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define_default(
        typename identity<std::function<std::unique_ptr<T>(const activation_context&, args_types...)>>::type&& factory)
{
    return define<T, args_types...>(definition::default_id, factory);
}

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define_default(
        typename identity<std::function<std::unique_ptr<T>(args_types...)>>::type&& factory)
{
    return define<T, args_types...>(definition::default_id, std::move(factory));
}

template <typename T>
inline definition_builder::registration<T> definition_builder::define_type(const std::string& id)
{
    static_assert(
            tools::ctor_count_n<T>::value > 0ul,
            "No suitable, unique constructors found for automatic injection. Use explicit define_explicit_type instead.");
    static_assert(
            tools::ctor_count_n<T>::value < 2ul,
            "Multiple suitable constructors found. Use explicit define_explicit_type instead.");

    return try_define<T>(
            id,
            [id](const activation_context& context) -> T*
            {
                return magic_factory<T>(id, context);
            },
            {});
}

template <typename T>
inline definition_builder::registration<T> definition_builder::define_type()
{
    return define_type<T>(definition::default_id);
}

template <typename T, typename... args_types>
inline definition_builder::registration<T> definition_builder::define_explicit_type(const std::string& id)
{
    return try_define<T>(
            id,
            [id](const activation_context& context) -> T*
            {
                return new T(
                        context.activate<
                                typename argument_type<args_types>::type>(argument_id<args_types>::value)...);
            },
            {});
}

template <typename T, typename... args_types>
inline definition_builder::registration<T> definition_builder::define_explicit_type()
{
    return define_explicit_type<T, args_types...>(definition::default_id);
}

template <typename T, typename... args_types>
inline definition_builder::registration<T> definition_builder::define_instance(const std::string& id, args_types... args)
{
    return try_define<T>(
            id,
            [id, args...](const activation_context& context) -> T*
            {
                return new T(args...);
            },
            {});
}

template <typename T, typename... args_types>
inline definition_builder::registration<T> definition_builder::define_default_instance(args_types... args)
{
    return define_instance<T, args_types...>(definition::default_id, args...);
}

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define_factory(
        const std::string &id,
        T factory(args_types...))
{
    return define<T, args_types...>(id, factory);
}

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define_factory(
        const std::string &id,
        T factory(const activation_context&, args_types...))
{
    return define<T, args_types...>(id, factory);
}

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define_factory(
        T factory(args_types...))
{
    return define_default<T, args_types...>(factory);
}

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define_factory(
        T factory(const activation_context&, args_types...))
{
    return define_default<T, args_types...>(factory);
}

template <typename T, typename class_type, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define_factory(
        const std::string &id,
        T (class_type::*factory)(args_types...),
        class_type* instance)
{
    return define<T, args_types...>(id,
            [instance, factory](args_types... args) -> T
            {
                return (*instance.*factory)(args...);
            });
}

template <typename T, typename class_type, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define_factory(
        const std::string &id,
        T (class_type::*factory)(const activation_context&, args_types...),
        class_type* instance)
{
    return define<T, args_types...>(id,
            [instance, factory](const activation_context& context, args_types... args) -> T
            {
                return (*instance.*factory)(context, args...);
            });
}

template <typename T, typename class_type, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define_factory(
        T (class_type::*factory)(args_types...),
        class_type* instance)
{
    return define_default<T, args_types...>(
            [instance, factory](args_types... args) -> T
            {
                return (*instance.*factory)(args...);
            });
}

template <typename T, typename class_type, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::define_factory(
        T (class_type::*factory)(const activation_context&, args_types...),
        class_type* instance)
{
    return define_default<T, args_types...>(
            [instance, factory](const activation_context& context, args_types... args) -> T
            {
                return (*instance.*factory)(context, args...);
            });
}

template <typename T, typename... args_types>
inline const interceptor_definition& definition_builder::define_interceptor(
        typename identity<std::function<void(T& activated, const activation_context&, args_types...)>>::type&& interceptor)
{
    return try_define_interceptor<T, args_types...>(std::move(interceptor));
}

template <typename T, typename... args_types>
inline const interceptor_definition& definition_builder::define_interceptor(
        typename identity<std::function<void(const activation_context&, args_types...)>>::type&& interceptor)
{
    return define_interceptor<T, args_types...>(
            [interceptor](T& instance, const activation_context& context, args_types... args)
            {
                interceptor(context, args...);
            });
}

template <typename T, typename... args_types>
inline const interceptor_definition& definition_builder::define_interceptor(
        typename identity<std::function<void(args_types...)>>::type&& interceptor)
{
    return define_interceptor<T, args_types...>(
            [interceptor](T& instance, const activation_context& context, args_types... args)
            {
                interceptor(args...);
            });
}

template <typename T, typename class_type, typename... args_types>
inline const interceptor_definition& definition_builder::define_interceptor(
        void (class_type::*interceptor)(T&, const activation_context&, args_types...),
        class_type* instance)
{
    return define_interceptor<T, args_types...>(
            [instance, interceptor](T& activated, const activation_context& context, args_types... args)
            {
                (*instance.*interceptor)(activated, context, args...);
            });
};

template <typename T, typename class_type, typename... args_types>
inline const interceptor_definition& definition_builder::define_interceptor(
        void (class_type::*interceptor)(T&, args_types...),
        class_type* instance)
{
    return define_interceptor<T, args_types...>(
            [instance, interceptor](T& activated, const activation_context& context, args_types... args)
            {
                (*instance.*interceptor)(activated, args...);
            });
};

template <typename T, typename class_type, typename... args_types>
inline const interceptor_definition& definition_builder::define_interceptor(
        void (class_type::*interceptor)(const activation_context&, args_types...),
        class_type* instance)
{
    return define_interceptor<T, args_types...>(
            [instance, interceptor](T& activated, const activation_context& context, args_types... args)
            {
                (*instance.*interceptor)(context, args...);
            });
};

template <typename T, typename class_type, typename... args_types>
inline const interceptor_definition& definition_builder::define_interceptor(
        void (class_type::*interceptor)(args_types...),
        class_type* instance)
{
    return define_interceptor<T, args_types...>(
            [instance, interceptor](T& activated, const activation_context& context, args_types... args)
            {
                (*instance.*interceptor)(args...);
            });
};

template <typename T>
inline const decorator_definition& definition_builder::define_decorator(
        typename identity<std::function<T*(T* activated, const activation_context&)>>::type&& decorator,
        typename identity<std::function<void(T*)>>::type&& deleter)
{
    return try_define_decorator<T>(
            std::move(decorator),
            std::move(deleter));
}

template <typename T>
inline const decorator_definition& definition_builder::define_decorator(
        typename identity<std::function<T*(T* activated)>>::type&& decorator,
        typename identity<std::function<void(T*)>>::type&& deleter)
{
    return define_decorator<T>(
            [decorator](T* instance, const activation_context& context) -> T*
            {
                return decorator(instance);
            },
            std::move(deleter));
}

template <typename T>
inline const decorator_definition& definition_builder::define_decorator(
        typename identity<std::function<
                std::unique_ptr<T>(std::unique_ptr<T>&& activated, const activation_context&)>>::type&& decorator)
{
    return try_define_decorator<T>(
            [decorator](T* instance, const activation_context& context) -> T*
            {
                auto decorated = decorator(std::unique_ptr<T>(instance), context);
                return decorated.release();
            },
            {});
}

template <typename T>
inline const decorator_definition& definition_builder::define_decorator(
        typename identity<std::function<
                std::unique_ptr<T>(std::unique_ptr<T>&& activated)>>::type&& decorator)
{
    return define_decorator<T>(
            [decorator](std::unique_ptr<T>&& undecorated, const activation_context& context) -> std::unique_ptr<T>
            {
                return decorator(std::move(undecorated));
            });
}

template <typename T>
inline const decorator_definition& definition_builder::define_decorator(
        typename identity<std::function<T(T&& activated, const activation_context&)>>::type&& decorator)
{
    return try_define_decorator<T>(
            [decorator](T* instance, const activation_context& context) -> T*
            {
                auto decorated = decorator(std::move(*instance), context);
                return new T(std::move(decorated));
            },
            [](T* pointer)
            {
                delete pointer;
            });
}

template <typename T>
inline const decorator_definition& definition_builder::define_decorator(
        typename identity<std::function<T(T&& activated)>>::type&& decorator)
{
    return define_decorator<T>(
            [decorator](T&& undecorated, const activation_context& context) -> T
            {
                return decorator(std::move(undecorated));
            });
}

template <typename T, typename class_type>
inline const decorator_definition& definition_builder::define_decorator(
        std::unique_ptr<T> (class_type::*decorator)(std::unique_ptr<T>&&, const activation_context&),
        class_type* instance)
{
    return define_decorator<T>(
            [instance, decorator](std::unique_ptr<T>&& activated, const activation_context& context)
                    -> std::unique_ptr<T>
            {
                return (*instance.*decorator)(std::move(activated), context);
            });
};

template <typename T, typename class_type>
inline const decorator_definition& definition_builder::define_decorator(
        std::unique_ptr<T> (class_type::*decorator)(std::unique_ptr<T>&&),
        class_type* instance)
{
    return define_decorator<T>(
            [instance, decorator](std::unique_ptr<T>&& activated, const activation_context& context) -> std::unique_ptr<T>
            {
                return (*instance.*decorator)(std::move(activated));
            });
};

template <typename T, typename class_type>
inline const decorator_definition& definition_builder::define_decorator(
        T (class_type::*decorator)(T&&, const activation_context&),
        class_type* instance)
{
    return define_decorator<T>(
            [instance, decorator](T&& activated, const activation_context& context) -> T
    {
        return (*instance.*decorator)(std::move(activated), context);
    });
};

template <typename T, typename class_type>
inline const decorator_definition& definition_builder::define_decorator(
        T (class_type::*decorator)(T&&),
        class_type* instance)
{
    return define_decorator<T>(
            [instance, decorator](T&& activated, const activation_context& context) -> T
    {
        return (*instance.*decorator)(std::move(activated));
    });
};

template <typename T>
inline const decorator_definition& definition_builder::define_decorator(
        T decorator(T&&, const activation_context&))
{
    return define_decorator<T>(
            [decorator](T&& activated, const activation_context& context) -> T
            {
                return decorator(std::move(activated), context);
            });
};

template <typename T>
inline const decorator_definition& definition_builder::define_decorator(
        T decorator(T&&))
{
    return define_decorator<T>(
            [decorator](T&& activated, const activation_context& context) -> T
            {
                return decorator(std::move(activated));
            });
};

template <typename module_type>
inline definition_builder& definition_builder::define_module(const module_type&& module)
{
    modules_.push_back(std::move(module));
    auto& stored_module = boost::any_cast<module_type&>(modules_.back());

    stored_module(*this);
    return *this;
}

template <typename T>
inline definition_builder::magic_argument<T>::magic_argument(const activation_context& context, size_t)
    :
        context_(context)
{

}

template <typename T>
template <typename U, typename _>
inline definition_builder::magic_argument<T>::operator U()
{
    return context_.activate_default<U>();
}

template <typename T>
template <typename U, typename _>
inline definition_builder::magic_argument<T>::operator std::unique_ptr<U>()
{
    return context_.activate_default<U>();
}

template <typename T>
template <typename U, typename _>
inline definition_builder::magic_argument<T>::operator std::shared_ptr<U>()
{
    return context_.activate_default<U>();
}

template <typename T, size_t... indices>
inline typename std::enable_if_t<sizeof...(indices) == 0, T>* definition_builder::magic_factory_impl(
        const std::string& id,
        const activation_context& context,
        std::integer_sequence<size_t, indices...>)
{
    return new T();
}

template <typename T, size_t... indices>
inline typename std::enable_if_t<sizeof...(indices) != 0, T>* definition_builder::magic_factory_impl(
        const std::string& id,
        const activation_context& context,
        std::integer_sequence<size_t, indices...>)
{
    return new T(magic_argument<T>(context, indices)...);
}

template <typename T>
inline T* definition_builder::magic_factory(
        const std::string& id,
        const activation_context& context)
{
    using sequence = std::make_integer_sequence<size_t, tools::ctor_args_count_n<T>::value>;
    return magic_factory_impl<T>(id, context, sequence());
}

template <typename T, typename... args_types>
inline definition_builder::registration<T, args_types...> definition_builder::try_define(
        const std::string& id,
        typename identity<std::function<T*(const activation_context&, args_types...)>>::type&& creator,
        typename identity<std::function<void(T*)>>::type&& deleter)
{
    auto definition_id = definition::make_id<T, args_types...>(id);
    auto result = definitions_.emplace(
            std::piecewise_construct,
                    std::forward_as_tuple(definition_id),
                    std::forward_as_tuple(definition { std::move(creator), std::move(deleter) }));

    if (!result.second)
    {
        std::stringstream message;
        message << "Duplicated definition for [" << id << ";" << typeid(T).name() << "]";

        throw std::invalid_argument(message.str());
    }

    return registration<T, args_types...>(id, result.first->second, *this);
}

template <typename T, typename... args_types>
inline const interceptor_definition& definition_builder::try_define_interceptor(
        typename identity<std::function<void(T&, const activation_context&, args_types...)>>::type&& interceptor)
{
    auto definition_id = interceptor_definition::make_id<T, args_types...>();
    auto emplace_result = interceptors_.emplace(
            std::piecewise_construct,
                    std::forward_as_tuple(definition_id),
                    std::forward_as_tuple(interceptor_definition { std::move(interceptor) }));

    return (*emplace_result).second;
}

template <typename T>
inline const decorator_definition& definition_builder::try_define_decorator(
        typename identity<std::function<T*(T*, const activation_context&)>>::type&& decorator,
        typename identity<std::function<void(T*)>>::type&& deleter)
{
    auto decorator_id = decorator_definition::make_id<T>();
    auto emplace_result = decorators_.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(decorator_id),
            std::forward_as_tuple(decorator_definition { std::move(decorator), std::move(deleter) }));

    return (*emplace_result).second;
}

}
