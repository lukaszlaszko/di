#include <di/definition_builder.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <string>
#include <type_traits>
#include <typeinfo>

using namespace std;
using namespace std::placeholders;
using namespace di;

struct TestObject_1
{
    string field1_;
};

const auto sample_id = "sample-id";


TEST(definition_builder, create)
{
    definition_builder builder;
}

TEST(definition_builder, define_as_rvalue_no_context)
{
    definition_builder builder;
    auto registration = builder.define<TestObject_1>(sample_id, []() -> TestObject_1
    {
        return { sample_id };
    });

    ASSERT_EQ(registration.id(), sample_id);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_as_rvalue_with_context)
{
    definition_builder builder;
    auto registration = builder.define<TestObject_1>(sample_id, [](auto& context) -> TestObject_1
    {
        return { sample_id };
    });

    ASSERT_EQ(registration.id(), sample_id);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_as_rvalue_one_argument)
{
    definition_builder builder;
    auto registration = builder.define<TestObject_1, string>(sample_id, [](string parameter0) -> TestObject_1
    {
        return { parameter0 };
    });

    ASSERT_EQ(registration.id(), sample_id);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1, string>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_as_rvalue_two_arguments)
{
    definition_builder builder;
    auto registration = builder.define<TestObject_1, string, string&>(sample_id, [](string parameter0, string& parameter1) -> TestObject_1
    {
        return { parameter0 };
    });

    ASSERT_EQ(registration.id(), sample_id);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1, string, string&>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_as_rvalue_duplicate_id)
{
    definition_builder builder;
    auto registration = builder.define<TestObject_1>(sample_id, []() -> TestObject_1
    {
        return { sample_id };
    });

    ASSERT_EQ(registration.id(), sample_id);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);

    ASSERT_THROW(builder.define<TestObject_1>(sample_id, []() -> TestObject_1
    {
        return { sample_id };
    }), invalid_argument);
}

TEST(definition_builder, define_as_pointer_no_context)
{
    definition_builder builder;
    auto registration = builder.define<TestObject_1>(sample_id, []() -> TestObject_1*
    {
        auto pointer = new TestObject_1();
        pointer->field1_ = sample_id;

        return pointer;
    });

    ASSERT_EQ(registration.id(), sample_id);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_as_pointer_with_context)
{
    definition_builder builder;
    auto registration = builder.define<TestObject_1>(sample_id, [](auto& context) -> TestObject_1*
    {
        auto pointer = new TestObject_1();
        pointer->field1_ = sample_id;

        return pointer;
    });

    ASSERT_EQ(registration.id(), sample_id);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_as_pointer_one_argument)
{
    definition_builder builder;
    auto registration = builder.define<TestObject_1, string>(sample_id, [](string parameter0) -> TestObject_1*
    {
        auto pointer = new TestObject_1();
        pointer->field1_ = parameter0;

        return pointer;
    });

    ASSERT_EQ(registration.id(), sample_id);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1, string>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_as_pointer_two_arguments)
{
    definition_builder builder;
    auto registration = builder.define<TestObject_1, string, string&>(sample_id, [](string parameter0, string& parameter1) -> TestObject_1*
    {
        auto pointer = new TestObject_1();
        pointer->field1_ = parameter0;

        return pointer;
    });

    ASSERT_EQ(registration.id(), sample_id);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1, string, string&>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_as_pointer_duplicate_id)
{
    definition_builder builder;
    auto registration = builder.define<TestObject_1>(sample_id, []() -> TestObject_1*
    {
        auto pointer = new TestObject_1();
        pointer->field1_ = sample_id;

        return pointer;
    });

    ASSERT_EQ(registration.id(), sample_id);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);

    ASSERT_THROW(builder.define<TestObject_1>(sample_id, []() -> TestObject_1*
    {
        auto pointer = new TestObject_1();
        pointer->field1_ = sample_id;

        return pointer;
    }), invalid_argument);
}

