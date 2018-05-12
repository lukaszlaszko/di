#include <di/tools/traits/veriadic_traits.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>

using namespace std;
using namespace testing;
using namespace di::tools;


TEST(argument_types, name_non_empty)
{
    auto count = argument_types<int, string, double>::count;
    ASSERT_EQ(count, 3u);

    string name = argument_types<int, string, double>::name(0u);
    ASSERT_EQ(name, typeid(int).name());

    name = argument_types<int, string, double>::name(1u);
    ASSERT_EQ(name, typeid(string).name());

    name = argument_types<int, string, double>::name(2u);
    ASSERT_EQ(name, typeid(double).name());
}

TEST(argument_types, at)
{
    using zero_type = argument_types<int, string>::at<0>::type;
    using one_type = argument_types<int, string>::at<1>::type;

    ASSERT_EQ(typeid(zero_type), typeid(int));
    ASSERT_EQ(typeid(one_type), typeid(string));
}

