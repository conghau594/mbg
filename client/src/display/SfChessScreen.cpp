// SfChessScreen.cpp

#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads

#include <functional>

#include "GameDisplay.h"
#include "SfChessScreen.h"
#include "SfBlockingScreen.h"

namespace iab
{
  SfChessScreen::SfChessScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<ScreenManager> screenMgr,
      SfTileMap tileMap) noexcept
      : SfBaseScreen(window, screenMgr),
        tileMap_(std::move(tileMap)),
        pressedButtonIndex_(-1)
  {
  }

  void SfChessScreen::update(sf::Time const &elapsed)
  {
    // if (elapsed == sf::Time::Zero || !isActive())
    // {
    //   // If no time has passed or the screen should deactivate, do nothing
    //   return;
    // }

    // update the window display
    ImGui::SFML::Update(*window(), elapsed);
    drawResignButton();

    window()->clear();
    window()->draw(tileMap_);
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
            std::shared_ptr<GameScreen> waitScreen(new SfBlockingScreen(
                window(),
                screenManager(),
                "Wait a second...",
                {},
                {}));

            // TODO: Send resign request
            screenManager()->popScreen();
            screenManager()->pushScreen(waitScreen);

            screenManager()->popScreen();
            screenManager()->popScreen();
          },
          [this]()
          {
            screenManager()->popScreen();
          }};

      std::shared_ptr<GameScreen> pauseScreen(new SfBlockingScreen(
          window(), screenManager(), msg, buttonLabels, buttonCallbacks));

      screenManager()->pushScreen(pauseScreen);
      deactivate();
    }

    pressedButtonIndex_ = -1;
  }

  void SfChessScreen::onWindowEventExceptClosed(std::optional<sf::Event> const &event)
  {
    if (const auto *windowResized = event->getIf<sf::Event::Resized>())
    {
      mapRegionBotRight_ = windowResized->size;
      tileMap_.fitRectangle(mapRegionTopLeft_, mapRegionBotRight_);
    }
    else
    {
    }
  }

  void SfChessScreen::doEnter()
  {
    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->Clear();

    char constexpr FONT_PATH[] = "resource/VeniteAdoremus-rgRBA.ttf";
    float constexpr FONT_SIZE = 24.0f;
    io.Fonts->AddFontFromFileTTF(FONT_PATH, FONT_SIZE);
    if (!ImGui::SFML::UpdateFontTexture())
    {
      // TODO: Handle the error more gracefully, e.g., log it or show a message to the user
      // ImGui::Begin("!", nullptr);
      // std::string errorMsg = "Failed to load font " + std::string(FONT_PATH);
      // ImGui::Text(errorMsg.c_str());
      // ImGui::End();
    }

    mapRegionTopLeft_ = {0, 60};
    mapRegionBotRight_ = window()->getSize();

    tileMap_.fitRectangle(mapRegionTopLeft_, mapRegionBotRight_);
  }

  void SfChessScreen::doExit()
  {
    // tileMap_.doExit();
  }

  void SfChessScreen::drawResignButton() noexcept
  {
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

    ImVec2 const BUTTON_SIZE(120.0f, mapRegionTopLeft_.y - VERTICAL_SPACING * 2.0f);
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
  }
}