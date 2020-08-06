#include "map.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <utility>

bool Coordinates::operator==(Coordinates coordinates) {
  return x == coordinates.x && y == coordinates.y;
}

bool Coordinates::operator!=(Coordinates coordinates) {
  return !(*this == coordinates);
}

Map::Map(int map_number) : current_width_(0), current_height_(0),
                           current_cell_width_(0), current_cell_height_(0) {
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

  QJsonArray array2D = map["map"].toArray();
  QJsonArray array;
  for (int i = 0; i < map_height_in_cells; ++i) {
    array = array2D[i].toArray();
    for (int j = 0; j < map_width_in_cells; ++j) {
      map_[i].push_back(static_cast<CellType>(array[j].toInt()));
    }
  }

  walls_precalculation_.resize(map_height_in_cells);
  WallsPrecalculation();

  QJsonObject player_tank = json["player_tank"].toObject();
  tank_initial_cell_ =
      {player_tank["initial_cell_x"].toInt(),
       player_tank["initial_cell_y"].toInt()};
  tank_start_direction_ =
      player_tank["initial_direction"].toString().toStdString();

  for (int i = 0; i < static_cast<int>(CellType::Last); ++i) {
    images_.emplace_back(":/textures/" + QString::number(i) + ".png");
    scaled_pixmaps_.emplace_back(":/textures/" + QString::number(i) + ".png");
  }
}

void Map::UpdateCoordinates(Coordinates upper_left_cell_coordinates,
                            int width,
                            int height) {
  current_upper_left_cell_coordinates_ = upper_left_cell_coordinates;
  current_cell_width_ = width / map_.size();
  current_cell_height_ = height / map_[0].size();
  current_width_ = current_cell_width_ * map_.size();
  current_height_ = current_cell_height_ * map_[0].size();
  RescaleImages();
  FormMapImage();
}

void Map::DrawMap(QPainter* painter) {
  painter->drawPixmap(current_upper_left_cell_coordinates_.x,
                      current_upper_left_cell_coordinates_.y,
                      map_scaled_pixmap_);
}

CellType Map::GetField(Coordinates cell) const {
  return map_[cell.x][cell.y];
}

int Map::GetWallsPrecalculation(int cell_x, int cell_y) const {
  return walls_precalculation_[cell_x][cell_y];
}

int Map::GetNumberOfCellsHorizontally() const {
  return map_[0].size();
}

int Map::GetNumberOfCellsVertically() const {
  return map_.size();
}

Coordinates Map::GetUpperLeftCellCoordinates() const {
  return current_upper_left_cell_coordinates_;
}

int Map::GetWidth() const {
  return current_width_;
}

int Map::GetHeight() const {
  return current_height_;
}

int Map::GetCellWidth() const {
  return current_cell_width_;
}

int Map::GetCellHeight() const {
  return current_cell_height_;
}

Coordinates Map::GetTankInitialCoordinates() const {
  return tank_initial_cell_;
}

std::string Map::GetTankStartDirection() const {
  return tank_start_direction_;
}

void Map::RescaleImages() {
  if (scaled_pixmaps_[0].width() == current_cell_width_ + 2 &&
      scaled_pixmaps_[0].height() == current_cell_height_ + 2) {
    return;
  }

  for (int i = 0; i < static_cast<int>(CellType::Last); ++i) {
    scaled_pixmaps_[i] = QPixmap::fromImage(images_[i].scaled(
        current_cell_width_ + 2,
        current_cell_height_ + 2,
        Qt::KeepAspectRatio));
  }
}

void Map::FormMapImage() {
  if (map_scaled_pixmap_.width() == current_width_ &&
      map_scaled_pixmap_.height() == current_height_) {
    return;
  }

  QImage temp_image =
      QImage(current_width_, current_height_, QImage::Format_ARGB32);
  QPainter p;
  p.begin(&temp_image);

  int width = map_.size();
  int height = map_[0].size();

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      p.drawPixmap(i * current_cell_width_, j * current_cell_height_,
                   scaled_pixmaps_[static_cast<int>(map_[i][j])]);
    }
  }
  p.end();

  map_scaled_pixmap_ = QPixmap::fromImage(std::move(temp_image));
}

void Map::WallsPrecalculation() {
  int height = GetNumberOfCellsVertically();
  int width = GetNumberOfCellsHorizontally();
  for (int i = 0; i < height - 1; ++i) {
    for (int j = 0; j < width - 1; ++j) {
      walls_precalculation_[j].push_back(0);
      if (i > 0 && j > 0) {
        walls_precalculation_[j][i] = walls_precalculation_[j - 1][i] +
            walls_precalculation_[j][i - 1] -
            walls_precalculation_[j - 1][i - 1];
        if (map_[j][i] == CellType::Wall) {
          walls_precalculation_[j][i]++;
        }
      }
    }
  }
}
