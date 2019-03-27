#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      map_(new Map(1)),
      static_objects_({std::shared_ptr<Movable>(
          new Tank(map_, map_->GetTankInitCellX(), map_->GetTankInitCellY(),
                   750, 500, Direction::Up))}) {
  new_game_button_ = new QPushButton("New game", this);
  swith_map_menu_ = new QComboBox(this);

  rotation_info_label_ = new QLabel(this);
  game_over_label_ = new QLabel(this);

  int map_number = 1;
  QFileInfo map_file(":/maps/map" + QString::number(map_number) + ".txt");
  while (map_file.exists() && map_file.isFile()) {
    swith_map_menu_->addItem("Map " + QString::number(map_number));
    map_number++;
    map_file = QFileInfo(":/maps/map" + QString::number(map_number) + ".txt");
  }

  setMinimumSize(600, 450);
  resize(600, 450);
  timer_id_ = startTimer(timer_duration_);
  connect(new_game_button_, SIGNAL(clicked()), this, SLOT(RedrawContent()));

  for (const auto &cell : map_->coordinates_) {
    static_objects_.append(std::shared_ptr<Movable>
                           (new Bot(map_, cell.first, cell.second,
                                    1000, 100, Direction::Up)));
    static_objects_[static_objects_.size() - 1]->StartRotation();
  }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
  auto tank = std::dynamic_pointer_cast<Tank>(static_objects_[0]);
  if (tank->IsMovingOrRotating()) return;
  moving_objects_.append(tank);
  // предыдущие строчки допустимы, ибо мы пока обрабатываем только
  // один пользовательский танк
  // если какие-то клавиши должны будут работать не для него,
  // строчки надо будет продублировать в каждой из секций W, A, S, D, Q

  switch (event->key()) {
    case Qt::Key_W:
      tank->TurnReverseOff();
      tank->StartMovement(1);
      break;
    case Qt::Key_S:
      tank->TurnReverseOn();
      tank->StartMovement(1);
      break;
    case Qt::Key_A:
      tank->TurnRotationReverseOn();
      tank->StartRotation();
      break;
    case Qt::Key_D:
      tank->TurnRotationReverseOff();
      tank->StartRotation();
      break;
    case Qt::Key_Q:
      if (tank->IsAbleToShoot()) {
        tank->SetZeroTimeFromLastShot();
        ShootRocket(tank);
      }
      break;
  }

  rotation_info_label_->setText(
      rotation_info_[static_cast<int>(tank->GetDirection())]);
}

void MainWindow::paintEvent(QPaintEvent *) {
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
  for (auto &object : static_objects_) {
    if (std::dynamic_pointer_cast<Bot>(object) != nullptr) {
      std::shared_ptr<Bot> bot = std::dynamic_pointer_cast<Bot>(object);
      std::shared_ptr<Tank> tank = std::dynamic_pointer_cast<Tank>(object);
      if (bot->DoesNeedToShoot(map_, std::dynamic_pointer_cast<Tank>(static_objects_[0]))) {
        std::shared_ptr<Tank> tank = std::dynamic_pointer_cast<Tank>(object);
        ShootRocket(tank);
      }

      if (bot->DoesNeedToStartRotation()) {
        bot->StartRotation();
      }

      if (bot->DoesNeedToTurn()) {
          bot->Rotate(timer_duration_);
      }

      tank->IncreaseTimeSinceLastShot(GetTimerDuration());
    }
  }

  for (const auto &object : moving_objects_) {
    if (object->GetTimeToFinishMovement() != 0) {
      object->Move(timer_duration_);
    } else if (object->GetTimeToFinishRotation() != 0) {
      object->Rotate(timer_duration_);
    }
  }

  auto it = moving_objects_.begin();
  while (it != moving_objects_.end()) {
    if ((*it)->GetTimeToFinishMovement() == 0 &&
        (*it)->GetCellsToFinishMovement() != 0) {
      (*it)->StartMovement(((*it)->GetCellsToFinishMovement()) - 1);
    }
    if (!(*it)->IsMovingOrRotating()) {
      it = moving_objects_.erase(it);
      continue;
    }
    it++;
  }

  std::dynamic_pointer_cast<Tank>(static_objects_[0])
      ->IncreaseTimeSinceLastShot(GetTimerDuration());

  FindInteractingObjects();

  for (int i = 0; i < number_of_player_tanks_; ++i) {
    if (std::dynamic_pointer_cast<Tank>(static_objects_[i])->IsDead()) {
      GameOver();
      return;
    }
  }

  // TEST
  // -----------

  if (static_objects_.length() != 2) {
    repaint();
    return;
  }

  auto test_tank = std::dynamic_pointer_cast<Tank>(static_objects_[1]);
  test_tank->IncreaseTimeSinceLastShot(GetTimerDuration());
  if (test_tank->IsAbleToShoot()) {
    test_tank->SetZeroTimeFromLastShot();
    ShootRocket(test_tank);
  }

  // -----------
  // TEST

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
  game_over_label_->setGeometry(w_indent_ + static_cast<int>(0.08 * sq_width_),
                                h_indent_ + static_cast<int>(0.7 * sq_height_),
                                static_cast<int>(sq_width_),
                                static_cast<int>(0.4 * sq_height_));
}

