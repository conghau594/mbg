// IDisplay.h
#pragma once

#include "service/ClientRequest.h"
#include "service/ServerMessage.h"

namespace std
{
  template <typename T>
  class shared_ptr;
}

namespace bgg
{
  class IScreen;
  class IDisplay
  {
  public:
    virtual ~IDisplay() = default;
    virtual void run() = 0;
    virtual void send(ClientRequest const &request) = 0;

    virtual void pushScreen(std::shared_ptr<IScreen> newScreen) = 0;
    virtual void popScreen() = 0;
    virtual void changeScreen(std::shared_ptr<IScreen> newScreen) = 0;
    virtual void clearScreens() = 0;
  };

} // namespace bgg