TEST(definition_builder, define_as_unique_ptr_no_context)
{
    definition_builder builder;
    auto registration = builder.define<TestObject_1>(sample_id, []() -> unique_ptr<TestObject_1>
    {
        unique_ptr<TestObject_1> pointer(new TestObject_1());
        pointer->field1_ = sample_id;

        return pointer;
    });

    ASSERT_EQ(registration.id(), sample_id);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_as_unique_ptr_with_context)
{
    definition_builder builder;
    auto registration = builder.define<TestObject_1>(sample_id, [](auto& context) -> unique_ptr<TestObject_1>
    {
        unique_ptr<TestObject_1> pointer(new TestObject_1());
        pointer->field1_ = sample_id;

        return pointer;
    });

    ASSERT_EQ(registration.id(), sample_id);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, defined_as_unique_ptr_one_argument)
{
    definition_builder builder;
    auto registration = builder.define<TestObject_1, string>(sample_id, [](string parameter0) -> unique_ptr<TestObject_1>
    {
        unique_ptr<TestObject_1> pointer(new TestObject_1());
        pointer->field1_ = parameter0;

        return pointer;
    });

    ASSERT_EQ(registration.id(), sample_id);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1, string>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, defined_as_unique_ptr_two_arguments)
{
    definition_builder builder;
    auto registration = builder.define<TestObject_1, string, string&>(sample_id, [](string parameter0, string& parameter1) -> unique_ptr<TestObject_1>
    {
        unique_ptr<TestObject_1> pointer(new TestObject_1());
        pointer->field1_ = parameter0;

        return pointer;
    });

    ASSERT_EQ(registration.id(), sample_id);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1, string, string&>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_as_unique_duplicate_id)
{
    definition_builder builder;
    auto registration = builder.define<TestObject_1>(sample_id, []() -> unique_ptr<TestObject_1>
    {
        unique_ptr<TestObject_1> pointer(new TestObject_1());
        pointer->field1_ = sample_id;

        return pointer;
    });

    ASSERT_EQ(registration.id(), sample_id);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);

    ASSERT_THROW(builder.define<TestObject_1>(sample_id, []() -> unique_ptr<TestObject_1>
    {
        unique_ptr<TestObject_1> pointer(new TestObject_1());
        pointer->field1_ = sample_id;

        return pointer;
    }), invalid_argument);
}

TEST(definition_builder, define_module)
{
    struct TestModule
    {
        void operator()(definition_builder& builder)
        {
            builder.define<TestObject_1>(sample_id, *this);
        }

        TestObject_1 operator()()
        {
            return { sample_id };
        }
    };

    definition_builder builder;
    builder.define_module(TestModule());
}

TEST(definition_builder, define_through_static_factory_method_no_params)
{
    struct FactoryObject
    {
        static TestObject_1 create()
        {
            return { sample_id };
        }
    };

    definition_builder builder;
    auto registration = builder.define_default<TestObject_1>(&FactoryObject::create);

    ASSERT_EQ(registration.id(), string(definition::default_id));

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_through_non_static_factory_method_no_params)
{
    struct FactoryObject
    {
        TestObject_1 create()
        {
            return { sample_id };
        }
    };

    FactoryObject factory;
    function<TestObject_1()> f = bind(&FactoryObject::create, ref(factory));

    definition_builder builder;
    auto registration = builder.define_default<TestObject_1>(std::move(f));

    ASSERT_EQ(registration.id(), string(definition::default_id));

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_through_static_factory_method_with_params)
{
    struct FactoryObject
    {
        static TestObject_1 create(string parameter0)
        {
            return { parameter0 };
        }
    };

    definition_builder builder;
    auto registration = builder.define_default<TestObject_1, string>(&FactoryObject::create);

    ASSERT_EQ(registration.id(), string(definition::default_id));

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1, string>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_through_non_static_factory_method_with_params)
{
    struct FactoryObject
    {
        TestObject_1 create(string parameter0)
        {
            return { parameter0 };
        }
    };

    FactoryObject factory;
    function<TestObject_1(string)> f = bind(&FactoryObject::create, ref(factory), _1);

    definition_builder builder;
    auto registration = builder.define_default<TestObject_1, string>(std::move(f));

    ASSERT_EQ(registration.id(), string(definition::default_id));

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1, string>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_as_base_type)
{
    struct BaseObject
    {
        BaseObject(string&& id) : field1_(std::move(id))
        {

        }

        string field1_;
    };

    struct DerivedObject : BaseObject
    {
        DerivedObject(string&& id) : BaseObject(std::move(id))
        { }
    };

    definition_builder builder;
    auto registration = builder.define<DerivedObject>(sample_id, []() -> DerivedObject
    {
        return { sample_id };
    });

    ASSERT_EQ(registration.id(), sample_id);

    definition& definition = registration;
    auto creator = definition.creator<DerivedObject>();
    ASSERT_TRUE(creator);

    auto registration_as_base = registration.as<BaseObject>();
    ASSERT_EQ(registration_as_base.id(), sample_id);
}

