// GameService.h
#pragma once

namespace std
{
  template <typename T>
  class future;

  template <typename T>
  class shared_ptr;
}

namespace iab
{
  class Request;
  class Response;

  class GameService
  {
  public:
    virtual ~GameService() = default;

    virtual std::future<Response> send(Request request) = 0;
  };
}
