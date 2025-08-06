// GeminiAgent.cpp

#include <map>
#include <sstream>
#include <chrono>

#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/json.hpp>

#include "GeminiAgent.h"
#include "base/JsonUtils.h"

namespace bgg
{
  namespace json = boost::json;
  GeminiAgent::GeminiAgent(
      std::string apiKey,
      std::string systemInstruction,
      std::string responseSchema,
      std::string promptPattern) noexcept
      : apiKey_(std::move(apiKey)),
        systemInstruction_(std::move(systemInstruction)),
        promptPattern_(std::move(promptPattern)),
        responseSchema_{},
        ioContext_(),
        sslContext_{ssl::context::tlsv12_client},
        sslStream_{nullptr}
  {
    boost::json::value jsonValue = boost::json::parse(responseSchema);
    BOOST_ASSERT_MSG(
        jsonValue.is_object(),
        "GeminiAgent: responseSchema must be a valid JSON object");

    responseSchema_ = std::move(jsonValue.as_object());

    sslContext_.set_default_verify_paths();
  }

  GeminiAgent::~GeminiAgent()
  {
    closeSslStream();
  }

  /**
   * TODO: need refactor this function using std::error_code
   */
  auto GeminiAgent::sendPrompt(std::string_view prompt) noexcept
      -> std::optional<std::string>
  {
    http::response<http::dynamic_body> response;
    try
    {
      openSslStream();

      // Send request
      http::request<http::string_body> request = createHtmlRequest(prompt);
      http::write(*sslStream_, request);

      // Read response
      beast::flat_buffer buffer;

      //=======================================================================
      // measure time to read response:
      auto start = std::chrono::steady_clock::now();
      http::read(*sslStream_, buffer, response);
      auto duration = std::chrono::steady_clock::now() - start;
      SPDLOG_INFO(
          "Time to read response: {}s",
          std::chrono::duration<double>(duration).count());
      //=======================================================================
    }
    catch (const beast::system_error &e)
    {
      SPDLOG_ERROR("Connection error: {} (code: {})", e.what(), e.code().value());
      closeSslStream();
      return std::nullopt;
    }

    json::value jsonResponse;
    std::optional<std::string> responseText{std::nullopt};
    try
    {
      // Parse JSON response and extract "text" from "parts" in "content"
      std::string responseBody = beast::buffers_to_string(response.body().data());
      jsonResponse = json::parse(responseBody);

      auto jsonObj = jsonResponse.as_object();
      printTokenUsage(jsonObj);

      auto candidates = jsonObj.at("candidates").as_array();
      auto content = candidates.at(0).as_object().at("content").as_object();
      auto parts = content.at("parts").as_array();
      responseText = parts.at(0).as_object()["text"].as_string().c_str();
    }
    catch (const std::exception &e)
    {
      SPDLOG_ERROR("JSON parsing error: {}", e.what());
      // LOG_DEBUG jsonResponse
      std::ostringstream oss;
      utils::printPrettyJson(oss, jsonResponse);
      SPDLOG_DEBUG("Json response on error: {}", oss.str());
    }

    if (responseText)
    {
      SPDLOG_DEBUG("Gemini response: {}", *responseText);
    }

    return responseText;
  }

  void GeminiAgent::openSslStream()
  {
    if (sslStream_)
    {
      return;
    }
    sslStream_ = std::make_unique<beast::ssl_stream<beast::tcp_stream>>(
        ioContext_, sslContext_);

    // prepare connection objects
    int constexpr TIME_OUT_IN_SEC = 60;
    sslStream_->next_layer().expires_after(std::chrono::seconds(TIME_OUT_IN_SEC));
    // if (SSL_ctrl(sslStream_.native_handle(),
    //              SSL_CTRL_SET_TLSEXT_HOSTNAME,
    //              TLSEXT_NAMETYPE_host_name,
    //              const_cast<char *>(host.c_str())))
    if (!SSL_set_tlsext_host_name(sslStream_->native_handle(), host_.c_str()))
    {
      beast::error_code errcode(
          int(::ERR_get_error()), net::error::get_ssl_category());
      throw boost::system::system_error(errcode);
    }

    // resolve and connect to the host
    tcp::resolver resolver(ioContext_);
    auto const results = resolver.resolve(host_, port_);
    beast::get_lowest_layer(*sslStream_).connect(results);
    sslStream_->handshake(ssl::stream_base::client);
  }

  void GeminiAgent::closeSslStream() noexcept
  {
    if (!sslStream_)
    {
      return; // No stream to close
    }

    try
    {
      beast::error_code ec;

      sslStream_->next_layer().cancel();
      sslStream_->shutdown(ec);
      sslStream_->next_layer().close();

      if (ec == net::error::eof || ec == ssl::error::stream_truncated)
      {
        ec.assign(0, ec.category());
      }

      if (ec)
      {
        throw beast::system_error{ec};
      }
    }
    catch (const std::exception &e)
    {
      SPDLOG_WARN("Failed to close SSL stream: {}", e.what());
    }

    sslStream_ = nullptr; // Release the stream
  }

  auto GeminiAgent::createHtmlRequest(std::string_view prompt)
      -> http::request<http::string_body>
  {
    ///< Build correct JSON request body according to Gemini API spec
    ///< See QuickNotes.md to understand the structure of the request

    ///< "contents":
    json::object userPart;
    userPart["text"] = prompt;

    json::object userContent;
    userContent["role"] = "user";
    userContent["parts"] = json::array{userPart};

    ///< "system_instruction":
    json::object systemPart;
    systemPart["text"] = systemInstruction_;
    json::object systemInstruction;
    systemInstruction["parts"] = json::array{systemPart};

    ///< "generation_config"
    json::object generationConfig;
    generationConfig["temperature"] = 0.0;
    generationConfig["responseMimeType"] = "application/json";
    generationConfig["responseSchema"] = responseSchema_;

    json::object thinkingConfig;
    thinkingConfig["thinkingBudget"] = 8192;
    generationConfig["thinkingConfig"] = thinkingConfig;

    ///< json request
    // json::array conversationHistory;
    // conversationHistory.push_back(userContent);

    json::object jsonRequest;
    jsonRequest["contents"] = json::array{userContent};
    jsonRequest["systemInstruction"] = systemInstruction;
    jsonRequest["generationConfig"] = generationConfig;

    // //=======================================================================
    // // LOG_DEBUG jsonRequest
    // {
    //   std::ostringstream oss;
    //   utils::printPrettyJson(oss, jsonRequest);
    //   SPDLOG_DEBUG("jsonRequest: {}", oss.str());
    // }
    // //=======================================================================

    // Prepare HTTP request
    http::request<http::string_body>
        request{http::verb::post, target_ + "?key=" + apiKey_, 11};
    request.set(http::field::host, host_);
    // request.set("x-api-key", api_key);
    request.set(http::field::content_type, "application/json");
    request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    // request.content_length(body_str.size());
    request.body() = json::serialize(jsonRequest);
    request.prepare_payload(); // auto-set content-length

    return request;
  }

  void GeminiAgent::printTokenUsage(boost::json::object const &jsonObj) noexcept
  {
    if (!jsonObj.contains("usageMetadata"))
    {
      SPDLOG_WARN("No usage metadata found in the response");
      return;
    }
    // if (!jsonObj.at("usageMetadata").is_object())
    // {
    //   SPDLOG_WARN("Usage metadata is not an object");
    //   return;
    // }

    std::ostringstream oss;
    utils::printPrettyJson(oss, jsonObj.at("usageMetadata"));

    SPDLOG_INFO("Usage metadata: {}", oss.str());
  }

} // namespace bgg
