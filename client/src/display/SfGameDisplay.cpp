// SfGameDisplay.cpp

#include <optional>
#include <memory>
#include <string>

#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads

#include "GameScreen.h"
#include "SfGameDisplay.h"

namespace iac
{
  SfGameDisplay::SfGameDisplay(std::shared_ptr<sf::RenderWindow> window)
      : window_(window)
  {
    BOOST_ASSERT_MSG(window, "`window_` of `SfGameDisplay` cannot be null.");
    if (!ImGui::SFML::Init(*window_))
    { // TODO: Define exception for this
      throw std::runtime_error("Failed to initialize ImGui.");
    }
  }

  SfGameDisplay::~SfGameDisplay() noexcept
  {
    ImGui::SFML::Shutdown();
  }

  void SfGameDisplay::run()
  {
    while (window_->isOpen())
    {
      auto currentScreen = screenStack_.back();
      currentScreen->update();
    }
  }

  void SfGameDisplay::pushScreen(std::shared_ptr<GameScreen> newScreen) noexcept
  {
    if (!screenStack_.empty())
    {
      screenStack_.back()->onExit();
    }

    screenStack_.push_back(newScreen);
    newScreen->onEnter();
  }

  void SfGameDisplay::popScreen() noexcept
  {
    if (screenStack_.empty())
    {
      return;
    }

    screenStack_.back()->onExit();
    screenStack_.pop_back();

    if (!screenStack_.empty())
    {
      screenStack_.back()->onEnter();
    }
  }

  void SfGameDisplay::changeScreen(std::shared_ptr<GameScreen> newScreen) noexcept
  {
    screenStack_.back()->onExit();
    screenStack_.pop_back();

    screenStack_.push_back(newScreen);
    newScreen->onEnter();
  }
}