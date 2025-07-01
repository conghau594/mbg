// PackTransform.hpp
#pragma once

#include <functional>

namespace ppt // abbr of `Parameter Pack Transformer`
{
  /////////////////////////////////////////////////////////////////////////////
  template <std::size_t I, typename... Ts>
  struct At;

  template <typename First, typename... Rest>
  struct At<0, First, Rest...>
  {
    using Type = First;
  };

  template <std::size_t I, typename First, typename... Rest>
  struct At<I, First, Rest...>
  {
    using Type = typename At<I - 1, Rest...>::Type;
  };
  /////////////////////////////////////////////////////////////////////////////
  template <typename RET, typename T>
  using AsFunction = RET(T);

  /////////////////////////////////////////////////////////////////////////////
  template <typename... Ts>
  struct Pack
  {
    // Unpack -> dung cho Pack 1 phan tu
    // IsInThis<T> -> bool: check 1 phan tu co nam trong pack ko
    // IsInThisAnyDepth<T>
    //  ToPointer
    //  Clear -> chuyen ve Pack<>
    //  PopBack, PopFront
    //  Remove<T>: -> remove all type T
    static constexpr std::size_t Count = sizeof...(Ts);
    template <std::size_t I>
    using At = typename At<I, Ts...>::Type;

    using ToRef = Pack<Ts &...>;
    using ToConst = Pack<const Ts...>;
    using ToConstRef = Pack<Ts const &...>;

    template <typename RET>
    using ToSignature = Pack<AsFunction<RET, Ts>...>;

    template <typename RET>
    using ToFunction = Pack<std::function<AsFunction<RET, Ts>>...>;

  private:
    // bo cai helper nay ra ngoai.
    template <typename V>
    struct PrependHelper
    {
      using Pack = Pack<V, Ts...>;
    };

    // template<typename V, typename... Vs>
    // struct PrependHelper<typename... Vs> {
    //   using Pack = Pack<V, Ts...>::PrependHelper<Vs...>
    // };

  public:
    template <typename V /*, typename... Vs*/>
    using Prepend = PrependHelper<V>::Pack;

    template <template <typename> typename WRAPPER>
    using WrapEachIn = Pack<WRAPPER<Ts>...>;

    template <template <typename...> typename ENCLOSER>
    using EncloseBy = ENCLOSER<Ts...>;
  };
}