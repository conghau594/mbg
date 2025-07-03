// SfChessBoard.cpp

#include <boost/assert.hpp>

#include <SFML/Window/Event.hpp>

#include "SfChessBoard.h"
#include "SfBoardState.h"

namespace bgg
{
  SfChessBoard::SfChessBoard(SfTileMap tileMap) noexcept
      : tileMap_(std::move(tileMap)),
        currentBoardState_(nullptr),
        lastBoardState_(nullptr)
  {
  }

  void SfChessBoard::onEvent(sf::Event const &event) noexcept
  {
    BOOST_ASSERT_MSG(currentBoardState_, "boardState_ of SfChessBoard cannot be null");
    if (lastBoardState_ != currentBoardState_)
    {
      lastBoardState_ = currentBoardState_;
    }

    if (auto mouseMoved = event.getIf<sf::Event::MouseMoved>())
    {
      currentBoardState_->onMouseMoved(mouseMoved->position);
    }
    else if (auto mouseBtnPressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
      if (mouseBtnPressed->button == sf::Mouse::Button::Left)
      {
        currentBoardState_->onMousePressed(mouseBtnPressed->position);
      }
    }
    else if (auto mouseBtnReleased = event.getIf<sf::Event::MouseButtonReleased>())
    {
      if (mouseBtnReleased->button == sf::Mouse::Button::Left)
      {
        currentBoardState_->onMousePressed(mouseBtnReleased->position);
      }
    }
  }

  void SfChessBoard::send(ClientEvent const &request) noexcept
  {
  }

  void SfChessBoard::draw(sf::RenderTarget &target, sf::RenderStates states) const noexcept
  {
    // draw the tileMap
    target.draw(tileMap_, states);
  }

  void SfChessBoard::changeState(std::shared_ptr<SfBoardState> newState) noexcept
  {
    if (currentBoardState_ != nullptr)
    {
      currentBoardState_->onExit();
    }

    currentBoardState_ = newState;
    if (newState != nullptr)
    {
      newState->onEnter();
    }
  }
} // namespace bgg