/**
 * This tests a definition with a wrapping type:
 * if type B has constructor B(A&&) we can define a registration as
 * define<A...>.as<B>
 */
TEST(definition_builder, define_as_wrapping_type)
{
    struct WrapperObject
    {
        WrapperObject(TestObject_1&& instance)
            : instance_(std::move(instance))
        {

        }

        TestObject_1 instance_;
    };

    definition_builder builder;
    auto registration = builder.define<TestObject_1>(
            sample_id,
            []() -> TestObject_1
            {
                return { sample_id };
            });

    ASSERT_EQ(registration.id(), sample_id);

    definition& unwrapped_definition = registration;
    auto creator = unwrapped_definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);

    auto registration_as_wrapped = registration.as<WrapperObject>();
    ASSERT_EQ(registration_as_wrapped.id(), sample_id);

    definition& definition_as_wrapped = registration_as_wrapped;
    auto creator_as_wrapped = definition_as_wrapped.template creator<WrapperObject>();
    ASSERT_TRUE(creator_as_wrapped);
}

TEST(definition_builder, define_factory_with_context)
{
    struct FactoryObject
    {
        static TestObject_1 create(const activation_context& context)
        {
            return { sample_id };
        }
    };

    definition_builder builder;
    auto registration = builder.define_factory(sample_id, &FactoryObject::create);

    ASSERT_EQ(registration.id(), sample_id);
    ASSERT_EQ(typeid(decltype(registration)::registered_type), typeid(TestObject_1));
    ASSERT_EQ(size_t(decltype(registration)::with_types::count), 0u);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_factory_with_context__member_method)
{
    struct FactoryObject
    {
        TestObject_1 create(const activation_context& context)
        {
            return { sample_id };
        }
    };

    FactoryObject instance;

    definition_builder builder;
    auto registration = builder.define_factory(sample_id, &FactoryObject::create, &instance);

    ASSERT_EQ(registration.id(), sample_id);
    ASSERT_EQ(typeid(decltype(registration)::registered_type), typeid(TestObject_1));
    ASSERT_EQ(size_t(decltype(registration)::with_types::count), 0u);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_factory_no_context)
{
    struct FactoryObject
    {
        static TestObject_1 create()
        {
            return { sample_id };
        }
    };

    definition_builder builder;
    auto registration = builder.define_factory(sample_id, &FactoryObject::create);

    ASSERT_EQ(registration.id(), sample_id);
    ASSERT_EQ(typeid(decltype(registration)::registered_type), typeid(TestObject_1));
    ASSERT_EQ(size_t(decltype(registration)::with_types::count), 0u);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_factory_no_context__member_method)
{
    struct FactoryObject
    {
        TestObject_1 create()
        {
            return { sample_id };
        }
    };

    FactoryObject instance;

    definition_builder builder;
    auto registration = builder.define_factory(sample_id, &FactoryObject::create, &instance);

    ASSERT_EQ(registration.id(), sample_id);
    ASSERT_EQ(typeid(decltype(registration)::registered_type), typeid(TestObject_1));
    ASSERT_EQ(size_t(decltype(registration)::with_types::count), 0u);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_factory_with_context_default_id)
{
    struct FactoryObject
    {
        static TestObject_1 create(const activation_context& context)
        {
            return { sample_id };
        }
    };

    definition_builder builder;
    auto registration = builder.define_factory(&FactoryObject::create);

    ASSERT_EQ(registration.id(), string(definition::default_id));
    ASSERT_EQ(typeid(decltype(registration)::registered_type), typeid(TestObject_1));
    ASSERT_EQ(size_t(decltype(registration)::with_types::count), 0u);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_factory_with_context_default_id__member_method)
{
    struct FactoryObject
    {
        TestObject_1 create(const activation_context& context)
        {
            return { sample_id };
        }
    };

    FactoryObject instance;

    definition_builder builder;
    auto registration = builder.define_factory(&FactoryObject::create, &instance);

    ASSERT_EQ(registration.id(), string(definition::default_id));
    ASSERT_EQ(typeid(decltype(registration)::registered_type), typeid(TestObject_1));
    ASSERT_EQ(size_t(decltype(registration)::with_types::count), 0u);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_factory_no_context_default_id)
{
    struct FactoryObject
    {
        static TestObject_1 create()
        {
            return { sample_id };
        }
    };

    definition_builder builder;
    auto registration = builder.define_factory(&FactoryObject::create);

    ASSERT_EQ(registration.id(), string(definition::default_id));
    ASSERT_EQ(typeid(decltype(registration)::registered_type), typeid(TestObject_1));
    ASSERT_EQ(size_t(decltype(registration)::with_types::count), 0u);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_factory_no_context_default_id__member_method)
{
    struct FactoryObject
    {
        TestObject_1 create()
        {
            return { sample_id };
        }
    };

    FactoryObject instance;

    definition_builder builder;
    auto registration = builder.define_factory(&FactoryObject::create, &instance);

    ASSERT_EQ(registration.id(), string(definition::default_id));
    ASSERT_EQ(typeid(decltype(registration)::registered_type), typeid(TestObject_1));
    ASSERT_EQ(size_t(decltype(registration)::with_types::count), 0u);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_factory_with_context_default_id_with_arguments)
{
    struct FactoryObject
    {
        static TestObject_1 create(const activation_context& context, int param0, string param1)
        {
            return { sample_id };
        }
    };

    definition_builder builder;
    auto registration = builder.define_factory(&FactoryObject::create);

    ASSERT_EQ(registration.id(), string(definition::default_id));
    ASSERT_EQ(typeid(decltype(registration)::registered_type), typeid(TestObject_1));
    ASSERT_EQ(size_t(decltype(registration)::with_types::count), 2u);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1, int, string>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_factory_with_context_default_id_with_arguments__member_method)
{
    struct FactoryObject
    {
        TestObject_1 create(const activation_context& context, int param0, string param1)
        {
            return { sample_id };
        }
    };

    FactoryObject instance;

    definition_builder builder;
    auto registration = builder.define_factory(&FactoryObject::create, &instance);

    ASSERT_EQ(registration.id(), string(definition::default_id));
    ASSERT_EQ(typeid(decltype(registration)::registered_type), typeid(TestObject_1));
    ASSERT_EQ(size_t(decltype(registration)::with_types::count), 2u);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1, int, string>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_factory_no_context_default_id_with_arguments)
{
    struct FactoryObject
    {
        static TestObject_1 create(int param0, string param1)
        {
            return { sample_id };
        }
    };

    definition_builder builder;
    auto registration = builder.define_factory(&FactoryObject::create);

    ASSERT_EQ(registration.id(), string(definition::default_id));
    ASSERT_EQ(typeid(decltype(registration)::registered_type), typeid(TestObject_1));
    ASSERT_EQ(size_t(decltype(registration)::with_types::count), 2u);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1, int, string>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, define_factory_no_context_default_id_with_arguments__member_method)
{
    struct FactoryObject
    {
        TestObject_1 create(int param0, string param1)
        {
            return { sample_id };
        }
    };

    FactoryObject instance;

    definition_builder builder;
    auto registration = builder.define_factory(&FactoryObject::create, &instance);

    ASSERT_EQ(registration.id(), string(definition::default_id));
    ASSERT_EQ(typeid(decltype(registration)::registered_type), typeid(TestObject_1));
    ASSERT_EQ(size_t(decltype(registration)::with_types::count), 2u);

    definition& definition = registration;
    auto creator = definition.creator<TestObject_1, int, string>();
    ASSERT_TRUE(creator);
}

