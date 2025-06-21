// SfPlayerSelectionScreen.cpp
#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>

#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads
#include <imgui_internal.h>

#include "model/PlayerType.h"

#include "SfBaseScreen.h"
#include "SfPlayerSelectionScreen.h"
#include "SfWaitingScreen.h"
#include "GameDisplay.h"

//=============================================================================
// JUST TESTING
#include "SfChessScreen.h"
//=============================================================================
namespace iab
{
  SfPlayerSelectionScreen::SfPlayerSelectionScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameDisplay> gameDisplay,
      std::vector<std::string> playerTypeNames,
      int gameType) noexcept
      : SfBaseScreen(window, gameDisplay),
        smallFont_(nullptr),
        pressedButtonIndex_(-1),
        playerTypeNames_(std::move(playerTypeNames)),
        gameType_(gameType)
  {
  }

  void SfPlayerSelectionScreen::doExit() noexcept
  {
  }

  void SfPlayerSelectionScreen::doEnter() noexcept
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
  }

  void SfPlayerSelectionScreen::update(sf::Time const &elapsed) noexcept
  {
    if (elapsed == sf::Time::Zero || !isActive())
    {
      // If no time has passed or the screen should deactivate, do nothing
      return;
    }

    // update the window display
    ImGui::SFML::Update(*window(), elapsed);
    drawMenu();

    window()->clear();
    ImGui::SFML::Render(*window());
    window()->display();

    // handle button presses
    if (pressedButtonIndex_ >= 0 && pressedButtonIndex_ < int(playerTypeNames_.size()))
    {
      int playerType = pressedButtonIndex_;
      // TODO: send requestGame(gameType_, playerType);
      std::shared_ptr<GameScreen> waitScreen(new SfWaitingScreen(
          window(),
          gameDisplay(),
          "Waiting for opponent...",
          {"Cancel"},
          {[this]()
           {
             // TODO: send cancelGameRequest
             gameDisplay()->popScreen();
           }}));

      //
      gameDisplay()->pushScreen(waitScreen);
      deactivate();

      //=============================================================================
      // Just for test, push the SfChessScreen:
      // define the level with an array of tile indices
      constexpr int const level[] = {
          0, 1, 0, 1, 0, 1, 0, 1,
          1, 0, 1, 0, 1, 0, 1, 0,
          0, 1, 0, 1, 0, 1, 0, 1,
          1, 0, 1, 0, 1, 0, 1, 0,
          0, 1, 0, 1, 0, 1, 0, 1,
          1, 0, 1, 0, 1, 0, 1, 0,
          0, 1, 0, 1, 0, 1, 0, 1,
          1, 0, 1, 0, 1, 0, 1, 0};
      std::shared_ptr<SfTileMap> tileMap(new SfTileMap(
          "resource/western-chess-tile-set.png",
          {0, 0},
          {200, 200},
          level, 8, 8));
      std::shared_ptr<GameScreen> chessScreen(new SfChessScreen(window(), gameDisplay(), *tileMap));
      gameDisplay()->pushScreen(chessScreen);
      //=============================================================================
    }

    else if (pressedButtonIndex_ == int(playerTypeNames_.size()))
    {
      gameDisplay()->popScreen();
      deactivate();
    }

    pressedButtonIndex_ = -1;
  }

  void SfPlayerSelectionScreen::onWindowEventExceptClosed(std::optional<sf::Event> const &) noexcept
  {
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
    ImGui::Text("Play with . . .");
    ImGui::PopFont();

    ImGui::Dummy(DUMMY_SIZE);

    for (int i = 0; i < (const int)playerTypeNames_.size(); ++i)
    {
      if (ImGui::Button(playerTypeNames_[i].c_str(), BUTTON_SIZE) && pressedButtonIndex_ < 0)
      {
        pressedButtonIndex_ = i; // Only allow one button to be pressed at a time
      }
      ImGui::Dummy(DUMMY_SIZE);
    }

    ImGui::Separator();
    ImGui::Dummy(ImVec2(DUMMY_SIZE.x * 2.0f, DUMMY_SIZE.y * 2.0f));

    if (ImGui::Button("Back", BUTTON_SIZE) && pressedButtonIndex_ < 0)
    {
      pressedButtonIndex_ = int(playerTypeNames_.size()); // Only allow one button to be pressed at a time
    }

    ImGui::End();
  }
} // namespace iab
