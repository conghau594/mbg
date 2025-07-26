// JsonUtils.h
#pragma once

#include <boost/json.hpp>
#include <iomanip>
#include <iostream>
#include <string>
#include <boost/json.hpp>

// This file must be manually included when
// using basic_parser to implement a parser.
#include <boost/json/basic_parser_impl.hpp>

namespace utils
{
  /*
    This example parses a JSON file and pretty-prints
    it to standard output.
*/

  namespace json = boost::json;

  // json::value parseFile(char const *filename)
  // {
  //   file f(filename, "r");
  //   json::stream_parser p;
  //   json::error_code ec;
  //   do
  //   {
  //     char buf[4096];
  //     auto const nread = f.read(buf, sizeof(buf));
  //     p.write(buf, nread, ec);
  //   } while (!f.eof());
  //   if (ec)
  //     return nullptr;
  //   p.finish(ec);
  //   if (ec)
  //     return nullptr;
  //   return p.release();
  // }

  inline void printPrettyJson(
      std::ostream &os,
      json::value const &jv,
      std::string indent = "") noexcept
  {
    int constexpr TAB_SIZE = 2;
    switch (jv.kind())
    {
    case json::kind::object:
    {
      os << "{\n";
      indent.append(TAB_SIZE, ' ');
      auto const &obj = jv.get_object();
      if (!obj.empty())
      {
        auto it = obj.begin();
        for (;;)
        {
          os << indent << json::serialize(it->key()) << " : ";
          printPrettyJson(os, it->value(), indent);
          if (++it == obj.end())
            break;
          os << ",\n";
        }
      }
      os << "\n";
      indent.resize(indent.size() - TAB_SIZE);
      os << indent << "}";
      break;
    }

    case json::kind::array:
    {
      os << "[\n";
      indent.append(TAB_SIZE, ' ');
      auto const &arr = jv.get_array();
      if (!arr.empty())
      {
        auto it = arr.begin();
        for (;;)
        {
          os << indent;
          printPrettyJson(os, *it, indent);
          if (++it == arr.end())
            break;
          os << ",\n";
        }
      }
      os << "\n";
      indent.resize(indent.size() - TAB_SIZE);
      os << indent << "]";
      break;
    }

    case json::kind::string:
    {
      os << json::serialize(jv.get_string());
      break;
    }

    case json::kind::uint64:
      os << jv.get_uint64();
      break;

    case json::kind::int64:
      os << jv.get_int64();
      break;

    case json::kind::double_:
      os << jv.get_double();
      break;

    case json::kind::bool_:
      if (jv.get_bool())
        os << "true";
      else
        os << "false";
      break;

    case json::kind::null:
      os << "null";
      break;
    }

    if (indent.empty())
      os << "\n";
  }

  // The null parser discards all the data
  // class null_parser
  // {
  //   struct handler
  //   {
  //     constexpr static std::size_t max_object_size = std::size_t(-1);
  //     constexpr static std::size_t max_array_size = std::size_t(-1);
  //     constexpr static std::size_t max_key_size = std::size_t(-1);
  //     constexpr static std::size_t max_string_size = std::size_t(-1);

  //     bool on_document_begin(error_code &) { return true; }
  //     bool on_document_end(error_code &) { return true; }
  //     bool on_object_begin(error_code &) { return true; }
  //     bool on_object_end(std::size_t, error_code &) { return true; }
  //     bool on_array_begin(error_code &) { return true; }
  //     bool on_array_end(std::size_t, error_code &) { return true; }
  //     bool on_key_part(string_view, std::size_t, error_code &) { return true; }
  //     bool on_key(string_view, std::size_t, error_code &) { return true; }
  //     bool on_string_part(string_view, std::size_t, error_code &) { return true; }
  //     bool on_string(string_view, std::size_t, error_code &) { return true; }
  //     bool on_number_part(string_view, error_code &) { return true; }
  //     bool on_int64(std::int64_t, string_view, error_code &) { return true; }
  //     bool on_uint64(std::uint64_t, string_view, error_code &) { return true; }
  //     bool on_double(double, string_view, error_code &) { return true; }
  //     bool on_bool(bool, error_code &) { return true; }
  //     bool on_null(error_code &) { return true; }
  //     bool on_comment_part(string_view, error_code &) { return true; }
  //     bool on_comment(string_view, error_code &) { return true; }
  //   };

  //   basic_parser<handler> p_;

  // public:
  //   null_parser()
  //       : p_(parse_options())
  //   {
  //   }

  //   ~null_parser()
  //   {
  //   }

  //   std::size_t
  //   write(
  //       char const *data,
  //       std::size_t size,
  //       error_code &ec)
  //   {
  //     auto const n = p_.write_some(false, data, size, ec);
  //     if (!ec && n < size)
  //       ec = error::extra_data;
  //     return n;
  //   }
  // };

  // auto validate(string_view s) -> bool
  // {
  //   // Parse with the null parser and return false on error
  //   null_parser p;
  //   error_code ec;
  //   p.write(s.data(), s.size(), ec);
  //   if (ec)
  //     return false;

  //   // The string is valid JSON.
  //   return true;
  // }

} // namespace utils
