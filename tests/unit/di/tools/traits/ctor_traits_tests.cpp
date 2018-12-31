#include <di/tools/traits/ctor_traits.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>

using namespace std;
using namespace di::tools;


TEST(ctor_traits, has_constructor__0_args)
{
    struct test_type
    {
        test_type()
        {
        }
    };

    {
        const bool result = has_constructor_n<test_type, 0>::value;
        ASSERT_TRUE(result);
    }

    {
        const bool result = has_constructor_n<test_type, 1>::value;
        ASSERT_FALSE(result);
    }
}

TEST(ctor_traits, has_constructor__1_args)
{
    struct test_type
    {
        test_type(int a)
        {
        }
    };

    struct test_type_2
    {
        test_type_2(test_type_2&)
        {
        }
    };

    {
        const bool result = has_constructor_n<test_type, 0>::value;
        ASSERT_FALSE(result);
    }

    {
        const bool result = has_constructor_n<test_type, 1>::value;
        ASSERT_TRUE(result);
    }

    {
        const bool result = has_constructor_n<test_type_2, 1>::value;
        ASSERT_FALSE(result);
    }

    //test_type t(arg_placeholder<test_type, 1>{});
}

TEST(ctor_traits, has_constructor__2_args)
{
    struct test_type
    {
        test_type(int a, string b)
        {
        }
    };

    {
        const bool result = has_constructor_n<test_type, 0>::value;
        ASSERT_FALSE(result);
    }

    {
        const bool result = has_constructor_n<test_type, 1>::value;
        ASSERT_FALSE(result);
    }

    {
        const bool result = has_constructor_n<test_type, 2>::value;
        ASSERT_TRUE(result);
    }
}

TEST(ctor_traits, ctor_args_count)
{
    struct test_type_1
    {
    };

    struct test_type_2
    {
        test_type_2(int a)
        {
        }
    };

    struct test_type_3
    {
        test_type_3(int a, float b)
        {
        }
    };

    ASSERT_EQ(ctor_args_count<test_type_1>(), 0ul);
    ASSERT_EQ(ctor_args_count<test_type_2>(), 1ul);
    ASSERT_EQ(ctor_args_count<test_type_3>(), 2ul);
}

TEST(ctor_traits, ctor_args_count__multiple_constructors)
{
    struct test_type_1
    {
        test_type_1(int a)
        {
        }

        test_type_1(string a)
        {
        }
    };

    struct test_type_2
    {
        test_type_2(int a)
        {
        }

        test_type_2(int a, string b)
        {
        }
    };

    ASSERT_EQ(ctor_args_count<test_type_1>(), 256ul);
    ASSERT_EQ(ctor_args_count<test_type_2>(), 1ul);
}

TEST(ctor_traits, ctor_count)
{
    struct test_type_0
    {
    };

    struct test_type_1
    {
        test_type_1(int a)
        {
        }

        test_type_1(string a)
        {
        }
    };

    struct test_type_2
    {
        test_type_2(int a)
        {
        }

        test_type_2(int a, string b)
        {
        }

        test_type_2(int a, string b, char c)
        {
        }
    };

    ASSERT_EQ(ctor_count<test_type_0>(), 1ul);
    ASSERT_EQ(ctor_count<test_type_1>(), 0ul);
    ASSERT_EQ(ctor_count<test_type_2>(), 3ul);
}




