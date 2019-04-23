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
  Rocket(const std::shared_ptr<Map>& map, const std::shared_ptr<Tank>& tank,
         const int speed, const int power, const TypeOfRocket& type);
  void LoadImage() override;
  void Draw(QPainter& painter) override;

  std::shared_ptr<Tank> GetAttachedTank() const;
  int GetPower() const;
  TypeOfRocket GetTypeOfRocket() const;

 private:
  const std::shared_ptr<Tank> tank_;
  const int power_;
  const TypeOfRocket type_;
};

#endif  // ROCKET_H
