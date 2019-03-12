#ifndef TANK_H
#define TANK_H

#include <QPainter>
#include "map.h"
#include "movable.h"

class Tank : public Movable {
  friend class MainWindow;

 public:
  Tank(Map* map, int speed, int rate_of_fire);
  void Draw(QPainter& painter) override;

  virtual int GetRateOfFire() const;
  virtual int GetTimeSinceLastShot() const;

 protected:
  // это скорострельность, потому что такой перевод
  // зато новое слово выучим :)
  int rate_of_fire_;
  int time_since_last_shot_ = 10000;
};

#endif  // TANK_H
