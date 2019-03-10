#ifndef MOVABLE_H
#define MOVABLE_H

#include "map.h"

enum class Direction { Up, Down, Left, Right };

class Movable {
 public:
  Movable(Map* map, int cell_x, int cell_y);
  virtual void UpdateCoordinates();
  virtual void Draw(QPainter& painter) = 0;
  virtual ~Movable() = default;

 protected:
  // координаты ячейки объекта
  int cell_x_;
  int cell_y_;

  // координаты объекта в окне
  int cur_upper_left_x_;
  int cur_upper_left_y_;
  int cur_width_;
  int cur_height_;

  // указатель на карту, к которой привязан объект
  Map* map_;

 protected:
  // milliseconds on cell
  int speed_;

  // for timer
  int time_to_finish_movement_ = 0;

  Direction cur_direction_;

 public:
  int GetSpeed();
  int GetTimeToFinishMovement();
  Direction GetDirection();
};

#endif  // MOVABLE_H
