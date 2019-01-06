#include <di/instance_activator.hpp>
#include <di/definition_builder.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/optional.hpp>

#include <memory>
#include <string>

using namespace std;
using namespace di;
using namespace testing;

struct TestObject_1
{
    string field1_;
};

const auto test_context = "";
const auto sample_id = "sample-id";


TEST(activation_context, activate_default_with_no_parameters)
{
    definition_builder builder;
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    activation_context context(test_context, activator);

    ASSERT_FALSE(context.uuid().is_nil());

    TestObject_1 instance = context.activate_default<TestObject_1>();
}

TEST(instance_activator, activate_default_with_single_parameter)
{
    definition_builder builder;
    builder.define_default<TestObject_1, string>([](string parameter0) -> TestObject_1
    {
        return { parameter0 };
    });

    instance_activator activator(std::move(builder));
    activation_context context(test_context, activator);

    string parameter("abc");
    TestObject_1 instance = context.activate_default<TestObject_1>().with(parameter);

    ASSERT_EQ(instance.field1_, parameter);
}

TEST(instance_activator, activate_default_with_single_parameter_require_ref_pass_value)
{
    definition_builder builder;
    builder.define_default<TestObject_1, string&>([](string& parameter0) -> TestObject_1
    {
        return { parameter0 };
    });

    instance_activator activator(std::move(builder));
    activation_context context(test_context, activator);

    string parameter("abc");
    TestObject_1 instance = context.activate_default<TestObject_1>().with_reference(parameter);

    ASSERT_EQ(instance.field1_, parameter);
}

TEST(instance_activator, activate_default_as_different_type)
{
    struct TestWrapper
    {
        TestWrapper(TestObject_1&& original)
            : original_(std::move(original))
        {

        }

        TestObject_1 original_;
    };

    definition_builder builder;
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    activation_context context(test_context, activator);

    TestWrapper wrapper = context.activate_default<TestObject_1>().as<TestWrapper>();

    ASSERT_EQ(wrapper.original_.field1_, sample_id);
}

TEST(instance_activator, activate_default_as_unique_ptr)
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
    builder.define_default<DerivedObject>([]() -> DerivedObject
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    activation_context context(test_context, activator);

    unique_ptr<BaseObject> instance = context.activate_default<DerivedObject>().as<BaseObject>();

    ASSERT_EQ(instance->field1_, sample_id);
}

TEST(instance_activator, activate_default_as_shared_ptr)
{
    struct BaseObject
    {
    public:
        string field1_;

    protected:
        BaseObject(string&& id) : field1_(std::move(id))
        {

        }

    };

    struct DerivedObject : BaseObject
    {
        DerivedObject(string&& id) : BaseObject(std::move(id))
        { }
    };

    definition_builder builder;
    builder.define_default<DerivedObject>([]() -> DerivedObject
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    activation_context context(test_context, activator);

    shared_ptr<BaseObject> instance = context.activate_default<DerivedObject>().as<BaseObject>();

    ASSERT_EQ(instance->field1_, sample_id);
}

TEST(activation_context, with_annotation)
{
    definition_builder builder;
    builder.define_default<TestObject_1>([](const activation_context& context) -> TestObject_1
    {
        EXPECT_TRUE(context.has_annotation<string>());
        EXPECT_EQ(context.annotation<string>(), string(sample_id));

        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    activation_context context(test_context, activator);

    TestObject_1 instance = context
            .activate_default<TestObject_1>()
            .with_annotation(string(sample_id));
}

TEST(activation_context, with_optional_annotation)
{
    definition_builder builder;
    builder.define_default<TestObject_1>([](const activation_context& context) -> TestObject_1
    {
        EXPECT_TRUE(context.has_annotation<string>());
        EXPECT_EQ(context.annotation<string>(), string(sample_id));
        EXPECT_FALSE(context.has_annotation<int>());

        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    activation_context context(test_context, activator);

    TestObject_1 instance = context
            .activate_default<TestObject_1>()
            .with_optional_annotation(boost::optional<string>(sample_id))
            .with_optional_annotation<boost::optional<int>>(boost::none);
}

TEST(activation_context, with_optional_annotation_and_transformation)
{
    definition_builder builder;
    builder.define_default<TestObject_1>([](const activation_context& context) -> TestObject_1
    {
        EXPECT_TRUE(context.has_annotation<string>());
        EXPECT_EQ(context.annotation<string>(), string("1"));

        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    activation_context context(test_context, activator);

    TestObject_1 instance = context
            .activate_default<TestObject_1>()
            .with_optional_annotation(
                    boost::optional<int>(1),
                    [](int arg) -> string
                    {
                        stringstream ss;
                        ss << arg;

                        return ss.str();
                    });
}

TEST(activation_context, activate_default_with_description)
{
    constexpr auto description = "This is a description";

    definition_builder builder;
    builder.define_default<TestObject_1>([&description](const activation_context& context) -> TestObject_1
                                         {
                                             EXPECT_EQ(context.description(), description);
                                             return { sample_id };
                                         });

    instance_activator activator(std::move(builder));
    activation_context context(test_context, activator);

    TestObject_1 instance = context.activate_default_with_description<TestObject_1>(description);
}

TEST(activation_context, activate_default_with_description_with_single_parameter)
{
    constexpr auto description = "This is a description";

    definition_builder builder;
    builder.define_default<TestObject_1, string>([&description](const activation_context& context, string parameter0) -> TestObject_1
                                                 {
                                                     EXPECT_EQ(context.description(), description);
                                                     return { parameter0 };
                                                 });

    instance_activator activator(std::move(builder));
    activation_context context(test_context, activator);

    string parameter("abc");
    TestObject_1 instance = context.activate_default_with_description<TestObject_1>(description).with(parameter);

    ASSERT_EQ(instance.field1_, parameter);
}

TEST(activation_context, access_parent_during_activation)
{
    struct component
    {
        component(int arg0)
        {}
    };

    definition_builder builder;
    builder.define_type<component>();
    builder.define_default_instance<int>(5);

    auto intercepted_count = 0ul;
    builder.define_interceptor<int>([&intercepted_count](const activation_context& ctx)
    {
        EXPECT_TRUE(ctx.parent());
        EXPECT_TRUE(ctx.parent()->parent());
        EXPECT_FALSE(ctx.parent()->parent()->parent());
        intercepted_count++;
    });

    instance_activator activator(std::move(builder));
    activation_context context(test_context, activator);
    ASSERT_FALSE(context.parent());

    context.activate_default_raii<component>();
    ASSERT_EQ(intercepted_count, 1ul);
}

TEST(activation_context, ostream_operator)
{
    struct component
    {
        component(int arg0)
        {}
    };

    definition_builder builder;
    builder.define_type<component>();
    builder.define_default_instance<int>(5);

    auto intercepted_count = 0ul;
    builder.define_interceptor<int>([&intercepted_count](const activation_context& ctx)
    {
        stringstream ss;
        ss << ctx;

        ASSERT_THAT(ss.str(), Not(IsEmpty()));
        ASSERT_THAT(ss.str(), HasSubstr(test_context));
        ASSERT_THAT(ss.str(), HasSubstr("parent"));
        intercepted_count++;
    });

    instance_activator activator(std::move(builder));
    activation_context context(test_context, activator);

    stringstream ss;
    ss << context;

    ASSERT_THAT(ss.str(), Not(IsEmpty()));
    ASSERT_THAT(ss.str(), HasSubstr(test_context));
    ASSERT_THAT(ss.str(), Not(HasSubstr("parent")));

    context.activate_default_raii<component>();
    ASSERT_EQ(intercepted_count, 1ul);
}