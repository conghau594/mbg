// SfMessageScreen.cpp

#include <optional>
#include <memory>

#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads

#include "SfMessageScreen.h"

namespace bgg
{
  SfMessageScreen::SfMessageScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::string message,
      std::vector<std::string> buttonLabels,
      std::vector<std::function<void()>> buttonCallbacks) noexcept
      : SfBaseScreen(std::move(window)),
        message_(std::move(message)),
        buttonLabels_(std::move(buttonLabels)),
        buttonCallbacks_(std::move(buttonCallbacks)),
        pressedButtonIndex_(-1)
  {
    BOOST_ASSERT_MSG(buttonLabels_.size() <= buttonCallbacks_.size(),
                     "Number of button labels must be less than or equal to number of button callbacks");
  }

  void SfMessageScreen::doEnter() noexcept
  {
  }

  void SfMessageScreen::doExit() noexcept
  {
    changeSubscreen(nullptr);
  }

  void SfMessageScreen::update(sf::Time const &elapsed) noexcept
  {
    // update the window display
    ImGui::SFML::Update(*window(), elapsed);
    layOutScreen();
    window()->clear();
    ImGui::SFML::Render(*window());
    window()->display();

    // handle button presses
    if (pressedButtonIndex_ >= 0 &&
        pressedButtonIndex_ < int(buttonCallbacks_.size()))
    {
      buttonCallbacks_[size_t(pressedButtonIndex_)]();
      pressedButtonIndex_ = -1; // Reset after executing the callback
    }
  }

  void SfMessageScreen::onWindowEventExceptClosed(
      std::optional<sf::Event> const &) noexcept
  {
  }

  void SfMessageScreen::layOutScreen() noexcept
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

    float BUTTON_WIDTH = 0.0f;
    if (BUTTON_COUNT != 0)
      BUTTON_WIDTH = float(MENU_WIDTH - 2 * MENU_PADDING.x - float(BUTTON_COUNT - 1) * ITEM_SPACING.x) /
                     float(BUTTON_COUNT);
    float const BUTTON_HEIGHT = BUTTON_COUNT == 0 ? 0.0f : 40.0f;
    ImVec2 BUTTON_SIZE(BUTTON_WIDTH, BUTTON_HEIGHT);

    // change window size
    float const MENU_HEIGHT = /*TITLE_BAR_HEIGHT +*/ MENU_PADDING.y * 2.0f +
                              TEXT_SIZE.y + BUTTON_HEIGHT + ITEM_SPACING.y * 2.0f +
                              DUMMY_SIZE.y;

    ImGui::SetNextWindowSize(ImVec2(MENU_WIDTH, MENU_HEIGHT), ImGuiCond_Always);

    // change window position
    ImVec2 center(0.5f * float(window()->getSize().x),
                  0.5f * float(window()->getSize().y));
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

    ImGui::Text("%s", message_.c_str());

    if (BUTTON_COUNT > 0)
    {
      ImGui::Dummy(DUMMY_SIZE);

      for (int i = 0; i < BUTTON_COUNT; ++i)
      {
        if (ImGui::Button(buttonLabels_[size_t(i)].c_str(), BUTTON_SIZE) &&
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