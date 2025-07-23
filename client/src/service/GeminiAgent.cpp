// GeminiAgent.cpp

#include <map>
#include <sstream>

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/json.hpp>

#include "GeminiAgent.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
namespace json = boost::json;
using tcp = net::ip::tcp;

namespace bgg
{
  /**
   * TODO: need refactor this function using std::error_code
   */
  auto GeminiAgent::sendPrompt(std::string_view prompt) const noexcept -> std::string
  {
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
      // {
      //   std::ostringstream oss;
      //   oss << jsonRequest;
      //   SPDLOG_DEBUG("json body string: {}", oss.str());
      // }
      //=======================================================================

      // Prepare HTTP request
      http::request<http::string_body> request{
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
    }

    SPDLOG_DEBUG("Response text from the agent: {}", responseText);
    return responseText;
  }
} // namespace bgg
