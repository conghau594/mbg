// GeminiAgent.h
#pragma once

#include <format>
#include <string_view>

namespace bgg
{
  class GeminiAgent
  {
    std::string apiKey_;
    std::string promptPattern_;

  public:
    GeminiAgent(std::string apiKey, std::string promptPattern) noexcept
        : apiKey_(std::move(apiKey)),
          promptPattern_(std::move(promptPattern))
    {
    }

    template <typename... ARGS>
    [[nodiscard]] auto sendPromptWithArgs(ARGS &&...args) noexcept -> std::string
    {
      std::string formattedPrompt = std::vformat(
          std::string_view(promptPattern_),
          std::make_format_args(std::forward<decltype(args)>(args)...));

      return sendPrompt(formattedPrompt);
    }

  private:
    [[nodiscard]] auto sendPrompt(
        std::string_view prompt) const noexcept -> std::string;
  };
} // namespace bgg
