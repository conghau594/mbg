// RandomUtils.h
#pragma once
#include <random>

namespace utils
{
  static std::random_device rd;
  static std::mt19937 engine(rd());

  template <typename T = int>
  [[nodiscard]] inline T randomInt(T a, T b)
  {
    std::uniform_int_distribution<T> dist(a, b);
    return dist(engine);
  }

  template <typename T = float>
  [[nodiscard]] inline T randomFloat(T a, T b)
  {
    std::uniform_real_distribution<T> dist(a, b);
    return dist(engine);
  }

} // namespace utils
