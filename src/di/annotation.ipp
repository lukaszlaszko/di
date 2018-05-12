#pragma once


namespace di {

template <typename T>
annotation<T> make_annotation(T&& value)
{
    return { std::move(value) };
}

template <size_t tag, typename T>
annotation<T, tag> make_annotation(T&& value)
{
    return { std::move(value) };
}

}

