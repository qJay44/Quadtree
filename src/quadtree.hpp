#include "SFML/Graphics.hpp"
#include <stdexcept>
#include <vector>

struct Rectangle {
  sf::Vector2f pos;
  sf::Vector2f size;

  Rectangle(const sf::Vector2f& position = { 0.f, 0.f }, const sf::Vector2f& size = { 1.f, 1.f })
    : pos(position), size(size) {}

  constexpr bool contains(const sf::Vector2f& p) const {
    return !(
      p.x < pos.x ||
      p.y < pos.y ||
      p.x >= (pos.x + size.x) ||
      p.y >= (pos.y + size.y)
    );
  }

  constexpr bool contains(const Rectangle& r) const {
    return (
      (r.pos.x >= pos.x) &&
      (r.pos.y >= pos.y) &&
      (r.pos.x + r.size.x < pos.x + size.x) &&
      (r.pos.y + r.size.y < pos.y + size.y)
    );
  }

  constexpr bool overlaps(const Rectangle& r) const {
    return (
      pos.x < r.pos.x + r.size.x &&
      pos.y < r.pos.y + r.size.y &&
      pos.x + size.x >= r.pos.x  &&
      pos.y + size.y >= r.pos.y
    );
  }
};

