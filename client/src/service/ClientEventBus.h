// ClientEventBus.h
#pragma once

#include "peeb/EventBus.hpp"
#include "ServerMessage.h"
#include "ClientRequest.h"

namespace bgg
{

  using ClientEventBus = peeb::Bus<
      ClientRequest,
      ServerMessage>;
} // namespace bgg

namespace peeb
{
  extern template class Bus<bgg::ClientRequest, bgg::ServerMessage>;

} // namespace peeb
