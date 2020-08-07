#include "portal.h"

Portal::Portal(std::shared_ptr<const Map> map,
               Coordinates cell, Coordinates new_cell)
    : StaticObject(std::move(map), ":/textures/portal.png", cell),
      new_cell_(new_cell) {}

Coordinates Portal::GetNewCell() const {
  return new_cell_;
}
