// SfPlayerSelectionScreen.cpp
#include <boost/assert.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads

#include "model/PlayerType.h"
#include "service/GameService.h"

#include "SfBaseScreen.h"
#include "SfPlayerSelectionScreen.h"
#include "SfMessageScreen.h"
#include "GameDisplay.h"

//=============================================================================
// JUST TESTING
#include "SfChessScreen.h"
//=============================================================================
namespace iab
{
  SfPlayerSelectionScreen::SfPlayerSelectionScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<GameService> gameService,
      std::shared_ptr<GameDisplay> gameDisplay,
      std::vector<std::string> playerTypeNames,
      int gameType) noexcept
      : SfBaseScreen(window, gameService),
        gameDisplay_(gameDisplay),
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
  }

  void SfPlayerSelectionScreen::update(sf::Time const &elapsed) noexcept
  {
    // update the window display
    ImGui::SFML::Update(*window(), elapsed);
    layOutScreen();

    window()->clear();
    ImGui::SFML::Render(*window());
    window()->display();

    // handle button presses
    if (pressedButtonIndex_ >= 0 &&
        pressedButtonIndex_ < int(playerTypeNames_.size()))
    {

      // if (!gameService()->isConnected())
      // {
      //   connectServer(window(), gameService(), gameDisplay(), shared_from_this());
      // }

      // if (gameService()->isConnected())
      // {
      //   int playerType = pressedButtonIndex_;

      //   gameService()->findOpponent("", gameType_, playerType, nullptr);

      //   // TODO: send requestGame(gameType_, playerType);
      //   std::shared_ptr<GameScreen> waitScreen(new SfMessageScreen(
      //       window(),
      //       gameService(),
      //       gameDisplay(),
      //       "Waiting for opponent...",
      //       {/*"Cancel"*/},
      //       {
      //           /*[this]()
      //           {
      //             // TODO: send cancelGameRequest
      //             changeSubscreen(nullptr);
      //           }*/
      //       }));

      //   changeSubscreen(waitScreen);
      // }
      // //=============================================================================
      // // Just for test, push the SfChessScreen:
      // // define the level with an array of tile indices
      // constexpr unsigned const level[] = {
      //     0, 1, 0, 1, 0, 1, 0, 1,
      //     1, 0, 1, 0, 1, 0, 1, 0,
      //     0, 1, 0, 1, 0, 1, 0, 1,
      //     1, 0, 1, 0, 1, 0, 1, 0,
      //     0, 1, 0, 1, 0, 1, 0, 1,
      //     1, 0, 1, 0, 1, 0, 1, 0,
      //     0, 1, 0, 1, 0, 1, 0, 1,
      //     1, 0, 1, 0, 1, 0, 1, 0};

      // SfTileMap tileMap(
      //     "resource/western-chess-tile-set.png",
      //     {200, 200},
      //     level,
      //     {8, 8});
      // std::shared_ptr<GameScreen> chessScreen(new SfChessScreen(
      //     window(), gameService(), gameDisplay(), tileMap));
      // gameDisplay()->pushScreen(chessScreen);
      // //=============================================================================
    }
    else if (pressedButtonIndex_ == int(playerTypeNames_.size()))
    {
      gameDisplay_->popScreen();
      // deactivate();
    }

    pressedButtonIndex_ = -1;
  }

  void SfPlayerSelectionScreen::onWindowEventExceptClosed(
      std::optional<sf::Event> const &) noexcept
  {
  }

  void SfPlayerSelectionScreen::layOutScreen() noexcept
  {
    int constexpr FONT_VENITE_ADOREMUS_36 = 1;
    ImFont *font36 = ImGui::GetIO().Fonts->Fonts[FONT_VENITE_ADOREMUS_36];
    ImGui::PushFont(font36);

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

    int constexpr FONT_VENITE_ADOREMUS_24 = 2;
    ImFont *font24 = ImGui::GetIO().Fonts->Fonts[FONT_VENITE_ADOREMUS_24];
    ImGui::PushFont(font24);
    ImGui::Text("Play with . . .");
    ImGui::PopFont();

    for (int i = 0; i < (const int)playerTypeNames_.size(); ++i)
    {
      ImGui::Dummy(DUMMY_SIZE);
      if (ImGui::Button(playerTypeNames_[i].c_str(), BUTTON_SIZE) &&
          pressedButtonIndex_ < 0)
      {
        pressedButtonIndex_ = i; // Only allow one button to be pressed at a time
      }
    }

    ImGui::Dummy(ImVec2(DUMMY_SIZE.x * 2.0f, DUMMY_SIZE.y * 2.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(DUMMY_SIZE.x * 2.0f, DUMMY_SIZE.y * 2.0f));

    if (ImGui::Button("Back", BUTTON_SIZE) && pressedButtonIndex_ < 0)
    {
      pressedButtonIndex_ = int(playerTypeNames_.size()); // Only allow one button to be pressed at a time
    }

    ImGui::End();
    ImGui::PopFont();
  }
} // namespace iab
