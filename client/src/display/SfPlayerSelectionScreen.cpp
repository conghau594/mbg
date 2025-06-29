// SfPlayerSelectionScreen.cpp
#include "SfPlayerSelectionScreen.h"

#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads

#include "model/PlayerType.h"
#include "service/GameService.h"

#include "GameDisplay.h"
#include "SfBaseScreen.h"
#include "SfGameFindingScreen.h"

namespace bgg
{
  SfPlayerSelectionScreen::SfPlayerSelectionScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameDisplay> gameDisplay,
      std::vector<std::string> const &playerTypeNames,
      int gameType) noexcept
      : SfBaseScreen(std::move(window)),
        gameDisplay_(std::move(gameDisplay)),
        pressedButtonIndex_(-1),
        playerTypeNames_(playerTypeNames),
        gameType_(gameType)
  {
  }

  void SfPlayerSelectionScreen::doExit() noexcept
  {
  }

  void SfPlayerSelectionScreen::doEnter() noexcept
  {
  }

  void SfPlayerSelectionScreen::update(sf::Time const &elapsed) noexcept
  {
    // update the window display
    ImGui::SFML::Update(*window(), elapsed);
    layOutScreen();

    window()->clear();
    ImGui::SFML::Render(*window());
    window()->display();

    // handle button presses
    if (pressedButtonIndex_ >= 0 &&
        pressedButtonIndex_ < int(playerTypeNames_.size()))
    {
      int playerType = pressedButtonIndex_;

      std::shared_ptr<GameScreen> findingScreen = std::make_shared<SfGameFindingScreen>(
          window(), gameDisplay_, gameType_, playerType);

      gameDisplay_->pushScreen(findingScreen);
    }
    else if (pressedButtonIndex_ == int(playerTypeNames_.size())) // if Back button is pressed
    {
      gameDisplay_->popScreen();
    }

    pressedButtonIndex_ = -1;
  }

  void SfPlayerSelectionScreen::onWindowEventExceptClosed(
      std::optional<sf::Event> const &) noexcept
  {
  }

  void SfPlayerSelectionScreen::layOutScreen() noexcept
  {
    int constexpr FONT_VENITE_ADOREMUS_36 = 1;
    ImFont *font36 = ImGui::GetIO().Fonts->Fonts[FONT_VENITE_ADOREMUS_36];
    ImGui::PushFont(font36);

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

    ImVec2 constexpr BUTTON_SIZE(300.0f, 60.0f);
    ImVec2 constexpr DUMMY_SIZE(0.0f, 10.0f);

    ImGui::Begin("Select Player", nullptr, IM_GUI_FLAGS);

    int constexpr FONT_VENITE_ADOREMUS_24 = 2;
    ImFont *font24 = ImGui::GetIO().Fonts->Fonts[FONT_VENITE_ADOREMUS_24];
    ImGui::PushFont(font24);
    ImGui::Text("Play with . . .");
    ImGui::PopFont();

    for (int i = 0; i < (const int)playerTypeNames_.size(); ++i)
    {
      ImGui::Dummy(DUMMY_SIZE);
      if (ImGui::Button(playerTypeNames_[i].c_str(), BUTTON_SIZE) &&
          pressedButtonIndex_ < 0)
      {
        pressedButtonIndex_ = i; // Only allow one button to be pressed at a time
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
