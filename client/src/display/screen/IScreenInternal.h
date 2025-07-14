// IScreen.h
#pragma once

#include <list>
#include <memory>
#include "service/ServerMessage.h"
#include "display/IScreen.h"

namespace bgg
{
  class IScreenInternal : public IScreen
  {
  public:
    virtual ~IScreenInternal() = default;

    [[nodiscard]] virtual auto isActive() const -> bool = 0;
    virtual void activate(bool active) = 0;
    virtual void changeSubscreen(std::shared_ptr<IScreenInternal> newSubscreen) = 0;
  };
}