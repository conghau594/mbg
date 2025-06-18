// ServerConnector.h
#pragma once

namespace iab
{
  class ServerConnector
  {
  public:
    virtual ~ServerConnector() = default;
    virtual void run() = 0;
  };
}
