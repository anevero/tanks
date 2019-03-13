#include "mainwindow.h"
#include <memory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      map_(1),
      tank1_(&map_, (&map_)->tank1_init_cell_x_, (&map_)->tank1_init_cell_y_,
             750, 500, Direction::Up),
      tank2_(&map_, (&map_)->tank2_init_cell_x_, (&map_)->tank2_init_cell_y_,
             750, 500, Direction::Down),
      moving_objects_({&tank1_, &tank2_}) {
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

void MainWindow::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Q) {
    if (tank1_.GetTimeSinceLastShot() > tank1_.GetRateOfFire() &&
      !tank1_.IsMovingOrRotating()) {
        tank1_.time_since_last_shot_ = 0;
        auto rocket = new Rocket(&map_, &tank1_, 250);
        moving_objects_.append(rocket);
        if (rocket->GetIntDirection() == 1 || rocket->GetIntDirection() == 3) {
          rocket->StartMovement(map_.GetNumberOfCellsHorizontally());
        } else {
          rocket->StartMovement(map_.GetNumberOfCellsVertically());
        }
    }
  }
  if (event->key() == Qt::Key_W) {
    if (tank1_.IsMovingOrRotating()) return;
    tank1_.TurnReverseOff();
    tank1_.StartMovement(1);
  } else if (event->key() == Qt::Key_S) {
    if (tank1_.IsMovingOrRotating()) return;
    tank1_.TurnReverseOn();
    tank1_.StartMovement(1);
  } else if (event->key() == Qt::Key_A) {
    if (tank1_.IsMovingOrRotating()) return;
    tank1_.TurnRotationReverseOn();
    tank1_.StartRotation();
  } else if (event->key() == Qt::Key_D) {
    if (tank1_.IsMovingOrRotating()) return;
    tank1_.TurnRotationReverseOff();
    tank1_.StartRotation();
  }

  if (event->key() == Qt::Key_U) {
    if (tank2_.GetTimeSinceLastShot() > tank2_.GetRateOfFire() &&
      !tank2_.IsMovingOrRotating()) {
        tank2_.time_since_last_shot_ = 0;
        auto rocket = new Rocket(&map_, &tank2_, 250);
        moving_objects_.append(rocket);
        if (rocket->GetIntDirection() == 1 || rocket->GetIntDirection() == 3) {
          rocket->StartMovement(map_.GetNumberOfCellsHorizontally());
        } else {
          rocket->StartMovement(map_.GetNumberOfCellsVertically());
        }
    }
  }
  if (event->key() == Qt::Key_I) {
    if (tank2_.IsMovingOrRotating()) return;
    tank2_.TurnReverseOff();
    tank2_.StartMovement(1);
  } else if (event->key() == Qt::Key_K) {
    if (tank2_.IsMovingOrRotating()) return;
    tank2_.TurnReverseOn();
    tank2_.StartMovement(1);
  } else if (event->key() == Qt::Key_J) {
    if (tank2_.IsMovingOrRotating()) return;
    tank2_.TurnRotationReverseOn();
    tank2_.StartRotation();
  } else if (event->key() == Qt::Key_L) {
    if (tank2_.IsMovingOrRotating()) return;
    tank2_.TurnRotationReverseOff();
    tank2_.StartRotation();
  }

  // в случае обработки других объектов необходимо делать их append
  // в moving_objects_, чтобы они двигались
  // танк игрока изначально в списке движущихся, поэтому для него это
  // делать не нужно

  rotation_info_label_->setText(
      rotation_info_[static_cast<int>(tank1_.GetDirection())]);
  rotation_info_label_->setText(
      rotation_info_[static_cast<int>(tank2_.GetDirection())]);
  // вообще, вместо SwitchDirection нужно написать отдельный метод Rotate
  // (наподобие связки StartMovement+Move+UpdateCoordinates) и нормально
  // реализовать отрисовку и другие вещи, связанные с этим
  // этим займемся позже
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {

}

void MainWindow::paintEvent(QPaintEvent *) {
  // координаты правой верхней точки карты, ее длина и высота
  map_.UpdateCoordinates(w_indent_ + static_cast<int>(0.28 * sq_width_),
                         h_indent_ + static_cast<int>(0.05 * sq_height_),
                         static_cast<int>(0.68 * sq_width_),
                         static_cast<int>(0.9 * sq_height_));
  for (const auto &object : moving_objects_) {
    object->UpdateCoordinates();
  }

  QPainter p;
  p.begin(this);
  map_.DrawMap(p);
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
  // цикл проходится по списку moving_objects_
  // - объекты, которые уже прошли в нужную клетку, удаляются
  // - для объектов, которые пришли в нужную клетку, но должны идти дальше,
  // вызывается startmovement
  // - для всех объектов в moving_objects_ вызывается Move

  for (auto it = moving_objects_.begin(); it != moving_objects_.end(); ++it) {
    if ((*it)->GetTimeToFinishMovement() == 0 &&
        (*it)->GetCellsToFinishMovement() != 0) {
      (*it)->StartMovement((*it)->GetCellsToFinishMovement());
    }
    if ((*it)->GetTimeToFinishMovement() == 0 &&
        dynamic_cast<Tank *>(*it) == nullptr) {
      delete *it;
      moving_objects_.erase(it);
      it--;
      continue;
    } else if ((*it)->GetTimeToFinishMovement() != 0) {
      (*it)->Move(timer_duration_);
    }

    if (dynamic_cast<Tank *>(*it) != nullptr) {
      (dynamic_cast<Tank *>(*it))->SetTimeSinceLastShot(GetTimerDuration());
    }
  }

  for (auto it = moving_objects_.begin(); it != moving_objects_.end(); ++it) {
    if ((*it)->GetTimeToFinishRotation() != 0) {
      (*it)->Rotate(timer_duration_);
    }
  }

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
  // карта перерисуется автоматически, так как resizeEvent автоматически
  // вызывает paintEvent
}

// функция вызывается при смене карты
void MainWindow::RedrawContent() {
  map_ = Map(swith_map_menu_->currentIndex() + 1);
  tank1_ = Tank(&map_, map_.tank1_init_cell_x_, map_.tank1_init_cell_y_,
                750, 500, Direction::Up);
  tank2_ = Tank(&map_, map_.tank2_init_cell_x_, map_.tank2_init_cell_y_,
                750, 500, Direction::Up);
  moving_objects_.clear();
  moving_objects_.append(&tank1_);
  moving_objects_.append(&tank2_);
  rotation_info_label_->setText("Up");
  repaint();
}

int MainWindow::GetTimerDuration() const { return timer_duration_; }
