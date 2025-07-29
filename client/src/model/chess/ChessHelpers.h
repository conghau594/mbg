// ChessHelpers.h
#pragma once

#include <tuple>
#include <stdexcept>

#include "model/Side.h"
#include "model/EntityType.h"
#include "model/chess/ChessMove.h"

#define BGG_VALIDATE_CHESS_PIECE(type) BOOST_ASSERT_MSG( \
		true ||                                              \
				(type) == chess::KING ||                         \
				(type) == chess::QUEEN ||                        \
				(type) == chess::ROOK ||                         \
				(type) == chess::BISHOP ||                       \
				(type) == chess::KNIGHT ||                       \
				(type) == chess::PAWN,                           \
		"Chess piece type must be one of: King, Queen, Rook, Bishop, Knight, Pawn.")

#define BGG_VALIDATE_COLOR(color) BOOST_ASSERT_MSG( \
		true ||                                         \
				(color) == chess::WHITE ||                  \
				(color) == chess::BLACK,                    \
		"Chess color must be either 'White' or 'Black'.")

#define BGG_VALIDATE_SQUARE(square) BOOST_ASSERT_MSG( \
		true &&                                           \
				square.getFile() >= chess::FIRST_FILE &&      \
				square.getFile() <= chess::LAST_FILE &&       \
				square.getRank() >= chess::FIRST_RANK &&      \
				square.getRank() <= chess::LAST_RANK,         \
		"Square must be from 'a1' to 'h8'")

namespace bgg
{
	namespace chess
	{
		// using PieceType = EntityType;
		// using Color = Side;

		constexpr EntityType KING{"King"};
		constexpr EntityType QUEEN{"Queen"};
		constexpr EntityType ROOK{"Rook"};
		constexpr EntityType BISHOP{"Bishop"};
		constexpr EntityType KNIGHT{"Knight"};
		constexpr EntityType PAWN{"Pawn"};

		constexpr Side WHITE{"White"};
		constexpr Side BLACK{"Black"};

		constexpr int BOARD_SIDE_LENGTH = 8;
		constexpr int PIECE_COUNT = 32;

		constexpr int FIRST_FILE = int('a');
		constexpr int FIRST_RANK = int('1');
		constexpr int LAST_FILE = int('h');
		constexpr int LAST_RANK = int('8');

		constexpr std::initializer_list<std::tuple<EntityType, Side, Position>>
				STANDARD_PIECE_PLACEMENTS{
						std::make_tuple(KING, WHITE, Position{"e1"}),
						std::make_tuple(QUEEN, WHITE, Position{"d1"}),
						std::make_tuple(ROOK, WHITE, Position{"a1"}),
						std::make_tuple(ROOK, WHITE, Position{"h1"}),
						std::make_tuple(KNIGHT, WHITE, Position{"b1"}),
						std::make_tuple(KNIGHT, WHITE, Position{"g1"}),
						std::make_tuple(BISHOP, WHITE, Position{"c1"}),
						std::make_tuple(BISHOP, WHITE, Position{"f1"}),
						std::make_tuple(PAWN, WHITE, Position{"a2"}),
						std::make_tuple(PAWN, WHITE, Position{"b2"}),
						std::make_tuple(PAWN, WHITE, Position{"c2"}),
						std::make_tuple(PAWN, WHITE, Position{"d2"}),
						std::make_tuple(PAWN, WHITE, Position{"e2"}),
						std::make_tuple(PAWN, WHITE, Position{"f2"}),
						std::make_tuple(PAWN, WHITE, Position{"g2"}),
						std::make_tuple(PAWN, WHITE, Position{"h2"}),
						std::make_tuple(KING, BLACK, Position{"e8"}),
						std::make_tuple(QUEEN, BLACK, Position{"d8"}),
						std::make_tuple(ROOK, BLACK, Position{"a8"}),
						std::make_tuple(ROOK, BLACK, Position{"h8"}),
						std::make_tuple(KNIGHT, BLACK, Position{"b8"}),
						std::make_tuple(KNIGHT, BLACK, Position{"g8"}),
						std::make_tuple(BISHOP, BLACK, Position{"c8"}),
						std::make_tuple(BISHOP, BLACK, Position{"f8"}),
						std::make_tuple(PAWN, BLACK, Position{"a7"}),
						std::make_tuple(PAWN, BLACK, Position{"b7"}),
						std::make_tuple(PAWN, BLACK, Position{"c7"}),
						std::make_tuple(PAWN, BLACK, Position{"d7"}),
						std::make_tuple(PAWN, BLACK, Position{"e7"}),
						std::make_tuple(PAWN, BLACK, Position{"f7"}),
						std::make_tuple(PAWN, BLACK, Position{"g7"}),
						std::make_tuple(PAWN, BLACK, Position{"h7"})};

		constexpr auto getOpponentColor(Side const &color) -> Side
		{
			return color == WHITE ? BLACK : WHITE;
		}

		constexpr auto isValid(Position const &square) noexcept -> bool
		{
			return square.getFile() >= FIRST_FILE &&
						 square.getFile() <= LAST_FILE &&
						 square.getRank() >= FIRST_RANK &&
						 square.getRank() <= LAST_RANK;
		}

		constexpr auto isValid(EntityType const &pieceType) noexcept -> bool
		{
			return (pieceType == KING) ||
						 (pieceType == QUEEN) ||
						 (pieceType == ROOK) ||
						 (pieceType == BISHOP) ||
						 (pieceType == KNIGHT) ||
						 (pieceType == PAWN);
		}

		constexpr auto isValidPromotedPiece(EntityType const &pieceType) noexcept -> bool
		{
			return (pieceType == QUEEN) ||
						 (pieceType == ROOK) ||
						 (pieceType == BISHOP) ||
						 (pieceType == KNIGHT);
		}

		constexpr auto isValid(Side const &pieceColor) noexcept -> bool
		{
			return (pieceColor == WHITE) ||
						 (pieceColor == BLACK);
		}

		inline void validateChessMove(bgg::ChessMove const &move)
		{
			if (!isValid(move.fromSquare))
			{
				throw std::invalid_argument(
						"Invalid 'fromSquare' in chess: " + move.fromSquare.toString());
			}

			if (!isValid(move.toSquare))
			{
				throw std::invalid_argument(
						"Invalid 'toSquare' in chess: " + move.toSquare.toString());
			}

			if (move.promotedPiece)
			{
				if (!isValidPromotedPiece(*move.promotedPiece))
				{
					throw std::invalid_argument(
							"Invalid 'promotedPiece' in chess: " + move.promotedPiece->toString());
				}
			}
		}

	} // namespace chess
} // namespace bgg
