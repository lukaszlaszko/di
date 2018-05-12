## Dependency Injection

### Introduction

Structuring an application with *inversion of control* (IoC) requires organised and easy to comprehend technique for composing 
complex hierarchy of components. That is where [Dependency Injection](https://en.wikipedia.org/wiki/Dependency_injection) - DI
comes in hand. In basic understanding, DI is about automated resolution of dependencies. Dependences for a component 
are defined at the level at which that particular component is resolved. As such interaction with *dependency injection*
framework comprises pf 2 phases:
1. **Registration of components**, when recipes of how to create components are registered with component factory map.   
2. **Activation of components**, where starting from requested component, recursively the component and all dependencies 
are created using definitions registered in the previous step.

Those two phases cannot non-interleave. Meaning component registration has to be completely finished before any activation takes place.
No additional definitions can be added once advanced to activation phase.

All components related to dependency injection are located in `tools::di` namespace.       

### Component registration

Component registration is implemented with [definition_builder](definition_builder.hpp). Any object convertable to [std::function](http://en.cppreference.com/w/cpp/utility/functional/function)
can be registered as a factory. A factory can define ownership and cleaup of allocated objects in 3 ways:
  
* **pointer**

    This type of definition requires registration of 2 callbacks:
    1. factory functor - invoked when object is intended to be activated.
    2. deleter functor - defining how object allocated with the factory should be destroyed.    
    
    Example:        
               
        #include <tools/di/definition_builder.hpp>
        
        [...]
        
        definition_builder builder;
        builder.define<TestObject_1>(sample_id, []() -> TestObject_1*
        {
            auto pointer = new TestObject_1();
            pointer->field1_ = sample_id;
    
            return pointer;
        },
        [](TestObject_1* instance)
        {
            delete instance;
        });
    
    To simplify the above signature, an overload has been defined to use default destroyer implented with c++ `delete`
    operator. 
        

* **[RAII](http://en.cppreference.com/w/cpp/language/raii) value**

    Direct by value initialisation. The most straight forward, but with limitations - it is impossible to expose only 
    abstract interface without complete type implementation. 

    Example:

        #include <tools/di/definition_builder.hpp>
        
        [...]
        
        definition_builder builder;
        builder.define_default<TestObject_1, string&>([](string& parameter0) -> TestObject_1
        {
            return { parameter0 };
        });

* **[std::unique_ptr](http://en.cppreference.com/w/cpp/memory/unique_ptr)**

    Ownership of an object is solely related to lifetime of wrapping `std::unique_ptr'. `std::unique_ptr' can be stored 
    on a stack or in a field while the actual object is allocted on the heap (or pool) and accessible by a pointer. This 
    type of definition allows registration of interface which implementation can be hidded from the end user.
    
[activation_context](activation_context.hpp) can be optionally specified as first argument factory definition. This componet 
provides entry point to activation of dependencies and interaction with activation hierarchy. For instance:

    [...]
    
    event_detector event_loader::construct_detector(
            const activation_context& context,
            const wavepatml::abstractEvent& event)
    {
        vector<comparison_function> comparisons;
    
        if (event.base())
        {
            auto base_name = event.base().get();
            comparisons.push_back(
                    context.activate<event_detector>(base_name)
                           .as<comparison_function>());
        }
        
    [...]     

In the above `context.activate` is used to resolve a dependency from a context of factory method. 
        

### Component activation

Once all definitions are registered with [definition_builder](definition_builder.hpp) that builder is used to create an 
instance of [instance_activator](instance_activator.hpp). Instance activator allows instatiation of registered types in 3 different ways:
* [RAII](http://en.cppreference.com/w/cpp/language/raii) - either direct on stack or inline in class instantiation of activated type.

    Example:

        #include <tools/di/definition_builder.hpp>
        #include <tools/di/instance_activator.hpp>
            
        [...]
            
        definition_builder builder;
        builder.define_default<TestObject_1>([]() -> TestObject_1
        {
            return { sample_id };
        });
    
        instance_activator activator(std::move(builder));
        auto instance = activator.activate_default_raii<TestObject_1>();    
    
* allocation of [std::unique_ptr](http://en.cppreference.com/w/cpp/memory/unique_ptr)

    Example:

        #include <tools/di/definition_builder.hpp>
        #include <tools/di/instance_activator.hpp>
        
        [...]
        
        definition_builder builder;
        builder.define_default<TestObject_1>([]() -> TestObject_1
        {
            return { sample_id };
        });
    
        instance_activator activator(std::move(builder));
        auto instance = activator.activate_default_unique<TestObject_1>();
        
        [...]

* allocation of [std::shared_ptr](http://en.cppreference.com/w/cpp/memory/shared_ptr)

    Example:

        #include <tools/di/definition_builder.hpp>
        #include <tools/di/instance_activator.hpp>
        
        [...]
    
            
        definition_builder builder;
        builder.define_default<TestObject_1, string&>([](string& parameter0) -> TestObject_1
        {
            return { parameter0 };
        });
    
        instance_activator activator(std::move(builder));
        auto instance = activator.activate_default_shared<TestObject_1, string&>(instance_id);
        
        [...] 

    Component activated as a shared pointer remains active until at least once instance of managing 
    [std::shared_ptr](http://en.cppreference.com/w/cpp/memory/shared_ptr) remains active. If all instances of managing 
    smart pointer are destroyed component is going to be recycled.      
        

### Modules

A module is a small class that can be used to bundle up a set of related components behind a ‘facade’ to simplify configuration 
and deployment. The module exposes a deliberate, restricted set of configuration parameters that can vary independently of 
the components used to implement the module.

The components within a module still make use dependencies at the component/service level to access components from other modules.

Modules do not, themselves, go through dependency injection. They are used to configure definitions, they are not actually 
registered and resolved like other components.

Example:

    #include <tools/di/definition_builder.hpp>
    #include <tools/di/instance_activator.hpp>
    
    [...]
    
    struct TestModule
    {
        void operator()(definition_builder& builder)
        {
            builder.define_default<TestObject_1>(*this);
        }
    
        TestObject_1 operator()()
        {
            return { sample_id };
        }
    };
    
    definition_builder builder;
    builder.define_module(TestModule());
    
    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_unique<TestObject_1>();

### Interception

An interceptor is a method injected just after a component is activated. In this way client code can subscribe to notifications
about component instances activation with corresponding activation arguments.

Example:

    #include <tools/di/definition_builder.hpp>
    #include <tools/di/instance_activator.hpp>
    
    [...]
    
    definition_builder builder;
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });
    
    auto intercepted = false;
    builder.define_interceptor<TestObject_1>([&intercepted](TestObject_1& activated, const activation_context& context)
    {
        intercepted = true;
    });
    
    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_raii<TestObject_1>();

An interceptor can be defined as a lambda expression or any other object convertable to [std::function](http://en.cppreference.com/w/cpp/utility/functional/function).

Modules implementing interception:
* [layaway_list](src/patterns/toolbox/layaway_list.hpp)
* [lookback_range](src/patterns/toolbox/lookback_range.hpp)
* [reference_tracker](src/patterns/toolbox/reference_tracker.hpp) 
    

### Decoration

[decorator pattern](https://en.wikipedia.org/wiki/Decorator_pattern) support has been implemented as a special type of interception. 
Registered decorator function (or any functional object which can be converted into [std::function](http://en.cppreference.com/w/cpp/utility/functional/function))
is invoked whenever an instance of decorated type is activated. Decorator factory is then given with rvalue of the created component and 
is intended to return a new instance of component of that type wrapping given component with additional 'decorations'.

For example:

    #include <tools/di/definition_builder.hpp>
    #include <tools/di/instance_activator.hpp>
    
    [...]
    
    using test_function = std::function<void()>;
    
    static auto component_count = 0u;
    static auto decorator_count = 0u;
    
    struct component
    {
        void operator()()
        {
            component_count++;
        }
    };
    
    struct decorator
    {
        decorator(test_function&& undecorated)
                : undecorated_(std::move(undecorated))
        {
    
        }
    
        void operator()()
        {
            decorator_count++;
            undecorated_();
        }
    
        test_function undecorated_;
    };
    
    definition_builder builder;
    builder.define_default<test_function>([]() -> component
    {
        return component();
    });
    
    // decorator 1
    builder.define_decorator<test_function>([](test_function&& undecorated) -> decorator
    {
        return decorator(std::move(undecorated));
    });
    // decorator 2
    builder.define_decorator<test_function>([](test_function&& undecorated) -> decorator
    {
        return decorator(std::move(undecorated));
    });
    
    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_raii<test_function>();

Due to nature of C++ following two modes of decoration are supported:
* **[std::unique_ptr](http://en.cppreference.com/w/cpp/memory/unique_ptr)**

    Consider following example:

        #include <tools/di/definition_builder.hpp>
        #include <tools/di/instance_activator.hpp>
        
        [...]
    
        
        static auto component_count = 0u;
        static auto decorator_count = 0u;
        
        struct interface
        {
            virtual void method() = 0;
        };
        
        struct component : interface
        {
            void method() override
            {
                component_count++;
            }
        };
        
        struct decorator : interface
        {
            decorator(unique_ptr<interface>&& undecorated)
                : undecorated_(std::move(undecorated))
            {
        
            }
        
            void method() override
            {
                decorator_count++;
                undecorated_->method();
            }
        
            unique_ptr<interface> undecorated_;
        };
        
        definition_builder builder;
        builder.define_default<interface>([]() -> unique_ptr<interface>
        {
            return unique_ptr<interface>(new component());
        });
        
        builder.define_decorator<interface>([](unique_ptr<interface>&& undecorated) -> unique_ptr<interface>
        {
            return unique_ptr<interface>(new decorator(std::move(undecorated)));
        });
        
        instance_activator activator(std::move(builder));
        auto instance = activator.activate_default_unique<interface>();
    
    `interface` is a pure virtual type from which both `component` and `decorator` types derive. `component` is the actual type 
    implementing desired logic and it is defined as implementation of `interface` which can be resolved.
    However, there is a desire to have a decorating counter incrementing each time `interface::method` is called. For this purpose 
    a decoration is registered. Decoration wraps newly activated component with a decorator and returns pointer to that decorator.
    In this way from perspective of client code there's no interface difference between requested type and activated type. However
    under the hood DI takes care of choosing implementation as well as wrapping with decorators.   

* **wrapping**

    This mode is reserved for situation when an intermediate type is used as component interface. For instance, consider the following:

        #include <tools/di/definition_builder.hpp>
        #include <tools/di/instance_activator.hpp>
        
        [...]
            
        using test_function = std::function<void()>;
        
        static auto component_count = 0u;
        static auto decorator_count = 0u;
        
        struct component
        {
            void operator()()
            {
                component_count++;
            }
        };
        
        struct decorator
        {
            decorator(test_function&& undecorated)
                : undecorated_(std::move(undecorated))
            {
        
            }
        
            void operator()()
            {
                decorator_count++;
                undecorated_();
            }
        
            test_function undecorated_;
        };
        
        definition_builder builder;
        builder.define_default<test_function>([]() -> component
        {
            return component();
        });
        builder.define_decorator<test_function>([](test_function&& undecorated) -> decorator
        {
            return decorator(std::move(undecorated));
        });
        
        instance_activator activator(std::move(builder));
        auto instance = activator.activate_default_raii<test_function>();
    
    Wrapping type `std::function<void()>` is used as as an interface. Factory method registered with `definition_builder`
    defines wrapping `component` as a way of creating instance of `std::function<void()>`. However, like in previous example,
    a `decorator` is defined. This decorator does not share common base type through which it would interface with the `component`
    instead a wrapping `std::function<void()>` interface is used. In this way `decorator` wrapped into `std::function<void()>`
    can be used as a replacement of `component` produced by decoration factory.          

Modules implementing decoration:
* [trace_decorator](src/patterns/toolbox/loaders/decorators/trace_decorator.hpp)
* [benchmark_decorator](src/patterns/toolbox/loaders/decorators/benchmark_decorator.hpp)

### Annotations

Annotation mechanism is used to pass context information into activation stack during activation. For instance, consider 
passing detector base resolution in a way it would become available to all invoked factory methods:

    // detector_loader.cpp
    [...]
    
    auto instance = activator.activate_raii<detection_function>(
            pattern.name(),
            annotations_map(
                    pattern.resolution(),
                    pattern.instrument()));
    
    [...] 

Top level activation of `detection_function` is annotated with pattern resolution and reference to instrument. They will be 
available as context information for all factory methods activating descendant objects. For instance:

    accessor_function event_loader::construct_accessor_position(
            const activation_context& context,
            const wavepatml::condition::position_type& position)
    {
        wavepatml::positionIndicator::value_type indicator = position.indicator();
        switch (indicator)
        {
            case wavepatml::positionIndicator::price_movement:
                return price_movement_accessor();
            case wavepatml::positionIndicator::bars_since_enter:
            {
                auto pattern_resolution = context.has_annotation<resolution::id>()
                                          ? context.annotation<resolution::id>()
                                          : resolution::__undefined;

Annotations can be overriden for lower nodes of activation tree. For instance:

    event_detector detector_instance = context
            .activate<event_detector>(event_reference.name())
            .with_optional_annotation(event_reference.timeFrame(), &to_resolution);

In general, annotations are registered on type basis. It means that once registered annotation can be retrieved using type id.
For instance `context.has_annotation<resolution::id>()` and `context.annotation<resolution::id>()` would check if `resolution::id`
has been defined as annotation and retrieve that annotation. 

This can be limitation in some situation, where multiple annotations of the same type would need to be defined. In such situation
[annotation](annotation.hpp) type can be used. [annotation](annotation.hpp) is a template wrapping annotated entry with tag information.
Consider following example:

    static const auto name_tag = 1u;
    static const auto surname_tag = 2u;
    
    definition_builder builder;
    builder.define_default<TestObject_1>([](const activation_context& context) ->TestObject_1
    {
        auto& name = context.annotation<annotation<string, name_tag>::type>();
        auto& surname = context.annotation<annotation<string, surname_tag>::type>();
    
        return {  name.value + surname.value };
    });
    
    instance_activator activator(std::move(builder));
    
    annotations_map annotations(
            make_annotation<name_tag>(string("John")),
            make_annotation<surname_tag>(string("Smith")));
    
    auto instance = activator.activate_default_raii<TestObject_1>(std::move(annotations));
   

