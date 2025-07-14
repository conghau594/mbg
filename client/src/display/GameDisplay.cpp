// GameDisplay.cpp

#include "GameDisplay.h"

#include <optional>
#include <memory>
#include <string>

#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads

#include "IScreen.h"
namespace bgg
{
  GameDisplay::GameDisplay(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<ClientEventBus> eventBus)
      : window_(std::move(window)),
        eventBus_(std::move(eventBus)),
        currentScreen_(nullptr)
  {
    BOOST_ASSERT_MSG(window_, "window_ of GameDisplay cannot be null.");
    BOOST_ASSERT_MSG(eventBus_, "eventBus_ of GameApp cannot be null.");

    if (!ImGui::SFML::Init(*window_))
    { // TODO: Define exception for this
      throw std::runtime_error("Failed to initialize ImGui.");
    }

    ImGuiIO &io = ImGui::GetIO();

    io.Fonts->AddFontFromFileTTF("resource/VeniteAdoremus-rgRBA.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("resource/VeniteAdoremus-rgRBA.ttf", 24.0f);
    io.Fonts->AddFontFromFileTTF("resource/Consola.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("resource/Consola.ttf", 24.0f);

    if (!ImGui::SFML::UpdateFontTexture())
    {
      throw std::runtime_error("Failed to update ImGui font texture.");
    }

    subscribeServerMessages();
  }

  GameDisplay::~GameDisplay() noexcept
  {
    ImGui::SFML::Shutdown();
    unsubscribeServerMessages();
  }

  void GameDisplay::run()
  {
    while (window_->isOpen())
    {
      if (screenStack_.empty())
      {
        // TODO: Need to add a default/empty screen instread of exiting the app
        break;
      }

      BOOST_ASSERT_MSG(screenStack_.back() != nullptr,
                       "The screen cannot be empty.");
      if (currentScreen_ != screenStack_.back())
      {
        currentScreen_ = screenStack_.back();
      }
      {
        std::lock_guard lock(messageMutex_);
        currentScreen_->handleServerMessages(serverMessages_);
      }
      currentScreen_->update();
    }
  }

  void GameDisplay::send(ClientRequest const &request) noexcept
  {
    eventBus_->emit<ClientRequest>(request);
  }

  void GameDisplay::subscribeServerMessages()
  {
    using MessagePack = typename ServerMessage::Event::Pack;
    [this]<std::size_t... I>(std::index_sequence<I...>)
    {
      (subscribe<MessagePack::At<I>>(), ...);
    }(std::make_index_sequence<MessagePack::Count>{});
  }

  void GameDisplay::unsubscribeServerMessages()
  {
    for (auto &id : subscriptionIDs_)
    {
      std::size_t removedCount = eventBus_->unsubscribe(id);

      //==============
      SPDLOG_INFO("{} message handler(s) with ID = {} has been removed from {}",
                  removedCount, id, typeid(*this).name());
      //==============
    }
    subscriptionIDs_.clear();
  }

  void GameDisplay::pushScreen(std::shared_ptr<IScreen> newScreen) noexcept
  {
    if (!screenStack_.empty())
    {
      screenStack_.back()->onExit();
    }

    screenStack_.push_back(std::move(newScreen));
    screenStack_.back()->onEnter();
  }

  void GameDisplay::popScreen() noexcept
  {
    BOOST_ASSERT_MSG(
        !screenStack_.empty(),
        "You should not pop a screen while the screen stack is empty");

    screenStack_.back()->onExit();
    screenStack_.pop_back();

    if (!screenStack_.empty())
    {
      screenStack_.back()->onEnter();
    }
  }

  void GameDisplay::changeScreen(std::shared_ptr<IScreen> newScreen) noexcept
  {
    BOOST_ASSERT_MSG(
        !screenStack_.empty(),
        "You should not change screen while the screen stack is empty");

    auto &lastScreen = screenStack_.back();
    lastScreen->onExit();
    lastScreen = std::move(newScreen);
    lastScreen->onEnter();
  }
}