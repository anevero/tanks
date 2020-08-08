#ifndef STATIC_OBJECTS_STATIC_OBJECT_H_
#define STATIC_OBJECTS_STATIC_OBJECT_H_

#include <memory>
#include <utility>

#include "../game_core/map.h"
#include "game_core/game_object.h"

class StaticObject : public GameObject {
 public:
  StaticObject(std::shared_ptr<const Map> map, const QString& texture_path,
               Coordinates coordinates);
  ~StaticObject() override = default;

  void Draw(QPainter* painter) override;

  void UpdateCoordinates();
};

class Obstacle : public StaticObject {
 public:
  Obstacle(std::shared_ptr<const Map> map, Coordinates coordinates);
  ~Obstacle() override = default;
};

class MedicalKit : public StaticObject {
 public:
  MedicalKit(std::shared_ptr<const Map> map, Coordinates coordinates);
  ~MedicalKit() override = default;
};

class Charge : public StaticObject {
 public:
  Charge(std::shared_ptr<const Map> map, Coordinates coordinates);
  ~Charge() override = default;
};

#endif  // STATIC_OBJECTS_STATIC_OBJECT_H_
