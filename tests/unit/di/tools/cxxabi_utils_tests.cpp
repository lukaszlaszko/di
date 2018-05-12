#include <di/tools/cxxabi_utils.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>

using namespace std;
using namespace di::tools;


struct sample_type
{
};

TEST(cxxabi_utils, demangle)
{
    string name = typeid(sample_type).name();
    ASSERT_NE(name, "sample_type");

    string demangled = demangle(name);
    ASSERT_EQ(demangled, "sample_type");
}
