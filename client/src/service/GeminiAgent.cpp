// GeminiAgent.cpp

#include <map>
#include <fstream>
#include <sstream>

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/json.hpp>

#include "GeminiAgent.h"
#include "base/Logger.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
namespace json = boost::json;
using tcp = net::ip::tcp;

namespace bgg
{

  static auto parseEnvFile(const std::string &filename)
      -> std::map<std::string, std::string>
  {
    std::map<std::string, std::string> env;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line))
    {
      // Skip comments and empty lines
      if (line.empty() || line[0] == '#')
        continue;

      std::istringstream iss(line);
      std::string key, value;

      if (std::getline(iss, key, '=') && std::getline(iss, value))
      {
        // Trim whitespace (optional)
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        env[key] = value;
      }
    }
    return env;
  }

  /**
   * TODO: need refactor this function using std::error_code
   */
  auto GeminiAgent::sendPrompt(std::string_view prompt) -> std::string
  {
    std::map<std::string, std::string> envMap = parseEnvFile("E:/src/.env");

    auto envIter = envMap.find("GEMINI_API_KEY");

    if (envIter == envMap.end())
    {
      SPDLOG_ERROR("GEMINI_API_KEY not found in the .env file!");
      return ""; // empty string
    }

    const std::string api_key = envIter->second; // Replace with your actual Gemini API key
    const std::string host = "generativelanguage.googleapis.com";
    const std::string port = "443";
    const std::string target = "/v1beta/models/gemini-2.5-flash-preview-05-20:generateContent";
    // const std::string target = "/v1beta/models/gemini-2.0-flash:generateContent";
    // const std::string target = "/v1beta/models/gemini-2.5-pro-preview-06-05:generateContent";
    // const std::string target = "/v1beta/models/gemini-1.5-pro:generateContent";

    std::string responseText;
    try
    {
      // prepare connection objects
      net::io_context ioc;
      ssl::context context(ssl::context::tlsv12_client);
      context.set_default_verify_paths();

      beast::ssl_stream<beast::tcp_stream> stream(ioc, context);

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

      // Build correct JSON request body according to Gemini API spec
      json::object userPart;
      userPart["text"] = prompt;

      json::object userContent;
      userContent["role"] = "user";
      userContent["parts"] = json::array{userPart};

      // json::array conversationHistory;
      // conversationHistory.push_back(userContent);

      json::object jsonRequest;
      jsonRequest["contents"] = json::array{userContent};

      // SPDLOG_DEBUG("json body: {}", body);

      //=======================================================================
      // LOG_DEBUG jsonRequest
      {
        std::ostringstream oss;
        oss << jsonRequest;
        SPDLOG_DEBUG("json body string: {}", oss.str());
      }
      //=======================================================================

      // Prepare HTTP request
      http::request<http::string_body> request{
          http::verb::post, target + "?key=" + api_key, 11};
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
      http::read(stream, buffer, response);

      // Parse JSON response and extract "text" from "parts" in "content"
      std::string responseBody = beast::buffers_to_string(response.body().data());
      json::value jsonResponse = json::parse(responseBody);

      //=======================================================================
      // LOG_DEBUG jsonResponse
      {
        std::ostringstream oss;
        oss << jsonResponse;
        SPDLOG_DEBUG("jsonResponse: {}", oss.str());
      }
      //=======================================================================

      if (jsonResponse.is_object())
      {
        auto jsonObj = jsonResponse.as_object();
        if (jsonObj.contains("candidates"))
        {
          auto candidates = jsonObj["candidates"].as_array();
          if (!candidates.empty())
          {
            auto content = candidates[0].as_object()["content"].as_object();
            auto parts = content["parts"].as_array();
            if (!parts.empty())
            {
              responseText = parts[0].as_object()["text"].as_string().c_str();
              SPDLOG_DEBUG("Responsed text from the agent: {}", responseText);
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
      if (ec == net::error::eof ||
          ec == boost::asio::ssl::error::stream_truncated)
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
    }
    return responseText;
  }
} // namespace bgg
