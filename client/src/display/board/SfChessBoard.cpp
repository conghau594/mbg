// SfChessBoard.cpp

#include "SfChessBoard.h"
#include "SfBoardState.h"

namespace bgg
{
  SfChessBoard::SfChessBoard(SfTileMap tileMap)
      : tileMap_(std::move(tileMap)),
        boardState_(nullptr)
  {
  }
  void SfChessBoard::onEvent(sf::Event const &event)
  {
  }

  void SfChessBoard::send(ClientEvent const &request)
  {
  }

  void SfChessBoard::draw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    // draw the tileMap
    target.draw(tileMap_, states);
  }

  void SfChessBoard::changeMouseState(std::shared_ptr<SfBoardState> newMouseState)
  {
  }
} // namespace bgg
