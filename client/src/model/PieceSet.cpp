// PieceSet.cpp

#include "PieceSet.h"

namespace bgg
{
  PieceSet::PieceSet(PieceSet const &other, IChessRule const *rule) noexcept
  {
    for (auto const &piece : other)
    {
      pieces_.emplace(piece->clone(rule));
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  void PieceSet::addPiece(std::shared_ptr<Piece> piece) noexcept
  {
    auto [iter, inserted] = pieces_.emplace(std::move(piece));
    BOOST_ASSERT_MSG(inserted, "PieceSet should not contain duplicate pieces");
  }

  auto PieceSet::removePiece(Position const &pos) noexcept -> bool
  {
    auto iter = pieces_.find(pos);
    if (iter == pieces_.end())
    {
      return false;
    }
    pieces_.erase(iter);

    return true;
  }

  // auto PieceSet::findPiece(Position const &pos) const noexcept
  //     -> std::shared_ptr<const Piece>
  // {
  //   auto iter = pieces_.find(pos);
  //   if (iter == pieces_.end())
  //   {
  //     return nullptr;
  //   }
  //   return *iter;
  // }

  auto PieceSet::findPiece(Position const &pos) const noexcept
      -> std::shared_ptr<Piece>
  {
    auto iter = pieces_.find(pos);
    if (iter == pieces_.end())
    {
      return nullptr;
    }
    return *iter;
  }

  // auto PieceSet::findPiecesIf(
  //     std::function<bool(std::shared_ptr<Piece> const &)> const &predicate,
  //     bool onlyFirst) const noexcept
  //     -> std::list<std::shared_ptr<const Piece>>
  // {
  //   std::list<std::shared_ptr<const Piece>> result;
  //   for (auto const &piece : pieces_)
  //   {
  //     if (predicate(piece))
  //     {
  //       result.emplace_back(piece);
  //       if (onlyFirst)
  //       {
  //         return result;
  //       }
  //     }
  //   }
  //   return result;
  // }

  auto PieceSet::findPiecesIf(
      std::function<bool(std::shared_ptr<Piece> const &)> const &predicate,
      bool onlyFirst) const noexcept
      -> std::list<std::shared_ptr<Piece>>
  {
    std::list<std::shared_ptr<Piece>> result;
    for (auto &piece : pieces_)
    {
      if (predicate(piece))
      {
        result.emplace_back(piece);
        if (onlyFirst)
        {
          return result;
        }
      }
    }
    return result;
  }

  auto PieceSet::begin() noexcept -> PieceSet::Iter
  {
    return Iter(pieces_.begin(), &pieces_);
  }

  auto PieceSet::end() noexcept -> PieceSet::Iter
  {
    return Iter(pieces_.end(), &pieces_);
  }

  auto PieceSet::begin() const noexcept -> PieceSet::Iter
  {
    return Iter(pieces_.begin(), &pieces_);
  }

  auto PieceSet::end() const noexcept -> PieceSet::Iter
  {
    return Iter(pieces_.end(), &pieces_);
  }

  /////////////////////////////////////////////////////////////////////////////
  PieceSet::Iter::Iter(
      PiecePlacementSet::iterator pieceIter,
      PiecePlacementSet const *const pieceSetPtr) noexcept
      : pieceIter_(std::move(pieceIter)), pieceSetPtr_(pieceSetPtr)
  {
  }

  void PieceSet::Iter::operator++() const noexcept
  {
    ++pieceIter_;
  }

  //  auto operator->() noexcept -> std::shared_ptr<Piece> *
  // {
  //   return &(*std::get<PiecePlacementSet::iterator>(pieceIter_));
  // }

  //  auto operator->() const noexcept -> std::shared_ptr<Piece> *
  // {
  //   return &(*std::get<std::set<Piece>::const_iterator>(pieceIter_));
  // }

  auto PieceSet::Iter::operator*() const noexcept -> std::shared_ptr<Piece>
  {
    return *pieceIter_;
  }

  auto PieceSet::Iter::operator==(Iter const &rhs) const noexcept -> bool
  {
    return (pieceSetPtr_ == rhs.pieceSetPtr_) && (pieceIter_ == rhs.pieceIter_);
  }

} // namespace bgg
