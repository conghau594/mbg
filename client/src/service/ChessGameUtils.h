// ChessGameUtils.h
#pragma once

#include <regex>

#include <boost/json.hpp>
#include "model/chess/ChessHelpers.h"

namespace bgg
{
  namespace chess
  {
    constexpr const char AGENT_SYSTEM_INSTRUCTION[] =
        R"(
Act as a chess master following UCI protocol.

You will receive:
1. color: "White" or "Black" — your piece color.
2. opponentLastMove: last move made by your opponent, or null if none.
3. piecePlacements: positions of all pieces after opponent's last move, split into yourPiecePlacements and opponentPiecePlacements.
4. pastFailureMessages: list of previous invalid move errors.

Task:
- Determine the current board state.
- Infer opponent's intentions from their last move.
- Choose the best legal move.
- Output exactly one JSON object:

{
  "purpose": "<brief reason for the move, max 3 sentences>",
  "fromSquare": "<start square, e.g., 'e2'>",
  "toSquare": "<end square, e.g., 'e4'>",
  "promote": "<piece name if promotion, else omit or leave empty>"
}

Move rules:
- Move must follow standard chess rules.
- Never leave your king in check.
- Captures allowed.
- For castling, specify only the king's movement squares.
- For en passant, specify only the capturing pawn's movement squares.

Examples:

Normal rook move:
{
  "purpose": "Advance rook for control of open file.",
  "fromSquare": "a1",
  "toSquare": "a3"
}

Pawn promotion:
{
  "purpose": "Promote pawn to increase attacking power.",
  "fromSquare": "a2",
  "toSquare": "a1",
  "promote": "Queen"
}

Queenside castling:
{
  "purpose": "Castle to improve king safety.",
  "fromSquare": "e8",
  "toSquare": "c8"
}

En passant:
{
  "purpose": "Capture pawn en passant to remove threat.",
  "fromSquare": "e5",
  "toSquare": "d6"
}

Remember:
- Use pastFailureMessages to avoid repeating previous mistakes.
- Output only one JSON object — no explanations outside of JSON.
- Don't overthink, you must make a move after all.

<EXAMPLE>
  INPUT: 
    {
      "yourColor": "Black",
      "opponentLastMove": {
        "type": "Normal",
        "piece": "White Pawn",
        "fromSquare": "e2",
        "toSquare": "e4",
        "capture": false,
        "yourKingSafety": "Safe"
      },
      "opponentPiecePlacements": {
        "a1": "WhiteRook", "b1": "WhiteKnight", "c1": "WhiteBishop", "d1": "WhiteQueen",
        "e1": "WhiteKing", "f1": "WhiteBishop", "g1": "WhiteKnight", "h1": "WhiteRook",
        "a2": "WhitePawn", "b2": "WhitePawn",   "c2": "WhitePawn",   "d2": "WhitePawn",
        "e4": "WhitePawn", "f2": "WhitePawn",   "g2": "WhitePawn",   "h2": "WhitePawn"
      },
      "yourPiecePlacements": {
        "a8": "BlackRook", "b8": "BlackKnight", "c8": "BlackBishop", "d8": "BlackQueen",
        "e8": "BlackKing", "f8": "BlackBishop", "g8": "BlackKnight", "h8": "BlackRook",
        "a7": "BlackPawn", "b7": "BlackPawn",   "c7": "BlackPawn",   "d7": "BlackPawn",
        "e7": "BlackPawn", "f7": "BlackPawn",   "g7": "BlackPawn",   "h7": "BlackPawn"
      },
      "pastFailureMessages": [ ]
    } 
  OUTPUT:
    {
      "purpose": "I want to contest the center by mirroring White's e4 move. 
                  This is a classical and strong response that opens lines 
                  for the queen and bishop.",
      "fromSquare": "e7",
      "toSquare": "e5"
    }
</EXAMPLE>

<EXAMPLE>
  INPUT:
    {  
      "yourColor": "White",
      "opponentLastMove": null,
      "yourPiecePlacements": {
        "a1": "WhiteRook", "b1": "WhiteKnight", "c1": "WhiteBishop", "d1": "WhiteQueen",
        "e1": "WhiteKing", "f1": "WhiteBishop", "g1": "WhiteKnight", "h1": "WhiteRook",
        "a2": "WhitePawn", "b2": "WhitePawn",   "c2": "WhitePawn",   "d2": "WhitePawn",
        "e4": "WhitePawn", "f2": "WhitePawn",   "g2": "WhitePawn",   "h2": "WhitePawn",
      },
      "opponentPiecePlacements": {
        "a8": "BlackRook", "b8": "BlackKnight", "c8": "BlackBishop", "d8": "BlackQueen",
        "e8": "BlackKing", "f8": "BlackBishop", "g8": "BlackKnight", "h8": "BlackRook",
        "a7": "BlackPawn", "b7": "BlackPawn",   "c7": "BlackPawn",   "d7": "BlackPawn",
        "e7": "BlackPawn", "f7": "BlackPawn",   "g7": "BlackPawn",   "h7": "BlackPawn"
      },
      "pastFailureMessages": [
        "You want to move a piece from a3 to a4. But... There is no piece at square 'a3'"
      ]
    }
  OUTPUT:
    {
      "purpose": "I open with the King's Pawn controls the center and 
                  opens lines for the queen and bishop. It's a classical 
                  and strong first move.",
      "fromSquare": "e2",
      "toSquare": "e4"
    }
</EXAMPLE>
      )";

