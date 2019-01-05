#pragma once

#include "argument.hpp"
#include "definition.hpp"
#include "decorator_definition.hpp"
#include "interceptor_definition.hpp"
#include "activation_context.hpp"

#include <di/tools/hash.hpp>

#include <boost/any.hpp>

#include <functional>
#include <list>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <di/tools/traits/ctor_traits.hpp>


namespace di {

/**
 * @brief Used for registration of type factories.
 * @details
 * An instance of **definition_builder** provides a platform for registration of types and type
 * factories. Based on these definitions **instance_activator**, is able to construct types and
 * their dependencies.
 */
class definition_builder final
{
public:
    template <typename T>
    struct identity
    {
        using type = T;
    };

    /**
     * @brief Represents a registered type factory.
     * @tparam T Type this registration is for.
     * @tparam args_types Required type arguments.
     */
    template <typename T, typename... args_types>
    class registration final
    {
    public:
        using registered_type = T;
        using with_types = tools::argument_types<args_types...>;

        /**
         * @brief Registration identifier.
         * @details
         * A registration is primarily identified by the type which creation it describes and required parameters. However,
         * it is possible to define more than one registration with identical signature. To distinguish them and a string
         * identifier is used.
         * @return A string identifier of this registration.
         */
        const std::string& id() const;

        /**
         * @brief Creates a derived registration for type **T** is convertable into.
         * @tparam D
         * @return
         */
        template <typename D>
        typename std::enable_if_t<std::is_base_of<D, T>::value, registration<D, args_types...>> as();

        /**
         * @brief Creates a derived registration for the base type of **T**.
         * @details
         * @paragraph
         * Consider this example:
         * @code
         *
         *
         *
         * @endcode
         *
         *
         * @tparam D Base type, type **T** derived from.
         * @return
         */
        template <typename D>
        typename std::enable_if_t<!std::is_base_of<D, T>::value, registration<D, args_types...>> as();

        /**
         * @brief Annotates registration with a range of objects.
         * @tparam annotation_types Types of annotation objects.
         * @param annotations Const references to annotating objects.
         * @return Self reference.
         */
        template <typename... annotation_types>
        registration& annotate(const annotation_types&... annotations);
        /**
         * @brief
         * @tparam annotation_types
         * @param annotations
         * @return
         */
        template <typename... annotation_types>
        registration& annotate(annotation_types&&... annotations);

        /**
         * @brief Retrieves a reference to the underlying definition.
         * @return A reference to the underlying definition.
         */
        operator definition&();

    private:
        friend definition_builder;

        explicit registration(
                const std::string& id,
                definition& definition,
                definition_builder& builder);

        std::string id_;
        definition& definition_;
        definition_builder& builder_;

    };

    /**
     * @brief Default constructable.
     */
    definition_builder() = default;

    /**
     * @brief Non-copy constructable.
     */
    definition_builder(const definition_builder& other) = delete;

    /**
     * @brief Default move constructable.
     */
    definition_builder(definition_builder&& other) = default;

    /**
     * @brief Non-copy assignable.
     */
    definition_builder& operator=(const definition_builder& other) = delete;

    /**
     * @brief Default move assignable.
     */
    definition_builder& operator=(definition_builder&& other) = default;

    /**
     * @brief Defines a component factory with a unique key.
     * @details
     * @paragraph
     * This method allows registration of multiple different factories for given type T. Each factory has its unique identifier.
     * For instance:
     * @code
     * definition_builder builder;
     * builder.define<TestObject_1, string>("direct", [](string description) -> TestObject_1
     * {
     *      return { description };
     * });
     *
     * builder.define<TestObject_1, string>("translated", [](string description) -> TestObject_1
     * {
     *      return { translate(description) };
     * });
     *
     *
     * instance_activator activator(std::move(builder));
     * auto non_translated = activator.activate_unique<TestObject_1, string>("direct", sample_id);
     *
     * @endcode
     * Registers 2 factories for the same type *TestObject_1* with the same arguments but under different identifiers.
     * During activation definition has to be referenced through registered type, complete argument set and identifier.
     *
     * @tparam T Component type for which the factory is being registered.
     * @tparam args_types Types of arguments required by the registered factory.
     * @param id An identifier under which the factory is being registered
     * @param factory Factory functor intended to create an instance of type T.
     * @return An instance of **registration** allowing further customisation of registered definition.
     */
    template <typename T, typename... args_types>
    registration<T, args_types...> define(
            const std::string& id,
            typename identity<std::function<T(const activation_context&, args_types...)>>::type&& factory);

