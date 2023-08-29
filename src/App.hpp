#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "myutils.hpp"
#include "quadtree.hpp"
#include <stdlib.h>
#include <vector>

#define WIDTH 800
#define HEIGHT 800

class App {
  sf::RenderWindow window;
  sf::Font genericFont;

  Rectangle boundary;
  QuadTree* qt;

  void setupSFML() {
    // Setup main window
    window.create(sf::VideoMode(WIDTH, HEIGHT), "Template text", sf::Style::Close);
    window.setFramerateLimit(75);

    // Font for some test text
    genericFont.loadFromFile("../../src/fonts/Minecraft rus.ttf");
  }

  void setupProgram() {
    srand((unsigned)time(NULL));

    boundary = {WIDTH / 2.f, HEIGHT / 2.f, WIDTH / 2.f, HEIGHT / 2.f};
    qt = new QuadTree(boundary, 4);
  }

  void draw() {
    qt->show(window);
  }

  public:
    App() {}

    ~App() {
      delete qt;
    }

    void setup() {
      setupSFML();
      setupProgram();
    }

    void run() {
      while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
          if (event.type == sf::Event::Closed)
            window.close();

          if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Key::Q)
            window.close();

          if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            Point p = {
              static_cast<float>(sf::Mouse::getPosition(window).x),
              static_cast<float>(sf::Mouse::getPosition(window).y)
            };
            qt->insert(p);
          }
        }

        window.clear();

        draw();

        window.display();
      }
    }
};

