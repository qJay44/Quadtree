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

  Rectangle* boundary;
  QuadTree* qt;

  float mouseX, mouseY;

  void setupSFML() {
    // Setup main window
    window.create(sf::VideoMode(WIDTH, HEIGHT), "Template text", sf::Style::Close);
    window.setFramerateLimit(75);

    // Font for some test text
    genericFont.loadFromFile("../../src/fonts/Minecraft rus.ttf");
  }

  void setupProgram() {
    srand((unsigned)time(NULL));

    boundary = new Rectangle{WIDTH / 2.f, HEIGHT / 2.f, WIDTH / 2.f, HEIGHT / 2.f};
    qt = new QuadTree(*boundary, 4);
  }

  void draw() {
    qt->show(window);

    Rectangle range{mouseX, mouseY, 50.f, 50.f};

    sf::RectangleShape rect({range.w * 2, range.h * 2});
    rect.setPosition(range.x - range.w, range.y - range.h);
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color::Green);
    rect.setOutlineThickness(1.f);

    std::vector<Point> found;
    qt->query(found, range);

    for (const Point& p : found) {
        sf::CircleShape c(2.f);
        c.setPosition(p.x, p.y);
        c.setFillColor(sf::Color::Green);
        window.draw(c);
    }

    window.draw(rect);
  }

  public:
    App() {}

    ~App() {
      delete boundary;
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

          if (event.type == sf::Event::MouseMoved) {
            mouseX = static_cast<float>(sf::Mouse::getPosition(window).x);
            mouseY = static_cast<float>(sf::Mouse::getPosition(window).y);
          }

          if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            qt->insert({mouseX, mouseY});
        }

        window.clear();

        draw();

        window.display();
      }
    }
};

