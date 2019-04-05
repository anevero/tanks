#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), map_(new Map(1)) {
  new_game_button_ = new QPushButton("New game", this);
  switch_map_menu_ = new QComboBox(this);
  switch_tank_menu_ = new QComboBox(this);
  switch_difficulty_menu_ = new QComboBox(this);
  switch_map_label_ = new QLabel(this);
  switch_tank_label_ = new QLabel(this);
  switch_difficulty_label_ = new QLabel(this);

  int map_number = 1;
  QFileInfo map_file(":/maps/map" + QString::number(map_number) + ".txt");
  while (map_file.exists() && map_file.isFile()) {
    switch_map_menu_->addItem("Map " + QString::number(map_number));
    map_number++;
    map_file = QFileInfo(":/maps/map" + QString::number(map_number) + ".txt");
  }

  QFile tanks_input_file(":/tanks_info/tanks.txt");
  tanks_input_file.open(QIODevice::ReadOnly);
  QTextStream in(&tanks_input_file);
  int number_of_tank_types;
  in >> number_of_tank_types;
  for (int i = 0; i < number_of_tank_types; ++i) {
    TankQualities qualities;
    in >> qualities.speed >> qualities.rate_of_fire >> qualities.max_health;
    available_tank_types_.push_back(qualities);
    switch_tank_menu_->addItem("Tank " + QString::number(i + 1));
  }
  tanks_input_file.close();

  switch_difficulty_menu_->addItem("Easy");
  switch_difficulty_menu_->addItem("Normal");
  switch_difficulty_menu_->addItem("Hard");

  switch_map_label_->setText("Map:");
  switch_tank_label_->setText("Tank:");
  switch_difficulty_label_->setText("Difficulty:");

  setMinimumSize(600, 450);
  resize(600, 450);
  connect(new_game_button_, SIGNAL(clicked()), this, SLOT(RedrawContent()));

  RedrawContent();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
  auto tank = std::dynamic_pointer_cast<Tank>(tanks_[0]);
  if (tank->IsMovingOrRotating()) return;
  // предыдущие строчки допустимы, ибо мы пока обрабатываем только
  // один пользовательский танк
  // если какие-то клавиши должны будут работать не для него,
  // строчки надо будет продублировать в каждой из секций W, A, S, D, Q

  switch (event->key()) {
    case Qt::Key_W:
      tank->TurnReverseOff();
      tank->StartMovement(1, tanks_);
      break;
    case Qt::Key_S:
      tank->TurnReverseOn();
      tank->StartMovement(1, tanks_);
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
}

void MainWindow::paintEvent(QPaintEvent *) {
  map_->UpdateCoordinates(w_indent_ + static_cast<int>(0.28 * sq_width_),
                          h_indent_ + static_cast<int>(0.05 * sq_height_),
                          static_cast<int>(0.68 * sq_width_),
                          static_cast<int>(0.9 * sq_height_));
  for (const auto &object : tanks_) {
    object->UpdateCoordinates();
  }
  for (const auto &object : rockets_) {
    object->UpdateCoordinates();
  }

  QPainter p;
  p.begin(this);
  map_->DrawMap(p);
  for (const auto &object : tanks_) {
    object->Draw(p);
  }
  for (const auto &object : rockets_) {
    object->Draw(p);
  }
  p.end();
}

void MainWindow::resizeEvent(QResizeEvent *) {
  UpdateIndents();
  RedrawButtons();
}

