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
      std::shared_ptr<ScreenManager> screenMgr) noexcept
      : window_(window),
        clock_(new sf::Clock),
        screenMgr_(screenMgr),
        isActive_(true)
  {
    BOOST_ASSERT_MSG(window, "`window_` of `SfBaseScreen` cannot be null.");
    BOOST_ASSERT_MSG(screenMgr, "`gameDisplay_` of `SfBaseScreen` cannot be null.");
  }

  void SfBaseScreen::update()
  {
    if (!screenStack_.empty())
    {
      if (currentScreen_ != screenStack_.back())
      {
        currentScreen_ = screenStack_.back();
      }
      currentScreen_->update();
      return;
    }

    sf::Time elapsed = clock_->restart();
    if (elapsed == sf::Time::Zero || !isActive_)
    {
      // If no time has passed or the screen should deactivate, do nothing
      return;
    }

    update(elapsed);

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
    askExitConfirmation(window_, shared_from_this(), "Do you want to quit?");
  }

  void SfBaseScreen::pushScreen(std::shared_ptr<GameScreen> newScreen) noexcept
  {
    if (!screenStack_.empty())
    {
      screenStack_.back()->onExit();
    }
    else
    {
      this->onExit();
    }

    screenStack_.push_back(newScreen);
    newScreen->onEnter();
  }

  void SfBaseScreen::popScreen() noexcept
  {
    if (screenStack_.empty())
    {
      return;
    }

    screenStack_.back()->onExit();
    screenStack_.pop_back();

    if (screenStack_.empty())
    {
      this->onEnter();
    }
    else
    {
      screenStack_.back()->onEnter();
    }
  }

  void SfBaseScreen::changeScreen(std::shared_ptr<GameScreen> newScreen) noexcept
  {
    auto &lastScreen = screenStack_.back();
    lastScreen->onExit();
    lastScreen = newScreen;
    lastScreen->onEnter();
  }

  void SfBaseScreen::askExitConfirmation(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<ScreenManager> screenMgr,
      std::string const &msg) noexcept
  {
    std::vector<std::string> &&buttonLabels{"Yes", "No"};
    std::vector<std::function<void()>> &&buttonCallbacks{
        [window]()
        {
          window->close();
        },
        [screenMgr]()
        {
          screenMgr->popScreen();
        }};

    std::shared_ptr<GameScreen> pauseScreen(new SfBlockingScreen(
        window, screenMgr, msg, buttonLabels, buttonCallbacks));

    screenMgr->pushScreen(pauseScreen);
  }
}