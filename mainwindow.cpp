#include "mainwindow.h"
#include <memory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      map_(new Map(1)),
      static_objects_(
          {std::shared_ptr<Movable>(new Tank(map_, 750, 500, Direction::Up))}) {
  new_game_button_ = new QPushButton("New game", this);
  swith_map_menu_ = new QComboBox(this);

  rotation_info_label_ = new QLabel(this);

  int map_number = 1;
  QFileInfo map_file(":/maps/map" + QString::number(map_number) + ".txt");
  while (map_file.exists() && map_file.isFile()) {
    swith_map_menu_->addItem("Map " + QString::number(map_number));
    map_number++;
    map_file = QFileInfo(":/maps/map" + QString::number(map_number) + ".txt");
  }

  setMinimumSize(600, 450);
  resize(600, 450);
  startTimer(timer_duration_);
  connect(new_game_button_, SIGNAL(clicked()), this, SLOT(RedrawContent()));
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
  auto player_tank_ = std::dynamic_pointer_cast<Tank>(static_objects_[0]);
  if (player_tank_->IsMovingOrRotating()) return;
  // предудущая строчка допустима, ибо мы пока обрабатываем только
  // один пользовательский танк
  // если какие-то клавиши должны будут работать не для него,
  // строчку надо будет продублировать в каждой из секций W, A, S, D, Q

  switch (event->key()) {
    case Qt::Key_W:
      player_tank_->TurnReverseOff();
      moving_objects_.append(player_tank_);
      player_tank_->StartMovement(1);
      break;
    case Qt::Key_S:
      player_tank_->TurnReverseOn();
      moving_objects_.append(player_tank_);
      player_tank_->StartMovement(1);
      break;
    case Qt::Key_A:
      player_tank_->TurnRotationReverseOn();
      moving_objects_.append(player_tank_);
      player_tank_->StartRotation();
      break;
    case Qt::Key_D:
      player_tank_->TurnRotationReverseOff();
      moving_objects_.append(player_tank_);
      player_tank_->StartRotation();
      break;
    case Qt::Key_Q:
      if (player_tank_->GetTimeSinceLastShot() >
          player_tank_->GetRateOfFire()) {
        player_tank_->time_since_last_shot_ = 0;
        std::shared_ptr<Rocket> rocket(
            new Rocket(map_, static_objects_[0], 250));
        moving_objects_.append(rocket);
        if (rocket->GetIntDirection() == 1 || rocket->GetIntDirection() == 3) {
          rocket->StartMovement(map_->GetNumberOfCellsHorizontally());
        } else {
          rocket->StartMovement(map_->GetNumberOfCellsVertically());
        }
      }
      break;
  }

  rotation_info_label_->setText(
      rotation_info_[static_cast<int>(player_tank_->GetDirection())]);
}

void MainWindow::paintEvent(QPaintEvent *) {
  // координаты правой верхней точки карты, ее длина и высота
  map_->UpdateCoordinates(w_indent_ + static_cast<int>(0.28 * sq_width_),
                          h_indent_ + static_cast<int>(0.05 * sq_height_),
                          static_cast<int>(0.68 * sq_width_),
                          static_cast<int>(0.9 * sq_height_));
  for (const auto &object : static_objects_) {
    object->UpdateCoordinates();
  }
  for (const auto &object : moving_objects_) {
    object->UpdateCoordinates();
  }

  QPainter p;
  p.begin(this);
  map_->DrawMap(p);
  for (const auto &object : static_objects_) {
    object->Draw(p);
  }
  for (const auto &object : moving_objects_) {
    object->Draw(p);
  }
  p.end();
}

void MainWindow::resizeEvent(QResizeEvent *) {
  UpdateIndents();
  RedrawButtons();
}

void MainWindow::timerEvent(QTimerEvent *) {
  for (auto it = moving_objects_.begin(); it != moving_objects_.end(); ++it) {
    if ((*it)->GetTimeToFinishMovement() == 0 &&
        (*it)->GetCellsToFinishMovement() != 0) {
      (*it)->StartMovement((*it)->GetCellsToFinishMovement());
    }
    if ((*it)->GetTimeToFinishMovement() != 0) {
      (*it)->Move(timer_duration_);
    } else if ((*it)->GetTimeToFinishRotation() != 0) {
      (*it)->Rotate(timer_duration_);
    }

    if (!(*it)->IsMovingOrRotating()) {
      it++;
      moving_objects_.erase(std::prev(it));
      it--;
    }
  }
  std::dynamic_pointer_cast<Tank>(static_objects_[0])
      ->IncreaseTimeSinceLastShot(GetTimerDuration());

  repaint();
}

void MainWindow::UpdateIndents() {
  sq_width_ = 4 * std::min(width() / 4, height() / 3);
  sq_height_ = 3 * std::min(width() / 4, height() / 3);

  w_indent_ = (width() - sq_width_) / 2;
  h_indent_ = (height() - sq_height_) / 2;
}

void MainWindow::RedrawButtons() {
  new_game_button_->setGeometry(w_indent_ + static_cast<int>(0.04 * sq_width_),
                                h_indent_ + static_cast<int>(0.05 * sq_height_),
                                static_cast<int>(0.2 * sq_width_),
                                static_cast<int>(0.05 * sq_height_));
  swith_map_menu_->setGeometry(w_indent_ + static_cast<int>(0.04 * sq_width_),
                               h_indent_ + static_cast<int>(0.15 * sq_height_),
                               static_cast<int>(0.2 * sq_width_),
                               static_cast<int>(0.05 * sq_height_));
  rotation_info_label_->setGeometry(
      w_indent_ + static_cast<int>(0.04 * sq_width_),
      h_indent_ + static_cast<int>(0.25 * sq_height_),
      static_cast<int>(0.2 * sq_width_), static_cast<int>(0.05 * sq_height_));
}

void MainWindow::RedrawContent() {
  map_.reset(new Map(swith_map_menu_->currentIndex() + 1));
  static_objects_.clear();
  moving_objects_.clear();
  static_objects_.append(
      std::shared_ptr<Tank>(new Tank(map_, 750, 500, Direction::Up)));
  rotation_info_label_->setText("No data");
  repaint();
}

int MainWindow::GetTimerDuration() const { return timer_duration_; }
