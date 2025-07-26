// GeminiAgent.h
#pragma once

#include <memory>
#include <format>
#include <string_view>

#include <boost/json.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include "base/Logger.h"

namespace bgg
{
  namespace net = boost::asio;
  namespace beast = boost::beast;
  namespace ssl = boost::asio::ssl;
  namespace http = beast::http;
  using tcp = net::ip::tcp;

  class GeminiAgent
  {
    std::string apiKey_;
    std::string systemInstruction_;
    std::string promptPattern_;
    boost::json::object responseSchema_;

    net::io_context ioContext_;
    ssl::context sslContext_;
    std::unique_ptr<beast::ssl_stream<beast::tcp_stream>> sslStream_;

    const std::string host_ = "generativelanguage.googleapis.com";
    const std::string port_ = "443";

    // TODO: make this configurable
    const std::string target_ = "/v1beta/models/gemini-2.5-flash-preview-05-20:generateContent";
    // const std::string target = "/v1beta/models/gemini-2.0-flash:generateContent";
    // const std::string target = "/v1beta/models/gemini-2.5-pro-preview-06-05:generateContent";
    // const std::string target = "/v1beta/models/gemini-1.5-pro:generateContent";

  public:
    GeminiAgent(
        std::string apiKey,
        std::string systemInstruction,
        std::string responseSchema,
        std::string promptPattern) noexcept;

    ~GeminiAgent();

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
    [[nodiscard]] auto sendPrompt(std::string_view prompt) noexcept
        -> std::optional<std::string>;

    void openSslStream();
    void closeSslStream() noexcept;
    auto createHtmlRequest(std::string_view prompt) -> http::request<http::string_body>;

    static void
    printTokenUsage(boost::json::object const &jsonObj) noexcept;
  };
} // namespace bgg
