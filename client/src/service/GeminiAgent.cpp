// GeminiAgent.cpp

#include <map>
#include <sstream>
#include <chrono>

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/json.hpp>

#include "GeminiAgent.h"
#include "base/JsonUtils.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
namespace json = boost::json;
using tcp = net::ip::tcp;

namespace bgg
{
  GeminiAgent::GeminiAgent(
      std::string apiKey,
      std::string systemInstruction,
      std::string responseSchema,
      std::string promptPattern) noexcept
      : apiKey_(std::move(apiKey)),
        systemInstruction_(std::move(systemInstruction)),
        promptPattern_(std::move(promptPattern)),
        responseSchema_{}
  {
    boost::json::value jsonValue = boost::json::parse(responseSchema);
    BOOST_ASSERT_MSG(
        jsonValue.is_object(),
        "GeminiAgent: responseSchema must be a valid JSON object");

    responseSchema_ = std::move(jsonValue.as_object());
  }

  /**
   * TODO: need refactor this function using std::error_code
   */
  auto GeminiAgent::sendPrompt(std::string_view prompt) const noexcept
      -> std::optional<std::string>
  {
    const std::string host = "generativelanguage.googleapis.com";
    const std::string port = "443";

    // TODO: make this configurable
    const std::string target = "/v1beta/models/gemini-2.5-flash-preview-05-20:generateContent";
    // const std::string target = "/v1beta/models/gemini-2.0-flash:generateContent";
    // const std::string target = "/v1beta/models/gemini-2.5-pro-preview-06-05:generateContent";
    // const std::string target = "/v1beta/models/gemini-1.5-pro:generateContent";

    json::value jsonResponse;
    std::optional<std::string> responseText{std::nullopt};
    try
    {
      // prepare connection objects
      net::io_context ioc;
      ssl::context context(ssl::context::tlsv12_client);
      context.set_default_verify_paths();

      beast::ssl_stream<beast::tcp_stream> stream(ioc, context);
      int constexpr TIME_OUT_IN_SEC = 60;
      stream.next_layer().expires_after(std::chrono::seconds(TIME_OUT_IN_SEC));
      // if (SSL_ctrl(stream.native_handle(),
      //              SSL_CTRL_SET_TLSEXT_HOSTNAME,
      //              TLSEXT_NAMETYPE_host_name,
      //              const_cast<char *>(host.c_str())))
      if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str()))
      {
        beast::error_code errcode(
            int(::ERR_get_error()), net::error::get_ssl_category());
        throw boost::system::system_error(errcode);
      }

      // resolve and connect to the host
      tcp::resolver resolver(ioc);
      auto const results = resolver.resolve(host, port);
      beast::get_lowest_layer(stream).connect(results);
      stream.handshake(ssl::stream_base::client);

      ///< Build correct JSON request body according to Gemini API spec
      /************************************************************************
        {
          "contents": {
            "role": "ROLE",
            "parts": { "text": "TEXT" }
          },
          "system_instruction":
          {
            "parts": [
              {
                "text": "SYSTEM_INSTRUCTION"
              }
            ]
          },
          "safety_settings": {
            "category": "SAFETY_CATEGORY",
            "threshold": "THRESHOLD"
          },
          "generation_config": {
            "responseMimeType": "application/json",
            "responseSchema": "JSON_SCHEMA",
            "temperature": 0.0,
            "thinkingConfig": {
            }


            "topP": TOP_P,
            "topK": TOP_K,
            "candidateCount": 1,
            "maxOutputTokens": MAX_OUTPUT_TOKENS,
            "stopSequences": STOP_SEQUENCES
          }
        }
      ************************************************************************/
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
          request{
              http::verb::post, target + "?key=" + apiKey_, 11};
      request.set(http::field::host, host);
      // request.set("x-api-key", api_key);
      request.set(http::field::content_type, "application/json");
      request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
      // request.content_length(body_str.size());
      request.body() = json::serialize(jsonRequest);
      request.prepare_payload(); // auto-set content-length

      // Send request
      http::write(stream, request);

      // Read response
      beast::flat_buffer buffer;
      http::response<http::dynamic_body> response;

