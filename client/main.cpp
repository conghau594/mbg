#include <memory>
#include "app/ChessAppFactory.h"

int main()
{
  try
  {
    std::shared_ptr<iab::GameAppFactory> appFactory(new iab::ChessAppFactory);
    iab::GameApp app = appFactory->createGameApp();
    app.run();
  }
  catch (std::exception const & /*e*/)
  {
    // TODO: Log failure with `e`
  }
  return 0;
}
// #include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h

// #include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads

// #include <SFML/Graphics.hpp>
// #include <iostream>
// #include <random>
// class ParticleSystem : public sf::Drawable, public sf::Transformable
// {
// public:
//   ParticleSystem(unsigned int count) : m_particles(count), m_vertices(sf::PrimitiveType::Points, count)
//   {
//   }

//   void setEmitter(sf::Vector2f position)
//   {
//     m_emitter = position;
//   }

//   void update(sf::Time elapsed)
//   {
//     for (std::size_t i = 0; i < m_particles.size(); ++i)
//     {
//       // update the particle lifetime
//       Particle &p = m_particles[i];
//       p.lifetime -= elapsed;

//       // if the particle is dead, respawn it
//       if (p.lifetime <= sf::Time::Zero)
//         resetParticle(i);

//       // update the position of the corresponding vertex
//       m_vertices[i].position += p.velocity * elapsed.asSeconds();

//       // update the alpha (transparency) of the particle according to its lifetime
//       float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
//       m_vertices[i].color = p.color;
//       m_vertices[i].color.a = static_cast<std::uint8_t>(ratio * 255);
//     }
//   }

// private:
//   void draw(sf::RenderTarget &target, sf::RenderStates states) const override
//   {
//     // apply the transform
//     states.transform *= getTransform();

//     // our particles don't use a texture
//     states.texture = nullptr;

//     // draw the vertex array
//     target.draw(m_vertices, states);
//   }

//   struct Particle
//   {
//     sf::Vector2f velocity;
//     sf::Time lifetime;
//     sf::Color color;
//   };

//   void resetParticle(std::size_t index)
//   {
//     // create random number generator
//     static std::random_device rd;
//     static std::mt19937 rng(rd());

//     // give a random velocity and lifetime to the particle
//     const sf::Angle angle = sf::degrees(std::uniform_real_distribution(0.f, 360.f)(rng));
//     const float speed = std::uniform_real_distribution(50.f, 100.f)(rng);
//     m_particles[index].velocity = sf::Vector2f(speed, angle);
//     m_particles[index].lifetime = sf::milliseconds(std::uniform_int_distribution(1000, 4000)(rng));
//     m_particles[index].color = sf::Color(
//         std::uniform_int_distribution(100, 255)(rng),
//         std::uniform_int_distribution(100, 255)(rng),
//         std::uniform_int_distribution(100, 255)(rng),
//         255);
//     //  reset the position of the corresponding vertex
//     m_vertices[index].position = m_emitter;
//   }

//   std::vector<Particle> m_particles;
//   sf::VertexArray m_vertices;
//   sf::Time m_lifetime{sf::seconds(4)};
//   sf::Vector2f m_emitter;
// };

// int main()
// {
//   // create the window
//   sf::RenderWindow window(sf::VideoMode({640, 480}), "Particles");

//   // create the particle system
//   ParticleSystem particles(50'000);

//   // create a clock to track the elapsed time
//   sf::Clock clock;

//   // run the main loop
//   while (window.isOpen())
//   {
//     // handle events
//     while (const std::optional event = window.pollEvent())
//     {
//       if (event->is<sf::Event::Closed>())
//         window.close();
//     }

//     // make the particle system emitter follow the mouse
//     sf::Vector2i mouse = sf::Mouse::getPosition(window);
//     particles.setEmitter(window.mapPixelToCoords(mouse));

//     // update it
//     sf::Time elapsed = clock.restart();
//     particles.update(elapsed);

//     // draw it
//     window.clear();
//     window.draw(particles);
//     window.display();
//   }
// }

// int main()
// {
//   sf::Time t(sf::microseconds(100));
//   std::cout << "SFML version: " << SFML_VERSION_MAJOR
//             << "." << SFML_VERSION_MINOR
//             << "." << SFML_VERSION_PATCH << std::endl;

//   std::cout << "SFML time in seconds: " << t.asSeconds() << std::endl;
//   std::cout << "SFML time in milliseconds: " << t.asMilliseconds() << std::endl;
//   std::cout << "SFML time in microseconds: " << t.asMicroseconds() << std::endl;

//   sf::RenderWindow window(sf::VideoMode({640, 480}), "ImGui + SFML = <3");
//   window.setVerticalSyncEnabled(true);
//   // window.setFramerateLimit(60);
//   if (!ImGui::SFML::Init(window))
//     return -1;

//   sf::CircleShape shape(100.f);
//   shape.setFillColor(sf::Color::Green);

//   sf::Clock deltaClock;
//   while (window.isOpen())
//   {
//     while (const auto event = window.pollEvent())
//     {
//       ImGui::SFML::ProcessEvent(window, *event);

//       if (event->is<sf::Event::Closed>())
//       {
//         window.close();
//       }
//     }

//     ImGui::SFML::Update(window, deltaClock.restart());

//     ImGui::Begin("Hello, world!");
//     ImGui::Button("Look at this pretty button");
//     ImGui::End();

//     sf::View view(sf::FloatRect({0.f, 0.f}, {640.f, 480.f}));

//     view.setViewport(sf::FloatRect({0.25f, 0.25}, {0.5f, 0.5f}));

//     // view.zoom(0.5f);

//     // activate it
//     window.setView(view);

//     window.clear();
//     window.draw(shape);
//     ImGui::SFML::Render(window);
//     window.display();
//   }

//   ImGui::SFML::Shutdown();
// }