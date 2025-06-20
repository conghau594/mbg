// SfGameSelectionScreen.cpp
#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>

#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads
#include <imgui_internal.h>

#include "model/GameType.h"
#include "model/PlayerType.h"

#include "SfGameDisplay.h"
#include "SfBaseScreen.h"
#include "SfGameSelectionScreen.h"
#include "SfPlayerSelectionScreen.h"

namespace iab
{
  SfGameSelectionScreen::SfGameSelectionScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameDisplay> gameDisplay,
      std::vector<std::string> gameNames) noexcept
      : SfBaseScreen(window, gameDisplay),
        gameNames_(std::move(gameNames)),
        pressedButtonIndex_(-1)
  {
  }

  void SfGameSelectionScreen::doExit() noexcept
  {
  }

  void SfGameSelectionScreen::doEnter() noexcept
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
  }

  void SfGameSelectionScreen::update(sf::Time const &elapsed) noexcept
  {
    if (pressedButtonIndex_ >= 0 && pressedButtonIndex_ < int(gameNames_.size()))
    {
      int gameType = pressedButtonIndex_;
      std::vector<std::string> playerTypeNames(std::begin(PlayerType::NAMES), std::end(PlayerType::NAMES));
      std::shared_ptr<GameScreen> playerSelectionScreen(new SfPlayerSelectionScreen(
          window(), gameDisplay(), playerTypeNames, gameType));

      gameDisplay()->pushScreen(playerSelectionScreen);
      pressedButtonIndex_ = -1;
      return;
    }
    else if (pressedButtonIndex_ == int(gameNames_.size()))
    {
      askExitConfirmation(window(), gameDisplay());
      pressedButtonIndex_ = -1;
      return;
    }

    pressedButtonIndex_ = -1;
    if (elapsed == sf::Time::Zero || shouldExit())
    {
      // If no time has passed or the screen should exit, do nothing
      return;
    }

    ImGui::SFML::Update(*window(), elapsed);
    drawMenu();

    window()->clear();
    ImGui::SFML::Render(*window());
    window()->display();
  }

  void SfGameSelectionScreen::onWindowEventExceptClosed(std::optional<sf::Event> const &) noexcept
  {
  }

  void SfGameSelectionScreen::drawMenu() noexcept
  {
    ImVec2 center(window()->getSize().x * 0.5f, window()->getSize().y * 0.5f);
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    int constexpr IM_GUI_FLAGS = ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoSavedSettings |
                                 ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoCollapse |
                                 ImGuiWindowFlags_NoDecoration |
                                 ImGuiWindowFlags_AlwaysAutoResize |
                                 ImGuiWindowFlags_NoMove;

    ImVec2 constexpr BUTTON_SIZE(400.0f, 70.0f);
    ImVec2 constexpr DUMMY_SIZE(0.0f, 10.0f);

    ImGui::Begin("Select Game", nullptr, IM_GUI_FLAGS);

    for (int i = 0; i < (const int)gameNames_.size(); ++i)
    {
      if (ImGui::Button(gameNames_[i].c_str(), BUTTON_SIZE) && pressedButtonIndex_ < 0)
      {
        pressedButtonIndex_ = i; // Only allow one button to be pressed at a time
      }
      ImGui::Dummy(DUMMY_SIZE);
    }

    ImGui::Separator();
    ImGui::Dummy(DUMMY_SIZE);

    if (ImGui::Button("Exit", BUTTON_SIZE) && pressedButtonIndex_ < 0)
    {
      pressedButtonIndex_ = int(gameNames_.size()); // Only allow one button to be pressed at a time
    }

    ImGui::End();
  }
} // namespace iab
