#include "portal.h"

Portal::Portal(std::shared_ptr<Map>& map, int x, int y,
               int new_cell_x, int new_cell_y)
    : ObjectOnMap(map, x, y), new_cell_x_(new_cell_x), new_cell_y_(new_cell_y) {
  LoadImage();
}

void Portal::LoadImage() {
  image_.load(":/textures/portal.png");
  scaled_image_ = image_;
}

int Portal::GetNewCellX() const { return new_cell_x_; }
int Portal::GetNewCellY() const { return new_cell_y_; }
