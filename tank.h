#ifndef TANK_H
#define TANK_H

#include <QPainter>
#include "map.h"
#include "movable.h"

class Tank : public Movable {
  friend class MainWindow;

 public:
  explicit Tank(Map* map);
  void Draw(QPainter& painter) override;

  void SetMoveDirection(Direction direction);
  void SetRotateDirection(Direction direction);
};

#endif  // TANK_H