TEST(definition_builder, annotate_simple_rvalue)
{
    definition_builder builder;
    auto registration = builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    auto sample_annotation = "sample_annotation";
    registration.annotate(string(sample_annotation));

    definition& definition = registration;
    auto& annotations = definition.annotations();

    ASSERT_EQ(annotations.get<string>(), sample_annotation);
}

TEST(definition_builder, annotate_simple_copy)
{
    definition_builder builder;
    auto registration = builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    string sample_annotation("sample_annotation");
    registration.annotate(string(sample_annotation));

    definition& definition = registration;
    auto& annotations = definition.annotations();

    ASSERT_EQ(annotations.get<string>(), sample_annotation);
}

TEST(definition_builder, define_interceptor__lambda__no_intercepted_instance__no_context)
{
    definition_builder builder;
    auto registration = builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    builder.define_interceptor<TestObject_1>([]()
    {
    });
}

TEST(definition_builder, define_interceptor__lambda__no_intercepted_instance__with_context)
{
    definition_builder builder;
    auto registration = builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    builder.define_interceptor<TestObject_1>([](const activation_context&)
    {
    });
}

TEST(definition_builder, define_interceptor__lambda__with_intercepted_instance__with_context)
{
    definition_builder builder;
    auto registration = builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    builder.define_interceptor<TestObject_1>([](TestObject_1& instance, const activation_context&)
    {
    });
}

