#ifndef BOT_H
#define BOT_H

#include <QDebug>
#include <QImage>
#include <QPainter>
#include <cmath>
#include <memory>
#include <utility>
#include <vector>
#include "map.h"
#include "tank.h"

struct BotQualities {
  TankQualities tank;
  int init_cell_x;
  int init_cell_y;
  int moving_length;
  int amount_of_turns;
  int side_rotation_frequency;
};

class Bot : public Tank {
 public:
  Bot(const std::shared_ptr<Map>& map, const BotQualities& qualities);
  ~Bot() override = default;
  void LoadImage() override;

  virtual bool IsTurnNeeded() const;
  virtual bool IsRotationStartNeeded(const std::shared_ptr<Tank>&);
  virtual bool IsMovingStartNeeded(
      const QList<std::shared_ptr<Movable>>&,
      const std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>&);
  virtual bool IsShotNeeded(const std::shared_ptr<Map>&,
                            const std::shared_ptr<Tank>&);

  void ChangeTypeOfCharge(int type) = delete;
  void MinusCharge(int type, int charge = 1) = delete;
  void PlusCharge() = delete;

  int GetTypeOfCharge() const = delete;
  int GetCurrentCharge(int type) const = delete;
  int GetMaxCharge(int type) const = delete;
  ChargeState GetChargeState() const = delete;

 protected:
  int number_of_cells_to_move_ = 0;
  int number_of_turns_ = 0;
  const int moving_length_;
  const int amount_of_turns_;
  const int side_rotation_frequency_;

  virtual bool CheckDirection(const int tank, const int bot, int direction);
  bool IsWallBetweenObjectsX(const std::shared_ptr<Map>& map, size_t tank_x,
                             size_t tank_y, size_t bot_x, size_t bot_y);
  bool IsWallBetweenObjectsY(const std::shared_ptr<Map>& map, size_t tank_x,
                             size_t tank_y, size_t bot_x, size_t bot_y);
};

#endif  // BOT_H
