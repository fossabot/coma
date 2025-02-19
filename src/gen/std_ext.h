#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

// idea of https://stackoverflow.com/a/24687161/8524139

template <bool... Bs> using bool_sequence = std::integer_sequence<bool, Bs...>;

template <bool... Bs> using bool_and = std::is_same<bool_sequence<Bs...>, bool_sequence<(Bs || true)...>>;

template <bool... Bs> using bool_or = std::integral_constant<bool, !bool_and<!Bs...>::value>;

namespace std_ext
{
template <typename R, bool... Bs> // note: R first, no default :(
using enable_if_any = std::enable_if<bool_or<Bs...>::value, R>;

template <typename R, bool... Bs> // note: R first, no default :(
using enable_if_all = std::enable_if<bool_and<Bs...>::value, R>;

template <typename T, typename... Ts> using are_same = bool_and<std::is_same<T, Ts>::value...>;
}
