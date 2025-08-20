#include <memory>
#include "app/GameAppFactory.h"
#include "app/MockAppFactory.h"
#include "app/QuickChessAppFactory.h"

#include "base/Logger.h"
/**
 * GAME_TYPE:
 *  -2: Mocked game
 *  -1: All games
 *   0: ChessGame
 *
 */
#define GAME_TYPE -1

int main()
{
  utils::Logger::setUp();

  try
  {
#if !defined(GAME_TYPE)
    SPDLOG_ERROR("GAME_TYPE is not defined!");
    return -1;
#endif

#if (GAME_TYPE == -2)
    std::shared_ptr<bgg::IAppFactory>
        appFactory = std::make_shared<bgg::MockAppFactory>();
#elif (GAME_TYPE == -1)
    std::shared_ptr<bgg::IAppFactory>
        appFactory = std::make_shared<bgg::GameAppFactory>();
#else
    std::shared_ptr<bgg::IAppFactory>
        appFactory = std::make_shared<bgg::QuickChessAppFactory>(GAME_TYPE);
#endif

    bgg::GameApp app = appFactory->createGameApp();
    app.run();
  }
  catch (std::exception const &e)
  {
    // TODO: Log failure with `e`
    SPDLOG_CRITICAL("Failure at running the app: {}", e.what());
  }
  return 0;
}

// Explitcit instantiation:

#include "service/ClientEventBus.h"

namespace peeb
{
  template class Bus<bgg::ClientRequest, bgg::ServerMessage>;
}
