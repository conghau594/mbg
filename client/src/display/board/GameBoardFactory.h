// GameBoardFactory.h
#pragma once

#include <filesystem>
#include <format>
#include <boost/assert.hpp>

#include "model/GameType.h"
#include "chess/ChessBoard.h"
#include "chess/ChessTextureAtlas001.h"
#include "chess/ChessPieceSelectableState.h"
#include "chess/ChessRuleAdapter.h"

#include "model/Piece.h"

namespace bgg
{
  // TODO: Make this class is specific to a single game, and a single texture.

  class GameBoardFactory final
  {
  public:
    [[nodiscard]]
    auto create(
        FindGameResponse const &findGameResponse,
        sf::IntRect const &boardRect,
        std::function<void(ClientRequest const &)> requestSender) const
        -> std::shared_ptr<IGameBoard>
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
        -> std::shared_ptr<IGameBoard>
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

      std::shared_ptr<BaseTextureAtlas>
          chessTextureAtlas = std::make_shared<ChessTextureAtlas001>();

      auto tileMap = std::make_shared<TileMap>(
          chessTextureAtlas, sf::Vector2i{8, 8}, tileLayout);

      auto itemStore = std::make_shared<ItemStore>(chessTextureAtlas);

      // this vector obj maps each ChessPiece enum to a coresponding texture index
      auto getTextureIndex = [](Piece const &piece) -> int
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
        return -1;
      };

      ChessRule chessRule(findGameResponse.initialBoard, findGameResponse.yourSide);
      // TODO: need to consider how to initiate the IGameRuleAdapter with
      //       findGameResponse.initialBoard
      std::shared_ptr<IGameRuleAdapter>
          gameRuleAdapter = std::make_shared<ChessRuleAdapter>(std::move(chessRule));

      // this vector obj maps each ChessPiece enum to a item entry
      ItemPlacementMap &itemPlacements = gameRuleAdapter->getItemPlacements();

      // add piece items to itemStore, also assign the returned entry
      for (auto &[square, piece] : findGameResponse.initialBoard)
      {
        ItemStore::Entry itemEntry = itemStore->addItem(
            getTextureIndex(piece), ZOrder::SECOND_LAYER, piece.toString());

        TileCoords tile = gameRuleAdapter->positionToTile(square);
        auto [iter, inserted] = itemPlacements.try_emplace(tile, itemEntry);

        BOOST_ASSERT_MSG(inserted, "There is no reason to fail this insertion");
      }

      // create chessBoard with the loaded texture atlas
      std::shared_ptr<IChessBoard>
          chessBoard = std::make_shared<ChessBoard>(
              boardRect, gameRuleAdapter, tileMap, itemStore, requestSender);

      // assign the initial state of the board
      std::shared_ptr<IBoardState> initialBoardState;

      if (findGameResponse.yourTurn == findGameResponse.currentTurn)
      {
        initialBoardState = std::make_shared<ChessPieceSelectableState>(
            chessBoard,
            std::move(gameRuleAdapter),
            std::move(tileMap),
            std::move(itemStore));

        chessBoard->pushState(std::move(initialBoardState), {-1000, -1000});  ///< to make initial mouse pos far away from window
                                                                              ///< => make choice highlighter not appear wrong 
      }

      return chessBoard;
    }
  };
} // namespace bgg