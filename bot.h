#ifndef BOT_H
#define BOT_H

#include <QDebug>
#include <QImage>
#include <QPainter>
#include <cmath>
#include <memory>
#include <utility>
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
  Bot(std::shared_ptr<Map>& map, BotQualities qualities);
  virtual ~Bot() override = default;
  virtual void LoadImage() override;
  virtual bool IsTurnNeeded() const;
  virtual bool IsMoveNeeded() const;
  virtual bool IsRotationStartNeeded(std::shared_ptr<Tank>);
  virtual bool IsMovingStartNeeded(const QList<std::shared_ptr<Movable>>&);
  virtual bool IsShotNeeded(std::shared_ptr<Map>, std::shared_ptr<Tank>);
  int GetCurrentCharge() const = delete;
  int GetMaxCharge() const = delete;
  void MinusCharge(int charge = 1) = delete;
  void PlusCharge(int charge = 10) = delete;

 protected:
  int number_of_cells_to_move_ = 0;
  int number_of_turns_ = 0;
  const int moving_length_;
  const int amount_of_turns_;
  const int side_rotation_frequency_;

  virtual bool CheckDirection(int& tank, int& bot, int direction);
  bool IsWallBetweenObjectsX(std::shared_ptr<Map> map, int tank_x, int tank_y,
                             int bot_x, int bot_y);
  bool IsWallBetweenObjectsY(std::shared_ptr<Map> map, int tank_x, int tank_y,
                             int bot_x, int bot_y);
};

#endif  // BOT_H