TEST(definition_builder, define_interceptor__non_static_method__no_intercepted_instance__no_context)
{
    struct test_handler
    {
        void intercept()
        {

        }
    };

    definition_builder builder;
    auto registration = builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    test_handler handler;
    builder.define_interceptor<TestObject_1>(&test_handler::intercept, &handler);
}

TEST(definition_builder, define_interceptor__non_static_method__with_intercepted_instance__no_context)
{
    struct test_handler
    {
        void intercept(TestObject_1& instance)
        {

        }
    };

    definition_builder builder;
    auto registration = builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    test_handler handler;
    builder.define_interceptor(&test_handler::intercept, &handler);
}

TEST(definition_builder, define_interceptor__non_static_method__with_intercepted_instance__with_context)
{
    struct test_handler
    {
        void intercept(TestObject_1& instance, const activation_context& context)
        {

        }
    };

    definition_builder builder;
    auto registration = builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    test_handler handler;
    builder.define_interceptor(&test_handler::intercept, &handler);
}

TEST(definition_builder, define_interceptor__non_static_method__no_intercepted_instance__with_context)
{
    struct test_handler
    {
        void intercept(const activation_context& context)
        {

        }
    };

    definition_builder builder;
    auto registration = builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    test_handler handler;
    builder.define_interceptor<TestObject_1>(&test_handler::intercept, &handler);
}

