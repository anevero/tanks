#ifndef GAME_CORE_MAP_H_
#define GAME_CORE_MAP_H_

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
#include <utility>
#include <string>
#include <vector>

enum class CellType {
  Wall = 0,
  Grass = 1,
  Sand = 2,
  Water = 3,
  Forest = 4,
  Last = 5
};

class Map {
 public:
  // The constructor creates necessary object by reading corresponding to
  // map_number file.
  explicit Map(int map_number);
  void UpdateCoordinates(int upper_left_x, int upper_left_y,
                         int width, int height);
  void DrawMap(QPainter* painter);

  CellType GetField(int cell_x, int cell_y) const;
  int GetWallsPrecalc(int x, int y) const;
  int GetNumberOfCellsHorizontally() const;
  int GetNumberOfCellsVertically() const;
  int GetUpperLeftX() const;
  int GetUpperLeftY() const;
  int GetWidth() const;
  int GetHeight() const;
  int GetCellWidth() const;
  int GetCellHeight() const;
  int GetTankInitCellX() const;
  int GetTankInitCellY() const;
  std::string GetTankStartDirection() const;

 private:
  void RescaleImages();
  void FormMapImage();
  void WallsPrecalc();

  std::vector<std::vector<CellType>> map_;
  std::vector<std::vector<int>> walls_precalc_;
  std::vector<QImage> images_;
  std::vector<QPixmap> scaled_pixmaps_;
  QPixmap map_scaled_pixmap_;

  int cur_upper_left_x_{};
  int cur_upper_left_y_{};
  int cur_width_{};
  int cur_height_{};
  int cur_cell_width_{};
  int cur_cell_height_{};
  int tank_init_cell_x_;
  int tank_init_cell_y_;
  std::string tank_start_direction_;
};

#endif  // GAME_CORE_MAP_H_
