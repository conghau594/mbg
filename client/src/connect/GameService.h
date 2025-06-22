// GameService.h
#pragma once

#include <string>
#include <functional>

namespace iab
{
  class Move;
  class GameService
  {
  public:
    using Callback = std::function<void(int, std::string)>;
    virtual ~GameService() = default;

    virtual void connect(
        std::string const &userId, Callback const &callback) = 0;

    virtual void disconnect() = 0;
    virtual auto isConnected() const -> bool = 0;

    /**ed
     * \return gameId if succeeds
     *
     * \exception if failed to find opponent
     */
    virtual auto findOpponent(
        std::string const &userId,
        int gameType,
        int playerType,
        Callback const &callback) -> std::string = 0;

    virtual void cancelMatchmaking(
        std::string const &userId, Callback const &callback) = 0;

    virtual void resignGame(
        std::string const &userId,
        std::string const &gameId,
        Callback const &callback) = 0;

    virtual void commitMove(
        std::string const &userId,
        std::string const &gameId,
        Move const *const move,
        Callback const &callback) = 0;
  };
}
