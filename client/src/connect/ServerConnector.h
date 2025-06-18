// ServerConnector.h
#pragma once

namespace iac
{
  class ServerConnector
  {
  public:
    virtual ~ServerConnector() = default;
    virtual void run() = 0;
  };
}
