#include "map.h"

Map::Map(int map_number) {
  QFile input_file(":/maps/map" + QString::number(map_number + 1) + ".json");
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
  tank_start_direction_ =
      player_tank["initial_direction"].toString().toStdString();

  for (int i = 0; i < static_cast<int>(CellType::Last); ++i) {
    images_.emplace_back(":/textures/" + QString::number(i) + ".png");
    scaled_pixmaps_.emplace_back(":/textures/" + QString::number(i) + ".png");
  }
}

void Map::UpdateCoordinates(int upper_left_x, int upper_left_y,
                            int width, int height) {
  cur_upper_left_x_ = upper_left_x;
  cur_upper_left_y_ = upper_left_y;
  cur_cell_width_ = width / map_.size();
  cur_cell_height_ = height / map_[0].size();
  cur_width_ = cur_cell_width_ * map_.size();
  cur_height_ = cur_cell_height_ * map_[0].size();
  RescaleImages();
  FormMapImage();
}

void Map::DrawMap(QPainter* painter) {
  painter->drawPixmap(cur_upper_left_x_, cur_upper_left_y_, map_scaled_pixmap_);
}

CellType Map::GetField(int cell_x, int cell_y) const {
  return map_[cell_x][cell_y];
}

int Map::GetWallsPrecalc(int cell_x, int cell_y) const {
  return walls_precalc_[cell_x][cell_y];
}

int Map::GetNumberOfCellsHorizontally() const {
  return map_[0].size();
}

int Map::GetNumberOfCellsVertically() const {
  return map_.size();
}

int Map::GetUpperLeftX() const {
  return cur_upper_left_x_;
}

int Map::GetUpperLeftY() const {
  return cur_upper_left_y_;
}

int Map::GetWidth() const {
  return cur_width_;
}

int Map::GetHeight() const {
  return cur_height_;
}

int Map::GetCellWidth() const {
  return cur_cell_width_;
}

int Map::GetCellHeight() const {
  return cur_cell_height_;
}

int Map::GetTankInitCellX() const {
  return tank_init_cell_x_;
}

int Map::GetTankInitCellY() const {
  return tank_init_cell_y_;
}

std::string Map::GetTankStartDirection() const {
  return tank_start_direction_;
}

void Map::RescaleImages() {
  if (scaled_pixmaps_[0].width() == cur_cell_width_ + 2 &&
      scaled_pixmaps_[0].height() == cur_cell_height_ + 2) {
    return;
  }

  for (int i = 0; i < static_cast<int>(CellType::Last); ++i) {
    scaled_pixmaps_[i] = QPixmap::fromImage(images_[i].scaled(
        cur_cell_width_ + 2, cur_cell_height_ + 2, Qt::KeepAspectRatio));
  }
}

void Map::FormMapImage() {
  if (map_scaled_pixmap_.width() == cur_width_ &&
      map_scaled_pixmap_.height() == cur_height_) {
    return;
  }

  QImage temp_image = QImage(cur_width_, cur_height_, QImage::Format_ARGB32);
  QPainter p;
  p.begin(&temp_image);

  int width = map_.size();
  int height = map_[0].size();

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      p.drawPixmap(i * cur_cell_width_, j * cur_cell_height_,
                   scaled_pixmaps_[static_cast<int>(map_[i][j])]);
    }
  }
  p.end();

  map_scaled_pixmap_ = QPixmap::fromImage(std::move(temp_image));
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
