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
#include "SfConfirmationScreen.h"
#include "SfBlockingScreen.h"

// #ifdef _DEBUG
#include <iostream>

// #endif
namespace iab
{
  SfBaseScreen::SfBaseScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameService> gameService,
      std::shared_ptr<GameDisplay> gameDisplay) noexcept
      : window_(window),
        gameService_(gameService),
        gameDisplay_(gameDisplay),
        clock_(new sf::Clock),
        isActive_(true),
        lastSubscreen_(nullptr),
        currentSubscreen_(nullptr)
  {
    BOOST_ASSERT_MSG(window, "window_ of SfBaseScreen cannot be null.");
    BOOST_ASSERT_MSG(gameService, "gameService_ of SfBaseScreen cannot be null.");
    BOOST_ASSERT_MSG(gameDisplay, "gameDisplay_ of SfBaseScreen cannot be null.");
  }

  void SfBaseScreen::update()
  {
    if (currentSubscreen_ != nullptr)
    {
      if (lastSubscreen_ != currentSubscreen_)
      {
        lastSubscreen_ = currentSubscreen_;
      }
      lastSubscreen_->update();
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
    currentSubscreen_ = nullptr;
    lastSubscreen_ = nullptr;
    clock_->stop();
    doExit();
    isActive_ = false;
  }

  void SfBaseScreen::onWindowClosed()
  {
    askExitConfirmation(window_, gameService_, gameDisplay_, shared_from_this(), "Do you want to quit?");
  }

  // void SfBaseScreen::pushScreen(std::shared_ptr<GameScreen> newScreen) noexcept
  // {
  //   if (!screenStack_.empty())
  //   {
  //     screenStack_.back()->onExit();
  //   }
  //   else
  //   {
  //     this->onExit();
  //   }

  //   screenStack_.push_back(newScreen);
  //   newScreen->onEnter();
  // }

  // void SfBaseScreen::popScreen() noexcept
  // {
  //   if (screenStack_.empty())
  //   {
  //     return;
  //   }

  //   screenStack_.back()->onExit();
  //   screenStack_.pop_back();

  //   if (screenStack_.empty())
  //   {
  //     this->onEnter();
  //   }
  //   else
  //   {
  //     screenStack_.back()->onEnter();
  //   }
  // }

  void SfBaseScreen::changeSubscreen(std::shared_ptr<GameScreen> newSubscreen) noexcept
  {
    {
      std::lock_guard lock(subscreenMutex_);
      if (currentSubscreen_ != nullptr)
      {
        currentSubscreen_->onExit();
      }

      currentSubscreen_ = newSubscreen;
    }

    if (newSubscreen != nullptr)
    {
      newSubscreen->onEnter();
    }
  }

  void SfBaseScreen::askExitConfirmation(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameService> gameService,
      std::shared_ptr<GameDisplay> gameDisplay,
      std::shared_ptr<SfBaseScreen> parentScreen,
      std::string const &msg) noexcept
  {
    std::vector<std::string> &&buttonLabels{"Yes", "No"};
    std::vector<std::function<void()>> &&buttonCallbacks{
        [window]()
        {
          window->close();
        },
        [parentScreen]()
        {
          parentScreen->changeSubscreen(nullptr);
        }};

    std::shared_ptr<GameScreen> confirmationScreen(new SfConfirmationScreen(
        window, gameService, gameDisplay, msg, buttonLabels, buttonCallbacks));

    parentScreen->changeSubscreen(confirmationScreen);
  }

  void SfBaseScreen::connectServer(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameService> gameService,
      std::shared_ptr<GameDisplay> gameDisplay,
      std::shared_ptr<SfBaseScreen> parentScreen) noexcept
  {
    std::shared_ptr<GameScreen> waitScreen(new SfBlockingScreen(
        window,
        gameService,
        gameDisplay,
        "Connecting to server...",
        {/*"Cancel"*/},
        {
            /*[parentScreen, gameService]()
            {
              // TODO: send cancelConnection
              //gameService->disconnect();
              parentScreen->changeSubscreen(nullptr);
            }*/
        }));

    parentScreen->changeSubscreen(waitScreen);

    gameService->connect(
        "",
        [parentScreen, window, gameDisplay, gameService](int code, std::string const &errMsg)
        {
          // if (code == 0)
          // {
          //   parentScreen->changeSubscreen(nullptr);
          // }
          // else
          // {
          //   std::vector<std::string> &&buttonLabels{"Retry", "Cancel"};
          //   std::vector<std::function<void()>> &&buttonCallbacks{
          //       [window, gameService, gameDisplay, parentScreen]()
          //       {
          //         parentScreen->changeSubscreen(nullptr);
          //         SfBaseScreen::connectServer(window, gameService, gameDisplay, parentScreen);
          //       },
          //       [parentScreen]()
          //       {
          //         parentScreen->changeSubscreen(nullptr);
          //       }};

          //   std::shared_ptr<GameScreen> retryScreen(new SfBlockingScreen(
          //       window,
          //       gameService,
          //       gameDisplay,
          //       errMsg + " (" + std::to_string(code) + ")",
          //       buttonLabels,
          //       buttonCallbacks));

          //   parentScreen->changeSubscreen(retryScreen);
          // }
        });
  }
}