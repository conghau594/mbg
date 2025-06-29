// "PackTransform.h"
#pragma once

#include <functional>

namespace ppt // abbr of `Parameter Pack Transformer`
{
  /////////////////////////////////////////////////////////////////////////////
  template <typename RET, typename T>
  using AsFunction = RET(T);

  /////////////////////////////////////////////////////////////////////////////
  template <typename... Ts>
  struct Pack
  {
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