void MainWindow::timerEvent(QTimerEvent *) {
  for (auto &object : tanks_) {
    if (std::dynamic_pointer_cast<Bot>(object) != nullptr) {
      std::shared_ptr<Bot> bot = std::dynamic_pointer_cast<Bot>(object);

      if (bot->IsShotNeeded(map_, std::dynamic_pointer_cast<Tank>(tanks_[0])) &&
          bot->IsAbleToShoot()) {
        std::shared_ptr<Tank> tank = std::dynamic_pointer_cast<Tank>(object);
        bot->SetZeroTimeFromLastShot();
        ShootRocket(tank);
      }

      if (bot->IsMovingStartNeeded(tanks_)) {
        bot->StartMovement(1, tanks_);
      } else if (bot->IsRotationStartNeeded(
                     std::dynamic_pointer_cast<Tank>(tanks_[0]))) {
        bot->StartRotation();
      }

      if (bot->IsTurnNeeded()) {
        bot->Rotate(timer_duration_);
      }
    }
  }

  for (const auto &object : tanks_) {
    if (object->GetTimeToFinishMovement() != 0) {
      object->Move(timer_duration_);
    } else if (object->GetTimeToFinishRotation() != 0) {
      object->Rotate(timer_duration_);
    }
  }

  for (const auto &object : rockets_) {
    if (object->GetTimeToFinishMovement() != 0) {
      object->Move(timer_duration_);
    } else if (object->GetTimeToFinishRotation() != 0) {
      object->Rotate(timer_duration_);
    }
  }

  auto it = rockets_.begin();
  while (it != rockets_.end()) {
    if ((*it)->GetTimeToFinishMovement() == 0 &&
        (*it)->GetCellsToFinishMovement() != 0) {
      (*it)->StartMovement(((*it)->GetCellsToFinishMovement()) - 1, tanks_);
    }
    if (!(*it)->IsMovingOrRotating()) {
      it = rockets_.erase(it);
      continue;
    }
    it++;
  }

  for (const auto &object : tanks_) {
    std::dynamic_pointer_cast<Tank>(object)->IncreaseTimeSinceLastShot(
        GetTimerDuration());
  }

  FindInteractingObjects();
  CheckDeadObjects();

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
  switch_map_label_->setGeometry(
      w_indent_ + static_cast<int>(0.04 * sq_width_),
      h_indent_ + static_cast<int>(0.12 * sq_height_),
      static_cast<int>(0.2 * sq_width_), static_cast<int>(0.05 * sq_height_));
  switch_map_menu_->setGeometry(w_indent_ + static_cast<int>(0.04 * sq_width_),
                                h_indent_ + static_cast<int>(0.17 * sq_height_),
                                static_cast<int>(0.2 * sq_width_),
                                static_cast<int>(0.05 * sq_height_));
  switch_tank_label_->setGeometry(
      w_indent_ + static_cast<int>(0.04 * sq_width_),
      h_indent_ + static_cast<int>(0.24 * sq_height_),
      static_cast<int>(0.2 * sq_width_), static_cast<int>(0.05 * sq_height_));
  switch_tank_menu_->setGeometry(
      w_indent_ + static_cast<int>(0.04 * sq_width_),
      h_indent_ + static_cast<int>(0.29 * sq_height_),
      static_cast<int>(0.2 * sq_width_), static_cast<int>(0.05 * sq_height_));
  switch_difficulty_label_->setGeometry(
      w_indent_ + static_cast<int>(0.04 * sq_width_),
      h_indent_ + static_cast<int>(0.36 * sq_height_),
      static_cast<int>(0.2 * sq_width_), static_cast<int>(0.05 * sq_height_));
  switch_difficulty_menu_->setGeometry(
      w_indent_ + static_cast<int>(0.04 * sq_width_),
      h_indent_ + static_cast<int>(0.41 * sq_height_),
      static_cast<int>(0.2 * sq_width_), static_cast<int>(0.05 * sq_height_));
}

