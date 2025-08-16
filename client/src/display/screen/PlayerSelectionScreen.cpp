// PlayerSelectionScreen.cpp
#include "PlayerSelectionScreen.h"

#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads

#include "base/Logger.h"
#include "model/PlayerType.h"
#include "model/GameType.h"
#include "service/GameService.h"

#include "display/IDisplay.h"
#include "BaseScreen.h"
#include "SideSelectionScreen.h"

namespace bgg
{
  PlayerSelectionScreen::PlayerSelectionScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<IDisplay> gameDisplay,
      std::vector<std::string> playerTypeNames,
      int gameType) noexcept
      : BaseScreen(std::move(window)),
        gameDisplay_(std::move(gameDisplay)),
        playerTypeNames_(std::move(playerTypeNames)),
        pressedButtonIndex_(-1),
        gameType_(gameType)
  {
  }

  void PlayerSelectionScreen::doExit() noexcept
  {
  }

  void PlayerSelectionScreen::doEnter() noexcept
  {
  }

  void PlayerSelectionScreen::update(sf::Time const &elapsed) noexcept
  {
    // update the window display
    ImGui::SFML::Update(*getWindow(), elapsed);
    layOutScreen();

    getWindow()->clear();
    ImGui::SFML::Render(*getWindow());
    getWindow()->display();

    // handle button presses
    if (pressedButtonIndex_ >= 0 &&
        pressedButtonIndex_ < int(playerTypeNames_.size()))
    {
      int playerType = pressedButtonIndex_;
      if (playerType != PlayerType::HUMAN)
      {
        if (gameType_ == GameType::CHESS)
        {
          std::vector<std::string> sideTypeNames{"White", "Black"};
          std::shared_ptr<IScreen>
              sideSelectionScreen = std::make_shared<SideSelectionScreen>(
                  getWindow(), gameDisplay_, sideTypeNames, gameType_, playerType);

          gameDisplay_->pushScreen(sideSelectionScreen);
        }
      } // deactivate();
    }
    else if (pressedButtonIndex_ == int(playerTypeNames_.size())) // if Back button is pressed
    {
      gameDisplay_->popScreen();
    }

    pressedButtonIndex_ = -1;
  }

  void PlayerSelectionScreen::onWindowEventExceptClosed(
      std::optional<sf::Event> const &) noexcept
  {
  }

  void PlayerSelectionScreen::layOutScreen() noexcept
  {
    int constexpr FONT_VENITE_ADOREMUS_36 = 1;
    ImFont *font36 = ImGui::GetIO().Fonts->Fonts[FONT_VENITE_ADOREMUS_36];
    ImGui::PushFont(font36);

    ImVec2 center(0.5f * float(getWindow()->getSize().x),
                  0.5f * float(getWindow()->getSize().y));
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    // ImGuiIO &io = ImGui::GetIO();
    // io.FontGlobalScale = 2.0f;

    int constexpr IM_GUI_FLAGS = int(ImGuiWindowFlags_NoBackground) |
                                 int(ImGuiWindowFlags_NoSavedSettings) |
                                 int(ImGuiWindowFlags_NoResize) |
                                 int(ImGuiWindowFlags_NoCollapse) |
                                 int(ImGuiWindowFlags_NoDecoration) |
                                 int(ImGuiWindowFlags_AlwaysAutoResize) |
                                 int(ImGuiWindowFlags_NoMove);

    ImVec2 constexpr BUTTON_SIZE(300.0f, 60.0f);
    ImVec2 constexpr DUMMY_SIZE(0.0f, 10.0f);

    ImGui::Begin("Select Player", nullptr, IM_GUI_FLAGS);

    int constexpr FONT_VENITE_ADOREMUS_24 = 2;
    ImFont *font24 = ImGui::GetIO().Fonts->Fonts[FONT_VENITE_ADOREMUS_24];
    ImGui::PushFont(font24);
    ImGui::Text("Play  with . . .");
    ImGui::PopFont();

    for (size_t i = 0; i < playerTypeNames_.size(); ++i)
    {
      ImGui::Dummy(DUMMY_SIZE);
      if (ImGui::Button(playerTypeNames_[i].c_str(), BUTTON_SIZE) &&
          pressedButtonIndex_ < 0)
      {
        pressedButtonIndex_ = int(i); // Only allow one button to be pressed at a time
      }
    }

    ImGui::Dummy(ImVec2(DUMMY_SIZE.x * 2.0f, DUMMY_SIZE.y * 2.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(DUMMY_SIZE.x * 2.0f, DUMMY_SIZE.y * 2.0f));

    if (ImGui::Button("Back", BUTTON_SIZE) && pressedButtonIndex_ < 0)
    {
      pressedButtonIndex_ = int(playerTypeNames_.size()); // Only allow one button to be pressed at a time
    }

    ImGui::End();
    ImGui::PopFont();
  }
} // namespace bgg
