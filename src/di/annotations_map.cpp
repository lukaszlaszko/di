#include "annotations_map.hpp"


namespace di {

annotations_map& annotations_map::operator <<(const annotations_map& other)
{
    for (auto& annotation_with_id : other.annotations_)
    {
        auto& annotation_id = annotation_with_id.first;
        if (annotations_.find(annotation_id) == annotations_.end())
        {
            auto& annotation = annotation_with_id.second;
            annotations_[annotation_id] = annotation;
        }
    }

    return *this;
}

}

