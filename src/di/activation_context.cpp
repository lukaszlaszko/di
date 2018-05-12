#include "activation_context.hpp"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <string>


using namespace std;
using namespace boost::uuids;


namespace di {

activation_context::activation_context(
        const string& id,
        const instance_activator& activator)
    :
        id_(id),
        uuid_(random_generator()()),
        activator_(activator),
        parent_(boost::none),
        annotations_()
{

}

activation_context::activation_context(
        const string& id,
        const instance_activator& activator,
        annotations_map&& annotations)
    :
        id_(id),
        uuid_(random_generator()()),
        activator_(activator),
        parent_(boost::none),
        annotations_(std::move(annotations))
{

}

activation_context::activation_context(
        const string& id,
        const string& description,
        const activation_context& parent)
    :
        id_(id),
        description_(description),
        uuid_(random_generator()()),
        activator_(parent.activator_),
        parent_(parent),
        annotations_(parent.annotations_)
{

}

const string& activation_context::id() const
{
    return id_;
}

const string& activation_context::description() const
{
    return description_;
}

const uuid& activation_context::uuid() const
{
    return uuid_;
}

boost::optional<const activation_context&> activation_context::parent() const
{
    return parent_;
}

ostream& operator<<(ostream& os, const activation_context& context)
{
    os << "current:[" << context.id_ << "]";
    for (auto parent = context.parent_; parent != boost::none; parent = parent->parent_)
    {
        os << "<-parent:[" << parent->id_ << "]";
    }

    return os;
}

}

