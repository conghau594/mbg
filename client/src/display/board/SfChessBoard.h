// SfChessBoard.h
#pragma once

#include <memory>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include "SfGameBoard.h"
#include "SfTileMap.h"
#include "SfBoardState.h"

namespace bgg
{
  class SfChessBoard : public SfGameBoard
  {
    std::shared_ptr<SfBoardState> boardState_;
    SfTileMap tileMap_;

  public:
    SfChessBoard(SfTileMap tileMap);

  private:
    void onEvent(sf::Event const &event) override;

    void send(ClientEvent const &request) override;

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    void changeMouseState(std::shared_ptr<SfBoardState> newMouseState) override;
  };

} // namespace bgg
