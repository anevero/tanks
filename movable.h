#ifndef MOVABLE_H
#define MOVABLE_H

#include <QDebug>
#include <QVector>
#include <QList>
#include "map.h"

enum class Direction { Up = 0, Right = 1, Down = 2, Left = 3 };

class Movable {
 public:
  Movable(Map* map, int cell_x, int cell_y, Direction direction, int speed);
  virtual ~Movable() = default;

  virtual void Move(int milliseconds_passed);
  virtual void TurnReverseOn();
  virtual void TurnReverseOff();
  virtual void StartMovement(int number_of_cells);
  virtual void SwitchToNextDirection();
  virtual void SwitchToPrevDirection();
  virtual void UpdateCoordinates();
  virtual void Draw(QPainter& painter) = 0;

  virtual int GetSpeed() const;
  virtual int GetTimeToFinishMovement() const;
  virtual int GetCellsToFinishMovement() const;
  virtual int GetReverseState() const;
  virtual Direction GetDirection() const;
  virtual int GetIntDirection() const;
  virtual int GetUpperLeftX() const;
  virtual int GetUpperLeftY() const;

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
  // направления движения
  QVector<int> directions_ = {0, 0, 0, 0};

  // миллисекунд на клетку
  int speed_;

  // 1: если задний ход включен
  // -1: если задний ход выключен
  int reverse_ = 1;

  // для таймера
  int time_to_finish_movement_ = 0;
  int cells_to_finish_movement_ = 0;
};

#endif  // MOVABLE_H
