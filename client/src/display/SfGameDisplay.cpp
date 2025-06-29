// SfGameDisplay.cpp

#include "SfGameDisplay.h"

#include <optional>
#include <memory>
#include <string>

#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads

#include "GameScreen.h"

namespace bgg
{
  SfGameDisplay::SfGameDisplay(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<ClientEventBus> eventBus)
      : window_(std::move(window)),
        eventBus_(std::move(eventBus)),
        currentScreen_(nullptr)
  {
    BOOST_ASSERT_MSG(window_, "window_ of SfGameDisplay cannot be null.");
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
  }

  SfGameDisplay::~SfGameDisplay() noexcept
  {
    ImGui::SFML::Shutdown();
  }

  void SfGameDisplay::run()
  {
    while (window_->isOpen())
    {
      if (screenStack_.empty())
      {
        // TODO: Need to add a default/empty screen instread of exiting the app
        break;
      }
      if (currentScreen_ != screenStack_.back())
      {
        currentScreen_ = screenStack_.back();
      }
      currentScreen_->update();
    }
  }

  void SfGameDisplay::send(ClientEvent const &request) noexcept
  {
    eventBus_->emit<ClientEvent>(request);
  }

  void SfGameDisplay::pushScreen(std::shared_ptr<GameScreen> newScreen) noexcept
  {
    if (!screenStack_.empty())
    {
      screenStack_.back()->onExit();
    }

    screenStack_.push_back(std::move(newScreen));
    screenStack_.back()->onEnter();
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
    auto &lastScreen = screenStack_.back();
    lastScreen->onExit();
    lastScreen = std::move(newScreen);
    lastScreen->onEnter();
  }
}