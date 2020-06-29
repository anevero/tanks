#include "mainwindow.h"

std::mt19937 MainWindow::random_generator_ = std::mt19937(
    std::chrono::system_clock::now().time_since_epoch().count());

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      map_(new Map(0)),
      screen_timer_(new QLCDNumber(this)),
      new_game_dialog_(new NewGameDialog(this)),
      about_dialog_(new AboutDialog(this)),
      main_buttons_layout_(new QVBoxLayout()),
      new_game_button_(new QPushButton(tr("New game"), this)),
      pause_continue_button_(new QPushButton(tr("Pause"), this)),
      settings_button_(new QPushButton(tr("Settings"), this)),
      about_button_(new QPushButton(tr("About"), this)),
      charge_buttons_layout_(new QHBoxLayout()),
      charge_buttons_({new QPushButton(this), new QPushButton(this),
                       new QPushButton(this)}),
      virtual_buttons_layout_(new QGridLayout()),
      virtual_keys_buttons_(
          {new QPushButton("Q", this), new QPushButton("W", this),
           new QPushButton("A", this), new QPushButton("S", this),
           new QPushButton("D", this)}),
      mobile_virtual_buttons_layout_left_(new QVBoxLayout()),
      mobile_virtual_buttons_layout_right_(new QHBoxLayout()),
      music_player_(new QMediaPlayer(this)),
      music_playlist_(new QMediaPlaylist(this)) {
  LoadApplicationSettings();
  LoadTanksTypesInfo();
  LoadRocketsTypesInfo();
  settings_dialog_ = new SettingsDialog(this);

  QSizePolicy expanding_size_policy = {
      QSizePolicy::Expanding, QSizePolicy::Expanding};

  new_game_button_->setSizePolicy(expanding_size_policy);
  pause_continue_button_->setSizePolicy(expanding_size_policy);
  settings_button_->setSizePolicy(expanding_size_policy);
  about_button_->setSizePolicy(expanding_size_policy);

  new_game_button_->setFocusPolicy(Qt::NoFocus);
  pause_continue_button_->setFocusPolicy(Qt::NoFocus);
  settings_button_->setFocusPolicy(Qt::NoFocus);
  about_button_->setFocusPolicy(Qt::NoFocus);

  main_buttons_layout_->addWidget(new_game_button_);
  main_buttons_layout_->addWidget(pause_continue_button_);
  main_buttons_layout_->addWidget(settings_button_);
  main_buttons_layout_->addWidget(about_button_);
  main_buttons_layout_->addWidget(screen_timer_);

  for (int i = 0; i < main_buttons_layout_->count(); ++i) {
    main_buttons_layout_->setStretch(i, 1);
  }

  connect(new_game_button_, &QPushButton::clicked,
          this, &MainWindow::ExecNewGameDialog);
  connect(pause_continue_button_, &QPushButton::clicked,
          this, &MainWindow::PauseOrContinue);
  connect(settings_button_, &QPushButton::clicked,
          this, &MainWindow::ExecSettingsDialog);
  connect(about_button_, &QPushButton::clicked,
          this, &MainWindow::ExecAboutDialog);

  for (int i = 0; i < constants::kChargesNumber; ++i) {
    charge_buttons_[i]->setSizePolicy(expanding_size_policy);
    charge_buttons_[i]->setFocusPolicy(Qt::NoFocus);
    charge_buttons_layout_->addWidget(charge_buttons_[i]);
    charge_buttons_[i]->hide();
    connect(charge_buttons_[i], &QPushButton::clicked,
            [this, i]() { SwitchCharge(i); });
  }

  standard_button_palette_ = charge_buttons_[0]->palette();
  charge_palettes_.resize(charge_colors_.size(), standard_button_palette_);
  for (int i = 0; i < constants::kChargesNumber; ++i) {
    charge_palettes_[i].setColor(QPalette::Button, charge_colors_[i]);
  }

  charge_buttons_[0]->setToolTip(
      tr("High speed, low charge, can destroy obstacles"));
  charge_buttons_[1]->setToolTip(
      tr("Medium speed, medium charge, can destroy obstacles"));
  charge_buttons_[2]->setToolTip(
      tr("Low speed, high charge, can't destroy obstacles"));

  for (int i = 0; i < constants::kVirtualKeysNumber; ++i) {
    virtual_keys_buttons_[i]->setAutoRepeat(true);
    connect(virtual_keys_buttons_[i], &QPushButton::released, [this, i]() {
      QApplication::sendEvent(
          this,
          new QKeyEvent(
              QEvent::KeyRelease, virtual_keys_encodings_[i], Qt::NoModifier));
    });
    virtual_keys_buttons_[i]->setSizePolicy(expanding_size_policy);
    virtual_keys_buttons_[i]->setFocusPolicy(Qt::NoFocus);
  }

  screen_timer_->setSegmentStyle(QLCDNumber::Flat);
  screen_timer_->setToolTip(
      tr("You have %1 minutes per round").arg(constants::kMinutesPerRound));
  standard_lcdnumber_palette_ = screen_timer_->palette();
  red_lcdnumber_palette_ = standard_lcdnumber_palette_;
  red_lcdnumber_palette_.setColor(QPalette::WindowText, QColor(255, 0, 0));
  UpdateScreenTimer();

  setMouseTracking(true);
  setMinimumSize(600, 450);
  resize(600, 450);

  music_playlist_->addMedia(QUrl("qrc:/sounds/backgroundmusic1.mp3"));
  music_playlist_->addMedia(QUrl("qrc:/sounds/backgroundmusic2.mp3"));
  music_playlist_->addMedia(QUrl("qrc:/sounds/backgroundmusic3.mp3"));
  music_playlist_->addMedia(QUrl("qrc:/sounds/backgroundmusic4.mp3"));
  music_player_->setPlaylist(music_playlist_);
  music_player_->setVolume(50);
}

