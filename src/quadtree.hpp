#include "SFML/Graphics.hpp"
#include <array>
#include <list>
#include <memory>
#include <stdexcept>
#include <vector>

namespace qt {
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
}

constexpr size_t MAX_DEPTH = 8;

template<typename T>
class StaticQuadTree {

public:
  StaticQuadTree(const qt::Rectangle& size = {{0.f, 0.f}, {100.f, 100.f}}, size_t depth = 0)
   : depth(depth) {
    resize(size);
  }

  void resize(const qt::Rectangle& area) {
    clear();
    rect = area;
    sf::Vector2f childSize = rect.size / 2.f;
    childrenAreas = {
      qt::Rectangle(rect.pos, childSize),
      qt::Rectangle({rect.pos.x + childSize.x, rect.pos.y}, childSize),
      qt::Rectangle({rect.pos.x, rect.pos.y + childSize.y}, childSize),
      qt::Rectangle(rect.pos + childSize, childSize)
    };
  }

  void clear() {
    items.clear();
    for (int i = 0; i < 4; i++) {
      if (childrenPtrs[i])
        childrenPtrs[i]->clear();
      childrenPtrs[i].reset();
    }
  }

  size_t size() const {
    size_t count = items.size();
    for (int i = 0; i < 4; i++)
      if (childrenPtrs[i]) count += childrenPtrs[i]->size();

    return count;
  }

  void insert(const T& item, const qt::Rectangle& itemSize) {
    for (int i = 0; i < 4; i++) {
      if (childrenAreas[i].contains(itemSize)) {
        if (depth + 1 < MAX_DEPTH) {
          if (!childrenPtrs[i]) {
            childrenPtrs[i] = std::make_shared<StaticQuadTree<T>>(childrenAreas[i], depth + 1);
          }

          childrenPtrs[i]->insert(item, itemSize);
          return;
        }
      }
    }
    items.push_back({itemSize, item});
  }

  std::list<T> search(const qt::Rectangle& area) const {
    std::list<T> listItems;
    search(area, listItems);

    return listItems;
  }

  void search(const qt::Rectangle& area, std::list<T>& listItems) const {
    // First, check for items belonging to this area, add them to the list
    // if there is overlap
    for (const auto& pair : items)
      if (area.overlaps(pair.first))
        listItems.push_back(pair.second);

    // Secondly, recurse through children and see if they can
    // add to the list
    for (int i = 0; i < 4; i++) {
      if (childrenPtrs[i]) {
        // If child is entirely contained within area, recursively
        // add all of its children, no need to check boundaries
        if (area.contains(childrenAreas[i]))
          childrenPtrs[i]->getItems(listItems);

        // If child overlaps with search area then checks need to be made
        else if (childrenAreas[i].overlaps(area))
          childrenPtrs[i]->search(area, listItems);
      }
    }
  }

  void getItems(std::list<T>& listItems) const {
    for (const auto& pair : items)
      listItems.push_back(pair.second);

    for (int i = 0; i < 4; i++)
      if (childrenPtrs[i])
        childrenPtrs[i]->getItems(listItems);
  }

  const qt::Rectangle& getArea() const {
    return rect;
  }

protected:
  size_t depth = 0;

  // Area of this quad tree
  qt::Rectangle rect;

  // 4 child areas of this quad tree
  std::array<qt::Rectangle, 4> childrenAreas{};

  // 4 potential children of this quad tree
  std::array<std::shared_ptr<StaticQuadTree<T>>, 4> childrenPtrs{};

  // Items which belong to this quad tree
  std::vector<std::pair<qt::Rectangle, T>> items;
};

