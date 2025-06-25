// Request.h
#pragma once

#include <string>
#include <variant>

namespace iab
{

  class Request
  {
  public:
    struct Login
    {
      std::string username;
      std::string password;
    };

    struct FindGame
    {
      std::string userId;
      int gameType;
      int playerType;
    };

    struct CancelMatchmaking
    {
      std::string userId;
    };

    struct CommitMove
    {
      std::string userId;
      std::string gameId;

      int pieceType;
      int turn;

      struct
      {
        int x, y, z;
      } pos;
    };

    struct ResignGame
    {
      std::string userId;
      std::string gameId;
    };

    // ==========================================================================

  private:
    using Variant = std::variant<
        Login,
        FindGame,
        CancelMatchmaking,
        CommitMove,
        ResignGame>;

    Variant data_;

  public:
    template <typename R>
    Request(R const &request) noexcept;

    template <typename R>
    R const *getIf() const noexcept;

    template <typename R>
    bool is() const noexcept;

    template <typename Visitor>
    decltype(auto) visit(Visitor &&visitor) const;
  };

  // ==========================================================================

  template <typename R>
  Request::Request(R const &request) noexcept
      : data_(request)
  {
  }

  template <typename R>
  R const *Request::getIf() const noexcept
  {
    return std::get_if<R>(&data_);
  }

  template <typename R>
  bool Request::is() const noexcept
  {
    return std::holds_alternative<R>(data_);
  }

  template <typename Visitor>
  decltype(auto) Request::visit(Visitor &&visitor) const
  {
    return std::visit(std::forward<Visitor>(visitor), data_);
  }
}