#pragma once

#include <boost/any.hpp>

#include <iostream>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>


namespace di {

class annotations_map
{
public:
    template <typename... annotation_types>
    annotations_map(annotation_types&&... annotations);

    template <typename A>
    void set(const A& annotation);

    template <typename A>
    void set(A&& annotation);

    template <typename A>
    bool contains() const;

    template <typename A>
    const A& get() const;

    annotations_map& operator<<(const annotations_map& other);

private:
    std::unordered_map<std::type_index, boost::any> annotations_;

};

}

#include "annotations_map.ipp"
