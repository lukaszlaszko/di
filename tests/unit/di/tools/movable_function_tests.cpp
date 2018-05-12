#include <di/tools/movable_function.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <typeindex>
#include <type_traits>

using namespace std;
using namespace di::tools;


TEST(movable_function, target_type__non_function)
{
    struct some_struct
    { };

    some_struct instance;
    ASSERT_EQ(type_index(typeid(some_struct)), target_type(instance));
}

TEST(movable_function, target_type__non_const_function)
{
    struct some_functor
    {
        void operator()(){};
    };

    some_functor instance;
    movable_function<void()> function = instance;
    ASSERT_EQ(type_index(typeid(some_functor)), target_type(instance));
}

TEST(movable_function, target_type__const_function)
{
    struct some_functor
    {
        void operator()() const {};
    };

    const some_functor instance;
    movable_function<void() const> function = instance;
    ASSERT_EQ(type_index(typeid(some_functor)), target_type(instance));
}
