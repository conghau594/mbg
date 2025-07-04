// ClientEventBus.h
#pragma once

#include "peeb/EventBus.hpp"
#include "ServerMessage.h"
#include "ClientEvent.h"

namespace bgg
{

  using ClientEventBus = peeb::Bus<
      ClientEvent,
      ServerMessage>;
} // namespace bgg

namespace peeb
{
  extern template class Bus<bgg::ClientEvent, bgg::ServerMessage>;

} // namespace peeb
