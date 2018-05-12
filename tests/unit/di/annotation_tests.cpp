#include <di/annotation.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>

using namespace std;
using namespace di;


TEST(annotation, make_annotation)
{
    static const auto my_tag = 1u;
    auto annotation = make_annotation<my_tag>(string("abc"));
}

TEST(annotation, make_annotation_default_tag)
{
    auto annotation = make_annotation(string("abc"));
}
