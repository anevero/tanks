#ifndef MOVABLES_ROCKET_H_
#define MOVABLES_ROCKET_H_

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
         int speed, int power, const TypeOfRocket& type);
  void Draw(QPainter* painter) override;

  std::shared_ptr<Tank> GetAttachedTank() const;
  int GetPower() const;
  TypeOfRocket GetTypeOfRocket() const;

 private:
  const std::shared_ptr<Tank> tank_;
  const int power_;
  const TypeOfRocket type_;
};

#endif  // MOVABLES_ROCKET_H_
