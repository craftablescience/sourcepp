#pragma once

#include <type_traits>

namespace sourcepp {

namespace detail {

template<typename T>
struct member_type_helper;

template<typename C, typename T>
struct member_type_helper<T C::*> { using type = T; };

} // namespace detail

template<typename T>
struct member_type : detail::member_type_helper<std::remove_cvref_t<T>> {};

template<typename T>
using member_type_t = typename member_type<T>::type;

} // namespace sourcepp
