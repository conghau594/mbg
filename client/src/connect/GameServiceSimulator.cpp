// GameServiceSimulator.cpp

#include <thread>

#include <boost/uuid.hpp>

#include "base/RandomUtils.h"
#include "GameServiceSimulator.h"

namespace iab
{
  GameServiceSimulator::GameServiceSimulator()
      : uuidGenerator_(new boost::uuids::random_generator)
  {
  }

  void GameServiceSimulator::connect(std::string const &userId)
  {
    simulateNetworkLatencyAndFailure(30);

    // connect successfully
  }

  void GameServiceSimulator::cancelMatchmaking(std::string const & /*userId*/)
  {
    simulateNetworkLatencyAndFailure(10);

    // cancel successfully
  }

  auto GameServiceSimulator::findOpponent(std::string const & /*userId*/, int /*gameType*/, int /*playerType*/)
      -> std::string
  {
    simulateNetworkLatencyAndFailure(50, 2000, 5000);

    boost::uuids::uuid id = (*uuidGenerator_)();
    if (id.is_nil())
    {
      throw std::runtime_error("Failed to create UUID");
    }
    return boost::uuids::to_string(id);
  }

  void GameServiceSimulator::resignGame(std::string const & /*userId*/, std::string const & /*gameId*/)
  {
    simulateNetworkLatencyAndFailure(10);

    // resign successfully
  }

  void GameServiceSimulator::commitMove(std::string const & /*userId*/, std::string const & /*gameId*/, Move const *const /*move*/)
  {
    simulateNetworkLatencyAndFailure(10);
    // commit successfully
  }

  void GameServiceSimulator::simulateNetworkLatencyAndFailure(
      int failurePercent /*=0*/, int minDelay /*=100*/, int maxDelay /*=2000*/) noexcept
  {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(util::randomInt(minDelay, maxDelay)));

    if (util::randomInt(1, 100) < failurePercent)
    {
      throw std::runtime_error("Failed to connect to server");
    }
  }

} // namespace iab