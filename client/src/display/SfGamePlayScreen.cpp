// SfGamePlayScreen.cpp

#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads

#include <functional>

#include "GameDisplay.h"
#include "SfGamePlayScreen.h"
#include "SfConfirmationScreen.h"

#include "board/SfGameBoard.h"

namespace bgg
{
  SfGamePlayScreen::SfGamePlayScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameDisplay> gameDisplay,
      std::shared_ptr<SfGameBoard> gameBoard,
      unsigned resignButtonRegionHeight) noexcept
      : SfBaseScreen(std::move(window)),
        gameDisplay_(std::move(gameDisplay)),
        gameBoard_(std::move(gameBoard)),
        resignButtonRegionHeight_(resignButtonRegionHeight),
        pressedButtonIndex_(-1)
  {
  }

  void SfGamePlayScreen::update(sf::Time const &elapsed)
  {
    // update the window display
    ImGui::SFML::Update(*window(), elapsed);
    layOutScreen();

    window()->clear();
    window()->draw(*gameBoard_);
    ImGui::SFML::Render(*window());
    window()->display();

    // handle button presses
    if (pressedButtonIndex_ == 0)
    {
      char constexpr msg[] = "Are you sure you want to resign?";
      std::vector<std::string> &&buttonLabels{"Yes", "No"};
      std::vector<std::function<void()>> &&buttonCallbacks{
          [this]()
          {
            std::shared_ptr<GameScreen> waitScreen(new SfConfirmationScreen(
                window(), "Wait a second...", {}, {}));

            // TODO: Send resign request
            gameDisplay_->popScreen();
            gameDisplay_->pushScreen(waitScreen);

            gameDisplay_->popScreen();
            gameDisplay_->popScreen();
          },
          [this]()
          {
            gameDisplay_->popScreen();
          }};

      std::shared_ptr<GameScreen> pauseScreen(new SfConfirmationScreen(
          window(), msg, buttonLabels, buttonCallbacks));

      gameDisplay_->pushScreen(pauseScreen);
      // deactivate();
    }

    pressedButtonIndex_ = -1;
  }

  void SfGamePlayScreen::onWindowEventExceptClosed(std::optional<sf::Event> const &event)
  {
    if (event.has_value())
    {
      gameBoard_->onEvent(*event);
    }
  }

  void SfGamePlayScreen::doEnter()
  {
  }

  void SfGamePlayScreen::doExit()
  {
    // tileMap_.doExit();
  }

  void SfGamePlayScreen::layOutScreen() noexcept
  {
    int constexpr FONT_VENITE_ADOREMUS_24 = 2;
    ImFont *font24 = ImGui::GetIO().Fonts->Fonts[FONT_VENITE_ADOREMUS_24];
    ImGui::PushFont(font24);

    char constexpr buttonLabel[] = "Resign";

    ImVec2 const TEXT_SIZE = ImGui::CalcTextSize(buttonLabel);
    ImGuiStyle const &style = ImGui::GetStyle();
    // ImVec2 const ITEM_SPACING = style.ItemSpacing;
    ImVec2 const MENU_PADDING = style.WindowPadding;

    float constexpr VERTICAL_SPACING = 10.0f;
    float const buttonVerticalOffset = VERTICAL_SPACING + TEXT_SIZE.y * 0.5f + MENU_PADDING.y; // Offset from the top of the window
    ImVec2 center(window()->getSize().x * 0.5f, buttonVerticalOffset);
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    int constexpr IM_GUI_FLAGS = ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoSavedSettings |
                                 ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoCollapse |
                                 ImGuiWindowFlags_NoDecoration |
                                 ImGuiWindowFlags_AlwaysAutoResize |
                                 ImGuiWindowFlags_NoMove;

    ImVec2 const BUTTON_SIZE(120.0f, resignButtonRegionHeight_ - VERTICAL_SPACING * 2.0f);
    // ImVec2 constexpr DUMMY_SIZE(0.0f, 10.0f);

    ImGui::Begin("Select Game", nullptr, IM_GUI_FLAGS);

    int numStyleColorsPushed = 0;

    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
    ++numStyleColorsPushed;

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    ++numStyleColorsPushed;

    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.1f, 0.1f, 1.0f));
    ++numStyleColorsPushed;

    if (ImGui::Button(buttonLabel, BUTTON_SIZE) && pressedButtonIndex_ < 0)
    {
      pressedButtonIndex_ = 0; // Only allow one button to be pressed at a time
    }

    ImGui::PopStyleColor(numStyleColorsPushed);

    ImGui::End();

    ImGui::PopFont();
  }
}