#ifndef MOVABLE_OBJECTS_ROCKET_H_
#define MOVABLE_OBJECTS_ROCKET_H_

#include <QPainter>
#include <memory>
#include <utility>

#include "tank.h"

struct RocketParameters {
  int power;
  int speed;
  bool can_break_obstacle;
};

class Rocket : public Movable {
 public:
  Rocket(std::shared_ptr<const Map> map, std::shared_ptr<Tank> tank,
         RocketParameters parameters);
  ~Rocket() override = default;

  void Draw(QPainter* painter) override;

  std::shared_ptr<const Tank> GetAttachedTank() const;
  int GetPower() const;
  bool CanBreakObstacles() const;

 private:
  const std::shared_ptr<const Tank> tank_;
  int power_;
  bool can_break_obstacle_;
};

#endif  // MOVABLE_OBJECTS_ROCKET_H_
