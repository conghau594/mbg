// GamePlayScreen.cpp

#include <thread>

#include <SFML/Graphics/RenderWindow.hpp>
#include <imgui.h>      // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <imgui-SFML.h> // for ImGui::SFML::* functions and SFML-specific overloads

#include "display/IDisplay.h"
#include "GamePlayScreen.h"
#include "ConfirmationScreen.h"

#include "display/IBoardView.h"

namespace bgg
{
  GamePlayScreen::GamePlayScreen(
      std::shared_ptr<sf::RenderWindow> window,
      std::shared_ptr<IDisplay> gameDisplay,
      std::shared_ptr<IBoardView> gameBoard,
      int resignRegionHeight) noexcept
      : BaseScreen(std::move(window)),
        gameDisplay_(std::move(gameDisplay)),
        gameBoard_(std::move(gameBoard)),
        buttonLabel_("Resign"),
        resignRegionHeight_(resignRegionHeight),
        pressedButtonIndex_(-1),
        gameFinished_(false)
  {
  }

  void GamePlayScreen::update(sf::Time const &elapsed) noexcept
  {
    // update the window display
    ImGui::SFML::Update(*getWindow(), elapsed);
    layOutScreen();

    getWindow()->clear();
    getWindow()->draw(*gameBoard_);
    ImGui::SFML::Render(*getWindow());
    getWindow()->display();

    // handle button presses
    if (pressedButtonIndex_ == 0)
    {
      if (gameFinished_)
      {
        gameDisplay_->popScreen();
      }
      else
      {
        char constexpr msg[] = "Do you want to give up?";
        std::vector<std::string> &&buttonLabels{"Yes", "No"};
        std::vector<std::function<void()>> &&buttonCallbacks{
            [this]()
            {
              std::shared_ptr<IScreenInternal> waitScreen(new ConfirmationScreen(
                  getWindow(), "Wait a second...", {}, {}));
              changeSubscreen(waitScreen);

              gameDisplay_->send(ResignGameRequest{});
            },
            [this]()
            {
              changeSubscreen(nullptr);
              // gameBoard_->
            }};

        std::shared_ptr<IScreenInternal> pauseScreen(new ConfirmationScreen(
            getWindow(), msg, buttonLabels, buttonCallbacks));

        changeSubscreen(pauseScreen);
        // deactivate();
      }

      pressedButtonIndex_ = -1;
    }
  }

  void GamePlayScreen::onWindowEventExceptClosed(
      std::optional<sf::Event> const &event) noexcept
  {
    if (!event.has_value())
    {
      return;
    }

    if (auto wndResized = event->getIf<sf::Event::Resized>())
    {
      float const windowRatio = float(wndResized->size.x) / float(wndResized->size.y);
      float constexpr viewRatio = 1.0f;

      float sizeX = 1.0f;
      float sizeY = 1.0f;
      float posX = 0.0f;
      float posY = 0.0f;

      if (windowRatio > viewRatio)
      {
        sizeX = viewRatio / windowRatio;
        posX = (1.0f - sizeX) / 2.0f;
      }
      else if (windowRatio < viewRatio)
      {
        sizeY = windowRatio / viewRatio;
        posY = (1.0f - sizeY) / 2.0f;
      }

      sf::View view(getWindow()->getView());
      view.setViewport(sf::FloatRect({posX, posY}, {sizeX, sizeY}));
      getWindow()->setView(view);
    }
    else if (!gameFinished_)
    {
      gameBoard_->onWindowEvent(*event);
    }
  }

  void GamePlayScreen::onGameFinishedNotification(GameFinishedNotification const &notif) noexcept
  {
    std::string msg;
    if (notif.result == "Win")
    {
      msg = "You won!            ";
    }
    else if (notif.result == "Lose")
    {
      msg = "You lost!           ";
    }
    else if (notif.result == "Draw")
    {
      msg = "It's a draw!        ";
    }
    else if (notif.result == "Error")
    {
      msg = "An error occurred!\nThe game has been terminated.";
    }

    std::vector<std::string> &&buttonLabels{"Review", "Exit"};
    std::vector<std::function<void()>> &&buttonCallbacks{
        [this]()
        {
          changeSubscreen(nullptr);
        },
        [this]()
        {
          gameDisplay_->popScreen();
        }};

    std::shared_ptr<IScreenInternal> resultScreen(new ConfirmationScreen(
        getWindow(), msg, buttonLabels, buttonCallbacks));

    // int constexpr WAIT_TIME_MS = 100;
    // std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME_MS));
    changeSubscreen(resultScreen);

    buttonLabel_ = "Exit";
    gameFinished_ = true;
  }

