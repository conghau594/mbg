// SfBaseScreen.cpp

#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads
#include <imgui_internal.h>

#include <memory>

#include "GameDisplay.h"
#include "SfBaseScreen.h"
#include "SfPauseScreen.h"

// #ifdef _DEBUG
#include <iostream>

// #endif
namespace iab
{
  SfBaseScreen::SfBaseScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameDisplay> displayContext) noexcept
      : window_(window),
        clock_(new sf::Clock),
        displayContext_(displayContext)
  {
    BOOST_ASSERT_MSG(window, "`window_` of `SfBaseScreen` cannot be null.");
    BOOST_ASSERT_MSG(displayContext, "`displayContext_` of `SfBaseScreen` cannot be null.");
  }

  void SfBaseScreen::update()
  {
    onTimeElapsed();

    while (const auto event = window_->pollEvent())
    {
      if (event->is<sf::Event::Closed>())
      {
        askExitConfirmation();
      }
      else
      {
        ImGui::SFML::ProcessEvent(*window_, *event);
        onEvent(event);
      }
    }
  }

  void SfBaseScreen::askExitConfirmation() noexcept
  {
    std::vector<std::string> &&buttonLabels{"Yes", "No"};
    std::vector<std::function<void()>> &&buttonCallbacks{
        [this]()
        {
          window_->close();
        },
        [this]()
        {
          displayContext_->popScreen();
        }};

    std::shared_ptr<GameScreen> pauseScreen(new SfPauseScreen(
        window_, "Do you want to exit?", buttonLabels, buttonCallbacks));

    displayContext_->pushScreen(pauseScreen);
  }
}