// SfBaseScreen.cpp

#include <memory>

#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads
#include <imgui_internal.h>

#include "connect/GameService.h"
#include "GameDisplay.h"
#include "SfBaseScreen.h"
#include "SfBlockingScreen.h"
#include "SfWaitingScreen.h"

// #ifdef _DEBUG
#include <iostream>

// #endif
namespace iab
{
  std::recursive_mutex SfBaseScreen::s_ImGuiMutex;

  SfBaseScreen::SfBaseScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameService> gameService,
      std::shared_ptr<ScreenManager> parentScreenMgr) noexcept
      : window_(window),
        gameService_(gameService),
        clock_(new sf::Clock),
        parentScreenMgr_(parentScreenMgr),
        isActive_(true)
  {
    BOOST_ASSERT_MSG(window, "window_ of SfBaseScreen cannot be null.");
    BOOST_ASSERT_MSG(gameService, "gameService_ of SfBaseScreen cannot be null.");
    BOOST_ASSERT_MSG(parentScreenMgr, "parentScreenMgr_ of SfBaseScreen cannot be null.");
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

    {
      std::lock_guard lock(s_ImGuiMutex);
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
  }

  void SfBaseScreen::onEnter()
  {
    std::lock_guard lock(s_ImGuiMutex);
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
    askExitConfirmation(window_, gameService_, shared_from_this(), "Do you want to quit?");
  }

  void SfBaseScreen::pushScreen(std::shared_ptr<GameScreen> newScreen) noexcept
  {
    std::lock_guard lock(s_ImGuiMutex);
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
    std::lock_guard lock(s_ImGuiMutex);
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
    std::lock_guard lock(s_ImGuiMutex);
    auto &lastScreen = screenStack_.back();
    lastScreen->onExit();
    lastScreen = newScreen;
    lastScreen->onEnter();
  }

  void SfBaseScreen::askExitConfirmation(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameService> gameService,
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
        window, gameService, screenMgr, msg, buttonLabels, buttonCallbacks));

    screenMgr->pushScreen(pauseScreen);
  }

  void SfBaseScreen::connectServer(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameService> gameService,
      std::shared_ptr<ScreenManager> screenMgr) noexcept
  {
    std::shared_ptr<GameScreen> waitScreen(new SfWaitingScreen(
        window,
        gameService,
        screenMgr,
        "Connecting to server...",
        {"Cancel"},
        {[window]()
         {
           window->close();
         }}));

    screenMgr->pushScreen(waitScreen);

    gameService->connect(
        "",
        [screenMgr, window, gameService](int code, std::string const &errMsg)
        {
          if (code == 0)
          {
            screenMgr->popScreen();
          }
          else
          {
            std::vector<std::string> &&buttonLabels{"Retry", "Quit"};
            std::vector<std::function<void()>> &&buttonCallbacks{
                [window, gameService, screenMgr]()
                {
                  screenMgr->popScreen();
                  SfBaseScreen::connectServer(window, gameService, screenMgr);
                },
                [window]()
                {
                  window->close();
                }};

            std::shared_ptr<GameScreen> pauseScreen(new SfBlockingScreen(
                window,
                gameService,
                screenMgr,
                errMsg + std::to_string(code),
                buttonLabels,
                buttonCallbacks));

            screenMgr->changeScreen(pauseScreen);
          }
        });
  }
}