MainWindow::~MainWindow() {
  delete charge_buttons_layout_;
  delete virtual_buttons_layout_;
  delete mobile_virtual_buttons_layout_left_;
  delete mobile_virtual_buttons_layout_right_;
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event) {
  int current_cell_x = (event->x() - map_->GetUpperLeftX())
      * map_->GetNumberOfCellsHorizontally() / map_->GetWidth();
  int current_cell_y = (event->y() - map_->GetUpperLeftY()) *
      map_->GetNumberOfCellsVertically() / map_->GetHeight();
  for (const auto& object : tanks_) {
    if (object->GetCellX() == current_cell_x &&
        object->GetCellY() == current_cell_y) {
      QToolTip::showText(event->globalPos(),
                         tr("Health") + ": "
                             + QString::number(std::dynamic_pointer_cast<Tank>(
                                 object)->GetCurrentHealth()));
      break;
    }
  }
}

void MainWindow::keyReleaseEvent(QKeyEvent* event) {
  // Returns if the game is not running.
  if (timer_id_ == 0 && !paused_) return;
  auto tank = std::dynamic_pointer_cast<Tank>(tanks_.front());

  // Returns if the game is paused and event key is not Esc / 1 / 2 / 3 (these
  // keys can be processed while the game is paused).
  if (paused_ &&
      event->key() != Qt::Key_1 &&
      event->key() != Qt::Key_2 &&
      event->key() != Qt::Key_3 &&
      event->key() != Qt::Key_Escape) {
    return;
  }

  // Returns if the game is not paused, but the tank is moving or rotating,
  // and event key is not Esc / 1 / 2 / 3 (these keys can be processed while
  // the tank is moving or rotating).
  if (!paused_ && tank->IsMovingOrRotating() &&
      event->key() != Qt::Key_1 &&
      event->key() != Qt::Key_2 &&
      event->key() != Qt::Key_3 &&
      event->key() != Qt::Key_Escape) {
    return;
  }

  switch (event->key()) {
    case Qt::Key_Escape: {
      pause_continue_button_->animateClick();
      break;
    }
    case 1062:
    case Qt::Key_Up:
    case Qt::Key_W: {
      tank->TurnReverseOff();
      tank->StartMovement(1, tanks_, &objects_copies_, &obstacles_and_bonuses_);
      break;
    }
    case 1067:
    case Qt::Key_Down:
    case Qt::Key_S: {
      tank->TurnReverseOn();
      tank->StartMovement(1, tanks_, &objects_copies_, &obstacles_and_bonuses_);
      break;
    }
    case 1060:
    case Qt::Key_Left:
    case Qt::Key_A: {
      tank->TurnRotationReverseOn();
      tank->StartRotation();
      break;
    }
    case 1042:
    case Qt::Key_Right:
    case Qt::Key_D: {
      tank->TurnRotationReverseOff();
      tank->StartRotation();
      break;
    }
    case 1049:
    case Qt::Key_Q: {
      if (tank->IsAbleToShoot()) {
        tank->SetZeroTimeFromLastShot();
        ShootRocket(tank);
      }
      break;
    }
    case Qt::Key_1: {
      SwitchCharge(0);
      break;
    }
    case Qt::Key_2: {
      SwitchCharge(1);
      break;
    }
    case Qt::Key_3: {
      SwitchCharge(2);
      break;
    }
  }
}