    template <typename T, typename... args_types>
    registration<T, args_types...> define(
            const std::string& id,
            typename identity<std::function<T(args_types...)>>::type&& factory);

    template <typename T, typename... args_types>
    registration<T, args_types...> define(
            const std::string& id,
            typename identity<std::function<T*(const activation_context&, args_types...)>>::type&& factory,
            typename identity<std::function<void(T*)>>::type&& deleter = {});

    template <typename T, typename... args_types>
    registration<T, args_types...> define(
            const std::string& id,
            typename identity<std::function<T*(args_types...)>>::type&& factory,
            typename identity<std::function<void(T*)>>::type&& deleter = {});

    template <typename T, typename... args_types>
    registration<T, args_types...> define(
            const std::string& id,
            typename identity<std::function<std::unique_ptr<T>(const activation_context&, args_types...)>>::type&& factory);

    template <typename T, typename... args_types>
    registration<T, args_types...> define(
            const std::string& id,
            typename identity<std::function<std::unique_ptr<T>(args_types...)>>::type&& factory);

    template <typename T, typename... args_types>
    registration<T, args_types...> define_default(
            typename identity<std::function<T(const activation_context&, args_types...)>>::type&& factory);

    template <typename T, typename... args_types>
    registration<T, args_types...> define_default(
            typename identity<std::function<T(args_types...)>>::type&& factory);

    template <typename T, typename... args_types>
    registration<T, args_types...> define_default(
            typename identity<std::function<T*(const activation_context&, args_types...)>>::type&& factory,
            typename identity<std::function<void(T*)>>::type&& deleter = {});

    template <typename T, typename... args_types>
    registration<T, args_types...> define_default(
            typename identity<std::function<T*(args_types...)>>::type&& factory,
            typename identity<std::function<void(T*)>>::type&& deleter = {});

    template <typename T, typename... args_types>
    registration<T, args_types...> define_default(
            typename identity<std::function<std::unique_ptr<T>(const activation_context&, args_types...)>>::type&& factory);

    template <typename T, typename... args_types>
    registration<T, args_types...> define_default(
            typename identity<std::function<std::unique_ptr<T>(args_types...)>>::type&& factory);

    /**
     * @brief
     * @tparam T
     * @param id
     * @return
     */
    template <typename T>
    registration<T> define_type(const std::string& id);
    /**
     * @brief
     * @tparam T
     * @tparam args_types
     * @return
     */
    template <typename T>
    registration<T> define_type();
    /**
     * @brief
     * @tparam T
     * @tparam args_types
     * @param id
     * @return
     */
    template <typename T, typename... args_types>
    registration<T> define_explicit_type(const std::string& id);
    /**
     * @brief
     * @tparam T
     * @tparam args_types
     * @return
     */
    template <typename T, typename... args_types>
    registration<T> define_explicit_type();

