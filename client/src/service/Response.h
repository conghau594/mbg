// Response.h
#pragma once

#include <variant>
#include <string>

#include "base/ErrorCode.h"

namespace iab
{
  class Response final
  {
  public:
    struct Login
    {
      ErrorCode errcode;
      std::string userId;
    };

    struct FindGame
    {
      ErrorCode errcode;
      std::string gameId;
      int turn;
    };

    struct CancelMatchmaking
    {
      ErrorCode errcode;
    };

    struct CommitMove
    {
      ErrorCode errcode;
    };

    struct ResignGame
    {
      ErrorCode errcode;
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
    Response(R const &response) noexcept;

    ~Response() = default;

    template <typename R>
    R const *getIf() const noexcept;

    template <typename R>
    bool is() const noexcept;
  };

  // ==========================================================================

  template <typename R>
  Response::Response(R const &request) noexcept
      : data_(request)
  {
  }

  template <typename R>
  R const *Response::getIf() const noexcept
  {
    return std::get_if<R>(&data_);
  }

  template <typename R>
  bool Response::is() const noexcept
  {
    return std::holds_alternative<R>(data_);
  }

} // namespace iab
