// SfBlockingScreen.cpp

#include <optional>
#include <memory>
#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads
#include <imgui_internal.h>

#include "SfBlockingScreen.h"

namespace iab
{
  SfBlockingScreen::SfBlockingScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::string message,
      std::vector<std::string> buttonLabels,
      std::vector<std::function<void()>> buttonCallbacks) noexcept
      : window_(window),
        message_(std::move(message)),
        buttonLabels_(std::move(buttonLabels)),
        buttonCallbacks_(std::move(buttonCallbacks)),
        clock_(new sf::Clock),
        pressedButtonIndex_(-1)
  {
    BOOST_ASSERT_MSG(buttonLabels_.size() <= buttonCallbacks_.size(),
                     "Number of button labels must be less than or equal to number of button callbacks");
  }

  void SfBlockingScreen::update() noexcept
  {
    if (pressedButtonIndex_ >= 0 && pressedButtonIndex_ < (int)buttonCallbacks_.size())
    {
      buttonCallbacks_[pressedButtonIndex_]();
      pressedButtonIndex_ = -1; // Reset after executing the callback
    }

    auto event = window_->pollEvent();
    if (event)
    {
      ImGui::SFML::ProcessEvent(*window_, *event);
    }

    elapsedTime_ += clock_->restart();
    if (elapsedTime_ == sf::Time::Zero)
    {
      return;
    }

    ImGui::SFML::Update(*window_, elapsedTime_);
    drawDialog();
    window_->clear();
    ImGui::SFML::Render(*window_);
    window_->display();
    elapsedTime_ = sf::Time::Zero;
  }

  void SfBlockingScreen::onEnter() noexcept
  {
    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->Clear();

    const char FONT_PATH[] = "resource/Consola.ttf";
    io.Fonts->AddFontFromFileTTF(FONT_PATH, 24.0f);

    if (!ImGui::SFML::UpdateFontTexture())
    {
      // TODO: handle this concretely
      // ImGui::Begin("!", nullptr);
      // std::string errorMsg = "Failed to load font " + std::string(FONT_PATH);
      // ImGui::Text(errorMsg.c_str());
      // ImGui::End();
    }

    clock_->restart();
  }

  void SfBlockingScreen::onExit() noexcept
  {
  }

  void SfBlockingScreen::drawDialog() noexcept
  {
    // Calculate button size
    int const BUTTON_COUNT = int(buttonLabels_.size());
    ImVec2 const TEXT_SIZE = ImGui::CalcTextSize(message_.c_str());
    auto &style = ImGui::GetStyle();

    ImVec2 const ITEM_SPACING = style.ItemSpacing;
    ImVec2 const WINDOW_PADDING = style.WindowPadding;
    // float const TITLE_BAR_HEIGHT = ImGui::GetFontSize() + style.FramePadding.y * 2.0f;
    ImVec2 constexpr DUMMY_SIZE(0.0f, 8.0f);

    float const WINDOW_WIDTH = TEXT_SIZE.x + WINDOW_PADDING.x * 2.0f + 50.0f;
    float const BUTTON_WIDTH = (WINDOW_WIDTH - 2 * WINDOW_PADDING.x - (BUTTON_COUNT - 1) * ITEM_SPACING.x) / BUTTON_COUNT;
    float constexpr BUTTON_HEIGHT = 40.0f;
    ImVec2 BUTTON_SIZE(BUTTON_WIDTH, BUTTON_HEIGHT);

    // change window size
    float const WINDOW_HEIGHT = /*TITLE_BAR_HEIGHT +*/ WINDOW_PADDING.y * 2.0f +
                                TEXT_SIZE.y + BUTTON_HEIGHT + ITEM_SPACING.y * 2.0f +
                                DUMMY_SIZE.y;

    ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT), ImGuiCond_Always);

    // change window position
    ImVec2 center(window_->getSize().x * 0.5f, window_->getSize().y * 0.5f);
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

    ImGui::Dummy(DUMMY_SIZE);

    for (int i = 0; i < (const int)(buttonLabels_.size()); ++i)
    {
      if (ImGui::Button(buttonLabels_[i].c_str(), BUTTON_SIZE) && pressedButtonIndex_ < 0)
      {
        pressedButtonIndex_ = i; // Only allow one button to be pressed at a time
      }
      ImGui::SameLine();
    }

    ImGui::End();
  }
}