TEST(definition_builder, define_decorator__lambda__pointer__context)
{
    struct interface
    {
        virtual ~interface(){}

        virtual void method() { };
    };

    struct component : interface
    {
        ~component() override
        {

        }

        void method() override
        {
        }
    };

    struct decorator : interface
    {
        decorator(size_t id, interface* undecorated)
            :
                id_(id),
                undecorated_(undecorated)
        {

        }

        ~decorator() override
        {
            delete undecorated_;
        }

        void method() override
        {
            undecorated_->method();
        }

        size_t id_;
        interface* undecorated_;
    };

    definition_builder builder;
    builder.define_default<interface>(
            []() -> interface*
            {
                return new component();
            });

    builder.define_decorator<interface>(
            [](interface* undecorated, const activation_context& context) -> interface*
            {
                return new decorator(1u, undecorated);
            },
            [](interface* decorated)
            {
                auto decorator_ptr = static_cast<decorator*>(decorated);
                delete decorator_ptr;
            });
}

TEST(definition_builder, define_decorator__lambda__pointer__no_context)
{
    struct interface
    {
        virtual ~interface(){}

        virtual void method() { };
    };

    struct component : interface
    {
        ~component() override
        {

        }

        void method() override
        {
        }
    };

    struct decorator : interface
    {
        decorator(size_t id, interface* undecorated)
                :
                id_(id),
                undecorated_(undecorated)
        {

        }

        ~decorator() override
        {
            delete undecorated_;
        }

        void method() override
        {
            undecorated_->method();
        }

        size_t id_;
        interface* undecorated_;
    };

    definition_builder builder;
    builder.define_default<interface>(
            []() -> interface*
            {
              return new component();
            });

    builder.define_decorator<interface>(
            [](interface* undecorated) -> interface*
            {
                return new decorator(1u, undecorated);
            },
            [](interface* decorated)
            {
                auto decorator_ptr = static_cast<decorator*>(decorated);
                delete decorator_ptr;
            });
}

TEST(definition_builder, define_decorator__lambda__unique_pointer__context)
{
    struct interface
    {
        virtual void method() { };
    };

    struct component : interface
    {
        void method() override
        {
        }
    };

    struct decorator : interface
    {
        decorator(size_t id, unique_ptr<interface> undecorated)
            :
                id_(id),
                undecorated_(std::move(undecorated))
        {

        }

        void method() override
        {
            undecorated_->method();
        }

        size_t id_;
        unique_ptr<interface> undecorated_;
    };

    definition_builder builder;
    builder.define_default<interface>(
            []() -> unique_ptr<interface>
            {
                return unique_ptr<interface>(new component());
            });

    builder.define_decorator<interface>(
            [](unique_ptr<interface> undecorated, const activation_context& context) -> unique_ptr<interface>
            {
                return unique_ptr<interface>(new decorator(1u, std::move(undecorated)));
            });
}

TEST(definition_builder, define_decorator__lambda__unique_pointer__no_context)
{
    struct interface
    {
        virtual void method() { };
    };

    struct component : interface
    {
        void method() override
        {
        }
    };

    struct decorator : interface
    {
        decorator(size_t id, unique_ptr<interface> undecorated)
                :
                id_(id),
                undecorated_(std::move(undecorated))
        {

        }

        void method() override
        {
            undecorated_->method();
        }

        size_t id_;
        unique_ptr<interface> undecorated_;
    };

    definition_builder builder;
    builder.define_default<interface>(
            []() -> unique_ptr<interface>
            {
                return unique_ptr<interface>(new component());
            });

    builder.define_decorator<interface>(
            [](unique_ptr<interface> undecorated) -> unique_ptr<interface>
            {
                return unique_ptr<interface>(new decorator(1u, std::move(undecorated)));
            });
}

TEST(definition_builder, define_decorator__lambda__rvalue__context)
{
    using test_function = std::function<void()>;

    struct component
    {
        void operator()()
        {
        }
    };

    struct decorator
    {
        decorator(size_t id, test_function&& undecorated)
            :
                id_(id),
                undecorated_(std::move(undecorated))
        {

        }

        void operator()()
        {
            undecorated_();
        }

        size_t id_;
        test_function undecorated_;
    };

    definition_builder builder;
    builder.define_default<test_function>(
            []() -> component
            {
                return component();
            });
    builder.define_decorator<test_function>(
            [](test_function&& undecorated, const activation_context& context) -> decorator
            {
                return decorator(1u, std::move(undecorated));
            });
}

