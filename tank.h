#ifndef TANK_H
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
  int max_light_charge;
  int max_medium_charge;
  int max_hard_charge;
  Direction direction;
};

class Tank : public Movable {
 public:
  Tank(std::shared_ptr<Map>& map, int init_cell_x, int init_cell_y,
       TankQualities qualities);
  virtual ~Tank() override = default;
  virtual void LoadImage() override;
  void Draw(QPainter& painter) override;
  virtual void DrawHealth(QPainter& painter);

  virtual bool IsAbleToShoot() const;
  virtual void IncreaseTimeSinceLastShot(int delta);
  virtual void SetZeroTimeFromLastShot();
  virtual int GetCurrentHealth() const;
  virtual int GetMaxHealth() const;
  virtual void MinusHealth(int health = 10);
  virtual void PlusHealth(int health = 10);
  int GetLightCurrentCharge() const;
  int GetMaxLightCharge() const;
  void MinusLightCharge(int charge = 1);
  void PlusLightCharge(int charge = 10);
  int GetMediumCurrentCharge() const;
  int GetMaxMediumCharge() const;
  void MinusMediumCharge(int charge = 1);
  void PlusMediumCharge(int charge = 10);
  int GetHardCurrentCharge() const;
  int GetMaxHardCharge() const;
  void MinusHardCharge(int charge = 1);
  void PlusHardCharge(int charge = 10);
  virtual bool IsDead() const;
  virtual int GetTimeSinceLastShot() const;
  virtual int GetRateOfFire() const;

 protected:
  int rate_of_fire_;
  int time_since_last_shot_{};
  int current_health_{};
  int light_current_charge_{};
  int medium_current_charge_{};
  int hard_current_charge_{};
  const int max_health_{};
  const int max_light_charge_{};
  const int max_medium_charge_{};
  const int max_hard_charge_{};
};

#endif  // TANK_H
