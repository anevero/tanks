#ifndef MOVABLE_OBJECTS_TANK_H_
#define MOVABLE_OBJECTS_TANK_H_

#include <QPainter>

#include <chrono>
#include <memory>
#include <random>
#include <utility>
#include <vector>

#include "../game_core/map.h"
#include "movable.h"

struct TankParameters {
  int speed;
  int rate_of_fire;
  int max_health;
  int max_light_charge;
  int max_medium_charge;
  int max_hard_charge;
};

enum class ChargeState {
  Empty = 0,
  LessThanHalf = 1,
  MoreThanHalf = 2
};

class Tank : public Movable {
 public:
  Tank(std::shared_ptr<const Map> map, Coordinates init_cell,
       TankParameters parameters, Direction direction);
  ~Tank() override = default;

  void Draw(QPainter* painter) override;
  virtual void DrawHealth(QPainter* painter);

  virtual bool IsAbleToShoot() const;
  virtual void IncreaseTimeSinceLastShot(int delta);
  virtual void SetZeroTimeFromLastShot();

  virtual int GetTimeSinceLastShot() const;
  virtual int GetRateOfFire() const;

  virtual void DecreaseHealth(int health);
  virtual void IncreaseHealth(int health);

  virtual int GetCurrentHealth() const;
  virtual int GetMaxHealth() const;
  virtual bool IsDead() const;

  void ChangeTypeOfCharge(int type);
  void DecreaseCharge(int type, int charge = 1);
  void IncreaseCharge();

  int GetTypeOfCharge() const;
  int GetCurrentCharge(int type) const;
  ChargeState GetChargeState() const;

 protected:
  int rate_of_fire_;
  int time_since_last_shot_;

  int current_type_of_charge_;
  std::vector<int> current_charge_;

  int current_health_;

  const std::vector<int> max_charge_;
  const int max_health_;

  static std::mt19937 random_generator_;
};

#endif  // MOVABLE_OBJECTS_TANK_H_
