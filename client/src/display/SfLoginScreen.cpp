// SfLoginScreen.cpp

#include "SfLoginScreen.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads
#include <imgui_internal.h>

#include "SfGameDisplay.h"
#include "connect/GameService.h"

namespace iab
{

  SfLoginScreen::SfLoginScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameService> gameService,
      std::shared_ptr<GameDisplay> gameDisplay) noexcept
      : SfBaseScreen(window, gameService, gameDisplay),
        passwordBuffer_(""),
        usernameBuffer_(""),
        pressedButtonIndex_(-1)
  {
  }

  void SfLoginScreen::doExit() noexcept
  {
  }

  void SfLoginScreen::doEnter() noexcept
  {
  }

  void SfLoginScreen::update(sf::Time const &elapsed) noexcept
  {
    // update the window display
    ImGui::SFML::Update(*window(), elapsed);
    layOutScreen();

    window()->clear();
    ImGui::SFML::Render(*window());
    window()->display();

    // handle button presses
    if (pressedButtonIndex_ == 0)
    {
      if (!gameService()->isConnected())
      {
        connectServer(window(), gameService(), gameDisplay(), shared_from_this());
      }
    }
    else if (pressedButtonIndex_ == 1)
    {
      askExitConfirmation(
          window(),
          gameService(),
          gameDisplay(),
          shared_from_this(),
          "Are you sure?");
    }

    pressedButtonIndex_ = -1;
  }

  void SfLoginScreen::onWindowEventExceptClosed(
      std::optional<sf::Event> const &) noexcept
  {
  }

  void SfLoginScreen::layOutScreen() noexcept
  {
    int constexpr FONT_CONSOLA_36 = 3;
    ImFont *font36 = ImGui::GetIO().Fonts->Fonts[FONT_CONSOLA_36];
    ImGui::PushFont(font36);

    // begin ImGui window
    int constexpr IM_GUI_FLAGS = ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoSavedSettings |
                                 ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoCollapse |
                                 ImGuiWindowFlags_NoDecoration |
                                 ImGuiWindowFlags_AlwaysAutoResize |
                                 ImGuiWindowFlags_NoMove;

    ImVec2 constexpr DUMMY_SIZE(0.0f, 10.0f);
    ImVec2 constexpr INPUT_BOX_SIZE(400.0f, 60.0f);
    ImVec2 const TEXT_SIZE = ImGui::CalcTextSize("Ayk");

    ImGuiStyle &style = ImGui::GetStyle();
    float const OLD_FRAME_PADDING_Y = style.FramePadding.y;
    float const OLD_ITEM_SPACING_X = style.ItemSpacing.x;

    style.FramePadding.y = (INPUT_BOX_SIZE.y - TEXT_SIZE.y) * 0.5f;
    style.ItemSpacing.x = 2.0f * DUMMY_SIZE.y;

    ImVec2 const ITEM_SPACING = style.ItemSpacing;
    ImVec2 const MENU_PADDING = style.WindowPadding;

    ImVec2 BUTTON_SIZE((INPUT_BOX_SIZE.x - ITEM_SPACING.x) * 0.5f, INPUT_BOX_SIZE.y);

    // change window position
    ImVec2 center(window()->getSize().x * 0.5f, window()->getSize().y * 0.5f);

    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Login Screen", nullptr, IM_GUI_FLAGS);

    int constexpr USERNAME_INPUT_FLAG = ImGuiInputTextFlags_CharsNoBlank;
    ImGui::InputTextEx("##Username", "Username", usernameBuffer_, IM_ARRAYSIZE(usernameBuffer_), INPUT_BOX_SIZE, USERNAME_INPUT_FLAG, 0, 0); // flags, callback, user_data
    ImGui::Dummy(DUMMY_SIZE);

    int constexpr PASSWORD_INPUT_FLAG = ImGuiInputTextFlags_Password | ImGuiInputTextFlags_CharsNoBlank;

    ImGui::InputTextEx("##Password", "Password", passwordBuffer_, IM_ARRAYSIZE(passwordBuffer_), INPUT_BOX_SIZE, PASSWORD_INPUT_FLAG, 0, 0); // flags, callback, user_data
    ImGui::Dummy(DUMMY_SIZE);
    ImGui::Dummy(DUMMY_SIZE);

    if (ImGui::Button("Login", BUTTON_SIZE) && pressedButtonIndex_ < 0)
    {
      pressedButtonIndex_ = 0; // Only allow one button to be pressed at a time
    }
    ImGui::SameLine();
    if (ImGui::Button("Quit", BUTTON_SIZE) && pressedButtonIndex_ < 0)
    {
      pressedButtonIndex_ = 1; // Only allow one button to be pressed at a time
    }

    ImGui::End();

    style.FramePadding.y = OLD_FRAME_PADDING_Y;
    style.ItemSpacing.x = OLD_ITEM_SPACING_X;

    ImGui::PopFont();
  }
} // namespace iab
