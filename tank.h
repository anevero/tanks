﻿#ifndef TANK_H
#define TANK_H

#include <QApplication>
#include <QMessageBox>
#include <QPainter>
#include <memory>
#include "map.h"
#include "movable.h"

struct TankQualities {
  int speed;
  int rate_of_fire;
  int max_health;
  Direction direction;
};

class Tank : public Movable {
 public:
  Tank(std::shared_ptr<Map>& map, int init_cell_x, int init_cell_y,
       TankQualities qualities);
  virtual void LoadImage() override;
  void Draw(QPainter& painter) override;
  void UpdateCoordinates(int cell_x, int cell_y);
  virtual void DrawHealth(QPainter& painter);

  virtual bool IsAbleToShoot() const;
  virtual void IncreaseTimeSinceLastShot(int delta);
  virtual void SetZeroTimeFromLastShot();
  virtual int GetCurrentHealth() const;
  virtual int GetMaxHealth() const;
  virtual void MinusHealth(int health = 10);
  virtual void PlusHealth(int health = 10);
  virtual bool IsDead() const;

 protected:  
  int rate_of_fire_;
  int time_since_last_shot_{};
  int current_health_{};
  const int max_health_{};
};

#endif  // TANK_H
