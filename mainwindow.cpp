﻿#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      new_game_button_(new QPushButton(tr("New game"), this)),
      pause_continue_button_(new QPushButton(tr("Pause"), this)),
      settings_button_(new QPushButton(tr("Settings"), this)),
      virtual_keys_buttons_(
          {new QPushButton("Q", this), new QPushButton("W", this),
           new QPushButton("A", this), new QPushButton("S", this),
           new QPushButton("D", this)}),
      virtual_keys_encodings_(
          {Qt::Key_Q, Qt::Key_W, Qt::Key_A, Qt::Key_S, Qt::Key_D}),
      map_(new Map(1)) {
  new_game_button_->setFocusPolicy(Qt::NoFocus);
  pause_continue_button_->setFocusPolicy(Qt::NoFocus);
  settings_button_->setFocusPolicy(Qt::NoFocus);

  setMinimumSize(600, 450);
  resize(600, 450);

  connect(new_game_button_, SIGNAL(clicked()), this, SLOT(NewGame()));
  connect(pause_continue_button_, SIGNAL(clicked()), this,
          SLOT(PauseOrContinue()));
  connect(settings_button_, SIGNAL(clicked()), this, SLOT(Settings()));

  for (int i = 0; i < virtual_keys_buttons_.size(); ++i) {
    connect(virtual_keys_buttons_[i], &QPushButton::clicked,
            [this, i]() { PressVirtualKey(virtual_keys_encodings_[i]); });
    virtual_keys_buttons_[i]->setFocusPolicy(Qt::NoFocus);
  }

  if (QTouchDevice::devices().empty()) {
    ToggleVirtualKeys();
  }

  InitializeNewGameDialog();
  InitializeSettingsDialog();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
  int current_cell_x = (event->x() - map_->GetUpperLeftX()) *
                       map_->GetNumberOfCellsHorizontally() / map_->GetWidth();
  int current_cell_y = (event->y() - map_->GetUpperLeftY()) *
                       map_->GetNumberOfCellsVertically() / map_->GetHeight();
  for (const auto &object : tanks_) {
    if (object->GetCellX() == current_cell_x &&
        object->GetCellY() == current_cell_y) {
      time_since_tooltip_appearing_ = 0;
      QToolTip::showText(
          event->globalPos(),
          tr("Health") + ": " +
              QString::number(
                  std::dynamic_pointer_cast<Tank>(object)->GetCurrentHealth()));
      break;
    }
  }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
  if (timer_id_ == 0) return;
  auto tank = std::dynamic_pointer_cast<Tank>(tanks_[0]);
  if (tank->IsMovingOrRotating() || (paused_ && event->key() != Qt::Key_Escape))
    return;

  switch (event->key()) {
    case Qt::Key_Escape:
      pause_continue_button_->animateClick();
      break;
    case 1062:
    case Qt::Key_Up:
    case Qt::Key_W:
      tank->TurnReverseOff();
      tank->StartMovement(1, tanks_);
      DestroyObstackle(tank);
      break;
    case 1067:
    case Qt::Key_Down:
    case Qt::Key_S:
      tank->TurnReverseOn();
      tank->StartMovement(1, tanks_);
      break;
    case 1060:
    case Qt::Key_Left:
    case Qt::Key_A:
      tank->TurnRotationReverseOn();
      tank->StartRotation();
      break;
    case 1042:
    case Qt::Key_Right:
    case Qt::Key_D:
      tank->TurnRotationReverseOff();
      tank->StartRotation();
      break;
    case 1049:
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

  for (const auto &vector : obstacles_and_bonuses_) {
    for (const auto &object : vector) {
      if (object != nullptr) {
        object->Draw(p);
      }
    }
  }
  p.end();
}

void MainWindow::resizeEvent(QResizeEvent *) {
  UpdateIndents();
  RedrawButtons();
}

void MainWindow::timerEvent(QTimerEvent *) {
  time_since_tooltip_appearing_ += timer_duration_;
  if (time_since_tooltip_appearing_ >= time_for_showing_tooltip_) {
    QToolTip::hideText();
    time_since_tooltip_appearing_ = 0;
  }

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
        DestroyObstackle(bot);
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
    }
  }

  auto it = rockets_.begin();
  while (it != rockets_.end()) {
    if ((*it)->GetTimeToFinishMovement() == 0 &&
        (*it)->GetCellsToFinishMovement() != 0) {
      (*it)->StartMovement(((*it)->GetCellsToFinishMovement()), tanks_);
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

void MainWindow::NewGame() {
  if (!paused_) PauseOrContinue();
  new_game_dialog_->exec();
  switch_map_menu_->setCurrentIndex(current_game_options_.map_number);
  switch_tank_menu_->setCurrentIndex(current_game_options_.tank_number);
  switch_difficulty_menu_->setCurrentIndex(
      current_game_options_.difficulty_level_number);
}

void MainWindow::Settings() {
  if (!paused_) PauseOrContinue();
  settings_dialog_->exec();
  virtual_keys_checkbox_->setChecked(virtual_keys_shown_);
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
  pause_continue_button_->setGeometry(
      w_indent_ + static_cast<int>(0.04 * sq_width_),
      h_indent_ + static_cast<int>(0.11 * sq_height_),
      static_cast<int>(0.2 * sq_width_), static_cast<int>(0.05 * sq_height_));
  settings_button_->setGeometry(w_indent_ + static_cast<int>(0.04 * sq_width_),
                                h_indent_ + static_cast<int>(0.17 * sq_height_),
                                static_cast<int>(0.2 * sq_width_),
                                static_cast<int>(0.05 * sq_height_));

  if (virtual_keys_shown_) {
    for (int i = 0; i < number_of_virtual_keys_in_first_row_; ++i) {
      virtual_keys_buttons_[i]->setGeometry(
          w_indent_ + static_cast<int>(0.04 * sq_width_) +
              i * static_cast<int>(0.2 * sq_width_ / 3),
          height() - static_cast<int>(0.19 * sq_height_),
          static_cast<int>(0.2 * sq_width_ / 3),
          static_cast<int>(0.07 * sq_height_));
    }
    for (int i = number_of_virtual_keys_in_first_row_;
         i < virtual_keys_buttons_.size(); ++i) {
      virtual_keys_buttons_[i]->setGeometry(
          w_indent_ + static_cast<int>(0.04 * sq_width_) +
              static_cast<int>((i - number_of_virtual_keys_in_first_row_) *
                               0.2 * sq_width_ / 3),
          height() - static_cast<int>(0.12 * sq_height_),
          static_cast<int>(0.2 * sq_width_ / 3),
          static_cast<int>(0.07 * sq_height_));
    }
  }
}

void MainWindow::RedrawContent() {
  killTimer(timer_id_);
  timer_id_ = 0;
  map_.reset(new Map(current_game_options_.map_number + 1));
  tanks_.clear();
  rockets_.clear();

  obstacles_and_bonuses_.resize(map_->GetNumberOfCellsVertically());
  for (int i = 0; i < map_->GetNumberOfCellsVertically(); i++) {
    obstacles_and_bonuses_[i].resize(map_->GetNumberOfCellsHorizontally());
  }

  pause_continue_button_->setText(tr("Pause"));
  paused_ = false;

  tanks_.append(std::shared_ptr<Movable>(
      new Tank(map_, map_->GetTankInitCellX(), map_->GetTankInitCellY(),
               available_tank_types_[current_game_options_.tank_number],
               Direction::Up)));

  QFile bots_input_file(
      ":/tanks_info/bots" +
      QString::number(current_game_options_.map_number + 1) +
      QString::number(current_game_options_.difficulty_level_number + 1) +
      ".txt");
  if (!bots_input_file.exists()) {
    QMessageBox message;
    message.setIcon(QMessageBox::Information);
    message.setText(
        tr("This level of difficulty isn't available on this map. \n"
           "Try to switch to another map."));
    message.exec();
    return;
  }

  bots_input_file.open(QIODevice::ReadOnly);
  QTextStream in(&bots_input_file);
  int number_of_standart_bots, number_of_improved_bots, number_of_clever_bots,
      number_of_bots;
  in >> number_of_standart_bots >> number_of_improved_bots >>
      number_of_clever_bots;
  number_of_bots =
      number_of_standart_bots + number_of_improved_bots + number_of_clever_bots;
  for (int i = 0; i < number_of_bots; ++i) {
    BotQualities qualities;
    qualities.tank.max_health =
        70 + 15 * current_game_options_.difficulty_level_number;
    qualities.tank.rate_of_fire =
        1000 - 150 * current_game_options_.difficulty_level_number;
    qualities.tank.speed =
        1000 - 150 * current_game_options_.difficulty_level_number;
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

  QFile obstacles_file(
      ":/obstacles/obstacle" +
      QString::number(current_game_options_.map_number + 1) +
      QString::number(current_game_options_.difficulty_level_number + 1) +
      ".txt");

  obstacles_file.open(QIODevice::ReadOnly);
  QTextStream ir(&obstacles_file);
  int number_of_obstacles, x, y;
  ir >> number_of_obstacles;

  for (int i = 0; i < map_->GetNumberOfCellsVertically(); ++i) {
    for (int j = 0; j < map_->GetNumberOfCellsVertically(); ++j) {
      obstacles_and_bonuses_[i][j] = nullptr;
    }
  }

  for (int i = 0; i < number_of_obstacles; ++i) {
    ir >> x >> y;
    obstacles_and_bonuses_[x][y] =
        std::shared_ptr<Obstacle>(new Obstacle(map_, x, y));
  }

  obstacles_file.close();

  timer_id_ = startTimer(timer_duration_);
  repaint();
}

void MainWindow::PauseOrContinue() {
  if (paused_) {
    pause_continue_button_->setText(tr("Pause"));
    paused_ = false;
    timer_id_ = startTimer(timer_duration_);
  } else if (timer_id_ != 0) {
    pause_continue_button_->setText(tr("Continue"));
    paused_ = true;
    killTimer(timer_id_);
    timer_id_ = 0;
  }
}

void MainWindow::PressVirtualKey(Qt::Key key) {
  QKeyEvent *event = new QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier);
  QApplication::instance()->sendEvent(this, event);
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

void MainWindow::ToggleVirtualKeys() {
  virtual_keys_shown_ = !virtual_keys_shown_;
  for (int i = 0; i < virtual_keys_buttons_.size(); ++i) {
    virtual_keys_buttons_[i]->setVisible(virtual_keys_shown_);
  }
  RedrawButtons();
}

void MainWindow::GameOver() {
  killTimer(timer_id_);
  timer_id_ = 0;

  QMessageBox message;
  message.setIcon(QMessageBox::Information);
  if (tanks_.size() == number_of_player_tanks_) {
    message.setText(
        tr("You win! \n"
           "You can start a new game with help of appropriate button "
           "on the left."));
  } else {
    message.setText(
        tr("You died! \n"
           "You can start a new game with help of appropriate button "
           "on the left."));
  }
  message.exec();
}

void MainWindow::InitializeNewGameDialog() {
  new_game_dialog_ = new QDialog(this);

  new_game_info_label_ =
      new QLabel(tr("Choose map, tank and difficulty"), new_game_dialog_);

  switch_map_label_ =
      new QLabel(QString(tr("Map")) + QString(":"), new_game_dialog_);
  switch_map_menu_ = new QComboBox(new_game_dialog_);

  int map_number = 1;
  QFileInfo map_file(":/maps/map" + QString::number(map_number) + ".txt");
  while (map_file.exists() && map_file.isFile()) {
    switch_map_menu_->addItem(tr("Map") + " " + QString::number(map_number));
    map_number++;
    map_file = QFileInfo(":/maps/map" + QString::number(map_number) + ".txt");
  }

  switch_tank_label_ =
      new QLabel(QString(tr("Tank")) + QString(":"), new_game_dialog_);
  switch_tank_menu_ = new QComboBox(new_game_dialog_);

  QFile tanks_input_file(":/tanks_info/tanks.txt");
  tanks_input_file.open(QIODevice::ReadOnly);
  QTextStream in(&tanks_input_file);
  int number_of_tank_types;
  in >> number_of_tank_types;
  for (int i = 0; i < number_of_tank_types; ++i) {
    TankQualities qualities;
    in >> qualities.speed >> qualities.rate_of_fire >> qualities.max_health;
    available_tank_types_.push_back(qualities);
    switch_tank_menu_->addItem(tr("Tank") + " " + QString::number(i + 1));
  }
  tanks_input_file.close();

  switch_difficulty_label_ =
      new QLabel(QString(tr("Difficulty")) + QString(":"), new_game_dialog_);
  switch_difficulty_menu_ = new QComboBox(new_game_dialog_);

  for (int i = 0; i < difficulty_levels_names_.size(); ++i) {
    switch_difficulty_menu_->addItem(difficulty_levels_names_[i]);
  }

  new_game_dialog_layout_ = new QFormLayout(new_game_dialog_);
  new_game_dialog_layout_->addRow(new_game_info_label_);
  new_game_dialog_layout_->addRow(switch_map_label_);
  new_game_dialog_layout_->addRow(switch_map_menu_);
  new_game_dialog_layout_->addRow(switch_tank_label_);
  new_game_dialog_layout_->addRow(switch_tank_menu_);
  new_game_dialog_layout_->addRow(switch_difficulty_label_);
  new_game_dialog_layout_->addRow(switch_difficulty_menu_);

  new_game_dialog_buttons_ = new QDialogButtonBox(
      QDialogButtonBox::Ok, Qt::Horizontal, new_game_dialog_);

  new_game_dialog_layout_->addRow(new_game_dialog_buttons_);

  connect(new_game_dialog_buttons_->button(QDialogButtonBox::Ok),
          &QPushButton::clicked, [&]() {
            current_game_options_.map_number = switch_map_menu_->currentIndex();
            current_game_options_.tank_number =
                switch_tank_menu_->currentIndex();
            current_game_options_.difficulty_level_number =
                switch_difficulty_menu_->currentIndex();
            RedrawContent();
          });
  connect(new_game_dialog_buttons_, SIGNAL(accepted()), new_game_dialog_,
          SLOT(accept()));
}

void MainWindow::InitializeSettingsDialog() {
  settings_dialog_ = new QDialog(this);

  virtual_keys_checkbox_ =
      new QCheckBox(tr("Activate virtual keys"), settings_dialog_);
  virtual_keys_checkbox_->setChecked(virtual_keys_shown_);

  language_menu_label_ =
      new QLabel(QString(tr("Language")) + QString(":"), settings_dialog_);

  language_menu_ = new QComboBox(settings_dialog_);
  language_menu_->addItem(tr("Default"));
  language_menu_->addItem(tr("Belarusian"));
  language_menu_->addItem(tr("English"));
  language_menu_->addItem(tr("Russian"));
  language_menu_->setEnabled(false);

  settings_separator_label_ = new QLabel(this);
  version_label_ = new QLabel(QString(tr("App version")) + QString(": ") +
                              QString("0.4.0.0"));

  settings_dialog_layout_ = new QFormLayout(settings_dialog_);
  settings_dialog_layout_->addRow(virtual_keys_checkbox_);
  settings_dialog_layout_->addRow(language_menu_label_);
  settings_dialog_layout_->addRow(language_menu_);
  settings_dialog_layout_->addRow(settings_separator_label_);
  settings_dialog_layout_->addRow(version_label_);

  settings_dialog_buttons_ = new QDialogButtonBox(
      QDialogButtonBox::Ok, Qt::Horizontal, settings_dialog_);

  settings_dialog_layout_->addRow(settings_dialog_buttons_);

  connect(settings_dialog_buttons_->button(QDialogButtonBox::Ok),
          &QPushButton::clicked, [&]() {
            if (virtual_keys_shown_ != virtual_keys_checkbox_->isChecked()) {
              ToggleVirtualKeys();
            }
          });

  connect(settings_dialog_buttons_, SIGNAL(accepted()), settings_dialog_,
          SLOT(accept()));
}

void MainWindow::DestroyObstackle(std::shared_ptr<Tank> tank) {
  if (std::dynamic_pointer_cast<Obstacle>(
          obstacles_and_bonuses_[tank->GetCellX()][tank->GetCellY()]) !=
      nullptr) {
    tank->SetCurrentSpeed(2);
    obstacles_and_bonuses_[tank->GetCellX()][tank->GetCellY()] = nullptr;
  }
}