void MainWindow::RedrawContent() {
  killTimer(timer_id_);
  timer_id_ = 0;
  map_.reset(new Map(switch_map_menu_->currentIndex() + 1));
  tanks_.clear();
  rockets_.clear();

  tanks_.append(std::shared_ptr<Movable>(
      new Tank(map_, map_->GetTankInitCellX(), map_->GetTankInitCellY(),
               available_tank_types_[switch_tank_menu_->currentIndex()],
               Direction::Up)));

  QFile bots_input_file(
      ":/tanks_info/bots" +
      QString::number(switch_map_menu_->currentIndex() + 1) +
      QString::number(switch_difficulty_menu_->currentIndex() + 1) + ".txt");
  if (!bots_input_file.exists()) {
    QMessageBox message;
    message.setWindowTitle("Error");
    message.setText("This level of difficulty isn't available on this map");
    message.exec();
    return;
  }

  bots_input_file.open(QIODevice::ReadOnly);
  QTextStream in(&bots_input_file);
  int number_of_standart_bots, number_of_improved_bots, number_of_clever_bots,
      number_of_bots;
  in >> number_of_standart_bots >> number_of_improved_bots >>
        number_of_clever_bots;
  number_of_bots = number_of_standart_bots + number_of_improved_bots +
        number_of_clever_bots;
  for (int i = 0; i < number_of_bots ; ++i) {
    BotQualities qualities;
    qualities.tank.max_health =
        70 + 15 * switch_difficulty_menu_->currentIndex();
    qualities.tank.rate_of_fire =
        1000 - 300 * switch_difficulty_menu_->currentIndex();
    qualities.tank.speed = 1000 - 300 * switch_difficulty_menu_->currentIndex();
    in >> qualities.init_cell_x >> qualities.init_cell_y >>
        qualities.moving_length >> qualities.amount_of_turns >>
        qualities.side_rotation_frequency;

    if (i < number_of_standart_bots) {
      tanks_.append(
          std::shared_ptr<Movable>(new Bot(map_, qualities, Direction::Up)));
    } else if (i < number_of_standart_bots + number_of_improved_bots) {
      tanks_.append(std::shared_ptr<Movable>(
          new ImprovedBot(map_, qualities, Direction::Up)));
    } else {
      tanks_.append(std::shared_ptr<Movable>(
          new CleverBot(map_, qualities, Direction::Up)));
    }
  }
  bots_input_file.close();

  timer_id_ = startTimer(timer_duration_);
  repaint();
}

void MainWindow::FindInteractingObjects() {
  auto tank = tanks_.begin();
  while (tank != tanks_.end()) {
    auto rocket = rockets_.begin();
    while (rocket != rockets_.end()) {
      if (HaveObjectsCollided(*rocket, *tank)) {
        std::dynamic_pointer_cast<Tank>(*tank)->MinusHealth(
            std::dynamic_pointer_cast<Rocket>(*rocket)->GetPower());
        rocket = rockets_.erase(rocket);
        continue;
      }
      rocket++;
    }
    tank++;
  }
}

bool MainWindow::HaveObjectsCollided(
    const std::shared_ptr<Movable> &obj1,
    const std::shared_ptr<Movable> &obj2) const {
  if (obj1 == obj2 || IsRocketByThisTank(obj1, obj2)) {
    return false;
  }

  return !(
      (obj1->GetUpperLeftX() >= obj2->GetUpperLeftX() + obj2->GetWidth()) ||
      (obj1->GetUpperLeftX() + obj1->GetWidth() <= obj2->GetUpperLeftX()) ||
      (obj1->GetUpperLeftY() >= obj2->GetUpperLeftY() + obj2->GetHeight()) ||
      (obj1->GetUpperLeftY() + obj1->GetHeight() <= obj2->GetUpperLeftY()));
}

void MainWindow::CheckDeadObjects() {
  auto bot = tanks_.begin();
  for (int i = 0; i < number_of_player_tanks_; ++i) {
    if (std::dynamic_pointer_cast<Tank>(tanks_[i])->IsDead()) {
      GameOver();
      return;
    }
    bot = std::next(bot);
  }
  while (bot != tanks_.end()) {
    if (std::dynamic_pointer_cast<Tank>(*bot)->IsDead()) {
      bot = tanks_.erase(bot);
      continue;
    }
    bot++;
  }
  if (tanks_.size() == number_of_player_tanks_) {
    GameOver();
  }
}

void MainWindow::ShootRocket(std::shared_ptr<Tank> &tank) {
  std::shared_ptr<Rocket> rocket(new Rocket(map_, tank, 250, 10));
  rockets_.append(rocket);
  if (rocket->GetIntDirection() == 1 || rocket->GetIntDirection() == 3) {
    rocket->StartMovement(map_->GetNumberOfCellsHorizontally(), tanks_);
  } else {
    rocket->StartMovement(map_->GetNumberOfCellsVertically(), tanks_);
  }
}

bool MainWindow::IsRocketByThisTank(
    const std::shared_ptr<Movable> &rocket,
    const std::shared_ptr<Movable> &tank) const {
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

  QMessageBox message;
  message.setWindowTitle("Tanks Alpha");
  if (tanks_.size() == number_of_player_tanks_) {
    message.setText("You win!");
  } else {
    message.setText("You died!");
  }
  message.exec();
}