void MainWindow::RedrawContent() {
  map_.reset(new Map(swith_map_menu_->currentIndex() + 1));
  static_objects_.clear();
  moving_objects_.clear();
  static_objects_.append(std::shared_ptr<Tank>(
      new Tank(map_, map_->GetTankInitCellX(), map_->GetTankInitCellY(), 750,
               500, Direction::Up)));
  for (const auto &cell : map_->coordinates_) {
    static_objects_.append(std::shared_ptr<Movable>
                           (new Bot(map_, cell.first, cell.second,
                                    1000, 100, Direction::Up)));
    static_objects_[static_objects_.size() - 1]->StartRotation();
  }
  rotation_info_label_->setText("No data");
  if (timer_id_ == 0) {
    timer_id_ = startTimer(timer_duration_);
  }
  game_over_label_->setText("");

  // TEST
  // -----------

  static_objects_.append(std::shared_ptr<Movable>(
      new Tank(map_, 1, 1, 750, 2000, Direction::Right)));

  // -----------
  // TEST

  repaint();
}

void MainWindow::FindInteractingObjects() {
  auto obj1 = moving_objects_.begin();
  while (obj1 != moving_objects_.end()) {
    auto obj2 = std::next(obj1);
    while (obj2 != moving_objects_.end()) {
      if (HaveObjectsCollided(*obj1, *obj2)) {
        qDebug() << "collision" << qrand();
        ;
      }
      obj2++;
    }

    obj2 = static_objects_.begin();
    while (obj2 != static_objects_.end()) {
      if (HaveObjectsCollided(*obj1, *obj2)) {
        qDebug() << "collision" << qrand();
        ;
      }
      obj2++;
    }
    obj1++;
  }
}

bool MainWindow::HaveObjectsCollided(std::shared_ptr<Movable> &obj1,
                                     std::shared_ptr<Movable> &obj2) const {
  if (obj1 == obj2 || IsRocketByThisTank(obj1, obj2) ||
      IsRocketByThisTank(obj2, obj1)) {
    return false;
  }

  return !(
      (obj1->GetUpperLeftX() >= obj2->GetUpperLeftX() + obj2->GetWidth()) ||
      (obj1->GetUpperLeftX() + obj1->GetWidth() <= obj2->GetUpperLeftX()) ||
      (obj1->GetUpperLeftY() >= obj2->GetUpperLeftY() + obj2->GetHeight()) ||
      (obj1->GetUpperLeftY() + obj1->GetHeight() <= obj2->GetUpperLeftY()));
}

void MainWindow::ShootRocket(std::shared_ptr<Tank> &tank) {
  std::shared_ptr<Rocket> rocket(new Rocket(map_, tank, 250, 1));
  moving_objects_.append(rocket);
  if (rocket->GetIntDirection() == 1 || rocket->GetIntDirection() == 3) {
    rocket->StartMovement(map_->GetNumberOfCellsHorizontally());
  } else {
    rocket->StartMovement(map_->GetNumberOfCellsVertically());
  }
}

bool MainWindow::IsRocketByThisTank(std::shared_ptr<Movable> &rocket,
                                    std::shared_ptr<Movable> &tank) const {
  auto casted_rocket = std::dynamic_pointer_cast<Rocket>(rocket);
  auto casted_tank = std::dynamic_pointer_cast<Tank>(tank);
  if (casted_rocket != nullptr && casted_tank != nullptr) {
    return casted_tank == casted_rocket->GetAttachedTank();
  }
  return false;
}

int MainWindow::GetTimerDuration() const { return timer_duration_; }

void MainWindow::GameOver() {
  killTimer(timer_id_);
  timer_id_ = 0;
  game_over_label_->setText("Game Over.");
}
