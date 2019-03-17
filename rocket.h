#ifndef ROCKET_H
#define ROCKET_H

#include <QDebug>
#include <QPainter>
#include "tank.h"

class Rocket : public Movable {
 public:
  Rocket(std::shared_ptr<Map> map, std::shared_ptr<Movable> movable, int speed);
  void Draw(QPainter& painter) override;

  void Explosion();
};

#endif  // ROCKET_H
