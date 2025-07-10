#include <memory>
#include "app/ChessAppFactory.h"
#include "app/QuickAppFactory.h"

#include "base/Logger.h"

/**
 * GAME_TYPE:
 *  -1: All games
 *   0: ChessGame
 *
 */
#define GAME_TYPE 0

int main()
{
  utils::Logger::setUp();

  try
  {
#if defined(GAME_TYPE) && (GAME_TYPE < 0)
    std::shared_ptr<bgg::GameAppFactory> appFactory = std::make_shared<bgg::ChessAppFactory>();
#else
    std::shared_ptr<bgg::GameAppFactory> appFactory = std::make_shared<bgg::QuickAppFactory>(GAME_TYPE);
#endif

    bgg::GameApp app = appFactory->createGameApp();
    app.run();
  }
  catch (std::exception const &e)
  {
    // TODO: Log failure with `e`
    spdlog::critical("Failure at running app: {}", e.what());
  }
  return 0;
}

// Explitcit instantiation:

#include "service/ClientEventBus.h"

namespace peeb
{
  template class Bus<bgg::ClientRequest, bgg::ServerMessage>;
}