  void GamePlayScreen::onResignGameResponse(ResignGameResponse const &response) noexcept
  {
    if (response.errcode.failed())
    {
      std::string msg = "Failed to resign the game: " + response.errcode.message;
      std::vector<std::string> &&buttonLabels{"OK"};
      std::vector<std::function<void()>> &&buttonCallbacks{
          [this]()
          {
            changeSubscreen(nullptr);
          }};

      std::shared_ptr<IScreenInternal> errorScreen(new ConfirmationScreen(
          getWindow(), msg, buttonLabels, buttonCallbacks));

      changeSubscreen(errorScreen);
    }
    else
    {
      changeSubscreen(nullptr);
    }
  }

  void GamePlayScreen::doEnter() noexcept
  {
    getServerMsgHandler().setHandler<GameUpdatedNotification>(
        [this](GameUpdatedNotification const &notif) -> bool
        {
          if (!gameFinished_)
          {
            gameBoard_->handleServerMessage(notif);
          }
          return true;
        });

    getServerMsgHandler().setHandler<MoveResponse>(
        [this](MoveResponse const &response) -> bool
        {
          if (!gameFinished_)
          {
            gameBoard_->handleServerMessage(response);
          }
          return true;
        });

    getServerMsgHandler().setHandler<GameFinishedNotification>(
        [this](GameFinishedNotification const &notif) -> bool
        {
          if (!gameFinished_)
          {
            onGameFinishedNotification(notif);
          }
          return true;
        });

    getServerMsgHandler().setHandler<ResignGameResponse>(
        [this](ResignGameResponse const &response) -> bool
        {
          if (!gameFinished_)
          {
            onResignGameResponse(response);
          }
          return true;
        });
  }

  void GamePlayScreen::doExit() noexcept
  {
    getServerMsgHandler().resetHandler<GameUpdatedNotification>();
    getServerMsgHandler().resetHandler<GameFinishedNotification>();
    getServerMsgHandler().resetHandler<MoveResponse>();
    getServerMsgHandler().resetHandler<ResignGameResponse>();
  }

  void GamePlayScreen::layOutScreen() noexcept
  {
    int constexpr FONT_VENITE_ADOREMUS_24 = 2;
    ImFont *font24 = ImGui::GetIO().Fonts->Fonts[FONT_VENITE_ADOREMUS_24];
    ImGui::PushFont(font24);

    // ImVec2 const TEXT_SIZE = ImGui::CalcTextSize(buttonLabel);
    // ImGuiStyle const &style = ImGui::GetStyle();
    // ImVec2 const ITEM_SPACING = style.ItemSpacing;
    // ImVec2 const MENU_PADDING = style.WindowPadding;

    float constexpr VERTICAL_SPACING = 15.0f;
    ImVec2 const BUTTON_SIZE(
        120.0f, float(resignRegionHeight_) - 2.0f * VERTICAL_SPACING);

    float const buttonVerticalOffset = 0.5f * float(resignRegionHeight_); // Offset from the top of the window
    ImVec2 center(0.5f * float(getWindow()->getSize().x), float(buttonVerticalOffset));
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    int constexpr IM_GUI_FLAGS = int(ImGuiWindowFlags_NoBackground) |
                                 int(ImGuiWindowFlags_NoSavedSettings) |
                                 int(ImGuiWindowFlags_NoResize) |
                                 int(ImGuiWindowFlags_NoCollapse) |
                                 int(ImGuiWindowFlags_NoDecoration) |
                                 int(ImGuiWindowFlags_AlwaysAutoResize) |
                                 int(ImGuiWindowFlags_NoMove);

    // ImVec2 constexpr DUMMY_SIZE(0.0f, 10.0f);

    ImGui::Begin("Select Game", nullptr, IM_GUI_FLAGS);

    int numStyleColorsPushed = 0;

    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
    ++numStyleColorsPushed;

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    ++numStyleColorsPushed;

    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.1f, 0.1f, 1.0f));
    ++numStyleColorsPushed;

    if (ImGui::Button(buttonLabel_.c_str(), BUTTON_SIZE) && pressedButtonIndex_ < 0)
    {
      pressedButtonIndex_ = 0; // Only allow one button to be pressed at a time
    }

    ImGui::PopStyleColor(numStyleColorsPushed);

    ImGui::End();

    ImGui::PopFont();
  }
}