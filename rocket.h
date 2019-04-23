#ifndef ROCKET_H
#define ROCKET_H

#include <QDebug>
#include <QPainter>
#include <memory>
#include "tank.h"

enum class TypeOfRocket { LightRocket = 0, MediumRocket = 1, HardRocket = 2 };

struct RocketParameters {
  int power_;
  int speed_;
  bool obstacle_break_;
};

class Rocket : public Movable {
 public:
  Rocket(std::shared_ptr<Map>& map, std::shared_ptr<Tank>& tank, int speed,
         int power, TypeOfRocket type);
  void LoadImage() override;
  void Draw(QPainter& painter) override;

  std::shared_ptr<Tank> GetAttachedTank() const;
  int GetPower() const;
  TypeOfRocket GetTypeOfRocket() const;

 private:
  std::shared_ptr<Tank> tank_;
  int power_ = 0;
  TypeOfRocket type_;
};

#endif  // ROCKET_H
