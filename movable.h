﻿#ifndef MOVABLE_H
#define MOVABLE_H

#include <QDebug>
#include <QImage>
#include <QList>
#include <QVector>
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#include "map.h"
#include "objectonmap.h"
#include "portal.h"

enum class Direction { Up = 0, Right = 1, Down = 2, Left = 3 };

struct Coordinates {
  int x, y;
};

class Movable : public std::enable_shared_from_this<Movable> {
 public:
  Movable(const std::shared_ptr<Map>& map, const int cell_x, const int cell_y,
          const Direction direction, const int speed);
  virtual ~Movable() = default;
  virtual void LoadImage() = 0;

  virtual void StartMovement(
      const int number_of_cells, const QList<std::shared_ptr<Movable>>& tanks,
      QList<QPair<std::shared_ptr<Movable>, Coordinates>>& objects_copies_,
      std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>& objects);
  virtual void Move(const int milliseconds_passed);
  virtual void TurnReverseOn();
  virtual void TurnReverseOff();

  virtual void StartRotation();
  virtual void Rotate(const int milliseconds_passed);
  virtual void TurnRotationReverseOn();
  virtual void TurnRotationReverseOff();

  virtual void UpdateCoordinates();
  virtual void Draw(QPainter& painter) = 0;
  virtual void ReturnToOriginal();

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
  virtual int GetWidth() const;
  virtual int GetHeight() const;
  virtual int GetCellX() const;
  virtual int GetCellY() const;

 protected:
  virtual void SwitchToNextDirection();
  virtual void SwitchToPrevDirection();
  virtual void RescaleImage();
  Coordinates GetNewPortalCells(int portal_cell_x, int portal_cell_y,
                                int new_cell_x, int new_cell_y);

  int cell_x_;
  int cell_y_;

  int cur_upper_left_x_{};
  int cur_upper_left_y_{};
  int prev_upper_left_x_{};
  int prev_upper_left_y_{};
  int cur_width_{};
  int cur_height_{};

  const std::shared_ptr<Map> map_;
  QVector<int> directions_ = {0, 0, 0, 0};

  int current_speed_{};
  const int basic_speed_{};
  int reverse_ = 1;
  int time_to_finish_movement_ = 0;
  int cells_to_finish_movement_ = 0;
  int current_rotate_degree_;
  int time_to_finish_rotation_ = 0;
  int rotate_reverse_ = 1;
  double opacity_ = 1;
  double prev_opacity_ = 1;
  bool copy_existence_ = false;

  QImage image_;
  QImage scaled_image_;
};

#endif  // MOVABLE_H
