// ZOrder.h
#pragma once

#include "base/EnumUtils.h"
namespace bgg
{
  // Note that the 0th layer is reserved for tiles of the board

#define Z_ORDER_ENTRIES(E) \
  E(ZEROTH_LAYER)          \
  E(FIRST_LAYER)           \
  E(SECOND_LAYER)          \
  E(THIRD_LAYER)           \
  E(FOURTH_LAYER)          \
  E(FIFTH_LAYER)           \
  E(SIXTH_LAYER)           \
  E(SEVENTH_LAYER)         \
  E(EIGHTH_LAYER)          \
  E(NINETH_LAYER)          \
  E(TENTH_LAYER)

#define Z_ORDER_NAMES(E) \
  E(ZerothLayer)         \
  E(FirstLayer)          \
  E(SecondLayer)         \
  E(ThirdLayer)          \
  E(FourthLayer)         \
  E(FifthLayer)          \
  E(SixthLayer)          \
  E(SeventhLayer)        \
  E(EighthLayer)         \
  E(NinethLayer)         \
  E(TenthLayer)

  DEFINE_ENUM(ZOrder, Z_ORDER_ENTRIES, Z_ORDER_NAMES);
} // namespace bgg
