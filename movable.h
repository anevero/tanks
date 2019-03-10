#ifndef MOVABLE_H
#define MOVABLE_H

#include <QDebug>
#include <QVector>
#include "map.h"

enum class Direction { Up = 0, Down = 1, Left = 2, Right = 3 };

class Movable {
 public:
  Movable(Map* map, int cell_x, int cell_y, Direction direction, int speed);
  virtual ~Movable() = default;

  virtual void Move(int milliseconds_passed);
  virtual void StartMovement();
  virtual void UpdateCoordinates();
  virtual void Draw(QPainter& painter) = 0;

  virtual int GetSpeed() const;
  virtual int GetTimeToFinishMovement() const;
  virtual Direction GetDirection() const;

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
  QVector<int> directions_;

  // milliseconds on cell
  int speed_;

  // for timer
  int time_to_finish_movement_ = 0;
};

#endif  // MOVABLE_H
