#ifndef CLEVERBOT_H
#define CLEVERBOT_H

#include "improvedbot.h"
#include <QVector>
#include <QQueue>

class CleverBot : public ImprovedBot {
 public:
  CleverBot(std::shared_ptr<Map>& map, BotQualities qualities);
  void LoadImage() override;

 protected:
  bool IsRotationStartNeeded(std::shared_ptr<Tank> tank) override;
  bool IsMovingStartNeeded(const QList<std::shared_ptr<Movable>>& objects) override;

 private:
  struct CellInfo {
    int cell_x, cell_y;
    int distance;
  };
  QVector<QVector<int>> distance_;

  void Bfs(const QList<std::shared_ptr<Movable>> objects,
           int cell_x, int cell_y);

  int height_;
  int width_;
};

#endif // CLEVERBOT_H
