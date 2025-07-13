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

#include "model/Piece.h"

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
      auto getTextureIndex = [](Piece const &piece)
      {
        if (Color::WHITE == piece.color)
        {
          if (ChessRule::PieceType::KING == piece.type)
          {
            return ChessTextureCell::WHITE_KING;
          }

          if (ChessRule::PieceType::QUEEN == piece.type)
          {
            return ChessTextureCell::WHITE_QUEEN;
          }

          if (ChessRule::PieceType::ROOK == piece.type)
          {
            return ChessTextureCell::WHITE_ROOK;
          }

          if (ChessRule::PieceType::BISHOP == piece.type)
          {
            return ChessTextureCell::WHITE_BISHOP;
          }

          if (ChessRule::PieceType::KNIGHT == piece.type)
          {
            return ChessTextureCell::WHITE_KNIGHT;
          }

          if (ChessRule::PieceType::PAWN == piece.type)
          {
            return ChessTextureCell::WHITE_PAWN;
          }
        }
        else if (Color::BLACK == piece.color)
        {
          if (ChessRule::PieceType::KING == piece.type)
          {
            return ChessTextureCell::BLACK_KING;
          }

          if (ChessRule::PieceType::QUEEN == piece.type)
          {
            return ChessTextureCell::BLACK_QUEEN;
          }

          if (ChessRule::PieceType::ROOK == piece.type)
          {
            return ChessTextureCell::BLACK_ROOK;
          }

          if (ChessRule::PieceType::BISHOP == piece.type)
          {
            return ChessTextureCell::BLACK_BISHOP;
          }

          if (ChessRule::PieceType::KNIGHT == piece.type)
          {
            return ChessTextureCell::BLACK_KNIGHT;
          }

          if (ChessRule::PieceType::PAWN == piece.type)
          {
            return ChessTextureCell::BLACK_PAWN;
          }
        }

        BOOST_ASSERT_MSG(false, "Invalid chess piece");
      };

      ChessRule chessRule(findGameResponse.yourSide);
      // TODO: need to consider how to initiate the GameRuleAdapter with
      //       findGameResponse.initialBoard
      std::shared_ptr<SfGameRuleAdapter>
          gameRuleAdapter = std::make_shared<SfChessRuleAdapter>(chessRule);

      // this vector obj maps each ChessPiece enum to a item entry
      SfItemPlacementMap &itemPlacements = gameRuleAdapter->getItemPlacements();
      auto squareToTileConverter = SfChessRuleAdapter::getSquareToTileConverter(
          findGameResponse.yourSide);

      // add piece items to itemStore, also assign the returned entry
      for (auto &[square, piece] : chessRule.getPiecePlacements())
      {
        SfItemStore::Entry itemEntry = itemStore->addItem(
            getTextureIndex(piece), ZOrder::SECOND_LAYER, piece.toString());

        TileCoords tile = squareToTileConverter(square);
        auto [iter, inserted] = itemPlacements.try_emplace(tile, itemEntry);

        BOOST_ASSERT_MSG(inserted, "There is no reason to fail this insertion");
      }

      // create chessBoard with the loaded texture atlas
      std::shared_ptr<SfGameBoard>
          chessBoard = std::make_shared<SfChessBoard>(
              boardRect, gameRuleAdapter, tileMap, itemStore, requestSender);

      // assign the initial state of the board
      std::shared_ptr<SfBoardState> initialBoardState;

      if (findGameResponse.yourTurn == findGameResponse.currentTurn)
      {
        initialBoardState = std::make_shared<SfPieceSelectableState>(
            chessBoard,
            std::move(gameRuleAdapter),
            std::move(tileMap),
            std::move(itemStore));

        chessBoard->pushState(std::move(initialBoardState), {-1000, -1000}); ///< to make it far away from window
      }

      return chessBoard;
    }
  };
} // namespace bgg