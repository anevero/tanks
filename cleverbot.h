#ifndef CLEVERBOT_H
#define CLEVERBOT_H

#include <QQueue>
#include <QVector>
#include <memory>
#include "improvedbot.h"

class CleverBot : public ImprovedBot {
 public:
  CleverBot(const std::shared_ptr<Map>& map, BotQualities& qualities);
  void LoadImage() override;

 protected:
  bool IsRotationStartNeeded(const std::shared_ptr<Tank>&) override;
  bool IsMovingStartNeeded(
      const QList<std::shared_ptr<Movable>>& objects) override;

 private:
  struct CellInfo {
    int cell_x;
    int cell_y;
    int distance;
  };
  QVector<QVector<int>> distance_;

  void Bfs(const QList<std::shared_ptr<Movable>>& objects, int cell_x,
           int cell_y);
};

#endif  // CLEVERBOT_H
