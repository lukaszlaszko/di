#include <di/annotations_map.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <stdexcept>
#include <string>
#include <utility>

using namespace std;
using namespace di;


TEST(annotations_map, create_from_list)
{
    annotations_map annotations(1, 3.0, string("abc"));

    ASSERT_TRUE(annotations.contains<int>());
    ASSERT_TRUE(annotations.contains<double>());
    ASSERT_TRUE(annotations.contains<string>());
}

TEST(annotations_map, set_reference)
{
    auto annotation_1 = string("abc");
    auto annotation_2 = make_pair(annotation_1, annotation_1);

    annotations_map annotations;
    annotations.set(annotation_1);
    annotations.set(annotation_2);
}

TEST(annotations_map, set_rvalue)
{
    auto annotation_1 = "abc";
    auto annotation_2 = make_pair(annotation_1, annotation_1);

    annotations_map annotations;
    annotations.set(string(annotation_1));
    annotations.set(std::move(annotation_2));
}

TEST(annotations_map, contains)
{
    auto annotation_1 = "abc";

    annotations_map annotations;
    annotations.set(string(annotation_1));

    ASSERT_TRUE(annotations.contains<string>());
    ASSERT_FALSE(annotations.contains<int>());
}

TEST(annotations_map, get)
{
    auto annotation_1 = "abc";

    annotations_map annotations;
    annotations.set(string(annotation_1));

    ASSERT_EQ(annotations.get<string>(), annotation_1);
}

TEST(annotations_map, get_doesntexist)
{
    annotations_map annotations;

    ASSERT_THROW(annotations.get<string>(), out_of_range);
}

TEST(annotations_map, operator_merge)
{
    annotations_map annotations_1(1, 3.0);
    annotations_map annotations_2(2, string("abc"));

    annotations_1 << annotations_2;

    ASSERT_TRUE(annotations_1.contains<int>());
    ASSERT_TRUE(annotations_1.contains<double>());
    ASSERT_TRUE(annotations_1.contains<string>());

    ASSERT_EQ(annotations_1.get<int>(), 1);
    ASSERT_EQ(annotations_1.get<double>(), 3.0);
    ASSERT_EQ(annotations_1.get<string>(), "abc");
}