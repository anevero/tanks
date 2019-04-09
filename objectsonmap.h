#ifndef OBSTACLANDBONUS_H
#define OBSTACLANDBONUS_H

class ObjectsOnMap {
 public:
  ObjectsOnMap(int x, int y, bool is_there_smth);
  virtual void LoadImage();
  virtual ~ObjectsOnMap() = default;
  virtual void Draw();
  bool IsThereSmth();

 protected:
  int x_map_object_, y_map_object_;
  bool is_there_smth_;
};

class Obstacle : public ObjectsOnMap {
 public:
  Obstacle(int x, int y);
  void Draw() override;
};

class Bonus : public ObjectsOnMap {
 public:
  Bonus(int x, int y);
  void Draw() override;
};

#endif  // OBSTACLANDBONUS_H
