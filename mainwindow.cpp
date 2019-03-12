#include "mainwindow.h"
#include <memory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), map_(1), tank_(&map_), moving_objects_({&tank_}) {
  new_game_button_ = new QPushButton("New game", this);
  swith_map_menu_ = new QComboBox(this);

  rotation_info_label_ = new QLabel(this);
  rotation_info_label_->setText("Up");

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
  switch (event->key()) {
    case Qt::Key_W:
      if (tank_.GetTimeToFinishMovement() != 0) return;
      tank_.TurnReverseOff();
      tank_.StartMovement(1);
      break;
    case Qt::Key_S:
      if (tank_.GetTimeToFinishMovement() != 0) return;
      tank_.TurnReverseOn();
      tank_.StartMovement(1);
      break;
    case Qt::Key_A:
      if (tank_.GetTimeToFinishMovement() != 0) return;
      tank_.Rotate(RotateDirection::Left);
      break;
    case Qt::Key_D:
      if (tank_.GetTimeToFinishMovement() != 0) return;
      tank_.Rotate(RotateDirection::Right);
      break;
    case Qt::Key_P:
      auto rocket = new Rocket(&map_, &tank_, 250);
      moving_objects_.append(rocket);
      if (rocket->GetIntDirection() == 1 || rocket->GetIntDirection() == 3) {
        rocket->StartMovement(map_.GetNumberOfCellsHorizontally());
      } else {
        rocket->StartMovement(map_.GetNumberOfCellsVertically());
      }
      break;
  }

  // в случае обработки других объектов необходимо делать их append
  // в moving_objects_, чтобы они двигались
  // танк игрока изначально в списке движущихся, поэтому для него это
  // делать не нужно

  rotation_info_label_->setText(
      rotation_info_[static_cast<int>(tank_.GetDirection())]);
  // вообще, вместо SwitchDirection нужно написать отдельный метод Rotate
  // (наподобие связки StartMovement+Move+UpdateCoordinates) и нормально
  // реализовать отрисовку и другие вещи, связанные с этим
  // этим займемся позже
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
  tank_ = Tank(&map_);
  moving_objects_.clear();
  moving_objects_.append(&tank_);
  rotation_info_label_->setText("Up");
  repaint();
}
