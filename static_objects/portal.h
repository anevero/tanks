#ifndef STATIC_OBJECTS_PORTAL_H_
#define STATIC_OBJECTS_PORTAL_H_

#include <QDebug>
#include <QImage>
#include <memory>
#include "objectonmap.h"

class Portal : public ObjectOnMap {
 public:
  Portal(std::shared_ptr<Map> map, int cell_x, int cell_y,
         int new_cell_x, int new_cell_y);

  int GetNewCellX() const;
  int GetNewCellY() const;

 private:
  int new_cell_x_;
  int new_cell_y_;
};

#endif  // STATIC_OBJECTS_PORTAL_H_
