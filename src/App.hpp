#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "myutils.hpp"
#include "quadtree.hpp"
#include <stdlib.h>

#define WIDTH 1280
#define HEIGHT 960

class App {
  struct SomeObjectWithArea {
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::Vector2f size;
    sf::Color color;

    sf::VertexArray vertices{sf::Quads, 4};

    SomeObjectWithArea(
      const sf::Vector2f& position,
      const sf::Vector2f& velocity,
      const sf::Vector2f& size,
      const sf::Color& color
      ) : pos(position), vel(velocity), size(size), color(color) {

      vertices[0].color = this->color;
      vertices[1].color = this->color;
      vertices[2].color = this->color;
      vertices[3].color = this->color;

      vertices[0].position = pos;
      vertices[1].position = {pos.x + size.x, pos.y};
      vertices[2].position = {pos.x + size.x, pos.y + size.y};
      vertices[3].position = {pos.x, pos.y + size.y};
    }
  };

  sf::RenderWindow window;
  sf::Font genericFont;
  sf::Vector2f mouse{0.f, 0.f};
  sf::Vector2f mousePressed{0.f, 0.f};
  sf::View view;
  sf::View viewStatic;
  sf::Text countText;
  sf::Text drawTimeText;
  sf::Text modeText;

  std::vector<SomeObjectWithArea> vecObjects;
  StaticQuadTree<SomeObjectWithArea> quadtree;
  const float area = 100000.f;
  const float zoomFactor = 0.3f;
  float dragFactor = 1.2f;

  bool usingQuadTree = false;

  void setupSFML() {
    // Setup main window
    window.create(sf::VideoMode(WIDTH, HEIGHT), "Template text", sf::Style::Close);
    window.setFramerateLimit(75);

    // Font for some test text
    genericFont.loadFromFile("../../src/fonts/Minecraft rus.ttf");

    // Rectangles draw count text setup
    countText.setString("1000000");
    countText.setFont(genericFont);
    countText.setCharacterSize(20);
    countText.setOutlineColor(sf::Color(31, 31, 31));
    countText.setOutlineThickness(3.f);
    countText.setPosition({0.f, 0.f});

    // Draw time text setup
    drawTimeText.setString("1000 ms");
    drawTimeText.setFont(genericFont);
    drawTimeText.setCharacterSize(20);
    drawTimeText.setOutlineColor(sf::Color(31, 31, 31));
    drawTimeText.setOutlineThickness(3.f);
    drawTimeText.setPosition({0.f , countText.getLocalBounds().height + 10.f});

    // Draw time text setup
    modeText.setString("Linear");
    modeText.setFont(genericFont);
    modeText.setCharacterSize(20);
    modeText.setOutlineColor(sf::Color(31, 31, 31));
    modeText.setOutlineThickness(3.f);
    modeText.setPosition({0.f , drawTimeText.getPosition().y + drawTimeText.getLocalBounds().height + 10.f});

    view.setSize(WIDTH, HEIGHT);
    view.setCenter(WIDTH / 2.f, HEIGHT / 2.f);

    viewStatic.setSize(WIDTH, HEIGHT);
    viewStatic.setCenter(WIDTH / 2.f, HEIGHT / 2.f);
  }

  void setupProgram() {
    srand((unsigned)time(NULL));
    quadtree.resize(qt::Rectangle({0.f, 0.f}, {area, area}));

    for (int i = 0; i < 1e6; i++) {
      SomeObjectWithArea obj(
        {random(0.f, area), random(0.f, area)},
        {},
        {random(0.1f, 100.f), random(0.1f, 100.f)},
        sf::Color(rand() % 256, rand() % 256, rand() % 256)
      );

      vecObjects.push_back(obj);
      quadtree.insert(obj, qt::Rectangle(obj.pos, obj.size));
    }
  }

  void onZoom(float direction) {
    sf::Vector2f prevSize = view.getSize();
    view.zoom(1.f + direction * zoomFactor);
    sf::Vector2f currSize = view.getSize();

    float diff = (currSize.x + currSize.y) / (prevSize.x + prevSize.y);
    dragFactor *= diff;
  }

  void updateView() {
    view.move((mousePressed - mouse) * dragFactor);
    mousePressed = mouse;
    window.setView(view);
  }

  void draw() {
    sf::VertexArray vertices{sf::Quads};
    qt::Rectangle screen = {view.getCenter() - view.getSize() / 2.f, view.getSize()};

    int count = 0;
    sf::Clock clock;

    if (usingQuadTree) {
      for (const SomeObjectWithArea& obj : quadtree.search(screen)) {
        for (int i = 0; i < 4; i++)
          vertices.append(obj.vertices[i]);
        count++;
      }
      modeText.setString("Quad Tree");
    } else {
      for (const SomeObjectWithArea& obj : vecObjects)
        if (screen.overlaps({obj.pos, obj.size})) {
          for (int i = 0; i < 4; i++)
            vertices.append(obj.vertices[i]);
          count++;
        }

      modeText.setString("Linear");
    }

    drawTimeText.setString("Draw time: " + std::to_string(clock.restart().asSeconds()) + " sec");
    countText.setString("Count: " + std::to_string(count) + " / " + std::to_string(vecObjects.size()));

    window.draw(vertices);

    window.setView(viewStatic);
    window.draw(countText);
    window.draw(drawTimeText);
    window.draw(modeText);
  }

  public:
    App() {}

    ~App() {}

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

          if (event.type == sf::Event::KeyReleased)
            switch (event.key.code) {
              case sf::Keyboard::Q:
                window.close();
                break;
              case sf::Keyboard::Tab:
                usingQuadTree = !usingQuadTree;
                break;
              default:
                break;
            }

          if (event.type == sf::Event::MouseMoved) {
            mouse.x = static_cast<float>(sf::Mouse::getPosition(window).x);
            mouse.y = static_cast<float>(sf::Mouse::getPosition(window).y);
          }

          if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            mousePressed.x = static_cast<float>(sf::Mouse::getPosition(window).x);
            mousePressed.y = static_cast<float>(sf::Mouse::getPosition(window).y);
          }

          if (event.type == sf::Event::MouseWheelScrolled) {
            if      (event.mouseWheelScroll.delta > 0.f) onZoom(-1);
            else if (event.mouseWheelScroll.delta < 0.f) onZoom(1);
          }
        }
        if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
          mousePressed = mouse;

        updateView();

        window.clear();

        draw();

        window.display();
      }
    }
};

