#pragma once

#include <functional>
#include <utility>


namespace di { namespace tools {

template <typename T1, typename T2>
inline size_t pair_hash::operator ()(const std::pair<T1, T2>& p) const
{
    auto h1 = std::hash<T1>{}(p.first);
    auto h2 = std::hash<T2>{}(p.second);

    return h1 ^ h2;
}

} }

namespace std {

template <typename first_type, typename second_type>
inline size_t hash<std::pair<first_type, second_type>>::operator()(const std::pair<first_type, second_type>& p) const
{
    auto h1 = std::hash<first_type>{}(p.first);
    auto h2 = std::hash<second_type>{}(p.second);

    return h1 ^ h2;
};

}