    /**
     * @brief Defines an instance of a type.
     * @details
     * An instance of a type will be created by calling constructor which matches passed arguments according to sfine
     * rules.
     *
     * @tparam T Type, which instance should be registered.
     * @tparam args_types Types of arguments passed to constructor.
     * @param id Instance identifier.
     * @param args Arguments which will be passed to the constructor when initialised.
     * @return An instance of **registration** allowing further customisation of registered definition.
     */
    template <typename T, typename... args_types>
    registration<T> define_instance(const std::string& id, args_types... args);
    /**
     * @brief Defines an type and arguments for its constructor.
     * @details
     * An instance of a type will be created by calling constructor which matches passed arguments according to sfine
     * rules.
     *
     * @tparam T Type, which instance should be registered.
     * @tparam args_types Types of arguments passed to constructor.
     * @param args Arguments which will be passed to the constructor when initialised.
     * @return An instance of **registration** allowing further customisation of registered definition.
     */
    template <typename T, typename... args_types>
    registration<T> define_default_instance(args_types... args);

    /**
     * @brief Defines static method or C style function as a type factory under unique registration id.
     * @details
     * This method implements automatic template argument resultion from factory method definition. It is recommended to
     * use factory methods in conjunction with a module. For instance:
     * @code
     *
     * [...]
     *
     * class event_loader
     * {
     * public:
     *      void operator()(tools::di::definition_builder& builder);
     *
     * private:
     *      // detector
     *      static event_detector construct_detector(
     *          const wavepatml::abstractEvent& event);
     * [...]
     *
     * void event_loader::operator ()(tools::di::definition_builder& builder)
     * {
     *      builder
     *          .define_factory("detector_id", &event_loader::construct_detector)
     *          .as<detection_function>();
     *
     * [...]
     *
     * @endcode
     *
     * In the above example use of factory method passes resolution of definition template arguments to compiler simplifying
     * definition.
     *
     * @tparam T Type returned by factory method. Factory will be registered as a definition of this type.
     * @tparam args_types Factory method arguments.
     * @param id An identifier under which the factory is being registered
     * @param factory A pointer to factory function. This overload strips access to **activation_context**.
     * @return An instance of **registration** allowing further customisation of registered definition.
     */
    template <typename T, typename... args_types>
    registration<T, args_types...> define_factory(
            const std::string &id,
            T factory(args_types...));
    /**
     * @brief Defines static method or C style function as a type factory under unique registration id.
     * @details
     * This method implements automatic template argument resolution from factory definition. It is recommended to
     * use factory methods in conjunction with a module. For instance:
     * @code
     *
     * [...]
     *
     * class event_loader
     * {
     * public:
     *      void operator()(tools::di::definition_builder& builder);
     *
     * private:
     *      // detector
     *      static event_detector construct_detector(
     *          const tools::di::activation_context& context,
     *          const wavepatml::abstractEvent& event);
     * [...]
     *
     * void event_loader::operator ()(tools::di::definition_builder& builder)
     * {
     *      builder
     *          .define_factory("detector_id", &event_loader::construct_detector)
     *          .as<detection_function>();
     *
     * [...]
     *
     * @endcode
     *
     * In the above example, template arguments of definitio nregistration are automatically resolved from the function
     * signature.
     *
     * @tparam T Type returned by factory method. Factory will be registered as a definition of this type.
     * @tparam args_types Factory method arguments.
     * @param id An identifier under which the factory is being registered
     * @param factory A pointer to factory function. This overload adds access to **activation_context**.
     * @return An instance of **registration** allowing further customisation of registered definition.
     */
    template <typename T, typename... args_types>
    registration<T, args_types...> define_factory(
            const std::string &id,
            T factory(const activation_context&, args_types...));
    /**
     * @brief Defines static method or C style function as a type factory under default registration id.
     * @details
     * @paragraph
     *
     * This method implements automatic template argument resolution from factory definition. It is recommended to
     * use factory methods in conjunction with a module. For instance:
     *
     * @code
     *
     * [...]
     *
     * class event_loader
     * {
     * public:
     *      void operator()(tools::di::definition_builder& builder);
     *
     * private:
     *      // detector
     *      static event_detector construct_detector(
     *          const wavepatml::abstractEvent& event);
     * [...]
     *
     * void event_loader::operator ()(tools::di::definition_builder& builder)
     * {
     *      builder
     *          .define_factory(&event_loader::construct_detector)
     *          .as<detection_function>();
     *
     * [...]
     *
     * @endcode
     *
     * In the above example, template arguments of definitio nregistration are automatically resolved from the function
     * signature.
     *
     * @tparam T Component type for which the factory is being registered.
     * @tparam args_types Types of arguments required by the registered factory.
     * @param factory A pointer to function or static method which will act as a factory.
     * @return An instance of **registration** allowing further customisation of registered definition.
     */
    template <typename T, typename... args_types>
    registration<T, args_types...> define_factory(
            T factory(args_types...));
    /**
     * @brief Defines static method or C style function as a type factory under default registration id.
     * @details
     * @paragraph
     *
     * This method implements automatic template argument resolution from factory definition. It is recommended to
     * use factory methods in conjunction with a module. For instance:
     *
     * @code
     *
     * [...]
     *
     * class event_loader
     * {
     * public:
     *      void operator()(tools::di::definition_builder& builder);
     *
     * private:
     *      // detector
     *      static event_detector construct_detector(
     *          const tools::di::activation_context& context,
     *          const wavepatml::abstractEvent& event);
     * [...]
     *
     * void event_loader::operator ()(tools::di::definition_builder& builder)
     * {
     *      builder
     *          .define_factory(&event_loader::construct_detector)
     *          .as<detection_function>();
     *
     * [...]
     *
     * @endcode
     *
     * In the above example, template arguments of definitio nregistration are automatically resolved from the function
     * signature.
     *
     * @tparam T Component type for which the factory is being registered.
     * @tparam args_types Types of arguments required by the registered factory.
     * @param factory A pointer to function or static method which will act as a factory. Provides access to **activation_context**.
     * @return An instance of **registration** allowing further customisation of registered definition.
     */
    template <typename T, typename... args_types>
    registration<T, args_types...> define_factory(
            T factory(const activation_context&, args_types...));
    /**
     * @brief Defines member method as a type factory.
     * @details
     * This method implements automatic template argument resultion from factory method definition. It is recommended to
     * use factory methods in conjunction with a module. For instance:
     * @code
     *
     * [...]
     *
     * class event_loader
     * {
     * public:
     *      void operator()(tools::di::definition_builder& builder);
     *
     * private:
     *      // detector
     *      event_detector construct_detector(
     *          const wavepatml::abstractEvent& event);
     * [...]
     *
     * void event_loader::operator ()(tools::di::definition_builder& builder)
     * {
     *      builder
     *          .define_factory("detector_id", &event_loader::construct_detector, this)
     *          .as<detection_function>();
     *
     * [...]
     *
     * @endcode
     *
     * In the above example use of factory method passes resolution of definition template arguments to compiler simplifying
     * definition.
     *
     * @tparam T Type returned by factory method. Factory will be registered as a definition of this type.
     * @tparam args_types Factory method arguments.
     * @param id An identifier under which the factory is being registered
     * @param factory A pointer to factory function. This overload strips access to **activation_context**.
     * @return n instance of **registration** allowing further customisation of registered definition.
     */
    template <typename T, typename class_type, typename... args_types>
    registration<T, args_types...> define_factory(
            const std::string &id,
            T (class_type::*factory)(args_types...),
            class_type* instance);
    /**
     * @brief Defines member method as a type factory under default registration id.
     * @details
     * This method implements automatic template argument resultion from factory method definition. It is recommended to
     * use factory methods in conjunction with a module. For instance:
     * @code
     *
     * [...]
     *
     * class event_loader
     * {
     * public:
     *      void operator()(tools::di::definition_builder& builder);
     *
     * private:
     *      // detector
     *      event_detector construct_detector(
     *          const tools::di::activation_context& context,
     *          const wavepatml::abstractEvent& event);
     * [...]
     *
     * void event_loader::operator ()(tools::di::definition_builder& builder)
     * {
     *      builder
     *          .define_factory(&event_loader::construct_detector, this)
     *          .as<detection_function>();
     *
     * [...]
     *
     * @endcode
     *
     * In the above example use of factory method passes resolution of definition template arguments to compiler simplifying
     * definition.
     *
     * @tparam T Type returned by factory method. Factory will be registered as a definition of this type.
     * @tparam args_types Factory method arguments.
     * @param id An identifier under which the factory is being registered
     * @param factory A pointer to factory function. This overload adds access to **activation_context**.
     * @return n instance of **registration** allowing further customisation of registered definition.
     */
    template <typename T, typename class_type, typename... args_types>
    registration<T, args_types...> define_factory(
            const std::string &id,
            T (class_type::*factory)(const activation_context&, args_types...),
            class_type* instance);
    /**
     * @brief Defines member method as a type factory under default registration id.
     * @details
     * @paragraph
     *
     * This method implements automatic template argument resolution from factory definition. It is recommended to
     * use factory methods in conjunction with modules. For instance:
     *
     * @code
     *
     * [...]
     *
     * class event_loader
     * {
     * public:
     *      void operator()(tools::di::definition_builder& builder);
     *
     * private:
     *      // detector
     *      static event_detector construct_detector(
     *          const wavepatml::abstractEvent& event);
     * [...]
     *
     * void event_loader::operator ()(tools::di::definition_builder& builder)
     * {
     *      builder
     *          .define_factory(&event_loader::construct_detector)
     *          .as<detection_function>();
     *
     * [...]
     *
     * @endcode
     *
     * In the above example, template arguments of definitio nregistration are automatically resolved from the function
     * signature.
     *
     * @tparam T Component type for which the factory is being registered.
     * @tparam args_types Types of arguments required by the registered factory.
     * @param factory A pointer to function or static method which will act as a factory.
     * @return An instance of **registration** allowing further customisation of registered definition.
     */
    template <typename T, typename class_type, typename... args_types>
    registration<T, args_types...> define_factory(
            T (class_type::*factory)(args_types...),
            class_type* instance);

