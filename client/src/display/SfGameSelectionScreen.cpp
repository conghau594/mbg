// SfGameSelectionScreen.cpp

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads
#include <imgui_internal.h>

#include "SfGameSelectionScreen.h"
#include "SfBaseScreen.h"

namespace iac
{
  SfGameSelectionScreen::SfGameSelectionScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameDisplay> displayContext,
      size_t updatePeriod) noexcept
      : SfBaseScreen(window, displayContext, updatePeriod)
  {
  }

  void SfGameSelectionScreen::onExit() noexcept
  {
  }

  void SfGameSelectionScreen::onEnter() noexcept
  {
    clock()->start();

    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->Clear();

    const char FONT_PATH[] = "resource/VeniteAdoremus-rgRBA.ttf";
    io.Fonts->AddFontFromFileTTF(FONT_PATH, 36.0f);
    if (!ImGui::SFML::UpdateFontTexture())
    {
      ImGui::Begin("!", nullptr);
      std::string errorMsg = "Failed to load font " + std::string(FONT_PATH);
      ImGui::Text(errorMsg.c_str());
      ImGui::End();
    }
  }

  void SfGameSelectionScreen::drawMenu() noexcept
  {

    ImVec2 center(window()->getSize().x * 0.5f, window()->getSize().y * 0.5f);
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    // ImGuiIO &io = ImGui::GetIO();
    // io.FontGlobalScale = 2.0f;

    int constexpr IM_GUI_FLAGS =
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoMove;

    ImVec2 constexpr BUTTON_SIZE(400.0f, 80.0f);
    ImVec2 constexpr DUMMY_SIZE(0.0f, 20.0f);

    ImGui::Begin("Select Game", nullptr, IM_GUI_FLAGS);

    if (ImGui::Button("Western Chess", BUTTON_SIZE))
    {
      // std::cout << "Button clicked!" << std::endl;
    }

    ImGui::Dummy(DUMMY_SIZE);

    ImGui::Button("Chinese Chess", BUTTON_SIZE);

    ImGui::Dummy(DUMMY_SIZE);

    ImGui::Button("Exit", BUTTON_SIZE);

    ImGui::End();
  }

  void SfGameSelectionScreen::render() noexcept
  {
    drawMenu();

    window()->clear();

    ImGui::SFML::Render(*window());

    window()->display();
  }

  void SfGameSelectionScreen::onPeriodicUpdate() noexcept
  {
  }

  void SfGameSelectionScreen::onEvent(std::optional<sf::Event> event) noexcept
  {
    auto event2 = event->getIf<sf::Event::Closed>();
  }
} // namespace iac