void MainWindow::paintEvent(QPaintEvent*) {
  map_->UpdateCoordinates(width_indent_ + 0.28 * game_width_,
                          height_indent_ + 0.05 * game_height_,
                          0.68 * game_width_,
                          0.9 * game_height_);
  for (auto& object : tanks_) {
    object->UpdateCoordinates(object->GetCellX(), object->GetCellY());
  }
  for (auto& object : rockets_) {
    object->UpdateCoordinates(object->GetCellX(), object->GetCellY());
  }
  for (auto& object : objects_copies_) {
    auto tank = std::dynamic_pointer_cast<Tank>(object.first);
    tank->UpdateCoordinates(object.second.x, object.second.y);
  }
  for (auto& vector : obstacles_and_bonuses_) {
    for (auto& object : vector) {
      if (object != nullptr) {
        object->UpdateCoordinates();
      }
    }
  }

  QPainter p(this);
  map_->DrawMap(&p);
  for (auto& vector : obstacles_and_bonuses_) {
    for (auto& object : vector) {
      if (object != nullptr) {
        object->Draw(&p);
      }
    }
  }
  for (auto& object : objects_copies_) {
    object.first->Draw(&p);
    object.first->ReturnToOriginal();
  }
  for (auto& object : tanks_) {
    object->Draw(&p);
  }
  for (auto& object : rockets_) {
    object->Draw(&p);
  }

  if (!tanks_.empty() && charge_line_enabled_) {
    auto tank = std::dynamic_pointer_cast<Tank>(tanks_.front());
    p.setBrush(charge_colors_[static_cast<int>(tank->GetChargeState())]);
    p.drawRect(width_indent_ + 0.04 * game_width_,
               height() - height_indent_ - 0.25 * game_height_,
               0.2 * game_width_ * tank->GetTimeSinceLastShot()
                   / tank->GetRateOfFire(),
               game_height_ / 32);
  }

  p.end();
}

void MainWindow::resizeEvent(QResizeEvent*) {
  UpdateIndents();
  RedrawButtons();
  RedrawChargeButtons();
}

