#pragma once

#include "annotations_map.hpp"

#include <type_traits>
#include <utility>
#include <vector>


namespace di {

template <typename... annotation_types>
inline annotations_map::annotations_map(annotation_types&&... annotations)
    : annotations_()
{
    std::vector<std::pair<std::type_index, boost::any>> initials = {
            std::make_pair(
                    std::type_index(typeid(typename std::decay<annotation_types>::type)),
                    boost::any(annotations))...
    };

    for (auto& initial : initials)
        annotations_[initial.first] = initial.second;
}

template <typename A>
inline void annotations_map::set(const A& annotation)
{
    using plain_type = typename std::decay<A>::type;

    auto annotation_id = std::type_index(typeid(plain_type));
    annotations_[annotation_id] = annotation;
}

template <typename A>
inline void annotations_map::set(A&& annotation)
{
    using plain_type = typename std::decay<A>::type;

    auto annotation_id = std::type_index(typeid(plain_type));
    annotations_[annotation_id] = std::move(annotation);
}

template <typename A>
inline bool annotations_map::contains() const
{
    using plain_type = typename std::decay<A>::type;

    auto annotation_id = std::type_index(typeid(plain_type));
    return annotations_.find(annotation_id) != annotations_.end();
}

template <typename A>
inline const A& annotations_map::get() const
{
    using plain_type = typename std::decay<A>::type;

    auto annotation_id = std::type_index(typeid(plain_type));
    auto& annotation = annotations_.at(annotation_id);

    return boost::any_cast<const A&>(annotation);
}

}