    template <typename T, typename class_type, typename... args_types>
    registration<T, args_types...> define_factory(
            T (class_type::*factory)(const activation_context&, args_types...),
            class_type* instance);

    template <typename T, typename... args_types>
    const interceptor_definition& define_interceptor(
            typename identity<std::function<void(T& activated, const activation_context&, args_types...)>>::type&& interceptor);

    template <typename T, typename... args_types>
    const interceptor_definition& define_interceptor(
            typename identity<std::function<void(const activation_context&, args_types...)>>::type&& interceptor);

    template <typename T, typename... args_types>
    const interceptor_definition& define_interceptor(
            typename identity<std::function<void(args_types...)>>::type&& interceptor);

    template <typename T, typename class_type, typename... args_types>
    const interceptor_definition& define_interceptor(
            void (class_type::*interceptor)(T&, const activation_context&, args_types...),
            class_type* instance);

    template <typename T, typename class_type, typename... args_types>
    const interceptor_definition& define_interceptor(
            void (class_type::*interceptor)(T&, args_types...),
            class_type* instance);

    template <typename T, typename class_type, typename... args_types>
    const interceptor_definition& define_interceptor(
            void (class_type::*interceptor)(const activation_context&, args_types...),
            class_type* instance);

    template <typename T, typename class_type, typename... args_types>
    const interceptor_definition& define_interceptor(
            void (class_type::*interceptor)(args_types...),
            class_type* instance);

