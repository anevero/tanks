#ifndef TANK_H
#define TANK_H

#include <QApplication>
#include <QMessageBox>
#include <QPainter>
#include <memory>
#include "map.h"
#include "movable.h"

class Tank : public Movable {
 public:
  Tank(std::shared_ptr<Map>& map, int init_cell_x, int init_cell_y, int speed,
       int rate_of_fire, Direction direction);
  virtual void Draw(QPainter& painter) override;
  virtual void DrawHealth(QPainter& painter);

  virtual bool IsAbleToShoot() const;
  virtual void IncreaseTimeSinceLastShot(int delta);
  virtual void SetZeroTimeFromLastShot();
  virtual void MinusHealth(int health = 10);
  virtual void PlusHealth(int health = 10);
  virtual bool IsDead() const;

 protected:
  int rate_of_fire_;
  int time_since_last_shot_{};
  int health_ = 100;
};

#endif  // TANK_H
