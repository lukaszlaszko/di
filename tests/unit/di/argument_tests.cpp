#include <di/argument.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace di;

char tag[] = "some_tag";


TEST(argument, argument_id__custom)
{
    argument<int, tag> arg;
    auto id = argument_id<decltype(arg)>::value;
    ASSERT_EQ(id, tag);
}

TEST(argument, argument_id__non_argument_type)
{
    auto a = 5;
    auto id = argument_id<decltype(a)>::value;
    ASSERT_EQ(id, "");
}

TEST(argument, argument_type__argument)
{
    argument<int, tag> arg;
    ASSERT_EQ(typeid(argument_type<decltype(arg)>::type), typeid(int));
}

TEST(argument, argument_type__non_argument_type)
{
    int arg;
    ASSERT_EQ(typeid(argument_type<decltype(arg)>::type), typeid(int));
}