    /**
     * @brief Defines a decorator for activated instance of T.
     * @details
     * @paragraph
     * Decorator pattern described here - https://en.wikipedia.org/wiki/Decorator_pattern. In object-oriented programming,
     * the decorator pattern is a design pattern that allows behavior to be added to an individual object, either
     * statically or dynamically, without affecting the behavior of other objects from the same class.
     *
     * @paragraph
     * This overload makes use of decorator factory functor which takes as an argument a pointer to object which should be
     * decorated and returnes a pointer to newly created decorator. Ownership of decorated object is transfered to the
     * decorator and it is decorators reposibility to recycle decorated instance.
     *
     * @paragraph
     * This overload provided access to current **activation_context** from within decorator factory functor.
     *
     * @tparam T Registered type for which decorator will be registered.
     * @param decorator Decorator factory functor respoble for creation of the actual decorator object
     * @param deleter A deleter function defining a way how created decorator should be disposed.
     */
    template <typename T>
    const decorator_definition& define_decorator(
            typename identity<std::function<T*(T* activated, const activation_context&)>>::type&& decorator,
            typename identity<std::function<void(T*)>>::type&& deleter);

    /**
     * @brief Defines a decorator for activated instance of T.
     * @details
     * @paragraph
     * Decorator pattern described here - https://en.wikipedia.org/wiki/Decorator_pattern. In object-oriented programming,
     * the decorator pattern is a design pattern that allows behavior to be added to an individual object, either
     * statically or dynamically, without affecting the behavior of other objects from the same class.
     *
     * @paragraph
     * This overload makes use of decorator factory functor which takes as an argument a pointer to object which should be
     * decorated and returnes a pointer to newly created decorator. Ownership of decorated object is transfered to the
     * decorator and it is decorators reposibility to recycle decorated instance.
     *
     * @tparam T Registered type for which decorator will be registered.
     * @param decorator Decorator factory functor respoble for creation of the actual decorator object
     * @param deleter A deleter function defining a way how created decorator should be disposed.
     */
    template <typename T>
    const decorator_definition& define_decorator(
            typename identity<std::function<T*(T* activated)>>::type&& decorator,
            typename identity<std::function<void(T*)>>::type&& deleter);

