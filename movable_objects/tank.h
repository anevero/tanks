#ifndef MOVABLE_OBJECTS_TANK_H_
#define MOVABLE_OBJECTS_TANK_H_

#include <QApplication>
#include <QMessageBox>
#include <QPainter>
#include <algorithm>
#include <memory>
#include ".//game_core/map.h"
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

enum class ChargeState { Empty = 0, LessThanHalf = 1, MoreThanHalf = 2 };

class Tank : public Movable {
 public:
  Tank(const std::shared_ptr<Map>& map, int init_cell_x,
       int init_cell_y, const TankQualities& qualities);
  ~Tank() override = default;
  void Draw(QPainter* painter) override;
  virtual void DrawHealth(QPainter* painter);

  virtual bool IsAbleToShoot() const;
  virtual void IncreaseTimeSinceLastShot(int delta);
  virtual void SetZeroTimeFromLastShot();
  virtual int GetCurrentHealth() const;
  virtual int GetMaxHealth() const;
  virtual void MinusHealth(int health = 10);
  virtual void PlusHealth(int health = 10);
  virtual bool IsDead() const;
  virtual int GetTimeSinceLastShot() const;
  virtual int GetRateOfFire() const;

  void ChangeTypeOfCharge(int type);
  void MinusCharge(int type, int charge = 1);
  void PlusCharge();

  int GetTypeOfCharge() const;
  int GetCurrentCharge(int type) const;
  [[maybe_unused]] int GetMaxCharge(int type) const;
  ChargeState GetChargeState() const;

 protected:
  int rate_of_fire_;
  int time_since_last_shot_{};
  int type_of_charge_{};
  QVector<int> current_charge_;
  const QVector<int> max_charge_;
  const int max_health_{};
  int current_health_{};
};

#endif  // MOVABLE_OBJECTS_TANK_H_
