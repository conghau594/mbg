// SfBaseScreen.cpp

#include <memory>

#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads
#include <imgui_internal.h>

#include "service/GameService.h"
#include "GameDisplay.h"
#include "SfBaseScreen.h"
#include "SfConfirmationScreen.h"

// #ifdef _DEBUG
#include <iostream>

// #endif
namespace bgg
{
  SfBaseScreen::SfBaseScreen(std::shared_ptr<sf::RenderWindow> window) noexcept
      : window_(window),
        clock_(new sf::Clock),
        isActive_(true),
        lastSubscreen_(nullptr),
        currentSubscreen_(nullptr)
  {
    BOOST_ASSERT_MSG(window, "window_ of SfBaseScreen cannot be null.");
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
    // currentSubscreen_ = nullptr;
    // lastSubscreen_ = nullptr;
    clock_->stop();
    doExit();
    isActive_ = false;
  }

  void SfBaseScreen::onWindowClosed()
  {
    askExitConfirmation(window_, shared_from_this(), "Do you want to quit?");
  }

  void SfBaseScreen::changeSubscreen(
      std::shared_ptr<GameScreen> newSubscreen) noexcept
  {

    // std::lock_guard lock(subscreenMutex_);
    if (currentSubscreen_ != nullptr)
    {
      currentSubscreen_->onExit();
    }

    currentSubscreen_ = newSubscreen;

    if (newSubscreen != nullptr)
    {
      newSubscreen->onEnter();
    }
  }

  void SfBaseScreen::askExitConfirmation(
      std::shared_ptr<sf::RenderWindow> window,
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
        window, msg, buttonLabels, buttonCallbacks));

    parentScreen->changeSubscreen(confirmationScreen);
  }
}