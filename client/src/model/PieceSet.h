// PieceSet.h
#pragma once

#include <memory>
#include <set>
#include <variant>
#include <functional>

#include "Piece.h"

namespace bgg
{
  ///////////////////////////////////////////////////////////////////////////
  class PieceSet final
  {
    class PieceComparator
    {
    public:
      using is_transparent = void;

      template <typename L, typename R>
      [[nodiscard]] auto operator()(
          L const &lhs, R const &rhs) const noexcept -> bool
      {
        Position const &lhsPos = getPosition(lhs);
        Position const &rhsPos = getPosition(rhs);
        if (lhsPos.getFile() == rhsPos.getFile())
        {
          return lhsPos.getRank() < rhsPos.getRank();
        }
        return lhsPos.getFile() < rhsPos.getFile();
      }

    private:
      [[nodiscard]] static auto getPosition(
          Position const &pos) noexcept -> Position const &
      {
        return pos;
      }
      // [[nodiscard]] static auto getPosition(Piece const &piece) -> Position const &
      // {
      //   return piece.getPosition();
      // }
      [[nodiscard]] static auto getPosition(
          std::shared_ptr<Piece> const &piece) noexcept -> Position const &
      {
        return piece->getPosition();
      }
    };

    using PiecePlacementSet = std::set<std::shared_ptr<Piece>, PieceComparator>;
    PiecePlacementSet pieces_;

  public:
    class Iter;

    PieceSet() noexcept = default;
    PieceSet(PieceSet const &other) noexcept;

    void addPiece(std::shared_ptr<Piece> piece) noexcept;

    auto removePiece(Position const &pos) noexcept -> bool;

    // [[nodiscard]] auto findPiece(Position const &pos) const noexcept
    //     -> std::shared_ptr<const Piece>;

    [[nodiscard]] auto findPiece(Position const &pos) const noexcept
        -> std::shared_ptr<Piece>;

    // [[nodiscard]] auto findPiecesIf(
    //     std::function<bool(std::shared_ptr<Piece> const &)> const &predicate,
    //     bool onlyFirst = false) const noexcept
    //     -> std::list<std::shared_ptr<const Piece>>;

    [[nodiscard]] auto findPiecesIf(
        std::function<bool(std::shared_ptr<Piece> const &)> const &predicate,
        bool onlyFirst = false) const noexcept
        -> std::list<std::shared_ptr<Piece>>;

    [[nodiscard]] auto begin() noexcept -> Iter;

    [[nodiscard]] auto end() noexcept -> Iter;

    [[nodiscard]] auto begin() const noexcept -> Iter;

    [[nodiscard]] auto end() const noexcept -> Iter;
  };

  ///////////////////////////////////////////////////////////////////////////
  class PieceSet::Iter final
  {
    friend class PieceSet;

    mutable PiecePlacementSet::iterator pieceIter_;
    PiecePlacementSet const *const pieceSetPtr_;

    Iter(PiecePlacementSet::iterator pieceIter,
         PiecePlacementSet const *const pieceSetPtr) noexcept;

  public:
    Iter() = delete;
    void operator++() const noexcept;

    // [[nodiscard]] auto operator->() noexcept -> std::shared_ptr<Piece> *
    // {
    //   return &(*std::get<PiecePlacementSet::iterator>(pieceIter_));
    // }

    // [[nodiscard]] auto operator->() const noexcept -> std::shared_ptr<Piece> *
    // {
    //   return &(*std::get<std::set<Piece>::const_iterator>(pieceIter_));
    // }

    [[nodiscard]] auto operator*() const noexcept -> std::shared_ptr<Piece>;
    [[nodiscard]] auto operator==(Iter const &rhs) const noexcept -> bool;

    void *operator new(std::size_t) = delete;
    void operator delete(void *) = delete;
  };
} // namespace bgg
