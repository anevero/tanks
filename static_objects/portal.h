#ifndef STATIC_OBJECTS_PORTAL_H_
#define STATIC_OBJECTS_PORTAL_H_

#include <memory>
#include <utility>
#include "static_object.h"
#include "../game_core/map.h"

class Portal : public StaticObject {
 public:
  Portal(std::shared_ptr<const Map> map, Coordinates cell,
         Coordinates new_cell);

  Coordinates GetNewCell() const;

 private:
  Coordinates new_cell_;
};

#endif  // STATIC_OBJECTS_PORTAL_H_
