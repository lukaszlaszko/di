#include "cxxabi_utils.hpp"

#include <typeinfo>
#include <stdexcept>

#include <cxxabi.h>

using namespace std;


namespace di { namespace tools {

string demangle(string mangled)
{
    int status;
    auto realname = abi::__cxa_demangle(mangled.c_str(), 0, 0, &status);
    if (status != 0)
    {
        free(realname);
        return mangled;
    }

    auto demangled = string(realname);
    free(realname);

    return demangled;
}

} }

