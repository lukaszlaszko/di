#include <di/annotation.hpp>
#include <di/annotations_map.hpp>
#include <di/instance_activator.hpp>
#include <di/definition_builder.hpp>

#include <di/tools/movable_function.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <string>

using namespace std;
using namespace testing;
using namespace di;
using namespace di::tools;


const auto sample_id = "sample-id";

struct TestObject_1
{
    string field1_;
};

TEST(instance_activator, create)
{
    definition_builder builder;
    instance_activator activator(std::move(builder));
}

TEST(instance_activator, can_activate_as_rvalue_no_parameters)
{
    definition_builder builder;
    builder.define<TestObject_1>(sample_id, []() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    ASSERT_TRUE(activator.can_activate<TestObject_1>(sample_id));
    ASSERT_FALSE(activator.can_activate<TestObject_1>("unknonw-id"));
}

TEST(instance_activator, can_activate_as_pointer_no_parameters)
{
    definition_builder builder;
    builder.define<TestObject_1>(sample_id, []() -> TestObject_1*
    {
        auto instance = new TestObject_1();
        instance->field1_ = sample_id;

        return instance;
    });

    instance_activator activator(std::move(builder));
    ASSERT_TRUE(activator.can_activate<TestObject_1>(sample_id));
    ASSERT_FALSE(activator.can_activate<TestObject_1>("unknonw-id"));
}

TEST(instance_activator, can_activate_as_unique_no_parameters)
{
    definition_builder builder;
    builder.define<TestObject_1>(sample_id, []() -> unique_ptr<TestObject_1>
    {
        auto instance = unique_ptr<TestObject_1>(new TestObject_1());
        instance->field1_ = sample_id;

        return instance;
    });

    instance_activator activator(std::move(builder));
    ASSERT_TRUE(activator.can_activate<TestObject_1>(sample_id));
    ASSERT_FALSE(activator.can_activate<TestObject_1>("unknonw-id"));
}

TEST(instance_activator, can_activate_as_rvalue_with_parameters)
{
    definition_builder builder;
    builder.define<TestObject_1, string&>(sample_id, [](auto& parameter0) -> TestObject_1
    {
        return { parameter0 };
    });

    instance_activator activator(std::move(builder));

    auto result = activator.can_activate<TestObject_1>(sample_id);
    ASSERT_FALSE(result);

    result = activator.can_activate<TestObject_1, string&>(sample_id);
    ASSERT_TRUE(result);
}

TEST(instance_activator, can_activate_as_pointer_with_parameters)
{
    definition_builder builder;
    builder.define<TestObject_1, string&>(sample_id, [](auto& parameter0) -> TestObject_1*
    {
        auto instance = new TestObject_1();
        instance->field1_ = parameter0;

        return instance;
    });

    instance_activator activator(std::move(builder));

    auto result = activator.can_activate<TestObject_1>(sample_id);
    ASSERT_FALSE(result);

    result = activator.can_activate<TestObject_1, string&>(sample_id);
    ASSERT_TRUE(result);
}

TEST(instance_activator, can_activate_as_unique_with_parameters)
{
    definition_builder builder;
    builder.define<TestObject_1, string&>(sample_id, [](auto& parameter0) -> unique_ptr<TestObject_1>
    {
        auto instance = unique_ptr<TestObject_1>(new TestObject_1());
        instance->field1_ = parameter0;

        return instance;
    });

    instance_activator activator(std::move(builder));

    auto result = activator.can_activate<TestObject_1>(sample_id);
    ASSERT_FALSE(activator.can_activate<TestObject_1>(sample_id));

    result = activator.can_activate<TestObject_1, string&>(sample_id);
    ASSERT_TRUE(result);
}

TEST(instance_activator, activate_unique_no_parameters)
{
    definition_builder builder;
    builder.define<TestObject_1>(sample_id, []() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_unique<TestObject_1>(sample_id);

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, sample_id);
}

TEST(instance_activator, activate_unique_no_parameters_default)
{
    definition_builder builder;
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_unique<TestObject_1>();

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, sample_id);
}

