// ChessUtils.h
#pragma once

#include <regex>
#include "model/chess/ChessRule.h"

namespace utils
{
  namespace
  {
    inline auto extractBacktickStrings(std::string const &input) noexcept
        -> std::list<std::string>
    {
      std::regex re("`([^`]*)`");

      auto words_begin = std::sregex_iterator(input.begin(), input.end(), re);
      auto words_end = std::sregex_iterator();

      std::list<std::string> resultList;
      for (std::sregex_iterator i = words_begin; i != words_end; ++i)
      {
        std::smatch match = *i;
        resultList.emplace_back(match[1].str()); // group 1 inside backticks
      }
      return resultList;
    }
  }

  /**
   * \return the pair of UIC formatted move and the corresponding ChessMove
   *         object of the first valid move in the `input`
   */
  inline auto extractChessMove(std::string const &input) noexcept
      -> std::optional<std::pair<std::string, bgg::ChessMove>>
  {
    std::list<std::string> uicMoves = extractBacktickStrings(input);
    for (auto &uicMove : uicMoves)
    {
      try
      {
        bgg::ChessMove chessMoveObj = bgg::ChessRule::fromUic(uicMove);
        return std::pair<std::string, bgg::ChessMove>{uicMove, chessMoveObj};
      }
      catch (std::exception const &e)
      {
        SPDLOG_DEBUG(
            "Cannot convert '{}' to a ChessMove object: {}",
            uicMove,
            e.what());
      }
    }
    return std::nullopt;
  }

} // namespace utils
