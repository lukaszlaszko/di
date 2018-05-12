#pragma once

#include "annotations_map.hpp"

#include <boost/optional.hpp>
#include <boost/uuid/uuid.hpp>

#include <memory>
#include <string>
#include <tuple>
#include <utility>


namespace di {

class instance_activator;

/**
 * @brief Represents an activation context accessible within activation callback.
 */
class activation_context
{
public:
    template <typename T, typename... args_types>
    class activation
    {
    public:
        template <typename D>
        class conversion
        {
        public:
            operator D() const;
            operator std::unique_ptr<D>() const;
            operator std::shared_ptr<D>() const;

        private:
            friend activation;
            explicit conversion(activation&& original);

            activation original_;
        };

        /**
         * @brief Copying activation isn't allowed.
         */
        activation(const activation& other) = delete;

        /**
         * @brief Default move constructable.
         */
        activation(activation&& other) = default;

        /**
         * @brief Copying by assignment isn't allowed.
         */
        activation& operator=(const activation& other) = delete;

        template <typename... extra_args_types>
        activation<T, args_types..., extra_args_types...> with(
                extra_args_types... extra_args) const;
        template <typename... extra_args_types>
        activation<T, args_types..., extra_args_types&...> with_reference(
                extra_args_types&... extra_args) const;

        template <typename A>
        activation& with_annotation(const A& annotation);
        template <typename A>
        activation& with_annotation(A&& annotation);
        template <typename A>
        activation& with_optional_annotation(const A& optional_annotation);
        template <typename A>
        activation& with_optional_annotation(A&& optional_annotation);
        template <typename A, typename transformer_type>
        activation& with_optional_annotation(const A& optional_annotation, const transformer_type& transformer);
        template <typename A, typename transformer_type>
        activation& with_optional_annotation(A&& optional_annotation, const transformer_type& transformer);


        template <typename D>
        conversion<D> as();

        operator T() const;
        operator std::unique_ptr<T>() const;
        operator std::shared_ptr<T>() const;

    private:
        friend activation_context;

        explicit activation(
                std::shared_ptr<activation_context> context,
                args_types... args);

        activation& operator=(activation&& other) = default;


        template <typename... extra_args_types, size_t... args_count>
        activation_context::activation<T, args_types..., extra_args_types...> forward_with(
                std::index_sequence<args_count...> args_sequence,
                extra_args_types... extra_args) const;
        template <typename... extra_args_types, size_t... args_count>
        activation_context::activation<T, args_types..., extra_args_types&...> forward_with_reference(
                std::index_sequence<args_count...> args_sequence,
                extra_args_types&... extra_args) const;

        template <size_t... args_count>
        T forward_args_raii(std::index_sequence<args_count...>) const;

        template <size_t... args_count>
        std::unique_ptr<T> forward_args_unique(std::index_sequence<args_count...>) const;

        template <size_t... args_count>
        std::shared_ptr<T> forward_args_shared(std::index_sequence<args_count...>) const;

        std::shared_ptr<activation_context> context_;
        std::tuple<args_types...> args_;

    };

    explicit activation_context(
            const std::string& id,
            const instance_activator& activator);
    explicit activation_context(
            const std::string& id,
            const instance_activator& activator,
            annotations_map&& annotations);
    explicit activation_context(
            const std::string& id,
            const std::string& description_,
            const activation_context& parent);

    const std::string& id() const;
    const std::string& description() const;
    const boost::uuids::uuid& uuid() const;

    boost::optional<const activation_context&> parent() const;

    template <typename A>
    bool has_annotation() const;

    template <typename A>
    const A& annotation() const;

    template <typename T, typename... args_types>
    activation<T, args_types...> activate(const std::string& id, args_types... args) const;

    template <typename T, typename... args_types>
    activation<T, args_types...> activate_with_description(const std::string& id, const std::string& description, args_types... args) const;

    template <typename T, typename... args_types>
    std::unique_ptr<T> activate_unique(const std::string& id, args_types... args) const;

    template <typename T, typename... args_types>
    std::shared_ptr<T> activate_shared(const std::string& id, args_types... args) const;

    template <typename T, typename... args_types>
    T activate_raii(const std::string& id, args_types... args) const;

    template <typename T, typename... args_types>
    activation<T, args_types...> activate_default(args_types... args) const;

    template <typename T, typename... args_types>
    activation<T, args_types...> activate_default_with_description(const std::string& description, args_types... args) const;

    template <typename T, typename... args_types>
    std::unique_ptr<T> activate_default_unique(args_types... args) const;

    template <typename T, typename... args_types>
    std::shared_ptr<T> activate_default_shared(args_types... args) const;

    template <typename T, typename... args_types>
    T activate_default_raii(args_types... args) const;

private:
    friend instance_activator;
    friend std::ostream& operator<<(std::ostream& os, const activation_context& context);

    std::string id_;
    std::string description_;
    boost::uuids::uuid uuid_;
    const instance_activator& activator_;
    boost::optional<const activation_context&> parent_;

    annotations_map annotations_;


};

}

#include "activation_context.ipp"
