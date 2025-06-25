// SfBlockingScreen.cpp

#include <optional>
#include <memory>

#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads

#include "SfBlockingScreen.h"

namespace iab
{
  SfBlockingScreen::SfBlockingScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameService> gameService,
      std::shared_ptr<GameDisplay> gameDisplay_,
      std::string message,
      std::vector<std::string> buttonLabels,
      std::vector<std::function<void()>> buttonCallbacks) noexcept
      : SfBaseScreen(window, gameService, gameDisplay_),
        message_(std::move(message)),
        buttonLabels_(std::move(buttonLabels)),
        buttonCallbacks_(std::move(buttonCallbacks)),
        pressedButtonIndex_(-1)
  {
    BOOST_ASSERT_MSG(buttonLabels_.size() <= buttonCallbacks_.size(),
                     "Number of button labels must be less than or equal to number of button callbacks");
  }

  void SfBlockingScreen::doEnter() noexcept
  {
  }

  void SfBlockingScreen::doExit() noexcept
  {
  }

  void SfBlockingScreen::update(sf::Time const &elapsed) noexcept
  {
    // update the window display
    ImGui::SFML::Update(*window(), elapsed);
    layOutScreen();
    window()->clear();
    ImGui::SFML::Render(*window());
    window()->display();

    // handle button presses
    if (pressedButtonIndex_ >= 0 &&
        pressedButtonIndex_ < (int)buttonCallbacks_.size())
    {
      buttonCallbacks_[pressedButtonIndex_]();
      pressedButtonIndex_ = -1; // Reset after executing the callback
    }
  }

  void SfBlockingScreen::onWindowEventExceptClosed(
      std::optional<sf::Event> const &) noexcept
  {
  }

  void SfBlockingScreen::layOutScreen() noexcept
  {
    int constexpr FONT_CONSOLA_24 = 4;
    ImFont *font24 = ImGui::GetIO().Fonts->Fonts[FONT_CONSOLA_24];
    ImGui::PushFont(font24);

    // Calculate button size
    int const BUTTON_COUNT = int(buttonLabels_.size());
    ImVec2 const TEXT_SIZE = ImGui::CalcTextSize(message_.c_str());
    ImGuiStyle const &style = ImGui::GetStyle();

    ImVec2 const ITEM_SPACING = style.ItemSpacing;
    ImVec2 const MENU_PADDING = style.WindowPadding;
    // float const TITLE_BAR_HEIGHT = ImGui::GetFontSize() + style.FramePadding.y * 2.0f;
    ImVec2 constexpr DUMMY_SIZE(0.0f, 8.0f);

    float const MENU_WIDTH = TEXT_SIZE.x + MENU_PADDING.x * 2.0f + 50.0f;

    float const BUTTON_WIDTH = BUTTON_COUNT == 0
                                   ? 0.0f
                                   : (MENU_WIDTH - 2 * MENU_PADDING.x - (BUTTON_COUNT - 1) * ITEM_SPACING.x) / BUTTON_COUNT;
    float const BUTTON_HEIGHT = BUTTON_COUNT == 0 ? 0.0f : 40.0f;
    ImVec2 BUTTON_SIZE(BUTTON_WIDTH, BUTTON_HEIGHT);

    // change window size
    float const MENU_HEIGHT = /*TITLE_BAR_HEIGHT +*/ MENU_PADDING.y * 2.0f +
                              TEXT_SIZE.y + BUTTON_HEIGHT + ITEM_SPACING.y * 2.0f +
                              DUMMY_SIZE.y;

    ImGui::SetNextWindowSize(ImVec2(MENU_WIDTH, MENU_HEIGHT), ImGuiCond_Always);

    // change window position
    ImVec2 center(window()->getSize().x * 0.5f, window()->getSize().y * 0.5f);
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    // begin ImGui window
    int constexpr IM_GUI_FLAGS =
        ImGuiWindowFlags_NoTitleBar |
        // ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        // ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoMove;

    ImGui::Begin("Blocking Screen", nullptr, IM_GUI_FLAGS);

    ImGui::Text(message_.c_str());

    if (BUTTON_COUNT > 0)
    {
      ImGui::Dummy(DUMMY_SIZE);

      for (int i = 0; i < BUTTON_COUNT; ++i)
      {
        if (ImGui::Button(buttonLabels_[i].c_str(), BUTTON_SIZE) &&
            pressedButtonIndex_ < 0)
        {
          pressedButtonIndex_ = i; // Only allow one button to be pressed at a time
        }
        ImGui::SameLine();
      }
    }

    ImGui::End();

    ImGui::PopFont();
  }

}