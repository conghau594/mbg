// PieceSet.cpp

#include "PieceSet.h"

namespace bgg
{

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
    return Iter(pieces_.cbegin(), &pieces_);
  }

  auto PieceSet::end() const noexcept -> PieceSet::Iter
  {
    return Iter(pieces_.cend(), &pieces_);
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
