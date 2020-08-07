#ifndef MOVABLE_OBJECTS_CLEVER_BOT_H_
#define MOVABLE_OBJECTS_CLEVER_BOT_H_

#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "improved_bot.h"
#include "tank.h"
#include "../game_core/map.h"
#include "../static_objects/portal.h"

class CleverBot : public ImprovedBot {
 public:
  CleverBot(std::shared_ptr<const Map> map, const QString& path,
            Coordinates initial_cell, TankParameters tank_parameters,
            BotParameters bot_parameters, Direction direction);
  ~CleverBot() override = default;

 protected:
  bool IsRotationStartNeeded(const std::shared_ptr<const Tank>& tank) override;
  bool IsMovingStartNeeded(
      const std::list<std::shared_ptr<Tank>>& objects,
      const std::vector<std::vector<std::shared_ptr<StaticObject>>>& portals)
  override;

 private:
  struct CellInfo {
    Coordinates cell;
    Coordinates previous_cell;
    int distance;

    CellInfo(Coordinates cell, Coordinates previous_cell, int distance)
        : cell(cell), previous_cell(previous_cell), distance(distance) {}
  };
  std::vector<std::vector<int>> distance_;

  void MaybeChangeRotationDirection(int delta_x, bool condition);

  void Bfs(
      const std::list<std::shared_ptr<Tank>>& objects,
      const std::vector<std::vector<std::shared_ptr<StaticObject>>>& portals,
      Coordinates cell);

  int height_;
  int width_;
};

#endif  // MOVABLE_OBJECTS_CLEVER_BOT_H_
