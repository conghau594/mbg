// SfLoginScreen.cpp

#include "SfLoginScreen.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads
#include <imgui_internal.h>

#include "SfGameDisplay.h"
#include "SfMessageScreen.h"
#include "SfGameSelectionScreen.h"

#include "model/GameType.h"

#include "service/GameService.h"
#include "service/ClientEvent.h"

namespace bgg
{
  SfLoginScreen::SfLoginScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameDisplay> gameDisplay)
      : SfBaseScreen(std::move(window)),
        gameDisplay_(std::move(gameDisplay)),
        usernameBuffer_{0},
        passwordBuffer_{0},
        pressedButtonIndex_(-1)
  {
    getServerMsgHandler().setHandler<LoginResponse>(
        [this](LoginResponse const &response) -> bool
        {
          onLoginResponse(response);
          return true;
        });
  }

  SfLoginScreen::~SfLoginScreen()
  {
    getServerMsgHandler().resetHandler<LoginResponse>();
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
    ImGui::SFML::Update(*getWindow(), elapsed);
    layOutScreen();

    getWindow()->clear();
    ImGui::SFML::Render(*getWindow());
    getWindow()->display();

    // handle button presses
    if (pressedButtonIndex_ == 0)
    {
      sendLoginRequest();
    }
    else if (pressedButtonIndex_ == 1)
    {
      askExitConfirmation(getWindow(), shared_from_this(), "Are you sure?");
    }

    pressedButtonIndex_ = -1;
  }

  void SfLoginScreen::onWindowEventExceptClosed(
      std::optional<sf::Event> const & /*event*/) noexcept
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
    // unused: ImVec2 const MENU_PADDING = style.WindowPadding;

    ImVec2 BUTTON_SIZE((INPUT_BOX_SIZE.x - ITEM_SPACING.x) * 0.5f, INPUT_BOX_SIZE.y);

    // change window position
    ImVec2 center(0.5f * float(getWindow()->getSize().x),
                  0.5f * float(getWindow()->getSize().y));

    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Login Screen", nullptr, IM_GUI_FLAGS);

    int constexpr USERNAME_INPUT_FLAG = ImGuiInputTextFlags_CharsNoBlank;
    ImGui::InputTextEx(
        "##Username",
        "Username",
        usernameBuffer_,
        IM_ARRAYSIZE(usernameBuffer_),
        INPUT_BOX_SIZE,
        USERNAME_INPUT_FLAG,
        0,
        0); // flags, callback, user_data

    ImGui::Dummy(DUMMY_SIZE);

    int constexpr PASSWORD_INPUT_FLAG = ImGuiInputTextFlags_Password |
                                        ImGuiInputTextFlags_CharsNoBlank;

    ImGui::InputTextEx(
        "##Password",
        "Password",
        passwordBuffer_,
        IM_ARRAYSIZE(passwordBuffer_),
        INPUT_BOX_SIZE,
        PASSWORD_INPUT_FLAG,
        0,
        0); // flags, callback, user_data
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

  void SfLoginScreen::sendLoginRequest() noexcept
  {
    LoginRequest request{usernameBuffer_, passwordBuffer_};
    gameDisplay_->send(request);
    std::shared_ptr<GameScreen> waitScreen = std::make_shared<SfMessageScreen>(
        getWindow(), "Connecting to server...");

    changeSubscreen(waitScreen);
  }

  void SfLoginScreen::onLoginResponse(LoginResponse const &response) noexcept
  {
    ErrorCode const &errcode = response.errcode;
    if (errcode.value == 0) // no error -> login successfully
    {
      std::vector<std::string> gameTypeNames(
          std::begin(GameType::NAMES), std::end(GameType::NAMES));
      std::shared_ptr<GameScreen> gameSelectionScreen = std::make_shared<SfGameSelectionScreen>(
          getWindow(), gameDisplay_, gameTypeNames);

      changeSubscreen(nullptr);
      gameDisplay_->pushScreen(gameSelectionScreen);
    }
    else
    {
      std::vector<std::string> &&buttonLabels{"Retry", "Cancel"};
      std::vector<std::function<void()>> &&buttonCallbacks{
          [this]()
          {
            sendLoginRequest();
          },
          [this]()
          {
            changeSubscreen(nullptr);
          }};

      std::string message = errcode.message + " (" +
                            std::to_string(errcode.value) + ")";

      std::shared_ptr<GameScreen> retryScreen = std::make_shared<SfMessageScreen>(
          getWindow(), message, buttonLabels, buttonCallbacks);

      changeSubscreen(retryScreen);
    }
  }
} // namespace bgg
