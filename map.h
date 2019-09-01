#ifndef MAP_H
#define MAP_H

#include <QDebug>
#include <QFile>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QPair>
#include <QString>
#include <QTextStream>
#include <QVector>
#include <vector>

enum class CellType { Wall = 0, Grass = 1, Sand = 2, Water = 3, Forest = 4 };

class Map {
 public:
  // The constructor creates necessary object by reading corresponding to
  // map_number file.
  explicit Map(int map_number);
  void UpdateCoordinates(int upper_left_x, int upper_left_y,
                         int width, int height);
  void DrawMap(QPainter& painter);

  CellType GetField(size_t cell_x, size_t cell_y) const;
  int GetWallsPrecalc(size_t x, size_t y) const;
  size_t GetNumberOfCellsHorizontally() const;
  size_t GetNumberOfCellsVertically() const;
  int GetUpperLeftX() const;
  int GetUpperLeftY() const;
  int GetWidth() const;
  int GetHeight() const;
  int GetCellWidth() const;
  int GetCellHeight() const;
  size_t GetTankInitCellX() const;
  size_t GetTankInitCellY() const;
  QString GetTankStartDirection() const;

 private:
  void RescaleImages();
  void FormMapImage();
  void WallsPrecalc();

  QVector<QVector<CellType>> map_;
  QVector<QVector<int>> walls_precalc_;
  std::vector<QImage> images_;
  std::vector<QImage> scaled_images_;
  QImage map_scaled_image_;

  int cur_upper_left_x_{};
  int cur_upper_left_y_{};
  int cur_width_{};
  int cur_height_{};
  int cur_cell_width_{};
  int cur_cell_height_{};
  size_t tank_init_cell_x_;
  size_t tank_init_cell_y_;
  QString tank_start_direction_;

  // Number of types in CellType enum class, defined earlier in this file.
  const uint32_t number_of_cell_types_ = 5;
};

#endif  // MAP_H
