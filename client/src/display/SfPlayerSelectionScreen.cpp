// SfPlayerSelectionScreen.cpp
#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>

#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads
#include <imgui_internal.h>

#include "SfPlayerSelectionScreen.h"
#include "SfBaseScreen.h"
#include "GameDisplay.h"

namespace iab
{
  SfPlayerSelectionScreen::SfPlayerSelectionScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameDisplay> displayContext) noexcept
      : SfBaseScreen(window, displayContext),
        elapsedTime_(sf::Time::Zero),
        smallFont_(nullptr),
        pressedButtonIndex_(-1)
  {
  }

  void SfPlayerSelectionScreen::onExit() noexcept
  {
    elapsedTime_ += clock()->reset();
    if (elapsedTime_ == sf::Time::Zero)
    {
      return;
    }
    update(elapsedTime_);
  }

  void SfPlayerSelectionScreen::onEnter() noexcept
  {
    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->Clear();

    char constexpr FONT_PATH[] = "resource/VeniteAdoremus-rgRBA.ttf";
    float constexpr DEFAULT_FONT_SIZE = 36.0f;

    io.Fonts->AddFontFromFileTTF(FONT_PATH, DEFAULT_FONT_SIZE);
    smallFont_ = io.Fonts->AddFontFromFileTTF(FONT_PATH, DEFAULT_FONT_SIZE * 0.75f);

    if (!ImGui::SFML::UpdateFontTexture())
    {
      // TODO: Handle the error more gracefully, e.g., log it or show a message to the user
      // ImGui::Begin("!", nullptr);
      // std::string errorMsg = "Failed to load font " + std::string(FONT_PATH);
      // ImGui::Text(errorMsg.c_str());
      // ImGui::End();
    }

    clock()->start();
  }

  void SfPlayerSelectionScreen::drawMenu() noexcept
  {
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

    ImGui::PushFont(smallFont_);
    ImGui::Text("Play with");
    ImGui::PopFont();

    ImGui::Dummy(DUMMY_SIZE);

    if (ImGui::Button("Human", BUTTON_SIZE) && pressedButtonIndex_ < 0)
    {
      pressedButtonIndex_ = 0; // Only allow one button to be pressed at a time
    }

    ImGui::Dummy(DUMMY_SIZE);

    if (ImGui::Button("ChatGPT", BUTTON_SIZE) && pressedButtonIndex_ < 0)
    {
      pressedButtonIndex_ = 1; // Only allow one button to be pressed at a time
    }
    ImGui::Dummy(DUMMY_SIZE);

    if (ImGui::Button("Gemini", BUTTON_SIZE) && pressedButtonIndex_ < 0)
    {
      pressedButtonIndex_ = 2; // Only allow one button to be pressed at a time
    }

    ImGui::Dummy(ImVec2(DUMMY_SIZE.x * 2.0f, DUMMY_SIZE.y * 2.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(DUMMY_SIZE.x * 2.0f, DUMMY_SIZE.y * 2.0f));

    if (ImGui::Button("Back", BUTTON_SIZE) && pressedButtonIndex_ < 0)
    {
      pressedButtonIndex_ = 3; // Only allow one button to be pressed at a time
    }

    ImGui::End();
  }

  void SfPlayerSelectionScreen::onTimeElapsed() noexcept
  {
    elapsedTime_ += clock()->restart();
    if (elapsedTime_ == sf::Time::Zero)
    {
      return;
    }
    update(elapsedTime_);

    switch (pressedButtonIndex_)
    {
    case 0:

      pressedButtonIndex_ = -1;
      return;

    case 1:

      pressedButtonIndex_ = -1;
      return;

    case 2:

      pressedButtonIndex_ = -1;
      return;

    case 3:
      gameDisplay()->popScreen();
      pressedButtonIndex_ = -1;
      return;

    default:
      break;
    }

    pressedButtonIndex_ = -1;
    elapsedTime_ = sf::Time::Zero;
  }

  void SfPlayerSelectionScreen::onEvent(std::optional<sf::Event> event) noexcept
  {
  }

  void SfPlayerSelectionScreen::update(sf::Time elapsedTime) noexcept
  {
    ImGui::SFML::Update(*window(), elapsedTime);
    drawMenu();
    window()->clear();
    ImGui::SFML::Render(*window());
    window()->display();
  }
} // namespace iab
