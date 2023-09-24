#include "SFML/Graphics.hpp"
#include <stdexcept>
#include <vector>

struct Point {
  float x, y;
};

struct Rectangle {
  // NOTE: Coords must point to the center of a rectangle
  float x, y, w, h;

  float top    = y - h;
  float right  = x + w;
  float bottom = y + h;
  float left   = x - w;

  bool contains(const Point& p) const {
    return (
      p.x >= x - w &&
      p.x <= x + w &&
      p.y >= y - h &&
      p.y <= y + h
    );
  }

  bool intersects(const Rectangle& r) const {
    return !(
      // Checking given rectangle is it off the quad by each side
      top    > r.bottom || // True: The rectangle under the current quad
      right  < r.left   || // True: The rectangle to the right of the current quad
      left   > r.right  || // True: The rectangle to the lefft the current quad
      bottom < r.top       // True: The rectangle above the current quad
    );
  }
};

class QuadTree {
  Rectangle boundary;
  int capacity;
  std::vector<Point> points;
  bool divided = false;

  QuadTree* northWest = nullptr;
  QuadTree* northEast = nullptr;
  QuadTree* southWest = nullptr;
  QuadTree* southEast = nullptr;

  void subdivide() {
    if (divided) return;
    divided = true;

    float& x = boundary.x;
    float& y = boundary.y;
    float& w = boundary.w;
    float& h = boundary.h;

    Rectangle nwRect{x - w / 2, y - h / 2, w / 2, h / 2};
    Rectangle neRect{x + w / 2, y - h / 2, w / 2, h / 2};
    Rectangle swRect{x - w / 2, y + h / 2, w / 2, h / 2};
    Rectangle seRect{x + w / 2, y + h / 2, w / 2, h / 2};

    northWest = new QuadTree(nwRect, capacity);
    northEast = new QuadTree(neRect, capacity);
    southWest = new QuadTree(swRect, capacity);
    southEast = new QuadTree(seRect, capacity);
  }

  public:

    QuadTree(Rectangle boundary, int capacity)
      : boundary(boundary), capacity(capacity) {
        points.reserve(capacity);
    }

    ~QuadTree() {
      delete northWest;
      delete northEast;
      delete southWest;
      delete southEast;
    }

    bool insert(Point p) {
      if (!boundary.contains(p)) return false;

      if (points.size() < capacity) {
        points.push_back(p);
        return true;
      } else {
        subdivide();
        if (
          northWest->insert(p) ||
          northEast->insert(p) ||
          southWest->insert(p) ||
          southEast->insert(p)
        ) return true;
        else
          throw std::runtime_error("The point somehow didn't inserted in any quad");
      }
    }

    void query(std::vector<Point>& found, const Rectangle& range) {
      if (boundary.intersects(range)) {
        for (const Point& p : points)
          if (range.contains(p))
            found.push_back(p);
      }

      if (divided) {
        northWest->query(found, range);
        northEast->query(found, range);
        southWest->query(found, range);
        southEast->query(found, range);
      }
    }

    void show(sf::RenderWindow& win) {
      float& x = boundary.x;
      float& y = boundary.y;
      float& w = boundary.w;
      float& h = boundary.h;

      sf::Vertex lineVertical[] = {
        sf::Vertex({x, y - h}),
        sf::Vertex({x, y + h})
      };

      sf::Vertex lineHorizontal[] = {
        sf::Vertex({x - w, y}),
        sf::Vertex({x + w, y})
      };

      for (Point p : points) {
        sf::CircleShape c(1.f);
        c.setPosition(p.x, p.y);
        win.draw(c);
      }

      win.draw(lineVertical, 2, sf::Lines);
      win.draw(lineHorizontal, 2, sf::Lines);

      if (divided) {
        northWest->show(win);
        northEast->show(win);
        southWest->show(win);
        southEast->show(win);
      }
    }
};

