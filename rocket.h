#ifndef ROCKET_H
#define ROCKET_H

#include <QPainter>
#include <QDebug>
#include "tank.h"

class Rocket : public Movable {
 public:
  Rocket(Map* map, Movable* movable, int speed);
  void Draw(QPainter& painter) override;

  void Explosion();
};

#endif // ROCKET_H
