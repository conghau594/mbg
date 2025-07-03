// SfChessBoard.h
#pragma once

#include "SfGameBoard.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

namespace bgg
{
  class SfChessBoard : public SfGameBoard
  {
  public:
    void onEvent(sf::Event const &event) override;

    void send(ClientEvent const &request) override;

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
  };

} // namespace bgg
