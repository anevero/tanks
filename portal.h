#ifndef PORTAL_H
#define PORTAL_H

#include <QDebug>
#include <QImage>
#include "objectonmap.h"

class Portal : public ObjectOnMap {
 public:
  Portal(const std::shared_ptr<Map>& map, const int cell_x, const int cell_y,
         const int new_cell_x, const int new_cell_y);

  void LoadImage() override;
  int GetNewCellX() const;
  int GetNewCellY() const;

 private:
  int new_cell_x_;
  int new_cell_y_;
};

#endif // PORTAL_H
