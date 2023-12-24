#include <list>
#include "pch.h"

#define CAPACITY 4

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
    return
      p.x >= left  &&
      p.x <= right &&
      p.y >= top   &&
      p.y <= bottom;
  }

  bool intersects(const Rectangle& r) const {
    return !(
      // Checking given rectangle is it off the quad by each side
      top    > r.bottom || // True: The rectangle under the current quad
      right  < r.left   || // True: The rectangle to the right of the current quad
      left   > r.right  || // True: The rectangle to the left the current quad
      bottom < r.top       // True: The rectangle above the current quad
    );
  }
};

class Node {
  Rectangle boundary;
  std::list<Point> points;
  bool divided = false;

  Node* northWest = nullptr;
  Node* northEast = nullptr;
  Node* southWest = nullptr;
  Node* southEast = nullptr;

  void subdivide() {
    if (!divided) {
      const float& x = boundary.x;
      const float& y = boundary.y;
      const float& w = boundary.w;
      const float& h = boundary.h;

      Rectangle nwRect{x - w / 2.f, y - h / 2.f, w / 2.f, h / 2.f};
      Rectangle neRect{x + w / 2.f, y - h / 2.f, w / 2.f, h / 2.f};
      Rectangle swRect{x - w / 2.f, y + h / 2.f, w / 2.f, h / 2.f};
      Rectangle seRect{x + w / 2.f, y + h / 2.f, w / 2.f, h / 2.f};

      northWest = new Node(nwRect);
      northEast = new Node(neRect);
      southWest = new Node(swRect);
      southEast = new Node(seRect);

      divided = true;
    }
  }

  public:

    Node(Rectangle boundary) : boundary(boundary) {}

    ~Node() {
      delete northWest;
      delete northEast;
      delete southWest;
      delete southEast;
    }

    bool insert(Point p) {
      if (!boundary.contains(p)) return false;

      if (points.size() < CAPACITY) {
        points.push_back(p);
        return true;
      } else {
        subdivide();
        return
          northWest->insert(p) ||
          northEast->insert(p) ||
          southWest->insert(p) ||
          southEast->insert(p);
      }
    }

    void query(std::list<const Point*>& found, const Rectangle& range) {
      if (boundary.intersects(range)) {
        for (const Point& p : points)
          if (range.contains(p))
            found.push_back(&p);

        if (divided) {
          northWest->query(found, range);
          northEast->query(found, range);
          southWest->query(found, range);
          southEast->query(found, range);
        }
      }
    }

    void show(sf::RenderWindow& win) {
      static const sf::Color color{30, 30, 30};
      const float& x = boundary.x;
      const float& y = boundary.y;
      const float& w = boundary.w;
      const float& h = boundary.h;

      sf::Vertex lineVertical[] = {
        sf::Vertex({x, y - h}, color),
        sf::Vertex({x, y + h}, color)
      };

      sf::Vertex lineHorizontal[] = {
        sf::Vertex({x - w, y}, color),
        sf::Vertex({x + w, y}, color)
      };

      for (const Point& p : points) {
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

