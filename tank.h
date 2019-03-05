#ifndef TANK_H
#define TANK_H

#include <QPainter>
#include "map.h"

class Tank {
  friend class MainWindow;

 public:
  explicit Tank(Map* map);
  void UpdateCoordinates();
  void DrawTank(QPainter& painter);

 private:
  // координаты ячейки танка
  int cell_x_;
  int cell_y_;

  // координаты танка в окне
  int cur_upper_left_x_;
  int cur_upper_left_y_;
  int cur_width_;
  int cur_height_;

  // указатель на карту, к которой привязан танк
  Map* map_;
};

#endif  // TANK_H
