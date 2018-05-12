#pragma once

#include <cstddef>
#include <utility>


namespace di { namespace tools {

struct pair_hash
{
    template <class T1, class T2>
    size_t operator()(const std::pair<T1,T2> &p) const;
};

} }

namespace std {

template <typename first_type, typename second_type>
struct hash<std::pair<first_type, second_type>>
{
    size_t operator()(const std::pair<first_type, second_type>& p) const;
};

}

#include "hash.ipp"

