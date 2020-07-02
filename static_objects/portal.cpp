#include "portal.h"

Portal::Portal(std::shared_ptr<const Map> map,
               int x, int y, int new_cell_x, int new_cell_y)
    : ObjectOnMap(std::move(map), x, y),
      new_cell_x_(new_cell_x),
      new_cell_y_(new_cell_y) {
  LoadImage(":/textures/portal.png");
}

int Portal::GetNewCellX() const {
  return new_cell_x_;
}

int Portal::GetNewCellY() const {
  return new_cell_y_;
}
