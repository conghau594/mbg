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

  std::string
  GameServiceSimulator::findOpponent(int /*gameType*/, int /*playerType*/)
  {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(util::randomInt(200, 2000)));

    if (util::randomInt(0, 3) == 0)
    {
      throw std::runtime_error("Failed to connect to server");
    }

    boost::uuids::uuid id = (*uuidGenerator_)();
    if (id.is_nil())
    {
      throw std::runtime_error("Failed to create UUID");
    }
    return boost::uuids::to_string(id);
  }

  void GameServiceSimulator::resignGame(std::string const & /*gameId*/)
  {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(util::randomInt(200, 2000)));

    if (util::randomInt(0, 3) == 0)
    {
      throw std::runtime_error("Failed to connect to server");
    }

    // resign successfully
  }

  void GameServiceSimulator::commitMove(std::string const & /*gameId*/, Move const *const /*move*/)
  {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(util::randomInt(200, 2000)));

    if (util::randomInt(0, 3) == 0)
    {
      throw std::runtime_error("Failed to connect to server");
    }

    // commit successfully
  }
} // namespace iab