TEST(instance_activator, activate_unique_with_parameters_default)
{
    string instance_id("abc");

    definition_builder builder;
    builder.define_default<TestObject_1, string&>([](string& parameter0) -> TestObject_1
    {
        return { parameter0 };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_unique<TestObject_1, string&>(instance_id);

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, instance_id);
}

TEST(instance_activator, activate_unique_with_parameters)
{
    definition_builder builder;
    builder.define<TestObject_1, string>(sample_id, [](string parameter0) -> TestObject_1
    {
        return { parameter0 };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_unique<TestObject_1, string>(sample_id, sample_id);

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, sample_id);
}

TEST(instance_activator, activate_unique_no_closure_copy)
{
    struct TestClosure
    {
        TestClosure(int& counter)
            : counter_(counter)
        {

        }

        TestClosure(const TestClosure& other)
            : counter_(other.counter_)
        {
            counter_++;
        }

        TestClosure(TestClosure&& other) = default;

        int& counter_;
    };

    auto copy_count = 0;
    TestClosure closure(copy_count);
    ASSERT_EQ(copy_count, 0);

    definition_builder builder;
    builder.define<TestObject_1>(sample_id, [closure = std::move(closure)]() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_unique<TestObject_1>(sample_id);

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, sample_id);
    ASSERT_EQ(copy_count, 0);
}

TEST(instance_activator, activate_unique_no_object_copy)
{
    struct TestObject
    {
        TestObject(int& counter)
            : counter_(counter)
        {

        }

        TestObject(const TestObject& other)
            : counter_(other.counter_)
        {
            counter_++;
        }

        TestObject(TestObject&& other)
            : counter_(other.counter_)
        {

        }

        TestObject& operator=(const TestObject& other)
        {
            counter_++;
            return *this;
        }
        TestObject& operator=(TestObject&& other) = default;

        int& counter_;
    };

    auto copy_count = 0;

    definition_builder builder;
    builder.define<TestObject>(sample_id, [&copy_count]() -> TestObject
    {
        return TestObject(copy_count);
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_unique<TestObject>(sample_id);

    ASSERT_TRUE(instance);
    ASSERT_EQ(copy_count, 0);
}

TEST(instance_activator, activate_unique_from_pointer_no_parameters)
{
    definition_builder builder;
    builder.define<TestObject_1>(sample_id, []() -> TestObject_1*
    {
        auto pointer = new TestObject_1();
        pointer->field1_ = sample_id;

        return pointer;
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_unique<TestObject_1>(sample_id);

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, sample_id);
}

TEST(instance_activator, activate_unique_from_pointer_with_parameters)
{
    string instance_id("abc");

    definition_builder builder;
    builder.define<TestObject_1, string&>(sample_id, [](auto& parameter0) -> TestObject_1*
    {
        auto pointer = new TestObject_1();
        pointer->field1_ = parameter0;

        return pointer;
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_unique<TestObject_1, string&>(sample_id, instance_id);

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, instance_id);
}

TEST(instance_activator, activate_unique_from_unique_with_parameters)
{
    string instance_id("abc");

    definition_builder builder;
    builder.define<TestObject_1, string&>(sample_id, [](auto& parameter0) -> unique_ptr<TestObject_1>
    {
        auto pointer = new TestObject_1();
        pointer->field1_ = parameter0;

        return unique_ptr<TestObject_1>(std::move(pointer));
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_unique<TestObject_1, string&>(sample_id, instance_id);

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, instance_id);
}

TEST(instance_activator, activate_shared_no_parameters)
{
    definition_builder builder;
    builder.define<TestObject_1>(sample_id, []() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_shared<TestObject_1>(sample_id);

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, sample_id);
}

TEST(instance_activator, activate_shared_no_parameters_default)
{
    definition_builder builder;
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_shared<TestObject_1>();

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, sample_id);
}

TEST(instance_activator, activate_shared_with_parameters_default)
{
    string instance_id("abc");

    definition_builder builder;
    builder.define_default<TestObject_1, string&>([](string& parameter0) -> TestObject_1
    {
        return { parameter0 };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_shared<TestObject_1, string&>(instance_id);

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, instance_id);
}

TEST(instance_activator, activate_shared_with_parameters)
{
    definition_builder builder;
    builder.define<TestObject_1, string>(sample_id, [](string parameter0) -> TestObject_1
    {
        return { parameter0 };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_shared<TestObject_1, string>(sample_id, sample_id);

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, sample_id);
}

TEST(instance_activator, activate_shared_no_closure_copy)
{
    struct TestClosure
    {
        TestClosure(int& counter)
            : counter_(counter)
        {

        }

        TestClosure(const TestClosure& other)
            : counter_(other.counter_)
        {
            counter_++;
        }

        TestClosure(TestClosure&& other) = default;

        int& counter_;
    };

    auto copy_count = 0;
    TestClosure closure(copy_count);
    ASSERT_EQ(copy_count, 0);

    definition_builder builder;
    builder.define<TestObject_1>(sample_id, [closure = std::move(closure)]() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_shared<TestObject_1>(sample_id);

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, sample_id);
    ASSERT_EQ(copy_count, 0);
}

TEST(instance_activator, activate_shared_no_object_copy)
{
    struct TestObject
    {
        TestObject(int& counter)
            : counter_(counter)
        {

        }

        TestObject(const TestObject& other)
            : counter_(other.counter_)
        {
            counter_++;
        }

        TestObject(TestObject&& other)
            : counter_(other.counter_)
        {

        }

        TestObject& operator=(const TestObject& other)
        {
            counter_++;
            return *this;
        }
        TestObject& operator=(TestObject&& other) = default;

        int& counter_;
    };

    auto copy_count = 0;

    definition_builder builder;
    builder.define<TestObject>(sample_id, [&copy_count]() -> TestObject
    {
        return TestObject(copy_count);
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_shared<TestObject>(sample_id);

    ASSERT_TRUE(instance);
    ASSERT_EQ(copy_count, 0);
}

TEST(instance_activator, activate_shared_from_unique_no_object_copy)
{
    struct TestObject
    {
        TestObject(int& counter)
            : counter_(counter)
        {

        }

        TestObject(const TestObject& other)
            : counter_(other.counter_)
        {
            counter_++;
        }

        TestObject(TestObject&& other)
            : counter_(other.counter_)
        {

        }

        TestObject& operator=(const TestObject& other)
        {
            counter_++;
            return *this;
        }
        TestObject& operator=(TestObject&& other) = default;

        int& counter_;
    };

    auto copy_count = 0;

    definition_builder builder;
    builder.define<TestObject>(sample_id, [&copy_count]() -> unique_ptr<TestObject>
    {
        return unique_ptr<TestObject>(new TestObject(copy_count));
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_shared<TestObject>(sample_id);

    ASSERT_TRUE(instance);
    ASSERT_EQ(copy_count, 0);
}

TEST(instance_activator, activate_shared_from_pointer_no_parameters)
{
    definition_builder builder;
    builder.define<TestObject_1>(sample_id, []() -> TestObject_1*
    {
        auto pointer = new TestObject_1();
        pointer->field1_ = sample_id;

        return pointer;
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_shared<TestObject_1>(sample_id);

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, sample_id);
}

TEST(instance_activator, activate_shared_from_pointer_with_parameters)
{
    string instance_id("abc");

    definition_builder builder;
    builder.define<TestObject_1, string&>(sample_id, [](auto& parameter0) -> TestObject_1*
    {
        auto pointer = new TestObject_1();
        pointer->field1_ = parameter0;

        return pointer;
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_shared<TestObject_1, string&>(sample_id, instance_id);

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, instance_id);
}

TEST(instance_activator, activate_raii_no_parameters)
{
    definition_builder builder;
    builder.define<TestObject_1>(sample_id, []() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_raii<TestObject_1>(sample_id);

    ASSERT_EQ(instance.field1_, sample_id);
}

TEST(instance_activator, activate_raii_no_parameters_default)
{
    definition_builder builder;
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_raii<TestObject_1>();

    ASSERT_EQ(instance.field1_, sample_id);
}

TEST(instance_activator, activate_raii_with_parameters_default)
{
    string instance_id("abc");

    definition_builder builder;
    builder.define_default<TestObject_1, string&>([](string& parameter0) -> TestObject_1
    {
        return { parameter0 };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_raii<TestObject_1, string&>(instance_id);

    ASSERT_EQ(instance.field1_, instance_id);
}

TEST(instance_activator, activate_raii_with_parameters)
{
    definition_builder builder;
    builder.define<TestObject_1, string>(sample_id, [](string parameter0) -> TestObject_1
    {
        return { parameter0 };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_raii<TestObject_1, string>(sample_id, sample_id);

    ASSERT_EQ(instance.field1_, sample_id);
}

TEST(instance_activator, activate_raii_no_closure_copy)
{
    struct TestClosure
    {
        TestClosure(int& counter)
            : counter_(counter)
        {

        }

        TestClosure(const TestClosure& other)
            : counter_(other.counter_)
        {
            counter_++;
        }

        TestClosure(TestClosure&& other) = default;

        int& counter_;
    };

    auto copy_count = 0;

    TestClosure closure(copy_count);
    ASSERT_EQ(copy_count, 0);

    definition_builder builder;
    builder.define<TestObject_1>(sample_id, [closure = std::move(closure)]() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_raii<TestObject_1>(sample_id);

    ASSERT_EQ(instance.field1_, sample_id);
    ASSERT_EQ(copy_count, 0);
}

TEST(instance_activator, activate_raii_no_object_copy)
{
    struct TestObject
    {
        TestObject(int& counter)
            : counter_(counter)
        {

        }

        TestObject(const TestObject& other)
            : counter_(other.counter_)
        {
            counter_++;
        }

        TestObject(TestObject&& other)
            : counter_(other.counter_)
        {

        }

        TestObject& operator=(const TestObject& other)
        {
            counter_++;
            return *this;
        }
        TestObject& operator=(TestObject&& other) = default;

        int& counter_;
    };

    auto copy_count = 0;

    definition_builder builder;
    builder.define<TestObject>(sample_id, [&copy_count]() -> TestObject
    {
        return TestObject(copy_count);
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_raii<TestObject>(sample_id);

    ASSERT_EQ(copy_count, 0);
    ASSERT_EQ(instance.counter_, 0);
}

TEST(instance_activator, activate_raii_from_unique_no_object_copy)
{
    struct TestObject
    {
        TestObject(int& counter)
            : counter_(counter)
        {

        }

        TestObject(const TestObject& other)
            : counter_(other.counter_)
        {
            counter_++;
        }

        TestObject(TestObject&& other)
            : counter_(other.counter_)
        {

        }

        TestObject& operator=(const TestObject& other)
        {
            counter_++;
            return *this;
        }
        TestObject& operator=(TestObject&& other) = default;

        int& counter_;
    };

    auto copy_count = 0;

    definition_builder builder;
    builder.define<TestObject>(sample_id, [&copy_count]() -> unique_ptr<TestObject>
    {
        return unique_ptr<TestObject>(new TestObject(copy_count));
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_raii<TestObject>(sample_id);

    ASSERT_EQ(copy_count, 0);
    ASSERT_EQ(instance.counter_, 0);
}

TEST(instance_activator, activate_raii_from_pointer_no_parameters)
{
    definition_builder builder;
    builder.define<TestObject_1>(sample_id, []() -> TestObject_1*
    {
        auto pointer = new TestObject_1();
        pointer->field1_ = sample_id;

        return pointer;
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_raii<TestObject_1>(sample_id);

    ASSERT_EQ(instance.field1_, sample_id);
}

TEST(instance_activator, activate_raii_from_pointer_with_parameters)
{
    string instance_id("abc");

    definition_builder builder;
    builder.define<TestObject_1, string&>(sample_id, [](auto& parameter0) -> TestObject_1*
    {
        auto pointer = new TestObject_1();
        pointer->field1_ = parameter0;

        return pointer;
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_raii<TestObject_1, string&>(sample_id, instance_id);

    ASSERT_EQ(instance.field1_, instance_id);
}

TEST(instance_activator, activate_unique_default_type_missing)
{
    definition_builder builder;
    instance_activator activator(std::move(builder));

    ASSERT_THROW(activator.activate_default_unique<TestObject_1>(), invalid_argument);
}

TEST(instance_activator, activate_unique_id_missing)
{
    definition_builder builder;
    builder.define<TestObject_1>(sample_id, []() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));

    ASSERT_THROW(activator.activate_unique<TestObject_1>("undefined-id"), invalid_argument);
}

TEST(instance_activator, activate_shared_default_type_missing)
{
    definition_builder builder;
    instance_activator activator(std::move(builder));

    ASSERT_THROW(activator.activate_default_shared<TestObject_1>(), invalid_argument);
}

TEST(instance_activator, activate_shared_id_missing)
{
    definition_builder builder;
    builder.define<TestObject_1>(sample_id, []() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));

    ASSERT_THROW(activator.activate_shared<TestObject_1>("undefined-id"), invalid_argument);
}

TEST(instance_activator, activate_raii_default_type_missing)
{
    definition_builder builder;
    instance_activator activator(std::move(builder));

    ASSERT_THROW(activator.activate_default_raii<TestObject_1>(), invalid_argument);
}

TEST(instance_activator, activate_raii_id_missing)
{
    definition_builder builder;
    builder.define<TestObject_1>(sample_id, []() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));

    ASSERT_THROW(activator.activate_raii<TestObject_1>("undefined-id"), invalid_argument);
}

TEST(instance_activator, activate_composite_as_unique_no_paramaters)
{
    struct CompositeObject
    {
        TestObject_1 field1_;
        unique_ptr<TestObject_1> field2_;
        shared_ptr<TestObject_1> field3_;
    };

    definition_builder builder;
    builder.define_default<CompositeObject>([](const activation_context& context) -> CompositeObject
    {
        CompositeObject instance;
        instance.field1_ = context.activate_default_raii<TestObject_1>();
        instance.field2_ = context.activate_default_unique<TestObject_1>();
        instance.field3_ = context.activate_default_shared<TestObject_1>();

        return instance;
    });
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_unique<CompositeObject>();

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_.field1_, sample_id);
    ASSERT_TRUE(instance->field2_);
    ASSERT_EQ(instance->field2_->field1_, sample_id);
    ASSERT_TRUE(instance->field3_);
    ASSERT_EQ(instance->field3_->field1_, sample_id);
}

TEST(instance_activator, activate_composite_as_unique_with_paramaters)
{
    struct CompositeObject
    {
        TestObject_1 field1_;
        unique_ptr<TestObject_1> field2_;
        shared_ptr<TestObject_1> field3_;
    };

    definition_builder builder;
    builder.define_default<CompositeObject>([](const activation_context& context) -> CompositeObject
    {
        int counter = 0;

        CompositeObject instance;
        instance.field1_ = context.activate_default_raii<TestObject_1, const string&>(to_string(counter++));
        instance.field2_ = context.activate_default_unique<TestObject_1, const string&>(to_string(counter++));
        instance.field3_ = context.activate_default_shared<TestObject_1, const string&>(to_string(counter++));

        return instance;
    });
    builder.define_default<TestObject_1, const string&>([](const auto& parameter0) -> TestObject_1
    {
        return { parameter0 };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_unique<CompositeObject>();

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_.field1_, "0");
    ASSERT_TRUE(instance->field2_);
    ASSERT_EQ(instance->field2_->field1_, "1");
    ASSERT_TRUE(instance->field3_);
    ASSERT_EQ(instance->field3_->field1_, "2");
}

TEST(instance_activator, activate_composite_as_unique_no_paramaters_auto)
{
    struct CompositeObject
    {
        TestObject_1 field1_;
        unique_ptr<TestObject_1> field2_;
        shared_ptr<TestObject_1> field3_;
    };

    definition_builder builder;
    builder.define_default<CompositeObject>([](const activation_context& context) -> CompositeObject
    {
        CompositeObject instance;
        instance.field1_ = context.activate_default<TestObject_1>();
        instance.field2_ = context.activate_default<TestObject_1>();
        instance.field3_ = context.activate_default<TestObject_1>();

        return instance;
    });
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_unique<CompositeObject>();

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_.field1_, sample_id);
    ASSERT_TRUE(instance->field2_);
    ASSERT_EQ(instance->field2_->field1_, sample_id);
    ASSERT_TRUE(instance->field3_);
    ASSERT_EQ(instance->field3_->field1_, sample_id);
}

TEST(instance_activator, activate_composite_as_unique_with_paramaters_auto)
{
    struct CompositeObject
    {
        TestObject_1 field1_;
        unique_ptr<TestObject_1> field2_;
        shared_ptr<TestObject_1> field3_;
    };

    definition_builder builder;
    builder.define_default<CompositeObject>([](const activation_context& context) -> CompositeObject
    {
        auto i = 0;

        CompositeObject instance;
        instance.field1_ = context.activate_default<TestObject_1>(to_string(i++));
        instance.field2_ = context.activate_default<TestObject_1>(to_string(i++));
        instance.field3_ = context.activate_default<TestObject_1>(to_string(i++));

        return instance;
    });
    builder.define_default<TestObject_1, string>([](string parameter0) -> TestObject_1
    {
        return { parameter0 };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_unique<CompositeObject>();

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_.field1_, "0");
    ASSERT_TRUE(instance->field2_);
    ASSERT_EQ(instance->field2_->field1_, "1");
    ASSERT_TRUE(instance->field3_);
    ASSERT_EQ(instance->field3_->field1_, "2");
}

TEST(instance_activator, activate_composite_as_shared_no_paramaters)
{
    struct CompositeObject
    {
        TestObject_1 field1_;
        unique_ptr<TestObject_1> field2_;
        shared_ptr<TestObject_1> field3_;
    };

    definition_builder builder;
    builder.define_default<CompositeObject>([](const activation_context& context) -> CompositeObject
    {
        CompositeObject instance;
        instance.field1_ = context.activate_default_raii<TestObject_1>();
        instance.field2_ = context.activate_default_unique<TestObject_1>();
        instance.field3_ = context.activate_default_shared<TestObject_1>();

        return instance;
    });
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_shared<CompositeObject>();

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_.field1_, sample_id);
    ASSERT_TRUE(instance->field2_);
    ASSERT_EQ(instance->field2_->field1_, sample_id);
    ASSERT_TRUE(instance->field3_);
    ASSERT_EQ(instance->field3_->field1_, sample_id);
}

TEST(instance_activator, activate_composite_as_shared_with_paramaters)
{
    struct CompositeObject
    {
        TestObject_1 field1_;
        unique_ptr<TestObject_1> field2_;
        shared_ptr<TestObject_1> field3_;
    };

    definition_builder builder;
    builder.define_default<CompositeObject>([](const activation_context& context) -> CompositeObject
    {
        int counter = 0;

        CompositeObject instance;
        instance.field1_ = context.activate_default_raii<TestObject_1, const string&>(to_string(counter++));
        instance.field2_ = context.activate_default_unique<TestObject_1, const string&>(to_string(counter++));
        instance.field3_ = context.activate_default_shared<TestObject_1, const string&>(to_string(counter++));

        return instance;
    });
    builder.define_default<TestObject_1, const string&>([](const auto& parameter0) -> TestObject_1
    {
        return { parameter0 };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_shared<CompositeObject>();

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_.field1_, "0");
    ASSERT_TRUE(instance->field2_);
    ASSERT_EQ(instance->field2_->field1_, "1");
    ASSERT_TRUE(instance->field3_);
    ASSERT_EQ(instance->field3_->field1_, "2");
}

TEST(instance_activator, activate_composite_as_shared_no_paramaters_auto)
{
    struct CompositeObject
    {
        TestObject_1 field1_;
        unique_ptr<TestObject_1> field2_;
        shared_ptr<TestObject_1> field3_;
    };

    definition_builder builder;
    builder.define_default<CompositeObject>([](const activation_context& context) -> CompositeObject
    {
        CompositeObject instance;
        instance.field1_ = context.activate_default<TestObject_1>();
        instance.field2_ = context.activate_default<TestObject_1>();
        instance.field3_ = context.activate_default<TestObject_1>();

        return instance;
    });
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_shared<CompositeObject>();

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_.field1_, sample_id);
    ASSERT_TRUE(instance->field2_);
    ASSERT_EQ(instance->field2_->field1_, sample_id);
    ASSERT_TRUE(instance->field3_);
    ASSERT_EQ(instance->field3_->field1_, sample_id);
}

TEST(instance_activator, activate_composite_as_shared_with_paramaters_auto)
{
    struct CompositeObject
    {
        TestObject_1 field1_;
        unique_ptr<TestObject_1> field2_;
        shared_ptr<TestObject_1> field3_;
    };

    definition_builder builder;
    builder.define_default<CompositeObject>([](const activation_context& context) -> CompositeObject
    {
        auto i = 0;

        CompositeObject instance;
        instance.field1_ = context.activate_default<TestObject_1>(to_string(i++));
        instance.field2_ = context.activate_default<TestObject_1>(to_string(i++));
        instance.field3_ = context.activate_default<TestObject_1>(to_string(i++));

        return instance;
    });
    builder.define_default<TestObject_1, string>([](string parameter0) -> TestObject_1
    {
        return { parameter0 };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_shared<CompositeObject>();

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_.field1_, "0");
    ASSERT_TRUE(instance->field2_);
    ASSERT_EQ(instance->field2_->field1_, "1");
    ASSERT_TRUE(instance->field3_);
    ASSERT_EQ(instance->field3_->field1_, "2");
}

TEST(instance_activator, activate_composite_as_raii_no_paramaters)
{
    struct CompositeObject
    {
        TestObject_1 field1_;
        unique_ptr<TestObject_1> field2_;
        shared_ptr<TestObject_1> field3_;
    };

    definition_builder builder;
    builder.define_default<CompositeObject>([](const activation_context& context) -> CompositeObject
    {
        CompositeObject instance;
        instance.field1_ = context.activate_default_raii<TestObject_1>();
        instance.field2_ = context.activate_default_unique<TestObject_1>();
        instance.field3_ = context.activate_default_shared<TestObject_1>();

        return instance;
    });
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_raii<CompositeObject>();

    ASSERT_EQ(instance.field1_.field1_, sample_id);
    ASSERT_TRUE(instance.field2_);
    ASSERT_EQ(instance.field2_->field1_, sample_id);
    ASSERT_TRUE(instance.field3_);
    ASSERT_EQ(instance.field3_->field1_, sample_id);
}

TEST(instance_activator, activate_composite_as_raii_with_paramaters)
{
    struct CompositeObject
    {
        TestObject_1 field1_;
        unique_ptr<TestObject_1> field2_;
        shared_ptr<TestObject_1> field3_;
    };

    definition_builder builder;
    builder.define_default<CompositeObject>([](const activation_context& context) -> CompositeObject
    {
        int counter = 0;

        CompositeObject instance;
        instance.field1_ = context.activate_default_raii<TestObject_1, const string&>(to_string(counter++));
        instance.field2_ = context.activate_default_unique<TestObject_1, const string&>(to_string(counter++));
        instance.field3_ = context.activate_default_shared<TestObject_1, const string&>(to_string(counter++));

        return instance;
    });
    builder.define_default<TestObject_1, const string&>([](const auto& parameter0) -> TestObject_1
    {
        return { parameter0 };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_raii<CompositeObject>();

    ASSERT_EQ(instance.field1_.field1_, "0");
    ASSERT_TRUE(instance.field2_);
    ASSERT_EQ(instance.field2_->field1_, "1");
    ASSERT_TRUE(instance.field3_);
    ASSERT_EQ(instance.field3_->field1_, "2");
}

TEST(instance_activator, activate_composite_as_raii_no_paramaters_auto)
{
    struct CompositeObject
    {
        TestObject_1 field1_;
        unique_ptr<TestObject_1> field2_;
        shared_ptr<TestObject_1> field3_;
    };

    definition_builder builder;
    builder.define_default<CompositeObject>([](const activation_context& context) -> CompositeObject
    {
        CompositeObject instance;
        instance.field1_ = context.activate_default<TestObject_1>();
        instance.field2_ = context.activate_default<TestObject_1>();
        instance.field3_ = context.activate_default<TestObject_1>();

        return instance;
    });
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_raii<CompositeObject>();

    ASSERT_EQ(instance.field1_.field1_, sample_id);
    ASSERT_TRUE(instance.field2_);
    ASSERT_EQ(instance.field2_->field1_, sample_id);
    ASSERT_TRUE(instance.field3_);
    ASSERT_EQ(instance.field3_->field1_, sample_id);
}

TEST(instance_activator, activate_composite_as_raii_with_paramaters_auto)
{
    struct CompositeObject
    {
        TestObject_1 field1_;
        unique_ptr<TestObject_1> field2_;
        shared_ptr<TestObject_1> field3_;
    };

    definition_builder builder;
    builder.define_default<CompositeObject>([](const activation_context& context) -> CompositeObject
    {
        auto i = 0;

        CompositeObject instance;
        instance.field1_ = context.activate_default<TestObject_1>(to_string(i++));
        instance.field2_ = context.activate_default<TestObject_1>(to_string(i++));
        instance.field3_ = context.activate_default<TestObject_1>(to_string(i++));

        return instance;
    });
    builder.define_default<TestObject_1, string>([](string parameter0) -> TestObject_1
    {
        return { parameter0 };
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_raii<CompositeObject>();

    ASSERT_EQ(instance.field1_.field1_, "0");
    ASSERT_TRUE(instance.field2_);
    ASSERT_EQ(instance.field2_->field1_, "1");
    ASSERT_TRUE(instance.field3_);
    ASSERT_EQ(instance.field3_->field1_, "2");
}

TEST(instance_activator, activate_unique_from_module_definition)
{
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

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, sample_id);
}

TEST(instance_activator, activate_shared_from_module_definition)
{
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
    auto instance = activator.activate_default_shared<TestObject_1>();

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, sample_id);
}

TEST(instance_activator, activate_raii_from_module_definition)
{
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
    auto instance = activator.activate_default_raii<TestObject_1>();

    ASSERT_EQ(instance.field1_, sample_id);
}

TEST(instance_activator, activate_with_different_params_types)
{
    string string_id("sample");
    int int_id(10);
    double double_id(20.3);

    definition_builder builder;
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });
    builder.define<TestObject_1>("a", []() -> TestObject_1
    {
        return { "a" };
    });
    builder.define<TestObject_1>("b", []() -> TestObject_1
    {
        return { "b" };
    });
    builder.define_default<TestObject_1, string>([](auto parameter0) -> TestObject_1
    {
        return { parameter0 };
    });
    builder.define_default<TestObject_1, int>([](auto parameter0) -> TestObject_1
    {
        return { "int " + to_string(parameter0) };
    });
    builder.define_default<TestObject_1, double>([](auto parameter0) -> TestObject_1
    {
        return { "double " + to_string(parameter0) };
    });

    instance_activator activator(std::move(builder));
    auto instance_1 = activator.activate_default_raii<TestObject_1>();
    ASSERT_EQ(instance_1.field1_, sample_id);

    auto instance_1_a = activator.activate_raii<TestObject_1>("a");
    ASSERT_EQ(instance_1_a.field1_, "a");

    auto instance_1_b = activator.activate_raii<TestObject_1>("b");
    ASSERT_EQ(instance_1_b.field1_, "b");

    auto instance_2 = activator.activate_default_raii<TestObject_1>(string_id);
    ASSERT_EQ(instance_2.field1_, string_id);

    auto instance_3 = activator.activate_default_raii<TestObject_1>(int_id);
    ASSERT_EQ(instance_3.field1_, "int " + to_string(int_id));

    auto instance_4 = activator.activate_default_raii<TestObject_1>(double_id);
    ASSERT_EQ(instance_4.field1_, "double " + to_string(double_id));
}

TEST(instance_activator, activate_through_base_type_alias_unique_ptr)
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
                    })
            .as<BaseObject>();

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_unique<BaseObject>();

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, sample_id);
}

TEST(instance_activator, activate_through_base_type_alias_shared_ptr)
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
                    })
            .as<BaseObject>();

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_shared<BaseObject>();

    ASSERT_TRUE(instance);
    ASSERT_EQ(instance->field1_, sample_id);
}

TEST(instance_activator, activate_type__no_args)
{
    struct TestObject_Local
    {

    };

    definition_builder builder;
    builder.define_type<TestObject_Local>();

    instance_activator activator(std::move(builder));
    auto obj = activator.activate_default_unique<TestObject_Local>();
    ASSERT_TRUE(obj);
}

TEST(instance_activator, activate_type__no_dependecies)
{
    struct TestObject_Local
    {
        TestObject_Local(int a)
            :
                a_(a)
        {

        }

        int a_;
    };

    definition_builder builder;
    builder.define_type<TestObject_Local>();
    builder.define_default_instance<int>(67);

    instance_activator activator(std::move(builder));
    auto obj = activator.activate_default_unique<TestObject_Local>();
    ASSERT_TRUE(obj);
    ASSERT_EQ(obj->a_, 67);
}

TEST(instance_activator, activate_type__dependency_rvalue)
{
    struct TestObject_1
    {
        TestObject_1(int a)
            :
                a_(a)
        {

        }

        int a_;
    };

    struct TestObject_2
    {
        TestObject_2(TestObject_1&& dependency)
            :
                dependency_(std::move(dependency))
        {

        }

        TestObject_1 dependency_;
    };

    definition_builder builder;
    builder.define_type<TestObject_1>();
    builder.define_type<TestObject_2>();
    builder.define_default_instance<int>(67);

    instance_activator activator(std::move(builder));
    auto obj = activator.activate_default_unique<TestObject_2>();
    ASSERT_TRUE(obj);
    ASSERT_EQ(obj->dependency_.a_, 67);
}

TEST(instance_activator, activate_type__dependency_unique_ptr)
{
    struct TestObject_1
    {
        TestObject_1(int a)
                :
                a_(a)
        {

        }

        int a_;
    };

    struct TestObject_2
    {
        TestObject_2(unique_ptr<TestObject_1> dependency)
            :
                dependency_(std::move(dependency))
        {

        }

        unique_ptr<TestObject_1> dependency_;
    };

    definition_builder builder;
    builder.define_type<TestObject_1>();
    builder.define_type<TestObject_2>();
    builder.define_default_instance<int>(67);

    instance_activator activator(std::move(builder));
    auto obj = activator.activate_default_unique<TestObject_2>();
    ASSERT_TRUE(obj);
    ASSERT_EQ(obj->dependency_->a_, 67);
}

TEST(instance_activator, activate_type__dependency_unique_ptr_rvalue)
{
    struct TestObject_1
    {
        TestObject_1(int a)
            :
                a_(a)
        {

        }

        int a_;
    };

    struct TestObject_2
    {
        TestObject_2(unique_ptr<TestObject_1>&& dependency)
            :
                dependency_(std::move(dependency))
        {

        }

        unique_ptr<TestObject_1> dependency_;
    };

    definition_builder builder;
    builder.define_type<TestObject_1>();
    builder.define_type<TestObject_2>();
    builder.define_default_instance<int>(67);

    instance_activator activator(std::move(builder));
    auto obj = activator.activate_default_unique<TestObject_2>();
    ASSERT_TRUE(obj);
    ASSERT_EQ(obj->dependency_->a_, 67);
}

TEST(instance_activator, activate_type__dependency_shared_ptr)
{
    struct TestObject_1
    {
        TestObject_1(int a)
                :
                a_(a)
        {

        }

        int a_;
    };

    struct TestObject_2
    {
        TestObject_2(shared_ptr<TestObject_1> dependency)
            :
                dependency_(std::move(dependency))
        {

        }

        shared_ptr<TestObject_1> dependency_;
    };

    definition_builder builder;
    builder.define_type<TestObject_1>();
    builder.define_type<TestObject_2>();
    builder.define_default_instance<int>(67);

    instance_activator activator(std::move(builder));
    auto obj = activator.activate_default_unique<TestObject_2>();
    ASSERT_TRUE(obj);
    ASSERT_EQ(obj->dependency_->a_, 67);
}

TEST(instance_activator, activate_explicit_type)
{
    struct TestObject_Local
    {
        TestObject_Local(string&& a, int b)
            :
                a_(move(a)),
                b_(b)
        {
        }

        string a_;
        int b_;
    };

    definition_builder builder;
    builder.define_explicit_type<TestObject_Local, string, int>();
    builder.define_default_instance<string>("abc");
    builder.define_default_instance<int>(5);

    instance_activator activator(std::move(builder));
    auto obj = activator.activate_default_raii<TestObject_Local>();
    ASSERT_EQ(obj.a_, "abc");
    ASSERT_EQ(obj.b_, 5);
}

TEST(instance_activator, activate_explicit_type__base_type)
{
    struct interface
    {
        virtual ~interface() = default;
    };

    struct TestObject_Local : interface
    {
        TestObject_Local(string&& a, int b)
            :
                a_(move(a)),
                b_(b)
        {
        }

        string a_;
        int b_;
    };

    definition_builder builder;
    builder.define_explicit_type<TestObject_Local, string, int>().as<interface>();
    builder.define_default_instance<string>("abc");
    builder.define_default_instance<int>(5);

    instance_activator activator(std::move(builder));
    auto obj = activator.activate_default_unique<interface>();
    ASSERT_TRUE(obj);
    //ASSERT_EQ(typeid(*obj), typeid(TestObject_Local));
}

TEST(instance_activator, activate_with_annotations)
{
    definition_builder builder;
    builder.define_default<TestObject_1>([](const activation_context& context) ->TestObject_1
    {
        return { context.annotation<string>() };
    });

    instance_activator activator(std::move(builder));

    auto annotation = "annotation_1";
    annotations_map annotations(string("annotation_1"));
    //annotations.set(string(annotation));

    auto instance = activator.activate_default_raii<TestObject_1>(std::move(annotations));
    ASSERT_EQ(instance.field1_, annotation);
}

TEST(instance_activator, activate_with_multiple_annotations)
{
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
    ASSERT_EQ(instance.field1_, "JohnSmith");
}

TEST(instance_activator, activate_annotations_derived_from_definition)
{
    definition_builder builder;
    builder.define_default<TestObject_1>([](const activation_context& context) ->TestObject_1
    {
        return { context.annotation<string>() };
    })
    .annotate(string("annotation_1"));

    instance_activator activator(std::move(builder));

    auto instance = activator.activate_default_raii<TestObject_1>();
    ASSERT_EQ(instance.field1_, "annotation_1");
}

TEST(instance_activator, activate__intercept_lambda_no_intercepted_instance_no_context)
{
    definition_builder builder;
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    auto intercepted = false;
    builder.define_interceptor<TestObject_1>([&intercepted]()
    {
        intercepted = true;
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_raii<TestObject_1>();

    ASSERT_TRUE(intercepted);
}

TEST(instance_activator, activate__intercept_lambda_no_intercepted_instance_with_context)
{
    definition_builder builder;
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    auto intercepted = false;
    builder.define_interceptor<TestObject_1>([&intercepted](const activation_context& context)
    {
        intercepted = true;
        EXPECT_EQ(context.id(), string(definition::default_id));
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_raii<TestObject_1>();

    ASSERT_TRUE(intercepted);
}

TEST(instance_activator, activate__intercept_lambda_with_intercepted_instance)
{
    definition_builder builder;
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    auto intercepted = false;
    builder.define_interceptor<TestObject_1>([&intercepted](TestObject_1& activated, const activation_context& context)
    {
        intercepted = true;
        EXPECT_EQ(activated.field1_, sample_id);
        EXPECT_EQ(context.id(), string(definition::default_id));
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_raii<TestObject_1>();

    ASSERT_TRUE(intercepted);
}

TEST(instance_activator, activate__intercept_all_ids)
{
    string id_1("a");
    string id_2("b");

    definition_builder builder;
    builder.define<TestObject_1>(id_1, [&]() -> TestObject_1
    {
        return { id_1 };
    });

    builder.define<TestObject_1>(id_2, [&]() -> TestObject_1
    {
        return { id_2 };
    });

    vector<string> intercepted;
    builder.define_interceptor<TestObject_1>([&intercepted](const activation_context& context)
    {
        intercepted.push_back(context.id());
    });

    instance_activator activator(std::move(builder));
    auto instance_1 = activator.activate_raii<TestObject_1>(id_1);
    ASSERT_EQ(intercepted.size(), 1u);
    ASSERT_THAT(intercepted, ElementsAre(id_1));

    auto instance_2 = activator.activate_raii<TestObject_1>(id_1);
    ASSERT_EQ(intercepted.size(), 2u);
    ASSERT_THAT(intercepted, ElementsAre(id_1, id_1));

    auto instance_3 = activator.activate_raii<TestObject_1>(id_2);
    ASSERT_EQ(intercepted.size(), 3u);
    ASSERT_THAT(intercepted, ElementsAre(id_1, id_1, id_2));

}

TEST(instance_activator, activate__intercept__non_static_method__no_intercepted_instance__no_context)
{
    struct test_handler
    {
        void intercept()
        {
            intercepted_ = true;
        }

        bool intercepted_{false};
    };

    definition_builder builder;
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    test_handler handler;
    builder.define_interceptor<TestObject_1>(&test_handler::intercept, &handler);

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_raii<TestObject_1>();

    ASSERT_TRUE(handler.intercepted_);
}

TEST(instance_activator, activate__intercept__non_static_method__with_intercepted_instance__no_context)
{
    struct test_handler
    {
        void intercept(TestObject_1& instance)
        {
            intercepted_ = true;
        }

        bool intercepted_{false};
    };

    definition_builder builder;
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    test_handler handler;
    builder.define_interceptor(&test_handler::intercept, &handler);

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_raii<TestObject_1>();

    ASSERT_TRUE(handler.intercepted_);
}

TEST(instance_activator, activate__intercept__non_static_method__with_intercepted_instance__with_context)
{
    struct test_handler
    {
        void intercept(TestObject_1& instance, const activation_context& context)
        {
            intercepted_ = true;
        }

        bool intercepted_{false};
    };

    definition_builder builder;
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    test_handler handler;
    builder.define_interceptor(&test_handler::intercept, &handler);

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_raii<TestObject_1>();

    ASSERT_TRUE(handler.intercepted_);
}

TEST(instance_activator, activate__intercept__non_static_method__no_intercepted_instance__with_context)
{
    struct test_handler
    {
        void intercept(const activation_context& context)
        {
            intercepted_ = true;
        }

        bool intercepted_{false};
    };

    definition_builder builder;
    builder.define_default<TestObject_1>([]() -> TestObject_1
    {
        return { sample_id };
    });

    test_handler handler;
    builder.define_interceptor<TestObject_1>(&test_handler::intercept, &handler);

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_raii<TestObject_1>();

    ASSERT_TRUE(handler.intercepted_);
}

TEST(instance_activator, activate__decorate__pointer)
{
    static auto component_count = 0u;
    static auto decorator_count = 0u;

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
            component_count++;
        }
    };

    struct decorator : interface
    {
        decorator(interface* undecorated)
            : undecorated_(undecorated)
        {

        }

        ~decorator() override
        {
            delete undecorated_;
        }

        void method() override
        {
            decorator_count++;
            undecorated_->method();
        }

        interface* undecorated_;
    };

    definition_builder builder;
    builder.define_default<interface>([]() -> interface*
    {
        return new component();
    });

    builder.define_decorator<interface>([](interface* undecorated) -> interface*
    {
        return new decorator(undecorated);
    },
    [](interface* decorated)
    {
        auto decorator_ptr = static_cast<decorator*>(decorated);
        delete decorator_ptr;
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_unique<interface>();

    instance->method();
    ASSERT_EQ(component_count, 1u);
    ASSERT_EQ(decorator_count, 1u);
}

TEST(instance_activator, activate__decorate__pointer__multidecoration)
{
    static auto component_count = 0u;
    static auto decorator_count = 0u;

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
            component_count++;
        }
    };

    struct decorator : interface
    {
        decorator(interface* undecorated)
            : undecorated_(undecorated)
        {

        }

        ~decorator() override
        {
            delete undecorated_;
        }

        void method() override
        {
            decorator_count++;
            undecorated_->method();
        }

        interface* undecorated_;
    };

    definition_builder builder;
    builder.define_default<interface>([]() -> interface*
    {
        return new component();
    });

    // decorator 1
    builder.define_decorator<interface>([](interface* undecorated) -> interface*
    {
        return new decorator(undecorated);
    },
    [](interface* decorated)
    {
        auto decorator_ptr = static_cast<decorator*>(decorated);
        delete decorator_ptr;
    });
    // decorator 2
    builder.define_decorator<interface>([](interface* undecorated) -> interface*
    {
        return new decorator(undecorated);
    },
    [](interface* decorated)
    {
        auto decorator_ptr = static_cast<decorator*>(decorated);
        delete decorator_ptr;
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_unique<interface>();

    instance->method();
    ASSERT_EQ(component_count, 1u);
    ASSERT_EQ(decorator_count, 2u);
}

TEST(instance_activator, activate__decorate__unique_ptr)
{
    static auto component_count = 0u;
    static auto decorator_count = 0u;

    struct interface
    {
        virtual ~interface() = default;

        virtual void method() = 0;
    };

    struct component : interface
    {
        virtual ~component() = default;

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

    instance->method();
    ASSERT_EQ(component_count, 1u);
    ASSERT_EQ(decorator_count, 1u);
}

TEST(instance_activator, activate__decorate__unique_ptr__multidecoration)
{
    static auto component_count = 0u;
    static auto decorator_count = 0u;

    struct interface
    {
        virtual ~interface() = default;

        virtual void method() = 0;
    };

    struct component : interface
    {
        virtual ~component() = default;

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

    // decorator 1
    builder.define_decorator<interface>([](unique_ptr<interface>&& undecorated) -> unique_ptr<interface>
    {
        return unique_ptr<interface>(new decorator(std::move(undecorated)));
    });
    // decorator 2
    builder.define_decorator<interface>([](unique_ptr<interface>&& undecorated) -> unique_ptr<interface>
    {
        return unique_ptr<interface>(new decorator(std::move(undecorated)));
    });

    instance_activator activator(std::move(builder));
    auto instance = activator.activate_default_unique<interface>();

    instance->method();
    ASSERT_EQ(component_count, 1u);
    ASSERT_EQ(decorator_count, 2u);
}

TEST(instance_activator, activate__decorate__function_object)
{
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

    instance();
    ASSERT_EQ(component_count, 1u);
    ASSERT_EQ(decorator_count, 1u);
}

TEST(instance_activator, activate__decorate__function_object__multidecoration)
{
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

    instance();
    ASSERT_EQ(component_count, 1u);
    ASSERT_EQ(decorator_count, 2u);
}

/**
 * Tests decoration when both decorator's and decorated object's lifetimes are longer than activator's lifetime.
 */
TEST(instance_activator, activate__decorate__resolvers_destroyed)
{
    using test_function = movable_function<void()>;

    static auto component_count = 0u;
    static auto decorator_count = 0u;

    static auto decorator_created = 0u;
    static auto decorator_deleted = 0u;

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
            decorator_created++;
        }

        decorator(decorator&& other)
            :
                undecorated_(std::move(other.undecorated_))
        {
            decorator_created++;
        }

        decorator& operator=(decorator&& other)
        {
            undecorated_ = std::move(other.undecorated_);
            return *this;
        }

        ~decorator()
        {
            decorator_deleted++;
        }

        void operator()()
        {
            decorator_count++;
            undecorated_();
        }

        test_function undecorated_;
    };

    auto creator = [&]()
    {
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
        return activator.activate_default_raii<test_function>();
    };

    auto instance = creator();
    instance();

    ASSERT_EQ(component_count, 1u);
    ASSERT_EQ(decorator_count, 1u);
    ASSERT_EQ(decorator_created, decorator_deleted + 1u);
}