    /**
     * @brief Defines a decorator for activated instance of T.
     * @details
     * @paragraph
     * Decorator pattern described here - https://en.wikipedia.org/wiki/Decorator_pattern. In object-oriented programming,
     * the decorator pattern is a design pattern that allows behavior to be added to an individual object, either
     * statically or dynamically, without affecting the behavior of other objects from the same class.
     *
     * @paragraph
     * This overload makes use of unique_ptr managed instance for both decorated and decorating instances. In C++, having
     * decorator pattern implemented through inheritance requires both decorator and decorated types to share the same base types.
     * Base type instance can't be referenced in RAII fashion, the only way is to use pointers, references or managed pointers.
     * This method provides support for managed pointers variant.
     *
     * @paragraph
     * This overload provided access to current **activation_context** from within decorator factory functor.
     *
     * @tparam T Registered type for which decorator will be registered.
     * @param decorator Decorator factory functor respoble for creation of the actual decorator object
     * @param deleter A deleter function defining a way how created decorator should be disposed.
     */
    template <typename T>
    const decorator_definition& define_decorator(
            typename identity<std::function<
                    std::unique_ptr<T>(std::unique_ptr<T>&& activated, const activation_context&)>>::type&& decorator);

    /**
     * @brief Defines a decorator for activated instance of T.
     * @details
     * @paragraph
     * Decorator pattern described here - https://en.wikipedia.org/wiki/Decorator_pattern. In object-oriented programming,
     * the decorator pattern is a design pattern that allows behavior to be added to an individual object, either
     * statically or dynamically, without affecting the behavior of other objects from the same class.
     *
     * @paragraph
     * This overload makes use of unique_ptr managed instance for both decorated and decorating instances. In C++, having
     * decorator pattern implemented through inheritance requires both decorator and decorated types to share the same base types.
     * Base type instance can't be referenced in RAII fashion, the only way is to use pointers, references or managed pointers.
     * This method provides support for managed pointers variant.
     *
     * @tparam T Registered type for which decorator will be registered.
     * @param decorator Decorator factory functor respoble for creation of the actual decorator object
     * @param deleter A deleter function defining a way how created decorator should be disposed.
     */
    template <typename T>
    const decorator_definition& define_decorator(
            typename identity<std::function<
                    std::unique_ptr<T>(std::unique_ptr<T>&& activated)>>::type&& decorator);