void MainWindow::timerEvent(QTimerEvent*) {
  time_since_last_medical_kit_ += timer_duration_;
  time_since_last_charge_ += timer_duration_;
  screen_timer_ms_ += timer_duration_;
  UpdateScreenTimer();

  for (auto& object : tanks_) {
    auto bot = std::dynamic_pointer_cast<Bot>(object);
    if (bot == nullptr) {
      continue;
    }

    if (bot->IsAbleToShoot() && bot->IsShotNeeded(
        map_, std::dynamic_pointer_cast<Tank>(tanks_.front()))) {
      auto tank = std::dynamic_pointer_cast<Tank>(object);
      ShootRocket(tank);
      bot->SetZeroTimeFromLastShot();
    }

    if (bot->IsMovingStartNeeded(tanks_, obstacles_and_bonuses_)) {
      bot->StartMovement(1, tanks_, &objects_copies_,
                         &obstacles_and_bonuses_);
    } else if (bot->IsRotationStartNeeded(
        std::dynamic_pointer_cast<Tank>(tanks_.front()))) {
      bot->StartRotation();
    }

    if (bot->IsTurnNeeded()) {
      bot->Rotate(timer_duration_);
    }
  }

  for (auto& object : tanks_) {
    if (object->GetTimeToFinishMovement() > 0) {
      object->Move(timer_duration_);
    } else if (object->GetTimeToFinishRotation() > 0) {
      object->Rotate(timer_duration_);
    }
  }

  for (auto& object : rockets_) {
    if (object->GetTimeToFinishMovement() > 0) {
      object->Move(timer_duration_);
    }
  }

  auto copy = objects_copies_.begin();
  while (copy != objects_copies_.end()) {
    if (copy->first->GetTimeToFinishMovement() <= 0) {
      std::dynamic_pointer_cast<Tank>(copy->first)->
          UpdateCoordinates(copy->second.x, copy->second.y);
      copy = objects_copies_.erase(copy);
      continue;
    }
    ++copy;
  }

  auto it = rockets_.begin();
  while (it != rockets_.end()) {
    if ((*it)->GetTimeToFinishMovement() <= 0 &&
        (*it)->GetCellsToFinishMovement() > 0) {
      (*it)->StartMovement(((*it)->GetCellsToFinishMovement()), tanks_,
                           &objects_copies_, &obstacles_and_bonuses_);
    }
    if (!(*it)->IsMovingOrRotating()) {
      if (std::dynamic_pointer_cast<Boom>(*it) != nullptr) {
        for (const auto& tank : tanks_) {
          if (Movable::HaveObjectsCollided(*it, tank)) {
            std::dynamic_pointer_cast<Tank>(tank)->DecreaseHealth(25);
          }
        }

        auto cell_x = (*it)->GetCellX();
        auto cell_y = (*it)->GetCellY();

        if (std::dynamic_pointer_cast<Portal>(
            obstacles_and_bonuses_[cell_x - 1][cell_y]) == nullptr) {
          obstacles_and_bonuses_[cell_x - 1][cell_y] = nullptr;
        }
        if (std::dynamic_pointer_cast<Portal>(
            obstacles_and_bonuses_[cell_x + 1][cell_y]) == nullptr) {
          obstacles_and_bonuses_[cell_x + 1][cell_y] = nullptr;
        }
      }

      it = rockets_.erase(it);
      continue;
    }
    ++it;
  }

  for (const auto& object : tanks_) {
    std::dynamic_pointer_cast<Tank>(object)->IncreaseTimeSinceLastShot(
        timer_duration_);
  }

  if (time_since_last_medical_kit_ >= constants::kMedicalKitSpawnPeriod) {
    RandomBonus<MedicalKit>();
    time_since_last_medical_kit_ = 0;
  }

  if (time_since_last_charge_ >= constants::kChargeSpawnPeriod) {
    RandomBonus<Charge>();
    time_since_last_charge_ = 0;
  }

  FindInteractingObjects();
  CheckDeadObjects();

  RedrawChargeButtons();
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

void MainWindow::ExecNewGameDialog() {
  if (!paused_) PauseOrContinue();

  new_game_dialog_->show();
  if (new_game_dialog_->exec() == QDialog::Rejected) {
    return;
  }

  current_map_number_ = new_game_dialog_->GetCurrentMapNumber();
  current_tank_number_ = new_game_dialog_->GetCurrentTankNumber();
  current_difficulty_level_ = new_game_dialog_->GetCurrentDifficultyLevel();
  StartRound();
}

void MainWindow::ExecSettingsDialog() {
  if (!paused_) PauseOrContinue();

  settings_dialog_->show();
  if (settings_dialog_->exec() == QDialog::Rejected) {
    return;
  }

  if (virtual_keys_enabled_ != settings_dialog_->AreVirtualKeysEnabled()) {
    SetVirtualKeysEnabled(settings_dialog_->AreVirtualKeysEnabled());
  }
  if (mobile_virtual_keys_style_enabled_
      != settings_dialog_->IsMobileVirtualKeysStyleEnabled()) {
    SetMobileVirtualKeysStyleEnabled(
        settings_dialog_->IsMobileVirtualKeysStyleEnabled());
  }
  if (music_enabled_ != settings_dialog_->IsMusicEnabled()) {
    SetMusicEnabled(settings_dialog_->IsMusicEnabled());
  }
  if (current_fps_option_ != settings_dialog_->GetCurrentFpsOption()) {
    SetFpsOption(settings_dialog_->GetCurrentFpsOption());
  }
  charge_line_enabled_ = settings_dialog_->IsChargeLineEnabled();

  repaint();
}

void MainWindow::ExecAboutDialog() {
  if (!paused_) PauseOrContinue();

#ifdef Q_OS_ANDROID
  about_dialog_->showFullScreen();
#else
  about_dialog_->resize(500, 500);
  about_dialog_->show();
#endif

  about_dialog_->exec();
}

void MainWindow::LoadApplicationSettings() {
  using constants::kVirtualKeysEnabledKey;
  using constants::kMobileVirtualKeysStyleEnabledKey;
  using constants::kChargeLineEnabledKey;
  using constants::kMusicEnabledKey;
  using constants::kCurrentFpsOptionKey;
  using constants::kLanguageKey;
  using constants::kLanguages;

  QSettings settings;

  if (settings.contains(kVirtualKeysEnabledKey)) {
    virtual_keys_enabled_ = settings.value(kVirtualKeysEnabledKey).toBool();
  } else {
    virtual_keys_enabled_ = !QTouchDevice::devices().empty();
    settings.setValue(kVirtualKeysEnabledKey, virtual_keys_enabled_);
  }

  if (settings.contains(kMobileVirtualKeysStyleEnabledKey)) {
    mobile_virtual_keys_style_enabled_ =
        settings.value(kMobileVirtualKeysStyleEnabledKey).toBool();
  } else {
    mobile_virtual_keys_style_enabled_ = false;

#ifdef Q_OS_ANDROID
    mobile_virtual_keys_style_enabled_ = true;
#endif

    settings.setValue(kMobileVirtualKeysStyleEnabledKey,
                      mobile_virtual_keys_style_enabled_);
  }

  if (settings.contains(kChargeLineEnabledKey)) {
    charge_line_enabled_ = settings.value(kChargeLineEnabledKey).toBool();
  } else {
    charge_line_enabled_ = true;
    settings.setValue(kChargeLineEnabledKey, charge_line_enabled_);
  }

  if (settings.contains(kMusicEnabledKey)) {
    music_enabled_ = settings.value(kMusicEnabledKey).toBool();
  } else {
    music_enabled_ = true;
    settings.setValue(kMusicEnabledKey, music_enabled_);
  }

  if (settings.contains(kCurrentFpsOptionKey)) {
    current_fps_option_ = settings.value(kCurrentFpsOptionKey).toInt();
  } else {
    current_fps_option_ = 2;

#ifdef Q_OS_ANDROID
    current_fps_option_ = 1;
#endif

    settings.setValue(kCurrentFpsOptionKey, current_fps_option_);
  }

  SetVirtualKeysEnabled(virtual_keys_enabled_);
  SetMobileVirtualKeysStyleEnabled(mobile_virtual_keys_style_enabled_);
  SetMusicEnabled(music_enabled_);
  SetFpsOption(current_fps_option_);
}

void MainWindow::LoadTanksTypesInfo() {
  QJsonObject json = GetJsonObjectFromFile(":/game_objects_data/tanks.json");
  QJsonArray tanks = json["tanks"].toArray();

  for (auto&& tank : tanks) {
    auto tank_json_object = tank.toObject();
    tanks_types_.push_back({});
    tanks_types_.back().speed =
        tank_json_object["speed"].toInt();
    tanks_types_.back().rate_of_fire =
        tank_json_object["rate_of_fire"].toInt();
    tanks_types_.back().max_health =
        tank_json_object["max_health"].toInt();
    tanks_types_.back().max_light_charge =
        tank_json_object["max_light_charge"].toInt();
    tanks_types_.back().max_medium_charge =
        tank_json_object["max_medium_charge"].toInt();
    tanks_types_.back().max_hard_charge =
        tank_json_object["max_hard_charge"].toInt();
  }
}

void MainWindow::LoadRocketsTypesInfo() {
  QJsonObject json = GetJsonObjectFromFile(":/game_objects_data/rockets.json");
  QJsonArray rockets = json["rockets"].toArray();

  for (auto&& rocket : rockets) {
    auto rocket_json_object = rocket.toObject();
    rockets_types_.push_back({});
    rockets_types_.back().power =
        rocket_json_object["power"].toInt();
    rockets_types_.back().speed =
        rocket_json_object["speed"].toInt();
    rockets_types_.back().obstacle_break =
        rocket_json_object["obstacle_break"].toBool();
  }
}

void MainWindow::SetVirtualKeysEnabled(bool enabled) {
  virtual_keys_enabled_ = enabled;

  for (auto& button : virtual_keys_buttons_) {
    button->setVisible(virtual_keys_enabled_);
  }

  RedrawButtons();
}

void MainWindow::SetMobileVirtualKeysStyleEnabled(bool enabled) {
  mobile_virtual_keys_style_enabled_ = enabled;

  if (!mobile_virtual_keys_style_enabled_) {
    mobile_virtual_buttons_layout_left_->removeWidget(virtual_keys_buttons_[1]);
    mobile_virtual_buttons_layout_left_->removeWidget(virtual_keys_buttons_[0]);
    mobile_virtual_buttons_layout_left_->removeWidget(virtual_keys_buttons_[3]);
    mobile_virtual_buttons_layout_right_->removeWidget(
        virtual_keys_buttons_[2]);
    mobile_virtual_buttons_layout_right_->removeWidget(
        virtual_keys_buttons_[4]);

    for (int i = 0; i < number_of_virtual_keys_in_first_row_; ++i) {
      virtual_buttons_layout_->addWidget(virtual_keys_buttons_[i], 0, i);
    }

    for (int i = number_of_virtual_keys_in_first_row_;
         i < constants::kVirtualKeysNumber; ++i) {
      virtual_buttons_layout_->addWidget(
          virtual_keys_buttons_[i], 1,
          i - number_of_virtual_keys_in_first_row_);
    }
  } else {
    for (auto& button : virtual_keys_buttons_) {
      virtual_buttons_layout_->removeWidget(button);
    }

    mobile_virtual_buttons_layout_left_->addWidget(virtual_keys_buttons_[1]);
    mobile_virtual_buttons_layout_left_->addWidget(virtual_keys_buttons_[0]);
    mobile_virtual_buttons_layout_left_->addWidget(virtual_keys_buttons_[3]);
    mobile_virtual_buttons_layout_right_->addWidget(virtual_keys_buttons_[2]);
    mobile_virtual_buttons_layout_right_->addWidget(virtual_keys_buttons_[4]);
  }

  RedrawButtons();
}

void MainWindow::SetMusicEnabled(bool enabled) {
  music_enabled_ = enabled;

  if (!music_enabled_) {
    music_player_->stop();
  } else if (music_enabled_ && timer_id_ == 0 && paused_) {
    music_player_->play();
  }
}

void MainWindow::SetFpsOption(int index, bool start_timer) {
  current_fps_option_ = index;
  timer_duration_ = 1000 / constants::kFpsOptions[current_fps_option_];

  if (timer_id_ != 0) {
    killTimer(timer_id_);
  }
  if (start_timer) {
    timer_id_ = startTimer(timer_duration_);
  }
}

void MainWindow::LoadRoundInfo() {
  map_.reset(new Map(current_map_number_));
  tanks_.clear();
  rockets_.clear();
  obstacles_and_bonuses_.clear();
  objects_copies_.clear();

  tanks_.push_back(std::make_shared<Tank>(
      map_,
      map_->GetTankInitCellX(),
      map_->GetTankInitCellY(),
      tanks_types_[current_tank_number_],
      Movable::GetDirectionFromString(map_->GetTankStartDirection())));

  QJsonObject json = GetJsonObjectFromFile(
      ":/maps/map" + QString::number(current_map_number_ + 1) + ".json");

  QJsonArray bots = json["difficulty"]
      .toArray()[current_difficulty_level_].toObject()["bots"].toArray();

  for (auto bot : bots) {
    TankParameters tank_parameters = {};
    tank_parameters.max_health = 70 + 15 * current_difficulty_level_;
    tank_parameters.rate_of_fire = 1000 - 150 * current_difficulty_level_;
    tank_parameters.speed = 1000 - 150 * current_difficulty_level_;
    tank_parameters.max_light_charge = 1;
    tank_parameters.max_medium_charge = 1;
    tank_parameters.max_hard_charge = 1;

    auto bot_object = bot.toObject();
    BotParameters bot_parameters = {};

    bot_parameters.moving_length = bot_object["moving_length"].toInt();
    bot_parameters.amount_of_turns = bot_object["amount_of_turns"].toInt();
    bot_parameters.side_rotation_frequency =
        bot_object["side_rotation_frequency"].toInt();

    int init_cell_x = bot_object["initial_cell_x"].toInt();
    int init_cell_y = bot_object["initial_cell_y"].toInt();
    Direction direction = Movable::GetDirectionFromString(
        bot_object["initial_direction"].toString().toStdString());

    if (bot_object["type"].toString() == "standard") {
      tanks_.push_back(std::make_shared<Bot>(
          map_, init_cell_x, init_cell_y,
          tank_parameters, bot_parameters, direction));
    } else if (bot_object["type"].toString() == "improved") {
      tanks_.push_back(std::make_shared<ImprovedBot>(
          map_, init_cell_x, init_cell_y,
          tank_parameters, bot_parameters, direction));
    } else {
      tanks_.push_back(std::make_shared<CleverBot>(
          map_, init_cell_x, init_cell_y,
          tank_parameters, bot_parameters, direction));
    }
  }

  obstacles_and_bonuses_.resize(
      map_->GetNumberOfCellsVertically(),
      std::vector<std::shared_ptr<ObjectOnMap>>(
          map_->GetNumberOfCellsHorizontally(), nullptr));

  QJsonArray obstacles = json["difficulty"]
      .toArray()[current_difficulty_level_].toObject()["obstacles"].toArray();

  for (auto obstacle : obstacles) {
    auto x = obstacle.toArray()[0].toInt();
    auto y = obstacle.toArray()[1].toInt();
    obstacles_and_bonuses_[x][y] = std::make_shared<Obstacle>(map_, x, y);
  }

  QJsonArray portals = json["difficulty"]
      .toArray()[current_difficulty_level_].toObject()["portals"].toArray();

  for (auto portal : portals) {
    auto current_x = portal.toArray()[0].toInt();
    auto current_y = portal.toArray()[1].toInt();
    auto new_x = portal.toArray()[2].toInt();
    auto new_y = portal.toArray()[3].toInt();
    obstacles_and_bonuses_[current_x][current_y] =
        std::make_shared<Portal>(map_, current_x, current_y, new_x, new_y);
    obstacles_and_bonuses_[new_x][new_y] =
        std::make_shared<Portal>(map_, new_x, new_y, current_x, current_y);
  }
}

void MainWindow::StartRound() {
  killTimer(timer_id_);
  timer_id_ = 0;

  music_player_->stop();

  screen_timer_ms_ = 0;
  screen_timer_sec_ = 0;
  screen_timer_min_ = 0;
  screen_timer_->setPalette(standard_lcdnumber_palette_);

  pause_continue_button_->setText(tr("Pause"));
  paused_ = false;

  for (auto& charge_button : charge_buttons_) {
    charge_button->show();
  }

  LoadRoundInfo();

  timer_id_ = startTimer(timer_duration_);

  music_playlist_->setCurrentIndex(current_map_number_);
  music_playlist_->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
  if (music_enabled_) {
    music_player_->play();
  }

  RedrawChargeButtons();
  repaint();
}

void MainWindow::FinishRound(bool win) {
  music_player_->stop();

  killTimer(timer_id_);
  timer_id_ = 0;

  QMessageBox message;
  message.setIcon(QMessageBox::Information);
  if (win) {
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

void MainWindow::UpdateIndents() {
  game_width_ = 4 * std::min(width() / 4, height() / 3);
  game_height_ = 3 * std::min(width() / 4, height() / 3);

  width_indent_ = (width() - game_width_) / 2;
  height_indent_ = (height() - game_height_) / 2;
}

void MainWindow::RedrawButtons() {
  main_buttons_layout_->setSpacing(0.01 * game_height_);
  main_buttons_layout_->setGeometry(QRect(
      width_indent_ + 0.04 * game_width_,
      height_indent_ + 0.05 * game_height_,
      0.2 * game_width_,
      0.4 * game_height_));

  if (!virtual_keys_enabled_) {
    return;
  }

  if (!mobile_virtual_keys_style_enabled_) {
    virtual_buttons_layout_->setSpacing(0.01 * game_height_);
    virtual_buttons_layout_->setGeometry(QRect(
        width_indent_ + 0.04 * game_width_,
        height() - height_indent_ - 0.2 * game_height_,
        0.2 * game_width_,
        0.15 * game_height_));
  } else {
    mobile_virtual_buttons_layout_left_->setSpacing(0.01 * game_height_);
    mobile_virtual_buttons_layout_right_->setSpacing(0.01 * game_height_);
    mobile_virtual_buttons_layout_left_->setGeometry(QRect(
        std::max(10, width_indent_ - static_cast<int>(0.2 * game_width_)),
        height() - height_indent_ - static_cast<int>(0.5 * game_height_),
        std::min(width_indent_, static_cast<int>(0.2 * game_width_)) - 10,
        static_cast<int>(0.45 * game_height_)));
    mobile_virtual_buttons_layout_right_->setGeometry(QRect(
        width() - width_indent_,
        height() - height_indent_ - static_cast<int>(0.5 * game_height_),
        std::min(width_indent_, static_cast<int>(0.2 * game_width_)) - 10,
        static_cast<int>(0.45 * game_height_)));
  }

  SetButtonsFontPixelSize(5 * new_game_button_->height() / 12);
}

void MainWindow::SetButtonsFontPixelSize(int pixel_size) {
  QFont adjusted_font = new_game_button_->font();
  adjusted_font.setPixelSize(pixel_size);

  new_game_button_->setFont(adjusted_font);
  pause_continue_button_->setFont(adjusted_font);
  settings_button_->setFont(adjusted_font);
  about_button_->setFont(adjusted_font);

  for (auto& charge_button : charge_buttons_) {
    charge_button->setFont(adjusted_font);
  }

  for (auto& virtual_key_button : virtual_keys_buttons_) {
    virtual_key_button->setFont(adjusted_font);
  }
}

void MainWindow::RedrawChargeButtons() {
  if (tanks_.empty()) return;
  auto tank = std::dynamic_pointer_cast<Tank>(tanks_.front());

  for (int i = 0; i < constants::kChargesNumber; ++i) {
    if (i == tank->GetTypeOfCharge()) {
      charge_buttons_[i]->setPalette(
          charge_palettes_[static_cast<int>(tank->GetChargeState())]);
    } else {
      charge_buttons_[i]->setPalette(standard_button_palette_);
    }
    charge_buttons_[i]->setText(QString::number(tank->GetCurrentCharge(i)));
  }

  charge_buttons_layout_->setSpacing(0.01 * game_height_);
  charge_buttons_layout_->setGeometry(QRect(
      width_indent_ + 0.04 * game_width_,
      height() - height_indent_ - 0.385 * game_height_,
      0.2 * game_width_,
      0.13 * game_height_));
}

void MainWindow::UpdateScreenTimer() {
  screen_timer_sec_ += screen_timer_ms_ / 1000;
  screen_timer_min_ += screen_timer_sec_ / 60;
  screen_timer_ms_ %= 1000;
  screen_timer_sec_ %= 60;

  if (screen_timer_min_ >= constants::kMinutesPerRound) {
    FinishRound(false);
  } else if (screen_timer_min_ >= constants::kMinutesPerRound - 2) {
    if (screen_timer_sec_ % 2 == 0) {
      screen_timer_->setPalette(red_lcdnumber_palette_);
    } else {
      screen_timer_->setPalette(standard_lcdnumber_palette_);
    }
  }

  QString time{};
  if (screen_timer_min_ < 10) {
    time += '0';
  }
  time += QString::number(screen_timer_min_) + ':';
  if (screen_timer_sec_ < 10) {
    time += '0';
  }
  time += QString::number(screen_timer_sec_);

  screen_timer_->display(time);
}

void MainWindow::FindInteractingObjects() {
  auto tank = tanks_.begin();
  while (tank != tanks_.end()) {
    auto rocket = rockets_.begin();
    while (rocket != rockets_.end()) {
      if (std::dynamic_pointer_cast<Rocket>(*rocket) == nullptr) {
        ++rocket;
        continue;
      }
      if (Movable::HaveObjectsCollided(*rocket, *tank)) {
        std::dynamic_pointer_cast<Tank>(*tank)->DecreaseHealth(
            std::dynamic_pointer_cast<Rocket>(*rocket)->GetPower());
        rocket = rockets_.erase(rocket);
        continue;
      }
      ++rocket;
    }
    ++tank;
  }
}

void MainWindow::CheckDeadObjects() {
  if (std::dynamic_pointer_cast<Tank>(tanks_.front())->IsDead()) {
    FinishRound(false);
    return;
  }

  auto object = std::next(tanks_.begin());
  while (object != tanks_.end()) {
    if (std::dynamic_pointer_cast<Tank>(*object)->IsDead()) {
      MakeBoom(*object);
      object = tanks_.erase(object);
      continue;
    }
    ++object;
  }

  auto copy = objects_copies_.begin();
  while (copy != objects_copies_.end()) {
    if (std::dynamic_pointer_cast<Tank>(copy->first)->IsDead()) {
      MakeBoom(copy->first);
      copy = objects_copies_.erase(copy);
      continue;
    }
    ++copy;
  }
  if (tanks_.size() == 1) {
    FinishRound(true);
  }
}

void MainWindow::ShootRocket(const std::shared_ptr<Tank>& tank) {
  std::shared_ptr<Rocket> rocket;
  if (std::dynamic_pointer_cast<Bot>(tank) == nullptr) {
    rocket = std::make_shared<Rocket>(
        map_, tank, rockets_types_[tank->GetTypeOfCharge()]);
    tank->DecreaseCharge(tank->GetTypeOfCharge());
    RedrawChargeButtons();
  } else {
    rocket = std::make_shared<Rocket>(map_, tank, rockets_types_[1]);
  }

  rockets_.push_back(rocket);
  if (rocket->GetIntDirection() == 1 || rocket->GetIntDirection() == 3) {
    rocket->StartMovement(map_->GetNumberOfCellsHorizontally(), tanks_,
                          &objects_copies_, &obstacles_and_bonuses_);
  } else {
    rocket->StartMovement(map_->GetNumberOfCellsVertically(), tanks_,
                          &objects_copies_, &obstacles_and_bonuses_);
  }
}

void MainWindow::MakeBoom(const std::shared_ptr<Movable>& object) {
  auto boom = std::make_shared<Boom>(map_, object, 1000);
  rockets_.push_back(boom);
  boom->StartMovement(1, tanks_, &objects_copies_, &obstacles_and_bonuses_);
}

template<class Bonus>
void MainWindow::RandomBonus() {
  int width = obstacles_and_bonuses_.size();
  int height = obstacles_and_bonuses_[0].size();

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      if (std::dynamic_pointer_cast<Bonus>(
          obstacles_and_bonuses_[i][j]) != nullptr) {
        obstacles_and_bonuses_[i][j] = nullptr;
      }
    }
  }

  int attempts = 100;
  bool flag = true;
  while (attempts > 0) {
    int x = random_generator_() % (map_->GetNumberOfCellsHorizontally() - 1);
    int y = random_generator_() % (map_->GetNumberOfCellsVertically() - 1);
    for (auto& object : tanks_) {
      if (object->GetCellX() == x && object->GetCellY() == y) {
        flag = false;
        break;
      }
    }
    if (obstacles_and_bonuses_[x][y] == nullptr &&
        flag && map_->GetField(x, y) != CellType::Wall) {
      obstacles_and_bonuses_[x][y] =
          std::make_shared<Bonus>(map_, x, y);
      return;
    }
    --attempts;
  }
}

void MainWindow::SwitchCharge(int type) {
  if (timer_id_ == 0 && !paused_) return;
  std::dynamic_pointer_cast<Tank>(tanks_.front())->ChangeTypeOfCharge(type);
  RedrawChargeButtons();
  repaint();
}

QJsonObject MainWindow::GetJsonObjectFromFile(const QString& filepath) {
  QFile file(filepath);
  file.open(QIODevice::ReadOnly);
  QString text = file.readAll();
  file.close();
  QJsonDocument json_document(QJsonDocument::fromJson(text.toUtf8()));
  return json_document.object();
}
