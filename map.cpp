#include "map.h"

Map::Map(int map_number) {
  QFile input_file(":/data/map" + QString::number(map_number) + ".json");
  input_file.open(QIODevice::ReadOnly);
  QString text = input_file.readAll();
  input_file.close();
  QJsonDocument json_document(QJsonDocument::fromJson(text.toUtf8()));
  QJsonObject json = json_document.object();

  QJsonObject map = json["map"].toObject();
  int map_width_in_cells = map["width"].toInt();
  int map_height_in_cells = map["height"].toInt();
  map_.resize(map_height_in_cells);
  walls_precalc_.resize(map_height_in_cells);

  QJsonArray array2D = map["map"].toArray();
  QJsonArray array;
  for (int i = 0; i < map_height_in_cells; ++i) {
    array = array2D[i].toArray();
    for (int j = 0; j < map_width_in_cells; ++j) {
      map_[i].push_back(static_cast<CellType>(array[j].toInt()));
    }
  }
  WallsPrecalc();

  QJsonObject player_tank = json["player_tank"].toObject();
  tank_init_cell_x_ = player_tank["initial_cell_x"].toInt();
  tank_init_cell_y_ = player_tank["initial_cell_y"].toInt();
  tank_start_direction_ = player_tank["initial_direction"].toString();

  QImage image;
  for (int i = 0; i < number_of_cell_types_; ++i) {
    image.load(":/textures/" + QString::number(i) + ".png");
    images_.push_back(image);
  }
  scaled_images_ = images_;
}

void Map::UpdateCoordinates(int upper_left_x, int upper_left_y, int width,
                            int height) {
  cur_upper_left_x_ = upper_left_x;
  cur_upper_left_y_ = upper_left_y;
  cur_width_ = width;
  cur_height_ = height;
  cur_cell_width_ = cur_width_ / map_.size();
  cur_cell_height_ = cur_height_ / map_[0].size();
  RescaleImages();
}

void Map::DrawMap(QPainter& painter) {
  for (int i = 0; i < map_.size(); ++i) {
    for (int j = 0; j < map_[i].size(); ++j) {
      painter.drawImage(cur_upper_left_x_ + i * cur_cell_width_ - 1,
                        cur_upper_left_y_ + j * cur_cell_height_ - 1,
                        scaled_images_[static_cast<int>(map_[i][j])]);
    }
  }
}

CellType Map::GetField(int cell_x, int cell_y) const {
  return map_[cell_x][cell_y];
}
int Map::GetWallsPrecalc(int cell_x, int cell_y) const {
  return walls_precalc_[cell_x][cell_y];
}
int Map::GetNumberOfCellsHorizontally() const { return map_[0].size(); }
int Map::GetNumberOfCellsVertically() const { return map_.size(); }
int Map::GetUpperLeftX() const { return cur_upper_left_x_; }
int Map::GetUpperLeftY() const { return cur_upper_left_y_; }
int Map::GetWidth() const { return cur_width_; }
int Map::GetHeight() const { return cur_height_; }
int Map::GetTankInitCellX() const { return tank_init_cell_x_; }
int Map::GetTankInitCellY() const { return tank_init_cell_y_; }
QString Map::GetTankStartDirection() const { return tank_start_direction_; }

void Map::RescaleImages() {
  if (scaled_images_[0].width() == cur_cell_width_ + 2 &&
      scaled_images_[0].height() == cur_cell_height_ + 2) {
    return;
  }
  for (int i = 0; i < number_of_cell_types_; ++i) {
    scaled_images_[i] = images_[i].scaled(
        cur_cell_width_ + 2, cur_cell_height_ + 2, Qt::KeepAspectRatio);
  }
}

void Map::WallsPrecalc() {
  int height = GetNumberOfCellsVertically();
  int width = GetNumberOfCellsHorizontally();
  for (int i = 0; i < height - 1; ++i) {
    for (int j = 0; j < width - 1; ++j) {
      walls_precalc_[j].push_back(0);
      if (i > 0 && j > 0) {
        walls_precalc_[j][i] = walls_precalc_[j - 1][i] +
                               walls_precalc_[j][i - 1] -
                               walls_precalc_[j - 1][i - 1];
        if (map_[j][i] == CellType::Wall) {
          walls_precalc_[j][i]++;
        }
      }
    }
  }
}
