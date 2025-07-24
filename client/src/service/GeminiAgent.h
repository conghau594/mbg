// GeminiAgent.h
#pragma once

#include <format>
#include <string_view>
#include <boost/json.hpp>

#include "base/Logger.h"

namespace bgg
{
  class GeminiAgent
  {
    std::string apiKey_;
    std::string systemInstruction_;
    std::string promptPattern_;
    boost::json::object responseSchema_;

  public:
    GeminiAgent(
        std::string apiKey,
        std::string systemInstruction,
        std::string responseSchema,
        std::string promptPattern) noexcept;

    template <typename... ARGS>
    [[nodiscard]] auto sendPromptWithArgs(ARGS &&...args)
        -> std::optional<std::string>
    {
      std::string formattedPrompt = std::vformat(
          std::string_view(promptPattern_),
          std::make_format_args(std::forward<decltype(args)>(args)...));

      // SPDLOG_DEBUG(formattedPrompt);
      return sendPrompt(formattedPrompt);
    }

  private:
    [[nodiscard]] auto sendPrompt(std::string_view prompt) const noexcept
        -> std::optional<std::string>;
  
    static void printTokenUsage(boost::json::object const &jsonObj);
      };
} // namespace bgg
