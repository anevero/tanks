#include "portal.h"

Portal::Portal(std::shared_ptr<const Map> map,
               Coordinates cell, Coordinates new_cell)
    : ObjectOnMap(std::move(map), cell), new_cell_(new_cell) {
  LoadImage(":/textures/portal.png");
}

Coordinates Portal::GetNewCell() const {
  return new_cell_;
}
