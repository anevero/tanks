#include "mainwindow.h"

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
      tank->StartMovement(1, tanks_, objects_copies_, obstacles_and_bonuses_);
      break;
    case 1067:
    case Qt::Key_Down:
    case Qt::Key_S:
      tank->TurnReverseOn();
      tank->StartMovement(1, tanks_, objects_copies_, obstacles_and_bonuses_);
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
  for (const auto &object : objects_copies_) {
    auto tank = std::dynamic_pointer_cast<Tank>(object.first);
    tank->UpdateCoordinates(object.second.x, object.second.y);
  }

  QPainter p;
  p.begin(this);
  map_->DrawMap(p);
  for (const auto &vector : obstacles_and_bonuses_) {
    for (const auto &object : vector) {
      if (object != nullptr) {
        object->Draw(p);
      }
    }
  }

  for (const auto &object : objects_copies_) {
    object.first->Draw(p);
    object.first->ReturnToOriginal();
  }
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
  time_since_tooltip_appearing_ += timer_duration_;
  if (time_since_tooltip_appearing_ >= time_for_showing_tooltip_) {
    QToolTip::hideText();
    time_since_tooltip_appearing_ = 0;
  }

  time_since_last_bonus_ += timer_duration_;

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
        bot->StartMovement(1, tanks_, objects_copies_, obstacles_and_bonuses_);
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
    if (object->GetTimeToFinishMovement() > 0) {
      object->Move(timer_duration_);
    } else if (object->GetTimeToFinishRotation() != 0) {
      object->Rotate(timer_duration_);
    }
  }

  for (const auto &object : rockets_) {
    if (object->GetTimeToFinishMovement() > 0) {
      object->Move(timer_duration_);
    }
  }

  auto copy = objects_copies_.begin();
  while (copy != objects_copies_.end()) {
    if (copy->first->GetTimeToFinishMovement() <= 0) {
      std::dynamic_pointer_cast<Tank>(copy->first)->UpdateCoordinates(
                  copy->second.x, copy->second.y);
      copy = objects_copies_.erase(copy);
      continue;
    }
    copy++;
  }

  auto it = rockets_.begin();
  while (it != rockets_.end()) {
    if ((*it)->GetTimeToFinishMovement() <= 0 &&
        (*it)->GetCellsToFinishMovement() != 0) {
      (*it)->StartMovement(((*it)->GetCellsToFinishMovement()), tanks_,
                           objects_copies_, obstacles_and_bonuses_);
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

  if (time_since_last_bonus_ == 20000) {
    RandomMedicalKit();
    time_since_last_bonus_ = 0;
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

  pause_continue_button_->setText(tr("Pause"));
  paused_ = false;

  available_tank_types_[current_game_options_.tank_number].direction =
      DetermineDirection(map_->GetTankStartDirection());
  tanks_.append(std::shared_ptr<Movable>(
      new Tank(map_, map_->GetTankInitCellX(), map_->GetTankInitCellY(),
               available_tank_types_[current_game_options_.tank_number])));

  QJsonObject json = GetJsonObjectFromFile(
      ":/data/map" + QString::number(current_game_options_.map_number + 1) +
      ".json");

  QJsonArray bots =
      json["difficulty"]
          .toArray()[current_game_options_.difficulty_level_number]
          .toObject()["bots"]
          .toArray();

  for (int i = 0; i < bots.size(); ++i) {
    BotQualities qualities;
    qualities.tank.max_health =
        70 + 15 * current_game_options_.difficulty_level_number;
    qualities.tank.rate_of_fire =
        1000 - 150 * current_game_options_.difficulty_level_number;
    qualities.tank.speed =
        1000 - 150 * current_game_options_.difficulty_level_number;
    qualities.init_cell_x = bots[i].toObject()["initial_cell_x"].toInt();
    qualities.init_cell_y = bots[i].toObject()["initial_cell_y"].toInt();
    qualities.moving_length = bots[i].toObject()["moving_length"].toInt();
    qualities.amount_of_turns = bots[i].toObject()["amount_of_turns"].toInt();
    qualities.side_rotation_frequency =
        bots[i].toObject()["side_rotation_frequency"].toInt();
    qualities.tank.direction =
        DetermineDirection(bots[i].toObject()["initial_direction"].toString());

    if (bots[i].toObject()["type"].toString() == "standart") {
      tanks_.append(std::shared_ptr<Movable>(new Bot(map_, qualities)));
    } else if (bots[i].toObject()["type"].toString() == "improved") {
      tanks_.append(std::shared_ptr<Movable>(new ImprovedBot(map_, qualities)));
    } else {
      tanks_.append(std::shared_ptr<Movable>(new CleverBot(map_, qualities)));
    }
  }

  obstacles_and_bonuses_ =
      std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>(
          static_cast<unsigned>(map_->GetNumberOfCellsVertically()),
          std::vector<std::shared_ptr<ObjectOnMap>>(
              static_cast<unsigned>(map_->GetNumberOfCellsHorizontally()),
              nullptr));

  QJsonArray obstacles =
      json["difficulty"]
          .toArray()[current_game_options_.difficulty_level_number]
          .toObject()["obstacles"]
          .toArray();

  for (int i = 0; i < obstacles.size(); ++i) {
    int x = obstacles[i].toArray()[0].toInt();
    int y = obstacles[i].toArray()[1].toInt();
    obstacles_and_bonuses_[static_cast<unsigned int>(
        x)][static_cast<unsigned int>(y)] =
        std::shared_ptr<Obstacle>(new Obstacle(map_, x, y));
  }

  QJsonArray portals =
      json["difficulty"]
          .toArray()[current_game_options_.difficulty_level_number]
          .toObject()["portals"]
          .toArray();

  for (int i = 0; i < portals.size(); ++i) {
    int curr_x = portals[i].toArray()[0].toInt();
    int curr_y = portals[i].toArray()[1].toInt();
    int new_x = portals[i].toArray()[2].toInt();
    int new_y = portals[i].toArray()[3].toInt();
    obstacles_and_bonuses_[static_cast<unsigned int>(
        curr_x)][static_cast<unsigned int>(curr_y)] =
        std::shared_ptr<Portal>(new Portal(map_, curr_x, curr_y, new_x, new_y));
    obstacles_and_bonuses_[static_cast<unsigned int>(
        new_x)][static_cast<unsigned int>(new_y)] =
        std::shared_ptr<Portal>(new Portal(map_, new_x, new_y, curr_x, curr_y));
  }

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
      if (std::dynamic_pointer_cast<Rocket>(*rocket) == nullptr) {
        rocket++;
        continue;
      }
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
  auto object = tanks_.begin();
  for (int i = 0; i < number_of_player_tanks_; ++i) {
    if (std::dynamic_pointer_cast<Tank>(tanks_[i])->IsDead()) {
      GameOver();
      return;
    }
    object = std::next(object);
  }
  while (object != tanks_.end()) {
    if (std::dynamic_pointer_cast<Tank>(*object)->IsDead()) {
      MakeBoom(*object);
      object = tanks_.erase(object);
      continue;
    }
    object++;
  }
  if (tanks_.size() == number_of_player_tanks_) {
    GameOver();
  }
}

void MainWindow::MakeBoom(std::shared_ptr<Movable> &object) {
  std::shared_ptr<Boom> boom(new Boom(map_, object, 500));
  rockets_.append(boom);
  boom->StartMovement(1, tanks_, objects_copies_, obstacles_and_bonuses_);
}

void MainWindow::ShootRocket(std::shared_ptr<Tank> &tank) {
  std::shared_ptr<Rocket> rocket(new Rocket(map_, tank, 250, 10));
  rockets_.append(rocket);
  if (rocket->GetIntDirection() == 1 || rocket->GetIntDirection() == 3) {
    rocket->StartMovement(map_->GetNumberOfCellsHorizontally(), tanks_,
                          objects_copies_, obstacles_and_bonuses_);
  } else {
    rocket->StartMovement(map_->GetNumberOfCellsVertically(), tanks_,
                          objects_copies_, obstacles_and_bonuses_);
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
  QFileInfo map_file(":/data/map" + QString::number(map_number) + ".json");
  while (map_file.exists() && map_file.isFile()) {
    switch_map_menu_->addItem(tr("Map") + " " + QString::number(map_number));
    map_number++;
    map_file = QFileInfo(":/data/map" + QString::number(map_number) + ".json");
  }

  switch_tank_label_ =
      new QLabel(QString(tr("Tank")) + QString(":"), new_game_dialog_);
  switch_tank_menu_ = new QComboBox(new_game_dialog_);

  QJsonObject json = GetJsonObjectFromFile(":/data/tanks.json");
  QJsonArray tanks = json["tanks"].toArray();

  for (int i = 0; i < tanks.size(); ++i) {
    TankQualities qualities;
    qualities.speed = tanks[i].toObject()["speed"].toInt();
    qualities.rate_of_fire = tanks[i].toObject()["rate_of_fire"].toInt();
    qualities.max_health = tanks[i].toObject()["max_health"].toInt();
    available_tank_types_.push_back(qualities);
    switch_tank_menu_->addItem(tr("Tank") + " " + QString::number(i + 1));
  }

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
  language_menu_->addItem(tr("Belarusian"));
  language_menu_->addItem(tr("English"));
  language_menu_->addItem(tr("Russian"));
  DetermineCurrentLanguageSettings();

  language_menu_restart_label_ =
      new QLabel(QString(tr("Language will be changed\n"
                            "after application restart")));

  settings_separator_label_ = new QLabel(this);
  version_label_ =
      new QLabel(QString(tr("App version")) + QString(": ") + app_version_);

  settings_dialog_layout_ = new QFormLayout(settings_dialog_);
  settings_dialog_layout_->addRow(virtual_keys_checkbox_);
  settings_dialog_layout_->addRow(language_menu_label_);
  settings_dialog_layout_->addRow(language_menu_);
  settings_dialog_layout_->addRow(language_menu_restart_label_);
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
            ChangeCurrentLanguageSettings();
          });

  connect(settings_dialog_buttons_, SIGNAL(accepted()), settings_dialog_,
          SLOT(accept()));
}

void MainWindow::DetermineCurrentLanguageSettings() {
  QString language;
  QJsonObject json = GetJsonObjectFromFile("settings.json");
  language = json["language"].toString();

  if (language == "be_BY") {
    language_menu_->setCurrentIndex(0);
  } else if (language == "en_US") {
    language_menu_->setCurrentIndex(1);
  } else if (language == "ru_RU") {
    language_menu_->setCurrentIndex(2);
  }
}

void MainWindow::ChangeCurrentLanguageSettings() {
  QString language;
  switch (language_menu_->currentIndex()) {
    case 0:
      language = "be_BY";
      break;
    case 1:
      language = "en_US";
      break;
    case 2:
      language = "ru_RU";
      break;
  }

  QFile settings_file("settings.json");
  QJsonObject new_json_obj;
  new_json_obj["language"] = language;
  QJsonDocument new_json_document(new_json_obj);
  QString new_json_string = new_json_document.toJson();

  settings_file.open(QIODevice::WriteOnly);
  settings_file.write(new_json_string.toUtf8());
  settings_file.close();
}

QJsonObject MainWindow::GetJsonObjectFromFile(const QString &filepath) {
  QFile file(filepath);
  file.open(QIODevice::ReadOnly);
  QString text = file.readAll();
  file.close();
  QJsonDocument json_document(QJsonDocument::fromJson(text.toUtf8()));
  return json_document.object();
}

Direction MainWindow::DetermineDirection(const QString &start_direction) const {
  if (start_direction == "up") {
    return Direction::Up;
  }
  if (start_direction == "down") {
    return Direction::Down;
  }
  if (start_direction == "left") {
    return Direction::Left;
  }
  return Direction::Right;
}

void MainWindow::RandomMedicalKit() {
  for (size_t i = 0; i < obstacles_and_bonuses_.size(); i++) {
    for (size_t j = 0; j < obstacles_and_bonuses_[i].size(); j++) {
      if (std::dynamic_pointer_cast<MedicalKit>(obstacles_and_bonuses_[i][j]) !=
          nullptr) {
        obstacles_and_bonuses_[i][j] = nullptr;
      }
    }
  }

  int temp = 100;
  while (temp > 0) {
    int x, y;
    x = rand() % (map_->GetNumberOfCellsHorizontally() - 1) + 1;
    y = rand() % (map_->GetNumberOfCellsVertically() - 1) + 1;
    for (auto &object : tanks_) {
      if (obstacles_and_bonuses_[static_cast<unsigned>(x)]
                                [static_cast<unsigned>(y)] == nullptr &&
          (object->GetCellX() != x || object->GetCellY() != y) &&
          map_->GetField(x, y) != CellType::Wall) {
        obstacles_and_bonuses_[static_cast<unsigned>(x)][static_cast<unsigned>(
            y)] = std::shared_ptr<MedicalKit>(new MedicalKit(map_, x, y));
        return;
      }
    }
    temp--;
  }
}
