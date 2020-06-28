#ifndef MOVABLE_OBJECTS_ROCKET_H_
#define MOVABLE_OBJECTS_ROCKET_H_

#include <QDebug>
#include <QPainter>
#include <memory>
#include <utility>
#include "tank.h"

struct RocketParameters {
  int power;
  int speed;
  bool obstacle_break;
};

class Rocket : public Movable {
 public:
  Rocket(std::shared_ptr<Map> map, std::shared_ptr<Tank> tank,
         RocketParameters parameters);
  ~Rocket() override = default;

  void Draw(QPainter* painter) override;

  std::shared_ptr<Tank> GetAttachedTank() const;
  int GetPower() const;
  bool CanBreakObstacles() const;

 private:
  const std::shared_ptr<Tank> tank_;
  int power_;
  bool obstacle_break_;
};

#endif  // MOVABLE_OBJECTS_ROCKET_H_