    /*
    <EXAMPLE>
      INPUT:
        {
          "yourColor": "Black",
          "opponentLastMove": {
            "type": "Normal",
            "piece": "White Pawn",
            "fromSquare": "g1",
            "toSquare": "f3",
            "capture": false,
            "yourKingSafety": "Safe"
          },
          "opponentPiecePlacements": {
            "a1": "WhiteRook", "b1": "WhiteKnight", "c1": "WhiteBishop", "d1": "WhiteQueen",
            "e1": "WhiteKing", "f1": "WhiteBishop", "f3": "WhiteKnight", "h1": "WhiteRook",
            "a2": "WhitePawn", "b2": "WhitePawn",   "c2": "WhitePawn",   "d2": "WhitePawn",
            "e4": "WhitePawn", "f2": "WhitePawn",   "g2": "WhitePawn",   "h2": "WhitePawn",
          },
          "yourPiecePlacements": {
            "a8": "BlackRook", "b8": "BlackKnight", "c8": "BlackBishop", "d8": "BlackQueen",
            "e8": "BlackKing", "f8": "BlackBishop", "g8": "BlackKnight", "h8": "BlackRook",
            "a7": "BlackPawn", "b7": "BlackPawn",   "c7": "BlackPawn",   "d7": "BlackPawn",
            "e5": "BlackPawn", "f7": "BlackPawn",   "g7": "BlackPawn",   "h7": "BlackPawn"
          },
          "pastFailureMessages": [
            "You want to move a piece from e8 to d8. But... Cannot move to square 'd8' because the piece at that square is an ally"
          ]
        }
      OUTPUT:
        {
          "purpose": "I develop the knight to a natural square to control
                      the center (d4 and e5), and prepare for kingside castling.",
          "fromSquare": "b8",
          "toSquare": "c6",
          "promotedPiece": null
        }
    </EXAMPLE>
    */
    constexpr const char PROMPT_PATTERN[] = "{}";

    constexpr const char MOVE_JSON_SCHEMA[] =
        "{\n"
        "  \"type\": \"object\",\n"
        "  \"properties\": {\n"
        "    \"purpose\": {\n"
        "      \"type\": \"string\"\n"
        "    },\n"
        "    \"fromSquare\": {\n"
        "      \"type\": \"string\",\n"
        "      \"pattern\": \"^[a-h][1-8]$\"\n"
        "    },\n"
        "    \"toSquare\": {\n"
        "      \"type\": \"string\",\n"
        "      \"pattern\": \"^[a-h][1-8]$\"\n"
        "    },\n"
        "    \"promotedPiece\": {\n"
        "      \"type\": \"string\",\n"
        "      \"enum\": [\"Queen\", \"Rook\", \"Bishop\", \"Knight\"],\n"
        "      \"nullable\": true,\n"
        "      \"default\": null\n"
        "    }\n"
        "  },\n"
        "  \"required\": [\"purpose\", \"fromSquare\", \"toSquare\"],\n"
        "  \"propertyOrdering\": [\"purpose\", \"fromSquare\", \"toSquare\", \"promotedPiece\"]\n"
        "}";

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
    //   validateStandardChessMove(move);
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

    //   validateStandardChessMove(move);

    //   return move;
    // }

    inline auto chessMoveToJson(ChessMove const &move) -> boost::json::object
    {
      // validateStandardChessMove(move);

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
        throw std::invalid_argument(std::format(
            "'fromSquare' is invalid or missing in JSON input: {}",
            fromSquare));
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
        throw std::invalid_argument(std::format(
            "'toSquare' is invalid or missing in JSON input: {}",
            toSquare));
      }

      if (jsonObj.contains("promotedPiece"))
      {
        boost::json::value promotionValue = jsonObj.at("promotedPiece");

        if (!promotionValue.is_null())
        {
          std::string promotedPiece;
          try
          {
            promotedPiece = std::string{promotionValue.as_string().c_str()};
            move.promotedPiece = EntityType{promotedPiece};
          }
          catch (...)
          {
            throw std::invalid_argument(std::format(
                "'promotedPiece' is invalid in JSON input: {}",
                promotedPiece));
          }
        }
      }

      // validateStandardChessMove(move);

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
        throw std::invalid_argument("Cannot parse JSON: " + jsonStr);
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
