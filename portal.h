#ifndef PORTAL_H
#define PORTAL_H

#include <QDebug>
#include <QImage>
#include <memory>
#include "objectonmap.h"

class Portal : public ObjectOnMap {
 public:
  Portal(const std::shared_ptr<Map>& map, size_t cell_x, size_t cell_y,
         size_t new_cell_x, size_t new_cell_y);

  void LoadImage() override;
  size_t GetNewCellX() const;
  size_t GetNewCellY() const;

 private:
  size_t new_cell_x_;
  size_t new_cell_y_;
};

#endif  // PORTAL_H
