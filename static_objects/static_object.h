#ifndef STATIC_OBJECTS_STATIC_OBJECT_H_
#define STATIC_OBJECTS_STATIC_OBJECT_H_

#include <memory>
#include <utility>

#include "../game_core/map.h"
#include "game_core/game_object.h"

class StaticObject : public GameObject {
 public:
  StaticObject(std::shared_ptr<const Map> map, const QString& path,
               Coordinates coordinates);
  ~StaticObject() override = default;

  void Draw(QPainter* painter) override;

  void UpdateCoordinates();
};

class Obstacle : public StaticObject {
 public:
  Obstacle(std::shared_ptr<const Map> map, Coordinates coordinates);
};

class MedicalKit : public StaticObject {
 public:
  MedicalKit(std::shared_ptr<const Map> map, Coordinates coordinates);
};

class Charge : public StaticObject {
 public:
  Charge(std::shared_ptr<const Map> map, Coordinates coordinates);
};

#endif  // STATIC_OBJECTS_STATIC_OBJECT_H_
