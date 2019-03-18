#ifndef TANK_H
#define TANK_H

#include <QPainter>
#include <memory>
#include "map.h"
#include "movable.h"

class Tank : public Movable {
  friend class MainWindow;

 public:
  Tank(std::shared_ptr<Map>& map, int speed, int rate_of_fire,
       Direction direction);
  void Draw(QPainter& painter) override;

  int GetRateOfFire() const;
  int GetTimeSinceLastShot() const;
  bool IsAbleToShoot() const;
  void IncreaseTimeSinceLastShot(int delta);

 protected:
  int rate_of_fire_;
  int time_since_last_shot_ = 0;
};

#endif  // TANK_H
