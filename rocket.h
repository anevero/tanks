#ifndef ROCKET_H
#define ROCKET_H

#include <QDebug>
#include <QPainter>
#include <memory>
#include "tank.h"

class Rocket : public Movable {
 public:
  Rocket(std::shared_ptr<Map>& map, std::shared_ptr<Tank>& tank, int speed,
         int power);
  void LoadImage() override;
  void Draw(QPainter& painter) override;

  std::shared_ptr<Tank> GetAttachedTank() const;
  int GetPower() const;

 private:
  std::shared_ptr<Tank> tank_;
  int power_ = 0;
};

class LightRocket : Rocket {
  LightRocket();
};

class MediumRocket : Rocket {
  MediumRocket();
};

class HardRocket : Rocket {
  HardRocket();
};

#endif  // ROCKET_H
