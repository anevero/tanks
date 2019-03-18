#ifndef TANK_H
#define TANK_H

#include <QPainter>
#include <memory>
#include "map.h"
#include "movable.h"

class Tank : public Movable {
 public:
  Tank(std::shared_ptr<Map>& map, int speed, int rate_of_fire,
       Direction direction);
  void Draw(QPainter& painter) override;

  bool IsAbleToShoot() const;
  void IncreaseTimeSinceLastShot(int delta);
  void SetZeroTimeFromLastShot();

 protected:
  int rate_of_fire_;
  int time_since_last_shot_{};
};

#endif  // TANK_H