TEST(definition_builder, define_decorator__lambda__rvalue__no_context)
{
    using test_function = std::function<void()>;

    struct component
    {
        void operator()()
        {
        }
    };

    struct decorator
    {
        decorator(size_t id, test_function&& undecorated)
            :
                id_(id),
                undecorated_(std::move(undecorated))
        {

        }

        void operator()()
        {
            undecorated_();
        }

        size_t id_;
        test_function undecorated_;
    };

    definition_builder builder;
    builder.define_default<test_function>(
            []() -> component
            {
                return component();
            });
    builder.define_decorator<test_function>(
            [](test_function&& undecorated) -> decorator
            {
                return decorator(1u, std::move(undecorated));
            });
}

TEST(definition_builder,  define_decorator__unique_pointer__static_method__context)
{
    struct interface
    {
        virtual void method() { };
    };

    struct component : interface
    {
        void method() override
        {
        }
    };

    struct decorator : interface
    {
        decorator(size_t id, unique_ptr<interface> undecorated)
                :
                id_(id),
                undecorated_(std::move(undecorated))
        {

        }

        void method() override
        {
            undecorated_->method();
        }

        size_t id_;
        unique_ptr<interface> undecorated_;
    };

    definition_builder builder;
    builder.define_default<interface>(
            []() -> unique_ptr<interface>
            {
                return unique_ptr<interface>(new component());
            });

    struct decorator_factory
    {
        static unique_ptr<interface> define(unique_ptr<interface>&& undecorated, const activation_context& context)
        {
            return unique_ptr<interface>(new decorator(1u, std::move(undecorated)));
        }
    };

    builder.define_decorator(&decorator_factory::define);
}

TEST(definition_builder,  define_decorator__unique_pointer__static_method__no_context)
{
    struct interface
    {
        virtual void method() { };
    };

    struct component : interface
    {
        void method() override
        {
        }
    };

    struct decorator : interface
    {
        decorator(size_t id, unique_ptr<interface> undecorated)
                :
                id_(id),
                undecorated_(std::move(undecorated))
        {

        }

        void method() override
        {
            undecorated_->method();
        }

        size_t id_{0};
        unique_ptr<interface> undecorated_;
    };

    definition_builder builder;
    builder.define_default<interface>(
            []() -> unique_ptr<interface>
            {
                return unique_ptr<interface>(new component());
            });

    struct decorator_factory
    {
        static unique_ptr<interface> define(unique_ptr<interface>&& undecorated)
        {
            return unique_ptr<interface>(new decorator(1u, std::move(undecorated)));
        }
    };

    builder.define_decorator(&decorator_factory::define);
}

TEST(definition_builder,  define_decorator__unique_pointer__non_static_method__context)
{
    struct interface
    {
        virtual void method() { };
    };

    struct component : interface
    {
        void method() override
        {
        }
    };

    struct decorator : interface
    {
        decorator(size_t id, unique_ptr<interface> undecorated)
            :
                id_(id),
                undecorated_(std::move(undecorated))
        {

        }

        void method() override
        {
            undecorated_->method();
        }

        size_t id_;
        unique_ptr<interface> undecorated_;
    };

    definition_builder builder;
    builder.define_default<interface>(
            []() -> unique_ptr<interface>
            {
                return unique_ptr<interface>(new component());
            });

    struct decorator_factory
    {
        unique_ptr<interface> define(unique_ptr<interface>&& undecorated, const activation_context& context)
        {
            return unique_ptr<interface>(new decorator(id_++, std::move(undecorated)));
        }

        size_t id_{0};
    };

    decorator_factory factory;
    builder.define_decorator(&decorator_factory::define, &factory);
}

