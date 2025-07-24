// ChessUtils.h
#pragma once

#include <regex>
#include <boost/json.hpp>
#include "ChessRule.h"

namespace utils
{
  // namespace
  // {
  //   inline auto extractBacktickStrings(std::string const &input) noexcept
  //       -> std::list<std::string>
  //   {
  //     std::regex re("`([^`]*)`");

  //     auto words_begin = std::sregex_iterator(input.begin(), input.end(), re);
  //     auto words_end = std::sregex_iterator();

  //     std::list<std::string> resultList;
  //     for (std::sregex_iterator i = words_begin; i != words_end; ++i)
  //     {
  //       std::smatch match = *i;
  //       resultList.emplace_back(match[1].str()); // group 1 inside backticks
  //     }
  //     return resultList;
  //   }
  // }

  inline auto isValid(bgg::Position const &square) noexcept -> bool
  {
    bool validCol = (square[0] >= 'a' && square[0] <= 'h') ||
                    (square[0] >= 'A' && square[0] <= 'H');
    bool validRow = (square[1] >= '1' && square[1] <= '8');
    return validCol && validRow;
  }

  inline auto isValidPieceType(std::string const &pieceType) noexcept -> bool
  {
    return (pieceType == bgg::ChessRule::KING) ||
           (pieceType == bgg::ChessRule::QUEEN) ||
           (pieceType == bgg::ChessRule::ROOK) ||
           (pieceType == bgg::ChessRule::BISHOP) ||
           (pieceType == bgg::ChessRule::KNIGHT) ||
           (pieceType == bgg::ChessRule::PAWN);
  }

  inline auto isValidPieceColor(std::string const &pieceColor) noexcept -> bool
  {
    return (pieceColor == bgg::Color::WHITE) ||
           (pieceColor == bgg::Color::BLACK);
  }

  inline auto isValid(bgg::Piece const &piece) noexcept -> bool
  {
    return isValidPieceType(piece.type) &&
           isValidPieceColor(piece.color);
  }

  inline auto pieceStringToChar(std::string const &pieceString) -> std::optional<char>
  {
    if (pieceString == bgg::ChessRule::QUEEN)
    {
      return 'q';
    }
    else if (pieceString == bgg::ChessRule::ROOK)
    {
      return 'r';
    }
    else if (pieceString == bgg::ChessRule::BISHOP)
    {
      return 'b';
    }
    else if (pieceString == bgg::ChessRule::KNIGHT)
    {
      return 'n';
    }
    else if (pieceString == bgg::ChessRule::KING)
    {
      return 'k';
    }
    else if (pieceString == bgg::ChessRule::PAWN)
    {
      return 'p';
    }
    else
    {
      throw std::invalid_argument("Invalid piece: " + pieceString);
    }
  }

  inline auto charToPieceString(char pieceChar) -> std::optional<std::string>
  {
    switch (pieceChar)
    {
    case 'q':
    case 'Q':
      return bgg::ChessRule::QUEEN;
    case 'r':
    case 'R':
      return bgg::ChessRule::ROOK;
    case 'b':
    case 'B':
      return bgg::ChessRule::BISHOP;
    case 'n':
    case 'N':
      return bgg::ChessRule::KNIGHT;
    case 'k':
    case 'K':
      return bgg::ChessRule::KING;
    case 'p':
    case 'P':
      return bgg::ChessRule::PAWN;
    default:
      throw std::invalid_argument("Invalid piece char: " + pieceChar);
    }
  }

  inline void validateChessMove(bgg::ChessMove const &move)
  {
    if (!isValid(move.fromSquare))
    {
      throw std::invalid_argument("Invalid 'fromSquare': " + std::string(move.fromSquare.data()));
    }

    if (!isValid(move.toSquare))
    {
      throw std::invalid_argument("Invalid 'toSquare': " + std::string(move.toSquare.data()));
    }

    if (move.promote)
    {
      if (!isValidPieceType(*move.promote))
      {
        throw std::invalid_argument("Invalid promoted piece type: " + *move.promote);
      }
    }
  }

  inline auto chessMoveObjectToUic(bgg::ChessMove const &move) -> std::string
  {
    validateChessMove(move);
    std::optional<char> promoteChar(std::nullopt);
    if (move.promote)
    {
      promoteChar = pieceStringToChar(*move.promote);
    }

    if (promoteChar)
    {
      return std::string(move.fromSquare.data()) +
             std::string(move.toSquare.data()) +
             *promoteChar;
    }
    else
    {
      return std::string(move.fromSquare.data()) +
             std::string(move.toSquare.data());
    }
  }

  inline auto uicToChessMoveObject(std::string const &uci) -> bgg::ChessMove
  {
    if (uci.length() != 4 && uci.length() != 5)
    {
      throw std::invalid_argument("Invalid UCI move length: " + uci);
    }

    bgg::ChessMove move;

    move.fromSquare = {uci[0], uci[1], '\0'};
    move.toSquare = {uci[2], uci[3], '\0'};
    if (uci.length() == 5)
    {
      move.promote = charToPieceString(uci[4]);
    }

    validateChessMove(move);

    return move;
  }

  inline auto chessMoveObjectToJson(bgg::ChessMove const &move) -> boost::json::object
  {
    validateChessMove(move);

    // Create a JSON object from the ChessMove object
    boost::json::object jsonObj;
    jsonObj["fromSquare"] = std::string(move.fromSquare.data());
    jsonObj["toSquare"] = std::string(move.toSquare.data());
    if (move.promote)
    {
      jsonObj["promote"] = *move.promote;
    }
    return jsonObj;
  }

  inline auto jsonToChessMoveObject(boost::json::object const &jsonObj) -> bgg::ChessMove
  {
    bgg::ChessMove move;

    if (jsonObj.contains("fromSquare"))
    {
      std::string fromSquare{jsonObj.at("fromSquare").as_string().c_str()};
      if (fromSquare.length() < 2)
      {
        throw std::invalid_argument("Invalid 'fromSquare' length: " + fromSquare);
      }
      move.fromSquare = {fromSquare[0], fromSquare[1], '\0'};
    }
    else
    {
      throw std::invalid_argument("Missing 'fromSquare' in JSON");
    }

    if (jsonObj.contains("toSquare"))
    {
      std::string toSquare{jsonObj.at("toSquare").as_string().c_str()};
      if (toSquare.length() < 2)
      {
        throw std::invalid_argument("Invalid 'toSquare' length: " + toSquare);
      }
      move.toSquare = {toSquare[0], toSquare[1], '\0'};
    }
    else
    {
      throw std::invalid_argument("Missing 'toSquare' in JSON");
    }

    if (jsonObj.contains("promote"))
    {
      boost::json::value promoteValue = jsonObj.at("promote");
      
      if (!promoteValue.is_null())
      {
        move.promote = std::string{jsonObj.at("promote").as_string().c_str()};
      }
    }
    
    validateChessMove(move);

    return move;
  }

  inline auto jsonToChessMoveObject(std::string const &jsonStr) -> bgg::ChessMove
  {
    auto jsonObj = boost::json::parse(jsonStr).as_object();

    return jsonToChessMoveObject(jsonObj);
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
        bgg::ChessMove chessMoveObj = utils::uicToChessMoveObject(uicMove);
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
