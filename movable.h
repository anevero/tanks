#ifndef MOVABLE_H
#define MOVABLE_H

#include <QDebug>
#include <QList>
#include <QVector>
#include "map.h"

enum class Direction { Up = 0, Right = 1, Down = 2, Left = 3 };

class Movable {
 public:
  Movable(Map* map, int cell_x, int cell_y, Direction direction, int speed);
  virtual ~Movable() = default;

  virtual void StartMovement(int number_of_cells);
  virtual void Move(int milliseconds_passed);
  virtual void TurnReverseOn();
  virtual void TurnReverseOff();

  virtual void StartRotation();
  virtual void Rotate(int milliseconds_passed);
  virtual void TurnRotationReverseOn();
  virtual void TurnRotationReverseOff();

  virtual void UpdateCoordinates();
  virtual void Draw(QPainter& painter) = 0;

  virtual int GetSpeed() const;

  virtual int GetTimeToFinishMovement() const;
  virtual int GetCellsToFinishMovement() const;
  virtual int GetTimeToFinishRotation() const;
  virtual bool IsMovingOrRotating() const;

  virtual int GetReverseState() const;
  virtual int GetRotationReverseState() const;
  virtual int GetIntDirection() const;
  virtual Direction GetDirection() const;

  virtual int GetUpperLeftX() const;
  virtual int GetUpperLeftY() const;
  virtual int GetCellX() const;
  virtual int GetCellY() const;

 protected:
  virtual void SwitchToNextDirection();
  virtual void SwitchToPrevDirection();

  int cell_x_;
  int cell_y_;

  int cur_upper_left_x_;
  int cur_upper_left_y_;
  int cur_width_;
  int cur_height_;

  Map* map_;
  QVector<int> directions_ = {0, 0, 0, 0};

  int speed_;
  int reverse_ = 1;
  int time_to_finish_movement_ = 0;
  int cells_to_finish_movement_ = 0;
  int current_rotate_degree_;
  int time_to_finish_rotation_ = 0;
  int rotate_reverse_ = 1;
};

#endif  // MOVABLE_H