TEST(definition_builder,  define_decorator__unique_pointer__non_static_method__no_context)
{
    struct interface
    {
        virtual void method() { };
    };

    struct component : interface
    {
        void method() override
        {
        }
    };

    struct decorator : interface
    {
        decorator(size_t id, unique_ptr<interface> undecorated)
                :
                id_(id),
                undecorated_(std::move(undecorated))
        {

        }

        void method() override
        {
            undecorated_->method();
        }

        size_t id_;
        unique_ptr<interface> undecorated_;
    };

    definition_builder builder;
    builder.define_default<interface>(
            []() -> unique_ptr<interface>
            {
                return unique_ptr<interface>(new component());
            });

    struct decorator_factory
    {
        unique_ptr<interface> define(unique_ptr<interface>&& undecorated)
        {
            return unique_ptr<interface>(new decorator(id_++, std::move(undecorated)));
        }

        size_t id_{0};
    };

    decorator_factory factory;
    builder.define_decorator(&decorator_factory::define, &factory);
}

TEST(definition_builder,  define_decorator__rvalue__static_method__context)
{
    using test_function = std::function<void()>;

    struct component
    {
        void operator()()
        {
        }
    };

    struct decorator
    {
        decorator(size_t id, test_function&& undecorated)
                :
                id_(id),
                undecorated_(std::move(undecorated))
        {

        }

        void operator()()
        {
            undecorated_();
        }

        size_t id_{0};
        test_function undecorated_;
    };

    definition_builder builder;
    builder.define_default<test_function>(
            []() -> component
            {
                return component();
            });

    struct decorator_factory
    {
        static test_function define(test_function&& undecorated, const activation_context& context)
        {
            return decorator(1u, std::move(undecorated));
        }
    };

    builder.define_decorator(&decorator_factory::define);
}

TEST(definition_builder,  define_decorator__rvalue__static_method__no_context)
{
    using test_function = std::function<void()>;

    struct component
    {
        void operator()()
        {
        }
    };

    struct decorator
    {
        decorator(size_t id, test_function&& undecorated)
                :
                id_(id),
                undecorated_(std::move(undecorated))
        {

        }

        void operator()()
        {
            undecorated_();
        }

        size_t id_;
        test_function undecorated_;
    };

    definition_builder builder;
    builder.define_default<test_function>(
            []() -> component
            {
                return component();
            });

    struct decorator_factory
    {
        static test_function define(test_function&& undecorated)
        {
            return decorator(1u, std::move(undecorated));
        }
    };

    builder.define_decorator(&decorator_factory::define);
}

TEST(definition_builder,  define_decorator__rvalue__non_static_method__context)
{
    using test_function = std::function<void()>;

    struct component
    {
        void operator()()
        {
        }
    };

    struct decorator
    {
        decorator(size_t id, test_function&& undecorated)
                :
                id_(id),
                undecorated_(std::move(undecorated))
        {

        }

        void operator()()
        {
            undecorated_();
        }

        size_t id_;
        test_function undecorated_;
    };

    definition_builder builder;
    builder.define_default<test_function>(
            []() -> component
            {
                return component();
            });

    struct decorator_factory
    {
        test_function define(test_function&& undecorated, const activation_context& context)
        {
            return decorator(id_++, std::move(undecorated));
        }

        size_t id_{0};
    };

    decorator_factory factory;
    builder.define_decorator(&decorator_factory::define, &factory);
}

TEST(definition_builder,  define_decorator__rvalue__non_static_method__no_context)
{
    using test_function = std::function<void()>;

    struct component
    {
        void operator()()
        {
        }
    };

    struct decorator
    {
        decorator(size_t id, test_function&& undecorated)
            :
                id_(id),
                undecorated_(std::move(undecorated))
        {

        }

        void operator()()
        {
            undecorated_();
        }

        size_t id_;
        test_function undecorated_;
    };

    definition_builder builder;
    builder.define_default<test_function>(
            []() -> component
            {
                return component();
            });

    struct decorator_factory
    {
        test_function define(test_function&& undecorated)
        {
            return decorator(id_++, std::move(undecorated));
        }

        size_t id_{0};
    };

    decorator_factory factory;
    builder.define_decorator(&decorator_factory::define, &factory);
}