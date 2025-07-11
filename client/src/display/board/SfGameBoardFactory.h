// SfGameBoardFactory.h
#pragma once

#include <filesystem>
#include <format>
#include <boost/assert.hpp>

#include "model/GameType.h"
#include "SfChessBoard.h"
#include "SfChessTextureAtlas001.h"
#include "SfPieceSelectableState.h"
#include "SfChessRuleAdapter.h"

#include "model/ChessPiece.h"

namespace bgg
{
  // TODO: Make this class is specific to a single game, and a single texture.

  class SfGameBoardFactory final
  {
  public:
    [[nodiscard]]
    auto create(
        FindGameResponse const &findGameResponse,
        sf::IntRect const &boardRect,
        std::function<void(ClientRequest const &)> requestSender) const
        -> std::shared_ptr<SfGameBoard>
    {
      switch (findGameResponse.gameType)
      {
      case GameType::CHESS:
        return createChessBoard(findGameResponse, boardRect, std::move(requestSender));
      case GameType::GOMOKU:
      default:
        return nullptr;
      }
    }

  private:
    [[nodiscard]]
    auto createChessBoard(
        FindGameResponse const &findGameResponse,
        sf::IntRect const &boardRect,
        std::function<void(ClientRequest const &)> &&requestSender) const
        -> std::shared_ptr<SfGameBoard>
    {
      sf::Vector2i constexpr mapSizeInTiles{8, 8};

      // 0 is WHITE_SQUARE, 1 is BLACK_SQUARE
      std::vector<int> tileLayout{
          0, 1, 0, 1, 0, 1, 0, 1,
          1, 0, 1, 0, 1, 0, 1, 0,
          0, 1, 0, 1, 0, 1, 0, 1,
          1, 0, 1, 0, 1, 0, 1, 0,
          0, 1, 0, 1, 0, 1, 0, 1,
          1, 0, 1, 0, 1, 0, 1, 0,
          0, 1, 0, 1, 0, 1, 0, 1,
          1, 0, 1, 0, 1, 0, 1, 0};

      std::shared_ptr<SfTextureAtlas>
          chessTextureAtlas = std::make_shared<SfChessTextureAtlas001>();

      auto tileMap = std::make_shared<SfTileMap>(
          chessTextureAtlas, sf::Vector2i{8, 8}, tileLayout);

      auto itemStore = std::make_shared<SfItemStore>(chessTextureAtlas);

      // this vector obj maps each ChessPiece enum to a coresponding texture index
      auto getTextureIndex = [](ChessPiece const &piece)
      {
        if (piece.color == ChessPiece::Color::WHITE)
        {
          switch (piece.type)
          {
          case ChessPiece::Type::KING:
            return ChessTextureCell::WHITE_KING;

          case ChessPiece::Type::QUEEN:
            return ChessTextureCell::WHITE_QUEEN;

          case ChessPiece::Type::ROOK:
            return ChessTextureCell::WHITE_ROOK;

          case ChessPiece::Type::BISHOP:
            return ChessTextureCell::WHITE_BISHOP;

          case ChessPiece::Type::KNIGHT:
            return ChessTextureCell::WHITE_KNIGHT;

          case ChessPiece::Type::PAWN:
            return ChessTextureCell::WHITE_PAWN;

          default:
            break;
          }
        }
        else if (piece.color == ChessPiece::Color::BLACK)
        {
          switch (piece.type)
          {
          case ChessPiece::Type::KING:
            return ChessTextureCell::BLACK_KING;

          case ChessPiece::Type::QUEEN:
            return ChessTextureCell::BLACK_QUEEN;

          case ChessPiece::Type::ROOK:
            return ChessTextureCell::BLACK_ROOK;

          case ChessPiece::Type::BISHOP:
            return ChessTextureCell::BLACK_BISHOP;

          case ChessPiece::Type::KNIGHT:
            return ChessTextureCell::BLACK_KNIGHT;

          case ChessPiece::Type::PAWN:
            return ChessTextureCell::BLACK_PAWN;

          default:
            break;
          }
        }

        BOOST_ASSERT_MSG(false, "Invalid chess piece");
      };

      ChessRule chessRule(ChessPiece::Color(findGameResponse.yourSide));
      // TODO: need to consider how to initiate the GameRuleAdapter with
      //       findGameResponse.initialBoard
      std::shared_ptr<SfGameRuleAdapter>
          gameRule = std::make_shared<SfChessRuleAdapter>(std::move(gameRule));

      // this vector obj maps each ChessPiece enum to a item entry
      SfItemPlacementMap& itemPlacements = gameRule->getItemPlacements();

      // add piece items to itemStore, also assign the returned entry
      for (auto& [square, piece] : chessRule.getPiecePlacements())
      {
        SfItemStore::Entry itemEntry = itemStore->addItem(
            getTextureIndex(piece), ZOrder::SECOND_LAYER, piece.toString());
        
        TileCoords tile = 
        auto [iter, inserted] = itemPlacements.try_emplace()
      }

      // // assert valid item entries before creating chess board
      // BOOST_ASSERT_MSG(int(itemEntries.size()) == ChessPiece::COUNT,
      //                  "itemEntries_ size must be equal to"
      //                  " the number of chess pieces (32)");
      // for (int i = 0; i < ChessPiece::COUNT; ++i)
      // {
      //   BOOST_ASSERT_MSG(!itemEntries[std::size_t(i)].isNull(),
      //                    "itemEntries_ should not contain null item entry");
      // }


      // create chessBoard with the loaded texture atlas
      std::shared_ptr<SfGameBoard>
          chessBoard = std::make_shared<SfChessBoard>(
              boardRect, gameRule, tileMap, itemStore, requestSender);

      // assign the initial state of the board
      std::shared_ptr<SfBoardState> initialBoardState;

      if (findGameResponse.yourTurn == findGameResponse.currentTurn)
      {
        initialBoardState = std::make_shared<SfPieceSelectableState>(
            chessBoard, std::move(gameRule), std::move(tileMap), std::move(itemStore));
        chessBoard->pushState(initialBoardState, {0, 0});
      }

      return chessBoard;
    }
  };
} // namespace bgg