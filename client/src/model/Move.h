// Move.h
#pragma once

#include <string>
#include <variant>
#include <type_traits>

namespace iab
{
  class ChessMove;
  class CardMove;

  // ==========================================================================
  using MoveVariant = std::variant<
      ChessMove,
      CardMove>;

  template <typename T, typename Variant>
  concept IN_VARIANT = requires {
    requires[]<typename... Ts>(std::variant<Ts...> *)
    {
      return std::disjunction<std::is_same_v<T, Ts>...>;
    }
    (static_cast<Variant *>(nullptr));
  };

  template <typename M>
  concept GAME_MOVE = IN_VARIANT<M, MoveVariant>;

  // ==========================================================================
  class Move
  {
    MoveVariant data_;

  public:
    template <GAME_MOVE M>
    Move(const M &move) noexcept;

    template <GAME_MOVE M>
    M const *getIf() const noexcept;

    template <GAME_MOVE M>
    bool is() const noexcept;

    virtual auto serialize() -> std::string = 0;
  };

  template <GAME_MOVE M>
  Move::Move(const M &move) noexcept
  {
    m_data = move.m_data;
  }

  template <GAME_MOVE M>
  M const *Move::getIf() const noexcept
  {
    return std::get_if<M>(&m_data);
  }

  template <GAME_MOVE M>
  bool Move::is() const noexcept
  {
    return std::holds_alternative<M>(m_data);
  }

  // ==========================================================================
  class ChessMove : public Move
  {
  public:
    int gameType;

    struct Piece
    {
      int name, side;
    } piece;

    struct Position
    {
      int x, y, z;
    } pos;

    auto serialize() -> std::string override { return ""; }
  };

  // ==========================================================================
  class CardMove : public Move
  {
  public:
    int gameType;
    struct Card
    {
      int suit, rank;
    };

    struct Action
    {
    };

    auto serialize() -> std::string override { return ""; }
  };
}