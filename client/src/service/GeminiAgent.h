// GeminiAgent.h
#pragma once

#include <format>

namespace bgg
{
  class GeminiAgent
  {
    std::string prompt_;

  public:
    GeminiAgent(std::string prompt_)
        : prompt_(std::move(prompt_))
    {
    }

    // template <typename... ARGS>
    // void sendPrompt(ARGS &&...args) const noexcept
    // {
    //   std::string formattedPrompt = std::format(
    //       std::string_view(prompt_),
    //       std::forward<decltype(args)>(args)...);
    // }

    // private:
    [[nodiscard]]
    static auto sendPrompt(std::string_view prompt) -> std::string;
  };
} // namespace bgg
