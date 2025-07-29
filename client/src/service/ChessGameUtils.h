// ChessGameUtils.h
#pragma once

#include <regex>

#include <boost/json.hpp>
#include "model/chess/ChessHelpers.h"

namespace bgg
{
  namespace chess
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

    // inline auto isValid(Piece const &piece) noexcept -> bool
    // {
    //   return isValidPieceType(piece.type) &&
    //          isValidPieceColor(piece.color);
    // }

    // inline auto pieceStringToChar(std::string const &pieceString) -> std::optional<char>
    // {
    //   if (pieceString == chess::QUEEN)
    //   {
    //     return 'q';
    //   }
    //   else if (pieceString == chess::ROOK)
    //   {
    //     return 'r';
    //   }
    //   else if (pieceString == chess::BISHOP)
    //   {
    //     return 'b';
    //   }
    //   else if (pieceString == chess::KNIGHT)
    //   {
    //     return 'n';
    //   }
    //   else if (pieceString == chess::KING)
    //   {
    //     return 'k';
    //   }
    //   else if (pieceString == chess::PAWN)
    //   {
    //     return 'p';
    //   }
    //   else
    //   {
    //     throw std::invalid_argument("Invalid piece: " + pieceString);
    //   }
    // }

    // inline auto charToPieceString(char pieceChar) -> std::optional<std::string>
    // {
    //   switch (pieceChar)
    //   {
    //   case 'q':
    //   case 'Q':
    //     return chess::QUEEN;
    //   case 'r':
    //   case 'R':
    //     return chess::ROOK;
    //   case 'b':
    //   case 'B':
    //     return chess::BISHOP;
    //   case 'n':
    //   case 'N':
    //     return chess::KNIGHT;
    //   case 'k':
    //   case 'K':
    //     return chess::KING;
    //   case 'p':
    //   case 'P':
    //     return chess::PAWN;
    //   default:
    //     throw std::invalid_argument("Invalid piece char: " + pieceChar);
    //   }
    // }

    // inline auto chessMoveObjectToUic(ChessMove const &move) -> std::string
    // {
    //   validateChessMove(move);
    //   std::optional<char> promoteChar(std::nullopt);
    //   if (move.promote)
    //   {
    //     promoteChar = pieceStringToChar(*move.promote);
    //   }

    //   if (promoteChar)
    //   {
    //     return std::string(move.fromSquare.data()) +
    //            std::string(move.toSquare.data()) +
    //            *promoteChar;
    //   }
    //   else
    //   {
    //     return std::string(move.fromSquare.data()) +
    //            std::string(move.toSquare.data());
    //   }
    // }

    // inline auto uicToChessMoveObject(std::string const &uci) -> ChessMove
    // {
    //   if (uci.length() != 4 && uci.length() != 5)
    //   {
    //     throw std::invalid_argument("Invalid UCI move length: " + uci);
    //   }

    //   ChessMove move;

    //   move.fromSquare = {uci[0], uci[1], '\0'};
    //   move.toSquare = {uci[2], uci[3], '\0'};
    //   if (uci.length() == 5)
    //   {
    //     move.promote = charToPieceString(uci[4]);
    //   }

    //   validateChessMove(move);

    //   return move;
    // }

    inline auto chessMoveToJson(ChessMove const &move) -> boost::json::object
    {
      validateChessMove(move);

      // Create a JSON object from the ChessMove object
      boost::json::object jsonObj;
      jsonObj["fromSquare"] = move.fromSquare.toString();
      jsonObj["toSquare"] = move.toSquare.toString();
      if (move.promotedPiece)
      {
        jsonObj["promotedPiece"] = move.promotedPiece->toString();
      }
      return jsonObj;
    }

    inline auto jsonToChessMove(boost::json::object const &jsonObj) -> ChessMove
    {
      ChessMove move;

      std::string fromSquare;
      try
      {
        fromSquare = std::string{jsonObj.at("fromSquare").as_string().c_str()};
        if (fromSquare.length() != 2)
        {
          throw "Invalid 'fromSquare' length";
        }
        move.fromSquare = Position{fromSquare};
      }
      catch (...)
      {
        throw std::invalid_argument("Invalid or missing 'fromSquare': " + fromSquare);
      }

      std::string toSquare;
      try
      {
        toSquare = std::string{jsonObj.at("toSquare").as_string().c_str()};
        if (toSquare.length() != 2)
        {
          throw "Invalid 'toSquare' length";
        }
        move.toSquare = Position{toSquare};
      }
      catch (...)
      {
        throw std::invalid_argument("Invalid or missing 'toSquare': " + toSquare);
      }

      if (jsonObj.contains("promotedPiece"))
      {
        boost::json::value promoteValue = jsonObj.at("promotedPiece");

        if (!promoteValue.is_null())
        {
          std::string promotedPiece;
          try
          {
            promotedPiece = std::string{promoteValue.as_string().c_str()};
            move.promotedPiece = EntityType{promotedPiece};
          }
          catch (...)
          {
            throw std::invalid_argument("Invalid 'promotedPiece': " + promotedPiece);
          }
        }
      }

      validateChessMove(move);

      return move;
    }

    inline auto jsonToChessMove(std::string const &jsonStr) -> ChessMove
    {
      boost::json::object jsonObj;
      try
      {
        jsonObj = boost::json::parse(jsonStr).as_object();
      }
      catch (...)
      {
        throw std::invalid_argument("Cannot parse JSON string: " + jsonStr);
      }

      return jsonToChessMove(jsonObj);
    }

    // /**
    //  * \return the pair of UIC formatted move and the corresponding ChessMove
    //  *         object of the first valid move in the `input`
    //  */
    // inline auto extractChessMove(std::string const &input) noexcept
    //     -> std::optional<std::pair<std::string, ChessMove>>
    // {
    //   const std::regex pattern(R"(([a-h][1-8][a-h][1-8][qrbn]?))");

    //   std::smatch match;
    //   std::string uicMove;

    //   if (std::regex_search(input, match, pattern))
    //   {
    //     uicMove = match.str(1);
    //   }

    //   if (!uicMove.empty())
    //   {
    //     try
    //     {
    //       ChessMove chessMoveObj = utils::uicToChessMoveObject(uicMove);
    //       return std::pair<std::string, ChessMove>{uicMove, chessMoveObj};
    //     }
    //     catch (std::exception const &e)
    //     {
    //       SPDLOG_DEBUG(
    //           "Cannot convert '{}' to a ChessMove object: {}",
    //           uicMove, e.what());
    //     }
    //   }
    //   return std::nullopt;
    // }

  } // namespace chess
} // namespace utils
