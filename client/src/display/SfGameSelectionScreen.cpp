// SfGameSelectionScreen.cpp
#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>

#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads
#include <imgui_internal.h>

#include "model/GameType.h"

#include "SfGameDisplay.h"
#include "SfBaseScreen.h"
#include "SfGameSelectionScreen.h"
#include "SfPlayerSelectionScreen.h"

namespace iab
{
  SfGameSelectionScreen::SfGameSelectionScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameDisplay> displayContext) noexcept
      : SfBaseScreen(window, displayContext),
        elapsedTime_(sf::Time::Zero),
        pressedButtonIndex_(-1)
  {
  }

  void SfGameSelectionScreen::onExit() noexcept
  {
    elapsedTime_ += clock()->reset();
    if (elapsedTime_ == sf::Time::Zero)
    {
      return;
    }
    update(elapsedTime_);
  }

  void SfGameSelectionScreen::onEnter() noexcept
  {
    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->Clear();

    char constexpr FONT_PATH[] = "resource/VeniteAdoremus-rgRBA.ttf";
    float constexpr FONT_SIZE = 36.0f;
    io.Fonts->AddFontFromFileTTF(FONT_PATH, FONT_SIZE);
    if (!ImGui::SFML::UpdateFontTexture())
    {
      // TODO: Handle the error more gracefully, e.g., log it or show a message to the user
      // ImGui::Begin("!", nullptr);
      // std::string errorMsg = "Failed to load font " + std::string(FONT_PATH);
      // ImGui::Text(errorMsg.c_str());
      // ImGui::End();
    }

    clock()->restart();
  }

  void SfGameSelectionScreen::drawMenu() noexcept
  {
    ImVec2 center(window()->getSize().x * 0.5f, window()->getSize().y * 0.5f);
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    // ImGuiIO &io = ImGui::GetIO();
    // io.FontGlobalScale = 2.0f;

    int constexpr IM_GUI_FLAGS = ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoSavedSettings |
                                 ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoCollapse |
                                 ImGuiWindowFlags_NoDecoration |
                                 ImGuiWindowFlags_AlwaysAutoResize |
                                 ImGuiWindowFlags_NoMove;

    ImVec2 constexpr BUTTON_SIZE(400.0f, 80.0f);
    ImVec2 constexpr DUMMY_SIZE(0.0f, 20.0f);

    ImGui::Begin("Select Game", nullptr, IM_GUI_FLAGS);

    if (ImGui::Button(GameType::toString(GameType::WESTERN_CHESS).value().c_str(), BUTTON_SIZE) &&
        pressedButtonIndex_ < 0)
    {
      pressedButtonIndex_ = GameType::WESTERN_CHESS; // Only allow one button to be pressed at a time
    }

    ImGui::Dummy(DUMMY_SIZE);

    if (ImGui::Button(GameType::toString(GameType::CHINESE_CHESS).value().c_str(), BUTTON_SIZE) &&
        pressedButtonIndex_ < 0)
    {
      pressedButtonIndex_ = GameType::CHINESE_CHESS; // Only allow one button to be pressed at a time
    }

    ImGui::Dummy(DUMMY_SIZE);
    ImGui::Separator();
    ImGui::Dummy(DUMMY_SIZE);

    if (ImGui::Button("Exit", BUTTON_SIZE) && pressedButtonIndex_ < 0)
    {
      pressedButtonIndex_ = 2; // Only allow one button to be pressed at a time
    }

    ImGui::End();
  }

  void SfGameSelectionScreen::onTimeElapsed() noexcept
  {
    elapsedTime_ += clock()->restart();
    if (elapsedTime_ == sf::Time::Zero)
    {
      return;
    }
    update(elapsedTime_);

    switch (pressedButtonIndex_)
    {
    case GameType::WESTERN_CHESS:
    case GameType::CHINESE_CHESS:
    {
      int gameType = pressedButtonIndex_;
      std::shared_ptr<GameScreen> playerSelectionScreen(new SfPlayerSelectionScreen(
          window(), gameDisplay(), gameType));

      gameDisplay()->pushScreen(playerSelectionScreen);
      break;
    }

    case 2:
      askExitConfirmation();
      break;

    default:
      break;
    }

    pressedButtonIndex_ = -1;
    elapsedTime_ = sf::Time::Zero;
  }

  void SfGameSelectionScreen::onEvent(std::optional<sf::Event> const &) noexcept
  {
  }

  void SfGameSelectionScreen::update(sf::Time const &elapsedTime) noexcept
  {
    ImGui::SFML::Update(*window(), elapsedTime);
    drawMenu();
    window()->clear();
    ImGui::SFML::Render(*window());
    window()->display();
  }

} // namespace iab
