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
  void DrawHealth(QPainter& painter);

  bool IsAbleToShoot() const;
  void IncreaseTimeSinceLastShot(int delta);
  void SetZeroTimeFromLastShot();
  void MinusHealth(int health = 10);
  void PlusHealth(int health = 10);

 protected:
  int rate_of_fire_;
  int time_since_last_shot_{};
  int health_ = 100;
};

#endif  // TANK_H
