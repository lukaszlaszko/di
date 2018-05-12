#pragma once

#include "annotations_map.hpp"

#include <di/tools/hash.hpp>

#include <boost/any.hpp>

#include <functional>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>


namespace di {

class activation_context;

class definition
{
public:
    template <typename... types>
    struct combined_identity
    { };

    using id_type = std::pair<std::string, std::type_index>;
    using map_type = std::unordered_map<id_type, definition, tools::pair_hash>;

    static constexpr auto default_id = "";

    template <typename creator_type, typename deleter_type>
    explicit definition(creator_type&& creator, deleter_type&& deleter);

    /**
     * @brief Non-copy constructable.
     */
    definition(const definition& other) = delete;
    /**
     * @brief Default move constructable.
     */
    definition(definition&& other) = default;
    /**
     * @brief Non-copy assignable.
     */
    definition& operator=(const definition& other) = delete;
    /**
     * @brief Default move assignable.
     */
    definition& operator=(definition&& other) = default;

    template <typename T, typename... args_types>
    static id_type make_id(const std::string& id);

    template <typename return_type, typename... args_types>
    const std::function<return_type*(const activation_context&, args_types...)>& creator() const;

    template <typename T>
    const std::function<void(T*)>& deleter() const;

    annotations_map& annotations();

private:
    boost::any creator_;
    boost::any deleter_;

    annotations_map annotations_;

};

}

#include "definition.ipp"