    /**
     * @brief Defines a decorator for activated instance of T.
     * @details
     * @paragraph
     * Decorator pattern described here - https://en.wikipedia.org/wiki/Decorator_pattern. In object-oriented programming,
     * the decorator pattern is a design pattern that allows behavior to be added to an individual object, either
     * statically or dynamically, without affecting the behavior of other objects from the same class.
     * (https://en.wikipedia.org/wiki/Decorator_pattern).
     *
     * @paragraph
     * This overload is designed for decoration with intermediate type in RAII fashion. Consider:
     * @code
     * [...]
     *
     * using test_function = std::function<void()>;
     *
     * static auto component_count = 0u;
     * static auto decorator_count = 0u;
     * struct component
     * {
     *      void operator()()
     *      {
     *          component_count++;
     *      }
     * };
     *
     * struct decorator
     * {
     *      decorator(test_function&& undecorated)
     *          : undecorated_(std::move(undecorated))
     *      {
     *
     *      }
     *
     *      void operator()()
     *      {
     *          decorator_count++;
     *          undecorated_();
     *      }
     *
     *      test_function undecorated_;
     * };
     *
     * definition_builder builder;
     * builder.define_default<test_function>([]() -> component
     * {
     *      return component();
     * });
     * builder.define_decorator<test_function>([](test_function&& undecorated) -> decorator
     * {
     *      return decorator(std::move(undecorated));
     * });
     *
     * [...]
     * @endcode
     *
     * **test_function** is registered as a type definition, decorator also will have a form of **test_function** in this
     * way both decorated and decorating types are wrapped in a third type. RAII can be used to manage both decorated and
     * decorating types in this situation. Interface of decorator factory functor ensures transfer of ownership of decorated
     * type into decorator.
     *
     * @paragraph
     * This overload provided access to current **activation_context** from within decorator factory functor.
     *
     * @tparam T Registered type for which decorator will be registered.
     * @param decorator Decorator factory functor respoble for creation of the actual decorator object
     * @param deleter A deleter function defining a way how created decorator should be disposed.
     */
    template <typename T>
    const decorator_definition& define_decorator(
            typename identity<std::function<T(T&& activated, const activation_context&)>>::type&& decorator);

    /**
     * @brief Defines a decorator for activated instance of T.
     * @details
     * @paragraph
     * Decorator pattern described here - https://en.wikipedia.org/wiki/Decorator_pattern. In object-oriented programming,
     * the decorator pattern is a design pattern that allows behavior to be added to an individual object, either
     * statically or dynamically, without affecting the behavior of other objects from the same class.
     * (https://en.wikipedia.org/wiki/Decorator_pattern).
     *
     * @paragraph
     * This overload is designed for decoration with intermediate type in RAII fashion. Consider:
     * @code
     * [...]
     *
     * using test_function = std::function<void()>;
     *
     * static auto component_count = 0u;
     * static auto decorator_count = 0u;
     * struct component
     * {
     *      void operator()()
     *      {
     *          component_count++;
     *      }
     * };
     *
     * struct decorator
     * {
     *      decorator(test_function&& undecorated)
     *          : undecorated_(std::move(undecorated))
     *      {
     *
     *      }
     *
     *      void operator()()
     *      {
     *          decorator_count++;
     *          undecorated_();
     *      }
     *
     *      test_function undecorated_;
     * };
     *
     * definition_builder builder;
     * builder.define_default<test_function>([]() -> component
     * {
     *      return component();
     * });
     * builder.define_decorator<test_function>([](test_function&& undecorated) -> decorator
     * {
     *      return decorator(std::move(undecorated));
     * });
     *
     * [...]
     * @endcode
     *
     * **test_function** is registered as a type definition, decorator also will have a form of **test_function** in this
     * way both decorated and decorating types are wrapped in a third type. RAII can be used to manage both decorated and
     * decorating types in this situation. Interface of decorator factory functor ensures transfer of ownership of decorated
     * type into decorator.
     *
     * @paragraph
     * This overload provided access to current **activation_context** from within decorator factory functor.
     *
     * @tparam T Registered type for which decorator will be registered.
     * @param decorator Decorator factory functor respoble for creation of the actual decorator object
     * @param deleter A deleter function defining a way how created decorator should be disposed.
     */
    template <typename T>
    const decorator_definition& define_decorator(
            typename identity<std::function<T(T&& activated)>>::type&& decorator);

