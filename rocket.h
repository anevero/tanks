#ifndef ROCKET_H
#define ROCKET_H

#include <QPainter>
#include <QDebug>
#include "tank.h"

class Rocket : public Movable {
  friend class MainWindow;

 public:
  explicit Rocket(Map* map, Movable* movable, int speed);
  void Draw(QPainter& painter) override;
};

#endif // ROCKET_H
