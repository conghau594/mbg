// EventBusHelper.hpp
#pragma once

#include <type_traits>

namespace peeb // abbr of `Powerful Elegant Event Bus`
{
  /////////////////////////////////////////////////////////////////////////////
  template <
      typename TYPE,
      typename TMPL>
  struct is_in_template_anydepth : std::false_type
  {
  };

  /////////////////////////////////////////////////////////////////////////////
  template <
      typename TYPE,
      template <typename...> typename TMPL,
      typename... ARGs>
  struct is_in_template_anydepth<TYPE, TMPL<ARGs...>>
      : std::bool_constant<
            (... || std::is_same_v<TYPE, ARGs>) ||
            (... || is_in_template_anydepth<TYPE, ARGs>::value)>
  {
  };

  /////////////////////////////////////////////////////////////////////////////
  template <
      typename TYPE,
      typename TMPL>
  struct is_in_template : std::false_type
  {
  };

  /////////////////////////////////////////////////////////////////////////////
  template <
      typename TYPE,
      template <typename...> typename TMPL,
      typename... ARGs>
  struct is_in_template<TYPE, TMPL<ARGs...>>
      : std::bool_constant<(... || std::is_same_v<TYPE, ARGs>)>
  {
  };

  /////////////////////////////////////////////////////////////////////////////
  template <typename TYPE, typename TEMPL>
  inline constexpr bool is_in_template_anydepth_v = is_in_template<TYPE, TEMPL>::value;

  /////////////////////////////////////////////////////////////////////////////
  template <typename TYPE, typename TEMPL>
  inline constexpr bool is_in_template_v = is_in_template<TYPE, TEMPL>::value;

  /////////////////////////////////////////////////////////////////////////////
  template <typename T, typename TUPLE>
  constexpr bool is_in_tuple_v = []<std::size_t... I>(std::index_sequence<I...>)
  {
    return (... || std::is_same_v<T, std::tuple_element_t<I, TUPLE>>);
  }(std::make_index_sequence<std::tuple_size_v<TUPLE>>{});

  /////////////////////////////////////////////////////////////////////////////
  template <typename T, typename VARIANT>
  constexpr bool is_in_variant_v = []<std::size_t... I>(std::index_sequence<I...>)
  {
    return (... || std::is_same_v<T, std::variant_alternative_t<I, VARIANT>>);
  }(std::make_index_sequence<std::variant_size_v<VARIANT>>{});
} // namespace peeb
