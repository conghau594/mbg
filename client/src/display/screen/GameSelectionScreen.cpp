// GameSelectionScreen.cpp
#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads

#include "model/GameType.h"
#include "model/PlayerType.h"

#include "service/IGameServer.h"

#include "GameSelectionScreen.h"
#include "display/IDisplay.h"
#include "BaseScreen.h"
#include "ConfirmationScreen.h"
#include "MessageScreen.h"
#include "PlayerSelectionScreen.h"

namespace bgg
{
  GameSelectionScreen::GameSelectionScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<IDisplay> gameDisplay,
      std::vector<std::string> gameNames) noexcept
      : BaseScreen(std::move(window)),
        gameDisplay_(std::move(gameDisplay)),
        gameNames_(std::move(gameNames)),
        pressedButtonIndex_(-1)
  {
  }

  void GameSelectionScreen::doExit() noexcept
  {
  }

  void GameSelectionScreen::doEnter() noexcept
  {
  }

  void GameSelectionScreen::update(sf::Time const &elapsed) noexcept
  {
    // update the window display
    ImGui::SFML::Update(*getWindow(), elapsed);
    layOutScreen();

    getWindow()->clear();
    ImGui::SFML::Render(*getWindow());
    getWindow()->display();

    // handle button presses
    if (pressedButtonIndex_ >= 0 &&
        pressedButtonIndex_ < int(gameNames_.size()))
    {
      int gameType = pressedButtonIndex_;
      std::vector<std::string> playerTypeNames(
          std::begin(PlayerType::NAMES), std::end(PlayerType::NAMES));
      std::shared_ptr<IScreen>
          playerSelectionScreen = std::make_shared<PlayerSelectionScreen>(
              getWindow(), gameDisplay_, playerTypeNames, gameType);

      gameDisplay_->pushScreen(playerSelectionScreen);
      // deactivate();
    }
    else if (pressedButtonIndex_ == int(gameNames_.size()))
    {
      askExitConfirmation(getWindow(), shared_from_this(), "Are you sure?");
    }

    pressedButtonIndex_ = -1;
  }

  void GameSelectionScreen::onWindowEventExceptClosed(
      std::optional<sf::Event> const &) noexcept
  {
  }

  void GameSelectionScreen::layOutScreen() noexcept
  {
    int constexpr FONT_VENITE_ADOREMUS_36 = 1;
    ImFont *font36 = ImGui::GetIO().Fonts->Fonts[FONT_VENITE_ADOREMUS_36];
    ImGui::PushFont(font36);

    ImVec2 center(0.5f * float(getWindow()->getSize().x),
                  0.5f * float(getWindow()->getSize().y));
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    int constexpr IM_GUI_FLAGS = int(ImGuiWindowFlags_NoBackground) |
                                 int(ImGuiWindowFlags_NoSavedSettings) |
                                 int(ImGuiWindowFlags_NoResize) |
                                 int(ImGuiWindowFlags_NoCollapse) |
                                 int(ImGuiWindowFlags_NoDecoration) |
                                 int(ImGuiWindowFlags_AlwaysAutoResize) |
                                 int(ImGuiWindowFlags_NoMove);

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
