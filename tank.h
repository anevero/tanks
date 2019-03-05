#ifndef TANK_H
#define TANK_H

#include <QPainter>
#include "map.h"

class Tank {
  friend class MainWindow;

 public:
  Tank(Map* map);
  void UpdateCoordinates();
  void DrawTank(QPainter& painter);

 private:
  int cell_x_;
  int cell_y_;

  int cur_upper_left_x_;
  int cur_upper_left_y_;
  int cur_width_;
  int cur_height_;

  int ms_for_one_cell_ = 1000;
  int time_after_last_animation_;  // if animation is finished - to 0

  Map* map_;
};

#endif  // TANK_H
