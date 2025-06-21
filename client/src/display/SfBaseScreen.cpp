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
#include "SfBlockingScreen.h"

// #ifdef _DEBUG
#include <iostream>

// #endif
namespace iab
{
  SfBaseScreen::SfBaseScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameDisplay> gameDisplay) noexcept
      : window_(window),
        clock_(new sf::Clock),
        gameDisplay_(gameDisplay),
        isActive_(true)
  {
    BOOST_ASSERT_MSG(window, "`window_` of `SfBaseScreen` cannot be null.");
    BOOST_ASSERT_MSG(gameDisplay, "`gameDisplay_` of `SfBaseScreen` cannot be null.");
  }

  void SfBaseScreen::update()
  {
    update(clock_->restart());

    while (const auto event = window_->pollEvent())
    {
      if (event->is<sf::Event::Closed>())
      {
        onWindowClosed();
      }
      else
      {
        ImGui::SFML::ProcessEvent(*window_, *event);
        onWindowEventExceptClosed(event);
      }
    }
  }

  void SfBaseScreen::onEnter()
  {
    isActive_ = true;
    doEnter();
    clock_->start();
  }

  void SfBaseScreen::onExit()
  {
    clock_->stop();
    doExit();
    isActive_ = false;
  }

  void SfBaseScreen::onWindowClosed()
  {
    askExitConfirmation(window_, gameDisplay_, "Do you want to quit?");
  }

  void SfBaseScreen::askExitConfirmation(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameDisplay> gameDisplay,
      std::string const &msg) noexcept
  {
    std::vector<std::string> &&buttonLabels{"Yes", "No"};
    std::vector<std::function<void()>> &&buttonCallbacks{
        [window]()
        {
          window->close();
        },
        [gameDisplay]()
        {
          gameDisplay->popScreen();
        }};

    std::shared_ptr<GameScreen> pauseScreen(new SfBlockingScreen(
        window, gameDisplay, msg, buttonLabels, buttonCallbacks));

    gameDisplay->pushScreen(pauseScreen);
  }
}