    template <typename T, typename class_type>
    const decorator_definition& define_decorator(
            std::unique_ptr<T> (class_type::*decorator)(std::unique_ptr<T>&&, const activation_context&),
            class_type* instance);

    template <typename T, typename class_type>
    const decorator_definition& define_decorator(
            std::unique_ptr<T> (class_type::*decorator)(std::unique_ptr<T>&&),
            class_type* instance);

    template <typename T>
    const decorator_definition& define_decorator(
            T decorator(T&&, const activation_context&));

    template <typename T>
    const decorator_definition& define_decorator(
            T decorator(T&&));

    template <typename T, typename class_type>
    const decorator_definition& define_decorator(
            T (class_type::*decorator)(T&&, const activation_context&),
            class_type* instance);

    template <typename T, typename class_type>
    const decorator_definition& define_decorator(
            T (class_type::*interceptor)(T&&),
            class_type* instance);

    template <typename module_type>
    definition_builder& define_module(const module_type&& module);

private:
    friend instance_activator;
    template <typename T, typename... args_types>
    friend class registration;

    template <typename T>
    class magic_argument final
    {
    public:
        explicit magic_argument(const activation_context& context, size_t);

        template <typename U=T, typename = typename std::enable_if_t<!std::is_same<U, T>::value>>
        operator U();

        template <typename U=T, typename = typename std::enable_if_t<!std::is_same<U, T>::value>>
        operator std::unique_ptr<U>();

        template <typename U=T, typename = typename std::enable_if_t<!std::is_same<U, T>::value>>
        operator std::shared_ptr<U>();

        template <typename U=T, typename = typename std::enable_if_t<!std::is_same<U, T>::value>>
        operator U&() const;

        template <typename U=T, typename = typename std::enable_if_t<!std::is_same<U, T>::value>>
        operator U&&() const;

        template <typename U, typename = typename std::enable_if_t<!std::is_same<U, T>::value>>
        operator const U&() const;

    private:
        const activation_context& context_;

    };

    template <typename T, size_t... indices>
    static typename std::enable_if_t<sizeof...(indices) == 0, T>* magic_factory_impl(
            const std::string& id,
            const activation_context& context,
            std::integer_sequence<size_t, indices...>);

    template <typename T, size_t... indices>
    static typename std::enable_if_t<sizeof...(indices) != 0, T>* magic_factory_impl(
            const std::string& id,
            const activation_context& context,
            std::integer_sequence<size_t, indices...>);

    template <typename T>
    static T* magic_factory(
            const std::string& id,
            const activation_context& context);

    template <typename T, typename... args_types>
    registration<T, args_types...> try_define(
            const std::string& id,
            typename identity<std::function<T*(const activation_context&, args_types...)>>::type&& creator,
            typename identity<std::function<void(T*)>>::type&& deleter);

    template <typename T, typename... args_types>
    const interceptor_definition& try_define_interceptor(
            typename identity<std::function<void(T&, const activation_context&, args_types...)>>::type&& interceptor);

    template <typename T>
    const decorator_definition& try_define_decorator(
            typename identity<std::function<T*(T*, const activation_context&)>>::type&& decorator,
            typename identity<std::function<void(T*)>>::type&& deleter);

    definition::map_type definitions_;
    interceptor_definition::map_type interceptors_;
    decorator_definition::map_type decorators_;
    std::list<boost::any> modules_;

};

}

#include "definition_builder.ipp"