      //=======================================================================
      // measure time to read response:
      auto start = std::chrono::steady_clock::now();
      http::read(stream, buffer, response);
      auto duration = std::chrono::steady_clock::now() - start;
      SPDLOG_INFO(
          "Time to read response: {}s",
          std::chrono::duration<double>(duration).count());
      //=======================================================================

      // Parse JSON response and extract "text" from "parts" in "content"
      std::string responseBody = beast::buffers_to_string(response.body().data());
      jsonResponse = json::parse(responseBody);

      // //=======================================================================
      // // LOG_DEBUG jsonResponse
      // {
      //   std::ostringstream oss;
      //   utils::printPrettyJson(oss, jsonResponse);
      //   SPDLOG_DEBUG("Json response: {}", oss.str());
      // }
      // //=======================================================================

      if (jsonResponse.is_object())
      {
        auto jsonObj = jsonResponse.as_object();
        if (jsonObj.contains("candidates"))
        {
          try
          {
            printTokenUsage(jsonObj);
          }
          catch (const std::exception &e)
          {
            SPDLOG_ERROR("Error printing token usage: {}", e.what());
          }

          auto candidates = jsonObj["candidates"].as_array();
          if (!candidates.empty())
          {
            auto content = candidates[0].as_object()["content"].as_object();
            auto parts = content["parts"].as_array();
            if (!parts.empty())
            {
              responseText = parts[0].as_object()["text"].as_string().c_str();
              // std::string processed_text = processResponseText(text.c_str());
              // std::cout << "BOT: " << processed_text << std::endl;

              // json::object agentPart;
              // agentPart["text"] = text;

              // json::object agentContent;
              // agentContent["role"] = "model";
              // agentContent["parts"] = json::array{agentPart};

              // conversation_history.push_back(agentContent);
            }
            else
            {
              SPDLOG_ERROR("The field 'parts' from response is empty");
            }
          }
          else
          {
            SPDLOG_ERROR("The field 'candidates' from response is empty");
          }
        }
        else
        {
          if (jsonObj.contains("error"))
          {
            std::string msg = "";
            std::int64_t errCode;
            auto errorObj = jsonObj["error"].as_object();

            if (errorObj.contains("code"))
            {
              errCode = errorObj["code"].as_int64();
            }

            if (errorObj.contains("message"))
            {
              msg = errorObj["message"].as_string().c_str();
            }

            SPDLOG_ERROR("Error from server: {} (code: {})", msg, errCode);
          }
          else
          {
            SPDLOG_ERROR("Unknown error");
          }
        }
      }

      // Shutdown SSL
      beast::error_code ec;
      stream.shutdown(ec);
      if (ec == net::error::eof || ec == ssl::error::stream_truncated)
      {
        ec.assign(0, ec.category());
      }

      if (ec)
      {
        throw beast::system_error{ec};
      }
    }
    catch (const beast::system_error &e)
    {
      SPDLOG_ERROR("System error: {} (code: {})", e.what(), e.code().value());
      // LOG_DEBUG jsonResponse
      {
        std::ostringstream oss;
        utils::printPrettyJson(oss, jsonResponse);
        SPDLOG_DEBUG("Json response on error: {}", oss.str());
      }
    }

    if (responseText)
    {
      SPDLOG_DEBUG("Gemini responses: {}", *responseText);
    }
    return responseText;
  }

  void GeminiAgent::printTokenUsage(boost::json::object const &jsonObj)
  {
    auto usageMetadata = jsonObj.at("usageMetadata").as_object();

    SPDLOG_INFO(
        "Prompt tokens: {}",
        usageMetadata["promptTokenCount"].as_int64());

    SPDLOG_INFO(
        "Response tokens: {}",
        usageMetadata["candidatesTokenCount"].as_int64());

    SPDLOG_INFO(
        "Thoughts tokens: {}",
        usageMetadata["thoughtsTokenCount"].as_int64());

    SPDLOG_INFO(
        "Total tokens: {}",
        usageMetadata["totalTokenCount"].as_int64());
  }
} // namespace bgg
