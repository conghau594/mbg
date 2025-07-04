// SfGameSelectionScreen.cpp
#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads

#include "model/GameType.h"
#include "model/PlayerType.h"

#include "service/GameService.h"

#include "SfGameSelectionScreen.h"
#include "SfGameDisplay.h"
#include "SfBaseScreen.h"
#include "SfConfirmationScreen.h"
#include "SfMessageScreen.h"
#include "SfPlayerSelectionScreen.h"

namespace bgg
{
  SfGameSelectionScreen::SfGameSelectionScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameDisplay> gameDisplay,
      std::vector<std::string> gameNames) noexcept
      : SfBaseScreen(std::move(window)),
        gameDisplay_(std::move(gameDisplay)),
        gameNames_(std::move(gameNames)),
        pressedButtonIndex_(-1)
  {
  }

  void SfGameSelectionScreen::doExit() noexcept
  {
  }

  void SfGameSelectionScreen::doEnter() noexcept
  {
  }

  void SfGameSelectionScreen::update(sf::Time const &elapsed) noexcept
  {
    // update the window display
    ImGui::SFML::Update(*window(), elapsed);
    layOutScreen();

    window()->clear();
    ImGui::SFML::Render(*window());
    window()->display();

    // handle button presses
    if (pressedButtonIndex_ >= 0 && pressedButtonIndex_ < int(gameNames_.size()))
    {
      int gameType = pressedButtonIndex_;
      std::vector<std::string> playerTypeNames(
          std::begin(PlayerType::NAMES), std::end(PlayerType::NAMES));
      std::shared_ptr<GameScreen> playerSelectionScreen = std::make_shared<SfPlayerSelectionScreen>(
          window(), gameDisplay_, playerTypeNames, gameType);

      gameDisplay_->pushScreen(playerSelectionScreen);
      // deactivate();
    }
    else if (pressedButtonIndex_ == int(gameNames_.size()))
    {
      askExitConfirmation(window(), shared_from_this(), "Are you sure?");
    }

    pressedButtonIndex_ = -1;
  }

  void SfGameSelectionScreen::onWindowEventExceptClosed(
      std::optional<sf::Event> const &) noexcept
  {
  }

  void SfGameSelectionScreen::layOutScreen() noexcept
  {
    int constexpr FONT_VENITE_ADOREMUS_36 = 1;
    ImFont *font36 = ImGui::GetIO().Fonts->Fonts[FONT_VENITE_ADOREMUS_36];
    ImGui::PushFont(font36);

    ImVec2 center(0.5f * float(window()->getSize().x),
                  0.5f * float(window()->getSize().y));
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

    for (size_t i = 0; i < gameNames_.size(); ++i)
    {
      if (ImGui::Button(gameNames_[i].c_str(), BUTTON_SIZE) &&
          pressedButtonIndex_ < 0)
      {
        pressedButtonIndex_ = int(i); // Only allow one button to be pressed at a time
      }
      ImGui::Dummy(DUMMY_SIZE);
    }

    ImGui::Separator();
    ImGui::Dummy(DUMMY_SIZE);

    if (ImGui::Button("Quit", BUTTON_SIZE) && pressedButtonIndex_ < 0)
    {
      pressedButtonIndex_ = int(gameNames_.size()); // Only allow one button to be pressed at a time
    }

    ImGui::End();

    ImGui::PopFont();
  }
} // namespace bgg
