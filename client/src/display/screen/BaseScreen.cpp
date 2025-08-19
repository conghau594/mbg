// BaseScreen.cpp

#include <memory>

#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads
#include <imgui_internal.h>

#include "service/IGameServer.h"
#include "display/IDisplay.h"
#include "BaseScreen.h"
#include "ConfirmationScreen.h"

namespace bgg
{
  BaseScreen::BaseScreen(std::shared_ptr<sf::RenderWindow> window) noexcept
      : window_(std::move(window)),
        lastSubscreen_(nullptr),
        currentSubscreen_(nullptr),
        isActive_(true)
  {
    BOOST_ASSERT_MSG(window_, "window_ of BaseScreen cannot be null.");
  }

  void BaseScreen::update()
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

    if (!isActive_)
    {
      window_->clear();
      window_->display();
      return;
    }

    sf::Time elapsed = clock_.restart();
    if (elapsed == sf::Time::Zero)
    {
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

  void BaseScreen::onEnter()
  {
    isActive_ = true;
    doEnter();
    clock_.start();
  }

  void BaseScreen::onExit()
  {
    clock_.stop();
    doExit();
    isActive_ = false;
  }

  void BaseScreen::handleServerMessages(std::list<ServerMessage> &messages) noexcept
  {
    for (auto msg = messages.begin(); msg != messages.end();)
    {
      bool isMsgHandled = msg->visit(serverMessageHandler_);
      if (isMsgHandled)
      {
        msg = messages.erase(msg);
      }
      else
      {
        std::advance(msg, 1);
      }
    }
  }

  void BaseScreen::activate(bool active) noexcept
  {
    isActive_ = active;
  }

  void BaseScreen::onWindowClosed()
  {
    askExitConfirmation(window_, shared_from_this(), "Do you want to quit?");
  }

  void BaseScreen::changeSubscreen(
      std::shared_ptr<IScreenInternal> newSubscreen) noexcept
  {
    if (currentSubscreen_ != nullptr)
    {
      currentSubscreen_->onExit();
    }

    currentSubscreen_ = std::move(newSubscreen);

    if (currentSubscreen_ != nullptr)
    {
      currentSubscreen_->onEnter();
    }
  }

  void BaseScreen::askExitConfirmation(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<IScreenInternal> parentScreen,
      std::string const &msg) noexcept
  {
    std::vector<std::string> &&buttonLabels{"Yes", "No"};
    std::weak_ptr<IScreenInternal> parentScreenWeakPtr(parentScreen);
    std::vector<std::function<void()>> &&buttonCallbacks{
        [window, parentScreenWeakPtr]()
        {
          if (auto parentScreen = parentScreenWeakPtr.lock())
          {
            parentScreen->changeSubscreen(nullptr);
          }
          window->close();
        },
        [parentScreenWeakPtr]()
        {
          if (auto parentScreen = parentScreenWeakPtr.lock())
          {
            parentScreen->changeSubscreen(nullptr);
          }
        }};

    std::shared_ptr<IScreenInternal>
        confirmationScreen = std::make_shared<ConfirmationScreen>(
            window, msg, buttonLabels, buttonCallbacks);

    parentScreen->changeSubscreen(confirmationScreen);
  }
}