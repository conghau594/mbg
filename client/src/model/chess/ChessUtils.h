// ChessUtils.h
#pragma once

#include <regex>
#include "ChessRule.h"

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

  inline auto toUic(ChessMove const &move) -> std::string
  {
    std::string result = std::string(move.fromSquare.data()) +
                         std::string(move.toSquare.data()) +
                         ' ';
    if (move.promote)
    {
      if (*(move.promote) == QUEEN)
      {
        result.back() = 'q';
      }
      else if (*(move.promote) == ROOK)
      {
        result.back() = 'r';
      }
      else if (*(move.promote) == BISHOP)
      {
        result.back() = 'b';
      }
      else if (*(move.promote) == KNIGHT)
      {
        result.back() = 'n';
      }
      else
      {
        throw std::invalid_argument("Invalid chess move");
      }
    }
    else
    {
      int constexpr MIN_CHARS = 4;
      result.resize(MIN_CHARS);
    }

    return result;
  }

  inline auto fromUic(const std::string &uci) -> ChessMove
  {
    if (uci.length() != 4 && uci.length() != 5)
    {
      throw std::invalid_argument("Invalid UCI move length");
    }

    ChessMove move;

    move.fromSquare = {uci[0], uci[1], '\0'};
    if (!ChessRule::isValid(move.fromSquare))
    {
      throw std::invalid_argument("Invalid square");
    }

    move.toSquare = {uci[2], uci[3], '\0'};
    if (!ChessRule::isValid(move.toSquare))
    {
      throw std::invalid_argument("Invalid square");
    }

    if (uci.length() == 5)
    {
      move.promote = std::string(1, uci[4]);
      if (!ChessRule::isValidPieceType(*move.promote))
      {
        throw std::invalid_argument("Invalid promotion");
      }
    }

    return move;
  }

  /**
   * \return the pair of UIC formatted move and the corresponding ChessMove
   *         object of the first valid move in the `input`
   */
  inline auto extractChessMove(std::string const &input) noexcept
      -> std::optional<std::pair<std::string, bgg::ChessMove>>
  {
    const std::regex pattern(R"(([a-h][1-8][a-h][1-8][qrbn]?))");

    std::smatch match;
    std::string uicMove;

    if (std::regex_search(input, match, pattern))
    {
      uicMove = match.str(1);
    }

    if (!uicMove.empty())
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
