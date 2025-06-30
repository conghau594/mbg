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

  extern template class peeb::Bus<ClientEvent, ServerMessage>;
} // namespace bgg
