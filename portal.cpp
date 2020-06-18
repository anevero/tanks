#include "portal.h"

Portal::Portal(const std::shared_ptr<Map>& map,
               const size_t x, const size_t y,
               const size_t new_cell_x, const size_t new_cell_y)
    : ObjectOnMap(map, x, y), new_cell_x_(new_cell_x), new_cell_y_(new_cell_y) {
  LoadImage(":/textures/portal.png");
}

size_t Portal::GetNewCellX() const { return new_cell_x_; }
size_t Portal::GetNewCellY() const { return new_cell_y_; }
