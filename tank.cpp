#include "tank.h"

Tank::Tank(Map* map)
    : cell_x_(map->tank_init_cell_x_),
      cell_y_(map->tank_init_cell_y_),
      map_(map) {}

void Tank::UpdateCoordinates() {
  // count cell coordinates from map coordinates
}

void Tank::DrawTank(QPainter& painter) {
  painter.setBrush(Qt::black);

  // painter.drawEllipse(ellipse_x, ellipse_y, ellipse_width, ellipse_height);
}
