#pragma once

#include <string>


namespace di { namespace tools {

/**
 * @brief Demangles function name using cxxabi.
 * @param mangled Mangled function name.
 * @return Demangled function name.
 */
std::string demangle(const std::string& mangled);

} }

