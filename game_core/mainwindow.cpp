#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      map_(new Map(1)),
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
      virtual_keys_encodings_(
          {Qt::Key_Q, Qt::Key_W, Qt::Key_A, Qt::Key_S, Qt::Key_D}),
      new_virtual_buttons_layout_left_(new QVBoxLayout),
      new_virtual_buttons_layout_right_(new QHBoxLayout) {
  LoadTanksTypesInfo();
  LoadRocketsTypesInfo();
  setMouseTracking(true);

  new_game_button_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                              QSizePolicy::Expanding));
  pause_continue_button_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                                    QSizePolicy::Expanding));
  settings_button_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                              QSizePolicy::Expanding));
  about_button_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                           QSizePolicy::Expanding));

  new_game_button_->setFocusPolicy(Qt::NoFocus);
  pause_continue_button_->setFocusPolicy(Qt::NoFocus);
  settings_button_->setFocusPolicy(Qt::NoFocus);
  about_button_->setFocusPolicy(Qt::NoFocus);

  screen_timer_->setSegmentStyle(QLCDNumber::Flat);
  screen_timer_->setToolTip(
      tr("You have") + " " + QString::number(minutes_per_round_) + " "
          + tr("minutes per round"));
  standart_lcdnumber_palette_ = screen_timer_->palette();
  red_lcdnumber_palette_ = standart_lcdnumber_palette_;
  red_lcdnumber_palette_.setColor(QPalette::WindowText, QColor(255, 0, 0));
  UpdateScreenTimer();

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
          this, &MainWindow::Settings);
  connect(about_button_, &QPushButton::clicked,
          this, &MainWindow::ExecAboutDialog);

  for (int i = 0; i < charge_buttons_.size(); ++i) {
    charge_buttons_[i]->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                                  QSizePolicy::Expanding));
    charge_buttons_[i]->setFocusPolicy(Qt::NoFocus);
    charge_buttons_layout_->addWidget(charge_buttons_[i]);
    charge_buttons_[i]->hide();
    connect(charge_buttons_[i], &QPushButton::clicked,
            [this, i]() { ChangeChargeButton(i); });
  }

  standart_button_palette_ = charge_buttons_[0]->palette();
  charge_palettes_.resize(charge_colors_.size());
  for (int i = 0; i < charge_palettes_.size(); ++i) {
    charge_palettes_[i] = standart_button_palette_;
    charge_palettes_[i].setColor(QPalette::Button, charge_colors_[i]);
  }

  charge_buttons_[0]->setToolTip(
      tr("Hight speed, low charge, can destroy obstacles"));
  charge_buttons_[1]->setToolTip(
      tr("Medium speed, medium charge, can destroy obstacles"));
  charge_buttons_[2]->setToolTip(
      tr("Low speed, high charge, can't destroy obstacles"));

  for (int i = 0; i < virtual_keys_buttons_.size(); ++i) {
    virtual_keys_buttons_[i]->setAutoRepeat(true);
    connect(virtual_keys_buttons_[i], &QPushButton::released,
            [this, i]() { PressVirtualKey(virtual_keys_encodings_[i]); });
    virtual_keys_buttons_[i]->setSizePolicy({QSizePolicy::Expanding,
                                             QSizePolicy::Expanding});
    virtual_keys_buttons_[i]->setFocusPolicy(Qt::NoFocus);
  }

#ifdef Q_OS_ANDROID
  new_virtual_keys_enabled_ = false;
  AdjustFont(new_game_button_);
  AdjustFont(pause_continue_button_);
  AdjustFont(settings_button_);
  AdjustFont(about_button_);
  for (int i = 0; i < virtual_keys_buttons_.size(); ++i) {
    AdjustFont(virtual_keys_buttons_[i]);
  }
  for (int i = 0; i < charge_buttons_.size(); ++i) {
    AdjustFont(charge_buttons_[i]);
  }
#endif

  SwitchVirtualButtonsLayout();

  if (QTouchDevice::devices().empty()) {
    ToggleVirtualKeys();
  }

  InitializeSettingsDialog();

  timer_duration_ = available_fps_options_[fps_option_].second;

  setMinimumSize(600, 450);
  resize(600, 450);

  music_playlist_.addMedia(QUrl("qrc:/sounds/backgroundmusic1.mp3"));
  music_playlist_.addMedia(QUrl("qrc:/sounds/backgroundmusic2.mp3"));
  music_playlist_.addMedia(QUrl("qrc:/sounds/backgroundmusic3.mp3"));
  music_playlist_.addMedia(QUrl("qrc:/sounds/backgroundmusic4.mp3"));
  music_player_.setPlaylist(&music_playlist_);
  music_player_.setVolume(50);
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event) {
  auto current_cell_x = (event->x() - map_->GetUpperLeftX())
      * map_->GetNumberOfCellsHorizontally() / map_->GetWidth();
  auto current_cell_y = (event->y() - map_->GetUpperLeftY()) *
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
  auto tank = std::dynamic_pointer_cast<Tank>(tanks_[0]);

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
      // Here and on number code is selected key code on cyrillic keyboard
      // layout.
    case 1062:[[fallthrough]];
    case Qt::Key_Up:[[fallthrough]];
    case Qt::Key_W: {
      tank->TurnReverseOff();
      tank->StartMovement(1, tanks_, &objects_copies_, &obstacles_and_bonuses_);
      break;
    }
    case 1067:[[fallthrough]];
    case Qt::Key_Down:[[fallthrough]];
    case Qt::Key_S: {
      tank->TurnReverseOn();
      tank->StartMovement(1, tanks_, &objects_copies_, &obstacles_and_bonuses_);
      break;
    }
    case 1060:[[fallthrough]];
    case Qt::Key_Left:[[fallthrough]];
    case Qt::Key_A: {
      tank->TurnRotationReverseOn();
      tank->StartRotation();
      break;
    }
    case 1042:[[fallthrough]];
    case Qt::Key_Right:[[fallthrough]];
    case Qt::Key_D: {
      tank->TurnRotationReverseOff();
      tank->StartRotation();
      break;
    }
    case 1049:[[fallthrough]];
    case Qt::Key_Q: {
      if (tank->IsAbleToShoot()) {
        tank->SetZeroTimeFromLastShot();
        ShootRocket(tank);
      }
      break;
    }
    case Qt::Key_1: {
      ChangeChargeButton(0);
      break;
    }
    case Qt::Key_2: {
      ChangeChargeButton(1);
      break;
    }
    case Qt::Key_3: {
      ChangeChargeButton(2);
      break;
    }
  }
}

void MainWindow::paintEvent(QPaintEvent*) {
  map_->UpdateCoordinates(w_indent_ + static_cast<int>(0.28 * sq_width_),
                          h_indent_ + static_cast<int>(0.05 * sq_height_),
                          static_cast<int>(0.68 * sq_width_),
                          static_cast<int>(0.9 * sq_height_));
  for (const auto& object : tanks_) {
    object->UpdateCoordinates(object->GetCellX(), object->GetCellY());
  }
  for (const auto& object : rockets_) {
    object->UpdateCoordinates(object->GetCellX(), object->GetCellY());
  }
  for (const auto& object : objects_copies_) {
    auto tank = std::dynamic_pointer_cast<Tank>(object.first);
    tank->UpdateCoordinates(object.second.x, object.second.y);
  }
  for (const auto& vector : obstacles_and_bonuses_) {
    for (const auto& object : vector) {
      if (object != nullptr) {
        object->UpdateCoordinates();
      }
    }
  }

  QPainter p(this);
  map_->DrawMap(&p);
  for (const auto& vector : obstacles_and_bonuses_) {
    for (const auto& object : vector) {
      if (object != nullptr) {
        object->Draw(&p);
      }
    }
  }
  for (const auto& object : objects_copies_) {
    object.first->Draw(&p);
    object.first->ReturnToOriginal();
  }
  for (const auto& object : tanks_) {
    object->Draw(&p);
  }
  for (const auto& object : rockets_) {
    object->Draw(&p);
  }

  if (!tanks_.empty() && charge_line_shown_) {
    auto tank = std::dynamic_pointer_cast<Tank>(tanks_[0]);
    p.setBrush(charge_colors_[static_cast<int>(tank->GetChargeState())]);
    p.drawRect(w_indent_ + static_cast<int>(0.04 * sq_width_),
               height() - h_indent_ - static_cast<int>(0.25 * sq_height_),
               static_cast<int>(0.2 * sq_width_ * tank->GetTimeSinceLastShot() /
                   tank->GetRateOfFire()),
               sq_height_ / 32);
  }

  p.end();
}

void MainWindow::resizeEvent(QResizeEvent*) {
  UpdateIndents();
  RedrawButtons();
  RedrawChargeButtons();
}

void MainWindow::timerEvent(QTimerEvent*) {
  time_since_last_medicalkit_ += timer_duration_;
  time_since_last_charge_ += timer_duration_;
  screen_timer_ms_ += timer_duration_;
  UpdateScreenTimer();

  for (auto& object : tanks_) {
    if (std::dynamic_pointer_cast<Bot>(object) != nullptr) {
      auto bot = std::dynamic_pointer_cast<Bot>(object);

      if (bot->IsShotNeeded(map_, std::dynamic_pointer_cast<Tank>(tanks_[0])) &&
          bot->IsAbleToShoot()) {
        auto tank = std::dynamic_pointer_cast<Tank>(object);
        bot->SetZeroTimeFromLastShot();
        ShootRocket(tank);
      }

      if (bot->IsMovingStartNeeded(tanks_, obstacles_and_bonuses_)) {
        bot->StartMovement(1, tanks_, &objects_copies_,
                           &obstacles_and_bonuses_);
      } else if (bot->IsRotationStartNeeded(
          std::dynamic_pointer_cast<Tank>(tanks_[0]))) {
        bot->StartRotation();
      }

      if (bot->IsTurnNeeded()) {
        bot->Rotate(timer_duration_);
      }
    }
  }

  for (const auto& object : tanks_) {
    if (object->GetTimeToFinishMovement() > 0) {
      object->Move(timer_duration_);
    } else if (object->GetTimeToFinishRotation() > 0) {
      object->Rotate(timer_duration_);
    }
  }

  for (const auto& object : rockets_) {
    if (object->GetTimeToFinishMovement() > 0) {
      object->Move(timer_duration_);
    }
  }

  auto copy = objects_copies_.begin();
  while (copy != objects_copies_.end()) {
    if (copy->first->GetTimeToFinishMovement() <= 0) {
      std::dynamic_pointer_cast<Tank>(copy->first)
          ->UpdateCoordinates(copy->second.x, copy->second.y);
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
          if (HaveObjectsCollided(*it, tank)) {
            std::dynamic_pointer_cast<Tank>(tank)->MinusHealth(25);
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
        GetTimerDuration());
  }

  if (time_since_last_medicalkit_ >= 20000) {
    RandomBonus(Bonus::TypeMedicalKit);
    time_since_last_medicalkit_ = 0;
  }
  if (time_since_last_charge_ >= 15000) {
    RandomBonus(Bonus::TypeCharge);
    time_since_last_charge_ = 0;
  }

  FindInteractingObjects();
  CheckDeadObjects();

  RedrawChargeButtons();
  repaint();
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
  ResetGame();
}

void MainWindow::Settings() {
  if (!paused_) PauseOrContinue();

#ifdef Q_OS_ANDROID
  settings_dialog_->showMaximized();
#endif

  settings_dialog_->exec();
  DetermineCurrentSettings();
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

void MainWindow::UpdateIndents() {
  sq_width_ = 4 * std::min(width() / 4, height() / 3);
  sq_height_ = 3 * std::min(width() / 4, height() / 3);

  w_indent_ = (width() - sq_width_) / 2;
  h_indent_ = (height() - sq_height_) / 2;
}

void MainWindow::RedrawButtons() {
  main_buttons_layout_->setSpacing(static_cast<int>(0.01 * sq_height_));
  main_buttons_layout_->setGeometry(QRect(
      w_indent_ + static_cast<int>(0.04 * sq_width_),
      h_indent_ + static_cast<int>(0.05 * sq_height_),
      static_cast<int>(0.2 * sq_width_),
      static_cast<int>(0.4 * sq_height_)));

  if (virtual_keys_shown_ && !new_virtual_keys_enabled_) {
    virtual_buttons_layout_->setSpacing(static_cast<int>(0.01 * sq_height_));
    virtual_buttons_layout_->setGeometry(
        QRect(w_indent_ + static_cast<int>(0.04 * sq_width_),
              height() - h_indent_ - static_cast<int>(0.2 * sq_height_),
              static_cast<int>(0.2 * sq_width_),
              static_cast<int>(0.15 * sq_height_)));
  } else if (virtual_keys_shown_ && new_virtual_keys_enabled_) {
    new_virtual_buttons_layout_left_->setSpacing(
        static_cast<int>(0.01 * sq_height_));
    new_virtual_buttons_layout_right_->setSpacing(
        static_cast<int>(0.01 * sq_height_));
    new_virtual_buttons_layout_left_->setGeometry(
        QRect(std::max(0, w_indent_ - static_cast<int>(0.2 * sq_width_)),
              height() - h_indent_ - static_cast<int>(0.5 * sq_height_),
              std::min(w_indent_, static_cast<int>(0.2 * sq_width_)),
              static_cast<int>(0.45 * sq_height_)));
    new_virtual_buttons_layout_right_->setGeometry(
        QRect(width() - w_indent_,
              height() - h_indent_ - static_cast<int>(0.5 * sq_height_),
              std::min(w_indent_, static_cast<int>(0.2 * sq_width_)),
              static_cast<int>(0.45 * sq_height_)));
  }
}

void MainWindow::RedrawChargeButtons() {
  if (tanks_.empty()) return;
  auto tank = std::dynamic_pointer_cast<Tank>(tanks_[0]);

  for (int i = 0; i < charge_buttons_.size(); ++i) {
    if (i == tank->GetTypeOfCharge()) {
      charge_buttons_[i]->setPalette(
          charge_palettes_[static_cast<int>(tank->GetChargeState())]);
    } else {
      charge_buttons_[i]->setPalette(standart_button_palette_);
    }
    charge_buttons_[i]->setText(QString::number(tank->GetCurrentCharge(i)));
  }

  charge_buttons_layout_->setSpacing(static_cast<int>(0.01 * sq_height_));
  charge_buttons_layout_->setGeometry(QRect(
      w_indent_ + static_cast<int>(0.04 * sq_width_),
      height() - h_indent_ - static_cast<int>(0.385 * sq_height_),
      static_cast<int>(0.2 * sq_width_),
      static_cast<int>(0.13 * sq_height_)));
}

void MainWindow::UpdateScreenTimer() {
  screen_timer_sec_ += screen_timer_ms_ / 1000;
  screen_timer_min_ += screen_timer_sec_ / 60;
  screen_timer_ms_ %= 1000;
  screen_timer_sec_ %= 60;

  if (screen_timer_min_ >= minutes_per_round_) {
    GameOver(false);
  } else if (screen_timer_min_ >= minutes_per_round_ - 2) {
    if (screen_timer_sec_ % 2 == 0) {
      screen_timer_->setPalette(red_lcdnumber_palette_);
    } else {
      screen_timer_->setPalette(standart_lcdnumber_palette_);
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

void MainWindow::AdjustFont(QWidget* widget) {
  QFont adjusted_font = widget->font();
  adjusted_font.setPixelSize(widget->height());
  widget->setFont(adjusted_font);
}

void MainWindow::ResetGame() {
  killTimer(timer_id_);
  timer_id_ = 0;

  music_player_.stop();

  screen_timer_ms_ = 0;
  screen_timer_sec_ = 0;
  screen_timer_min_ = 0;
  screen_timer_->setPalette(standart_lcdnumber_palette_);

  map_.reset(new Map(current_map_number_));
  tanks_.clear();
  rockets_.clear();
  objects_copies_.clear();

  pause_continue_button_->setText(tr("Pause"));
  paused_ = false;

  // TODO(anevero): странная логика, стоит поменять.
  types_of_tanks_[current_tank_number_].direction =
      DetermineDirection(map_->GetTankStartDirection());
  tanks_.append(std::make_shared<Tank>(
      map_, map_->GetTankInitCellX(), map_->GetTankInitCellY(),
      types_of_tanks_[current_tank_number_]));

  // TODO(anevero): тоже странная логика, что-то загружает класс Map, а что-то
  //  MainWindow.
  QJsonObject json = GetJsonObjectFromFile(
      ":/maps/map" + QString::number(current_map_number_ + 1) + ".json");

  QJsonArray bots = json["difficulty"]
      .toArray()[current_difficulty_level_].toObject()["bots"].toArray();

  for (auto bot : bots) {
    BotQualities qualities{};
    qualities.tank.max_health = 70 + 15 * current_difficulty_level_;
    qualities.tank.rate_of_fire = 1000 - 150 * current_difficulty_level_;
    qualities.tank.speed = 1000 - 150 * current_difficulty_level_;
    qualities.tank.max_light_charge = 1;
    qualities.tank.max_medium_charge = 1;
    qualities.tank.max_hard_charge = 1;
    qualities.init_cell_x = bot.toObject()["initial_cell_x"].toInt();
    qualities.init_cell_y = bot.toObject()["initial_cell_y"].toInt();
    qualities.moving_length = bot.toObject()["moving_length"].toInt();
    qualities.amount_of_turns = bot.toObject()["amount_of_turns"].toInt();
    qualities.side_rotation_frequency =
        bot.toObject()["side_rotation_frequency"].toInt();
    qualities.tank.direction =
        DetermineDirection(bot.toObject()["initial_direction"].toString());

    if (bot.toObject()["type"].toString() == "standart") {
      tanks_.append(std::make_shared<Bot>(map_, qualities));
    } else if (bot.toObject()["type"].toString() == "improved") {
      tanks_.append(std::make_shared<ImprovedBot>(map_, qualities));
    } else {
      tanks_.append(std::make_shared<CleverBot>(map_, qualities));
    }
  }

  obstacles_and_bonuses_ =
      std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>(
          map_->GetNumberOfCellsVertically(),
          std::vector<std::shared_ptr<ObjectOnMap>>(
              map_->GetNumberOfCellsHorizontally(), nullptr));

  QJsonArray obstacles = json["difficulty"]
      .toArray()[current_difficulty_level_].toObject()["obstacles"].toArray();

  for (auto obstacle : obstacles) {
    auto x = static_cast<uint32_t>(obstacle.toArray()[0].toInt());
    auto y = static_cast<uint32_t>(obstacle.toArray()[1].toInt());
    obstacles_and_bonuses_[x][y] = std::make_shared<Obstacle>(map_, x, y);
  }

  QJsonArray portals = json["difficulty"]
      .toArray()[current_difficulty_level_].toObject()["portals"].toArray();

  for (auto portal : portals) {
    auto curr_x = static_cast<uint32_t>(portal.toArray()[0].toInt());
    auto curr_y = static_cast<uint32_t>(portal.toArray()[1].toInt());
    auto new_x = static_cast<uint32_t>(portal.toArray()[2].toInt());
    auto new_y = static_cast<uint32_t>(portal.toArray()[3].toInt());
    obstacles_and_bonuses_[curr_x][curr_y] =
        std::make_shared<Portal>(map_, curr_x, curr_y, new_x, new_y);
    obstacles_and_bonuses_[new_x][new_y] =
        std::make_shared<Portal>(map_, new_x, new_y, curr_x, curr_y);
  }

  for (auto& charge_button : charge_buttons_) {
    charge_button->show();
  }

  timer_id_ = startTimer(timer_duration_);

  music_playlist_.setCurrentIndex(current_map_number_);
  music_playlist_.setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
  if (music_enabled_) {
    music_player_.play();
  }

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

void MainWindow::PressVirtualKey(Qt::Key key) {
  auto event = new QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier);
  QApplication::instance()->sendEvent(this, event);
}

void MainWindow::ChangeChargeButton(int type) {
  if (timer_id_ == 0 && !paused_) return;
  std::dynamic_pointer_cast<Tank>(tanks_[0])->ChangeTypeOfCharge(type);
  RedrawChargeButtons();
  repaint();
}

void MainWindow::LoadTanksTypesInfo() {
  QJsonObject json = GetJsonObjectFromFile(":/tanks/tanks.json");
  QJsonArray tanks = json["tanks"].toArray();

  for (auto&& tank : tanks) {
    auto tank_json_object = tank.toObject();
    types_of_tanks_.push_back({});
    types_of_tanks_.back().speed =
        tank_json_object["speed"].toInt();
    types_of_tanks_.back().rate_of_fire =
        tank_json_object["rate_of_fire"].toInt();
    types_of_tanks_.back().max_health =
        tank_json_object["max_health"].toInt();
    types_of_tanks_.back().max_light_charge =
        tank_json_object["max_light_charge"].toInt();
    types_of_tanks_.back().max_medium_charge =
        tank_json_object["max_medium_charge"].toInt();
    types_of_tanks_.back().max_hard_charge =
        tank_json_object["max_hard_charge"].toInt();
  }
}

void MainWindow::LoadRocketsTypesInfo() {
  // TODO(anevero): load this data from the file.
  types_of_rockets_ = {{7, 100, true}, {15, 200, true}, {30, 300, false}};
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
      if (HaveObjectsCollided(*rocket, *tank)) {
        std::dynamic_pointer_cast<Tank>(*tank)->MinusHealth(
            std::dynamic_pointer_cast<Rocket>(*rocket)->GetPower());
        rocket = rockets_.erase(rocket);
        continue;
      }
      ++rocket;
    }
    ++tank;
  }
}

bool MainWindow::HaveObjectsCollided(
    const std::shared_ptr<Movable>& obj1,
    const std::shared_ptr<Movable>& obj2) const {
  if (obj1 == obj2 || IsRocketByThisTank(obj1, obj2)) {
    return false;
  }

  return !(
      (obj1->GetUpperLeftX() >= obj2->GetUpperLeftX() + obj2->GetWidth()) ||
          (obj1->GetUpperLeftX() + obj1->GetWidth() <= obj2->GetUpperLeftX()) ||
          (obj1->GetUpperLeftY() >=
              obj2->GetUpperLeftY() + obj2->GetHeight()) ||
          (obj1->GetUpperLeftY() + obj1->GetHeight() <= obj2->GetUpperLeftY()));
}

void MainWindow::CheckDeadObjects() {
  if (std::dynamic_pointer_cast<Tank>(tanks_[0])->IsDead()) {
    GameOver(false);
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
    GameOver(true);
  }
}

void MainWindow::MakeBoom(const std::shared_ptr<Movable>& object) {
  auto boom = std::make_shared<Boom>(map_, object, 1000);
  rockets_.append(boom);
  boom->StartMovement(1, tanks_, &objects_copies_, &obstacles_and_bonuses_);
}

void MainWindow::ShootRocket(const std::shared_ptr<Tank>& tank) {
  std::shared_ptr<Rocket> rocket;
  if (std::dynamic_pointer_cast<Bot>(tank) == nullptr) {
    rocket = std::make_shared<Rocket>(
        map_, tank, types_of_rockets_[tank->GetTypeOfCharge()].speed_,
        types_of_rockets_[tank->GetTypeOfCharge()].power_,
        static_cast<TypeOfRocket>(tank->GetTypeOfCharge()));
    tank->MinusCharge(tank->GetTypeOfCharge());
    RedrawChargeButtons();
  } else {
    rocket = std::make_shared<Rocket>(map_, tank, 250, 10,
                                      TypeOfRocket::MediumRocket);
  }

  rockets_.append(rocket);
  if (rocket->GetIntDirection() == 1 || rocket->GetIntDirection() == 3) {
    rocket->StartMovement(map_->GetNumberOfCellsHorizontally(), tanks_,
                          &objects_copies_, &obstacles_and_bonuses_);
  } else {
    rocket->StartMovement(map_->GetNumberOfCellsVertically(), tanks_,
                          &objects_copies_, &obstacles_and_bonuses_);
  }
}

bool MainWindow::IsRocketByThisTank(
    const std::shared_ptr<Movable>& rocket,
    const std::shared_ptr<Movable>& tank) const {
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
  for (auto& button : virtual_keys_buttons_) {
    button->setVisible(virtual_keys_shown_);
  }
  RedrawButtons();
}

void MainWindow::SwitchVirtualButtonsLayout() {
  if (new_virtual_keys_enabled_) {
    new_virtual_buttons_layout_left_->removeWidget(virtual_keys_buttons_[1]);
    new_virtual_buttons_layout_left_->removeWidget(virtual_keys_buttons_[0]);
    new_virtual_buttons_layout_left_->removeWidget(virtual_keys_buttons_[3]);
    new_virtual_buttons_layout_right_->removeWidget(virtual_keys_buttons_[2]);
    new_virtual_buttons_layout_right_->removeWidget(virtual_keys_buttons_[4]);

    for (int i = 0; i < number_of_virtual_keys_in_first_row_; ++i) {
      virtual_buttons_layout_->addWidget(virtual_keys_buttons_[i], 0, i);
    }
    for (int i = number_of_virtual_keys_in_first_row_;
         i < virtual_keys_buttons_.size(); ++i) {
      virtual_buttons_layout_->addWidget(
          virtual_keys_buttons_[i], 1,
          i - number_of_virtual_keys_in_first_row_);
    }
    new_virtual_keys_enabled_ = false;
  } else {
    for (auto& button : virtual_keys_buttons_) {
      virtual_buttons_layout_->removeWidget(button);
    }

    new_virtual_buttons_layout_left_->addWidget(virtual_keys_buttons_[1]);
    new_virtual_buttons_layout_left_->addWidget(virtual_keys_buttons_[0]);
    new_virtual_buttons_layout_left_->addWidget(virtual_keys_buttons_[3]);
    new_virtual_buttons_layout_right_->addWidget(virtual_keys_buttons_[2]);
    new_virtual_buttons_layout_right_->addWidget(virtual_keys_buttons_[4]);
    new_virtual_keys_enabled_ = true;
  }
  RedrawButtons();
}

void MainWindow::ToggleMusic() {
  music_enabled_ = !music_enabled_;
  if (!music_enabled_) {
    music_player_.stop();
  } else if (music_enabled_ && timer_id_ == 0 && paused_) {
    music_player_.play();
  }
}

void MainWindow::ChangeFPSOption(const int new_option, bool start_timer) {
  fps_option_ = new_option;
  timer_duration_ = available_fps_options_[new_option].second;
  fps_menu_->setCurrentIndex(new_option);
  if (timer_id_ != 0) {
    killTimer(timer_id_);
  }
  if (start_timer) {
    timer_id_ = startTimer(timer_duration_);
  }
}

void MainWindow::GameOver(bool win) {
  music_player_.stop();

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

void MainWindow::InitializeSettingsDialog() {
  settings_dialog_ = new QDialog(this);

  virtual_keys_checkbox_ = new QCheckBox(tr("Virtual keys"), settings_dialog_);
  virtual_keys_checkbox_->setChecked(virtual_keys_shown_);

  new_virtual_keys_checkbox_ = new QCheckBox(
      tr("Experimental layout of virtual keys"), settings_dialog_);
  new_virtual_keys_checkbox_->setChecked(new_virtual_keys_enabled_);

  charge_line_checkbox_ =
      new QCheckBox(tr("Activate charge line"), settings_dialog_);

  music_checkbox_ = new QCheckBox(tr("Music"), settings_dialog_);

  fps_menu_label_ =
      new QLabel(QString(tr("Performance")) + QString(":"), settings_dialog_);

  fps_menu_ = new QComboBox(settings_dialog_);
  for (const auto& option : available_fps_options_) {
    fps_menu_->addItem(option.first + QString(" ") +
        QString(tr("frames per second")));
  }

  language_menu_label_ =
      new QLabel(QString(tr("Language")) + QString(":"), settings_dialog_);

  language_menu_ = new QComboBox(settings_dialog_);
  language_menu_->addItem(tr("Belarusian"));
  language_menu_->addItem(tr("English"));
  language_menu_->addItem(tr("Russian"));

  language_menu_restart_label_ = new QLabel(
      QString(tr("Language will be changed after application restart")));
  language_menu_restart_label_->setWordWrap(true);

  DetermineCurrentSettings();

  settings_dialog_layout_ = new QVBoxLayout(settings_dialog_);
  settings_dialog_layout_->addWidget(virtual_keys_checkbox_);
  settings_dialog_layout_->addWidget(new_virtual_keys_checkbox_);
  settings_dialog_layout_->addWidget(charge_line_checkbox_);
  settings_dialog_layout_->addWidget(music_checkbox_);
  settings_dialog_layout_->addWidget(fps_menu_label_);
  settings_dialog_layout_->addWidget(fps_menu_);
  settings_dialog_layout_->addWidget(language_menu_label_);
  settings_dialog_layout_->addWidget(language_menu_);
  settings_dialog_layout_->addWidget(language_menu_restart_label_);

#ifndef Q_OS_ANDROID
  new_virtual_keys_checkbox_->setDisabled(true);
#endif

  settings_dialog_buttons_ = new QDialogButtonBox(
      QDialogButtonBox::Ok, Qt::Horizontal, settings_dialog_);

  settings_dialog_layout_->addWidget(settings_dialog_buttons_);

  connect(settings_dialog_buttons_->button(QDialogButtonBox::Ok),
          &QPushButton::clicked, [&]() {
        ChangeCurrentSettings();
        DetermineCurrentSettings();
      });

  connect(settings_dialog_buttons_, SIGNAL(accepted()), settings_dialog_,
          SLOT(accept()));

  settings_dialog_->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void MainWindow::DetermineCurrentSettings() {
  QJsonObject json = GetJsonObjectFromFile("settings.json");

  virtual_keys_checkbox_->setChecked(virtual_keys_shown_);
  new_virtual_keys_checkbox_->setChecked(new_virtual_keys_enabled_);

  QString language = json["language"].toString();
  if (language == "be_BY") {
    language_menu_->setCurrentIndex(0);
  } else if (language == "en_US") {
    language_menu_->setCurrentIndex(1);
  } else if (language == "ru_RU") {
    language_menu_->setCurrentIndex(2);
  }

  charge_line_shown_ = json["charge_line"].toBool();
  charge_line_checkbox_->setChecked(charge_line_shown_);

  music_enabled_ = json["music_enabled"].toBool();
  music_checkbox_->setChecked(music_enabled_);

  fps_option_ = json["fps"].toInt();
  fps_menu_->setCurrentIndex(fps_option_);
}

void MainWindow::ChangeCurrentSettings() {
  if (virtual_keys_shown_ != virtual_keys_checkbox_->isChecked()) {
    ToggleVirtualKeys();
  }
  if (new_virtual_keys_enabled_ != new_virtual_keys_checkbox_->isChecked()) {
    SwitchVirtualButtonsLayout();
  }
  if (music_enabled_ != music_checkbox_->isChecked()) {
    ToggleMusic();
  }

  charge_line_shown_ = charge_line_checkbox_->isChecked();
  ChangeFPSOption(fps_menu_->currentIndex());

  QString language;
  switch (language_menu_->currentIndex()) {
    case 0:language = "be_BY";
      break;
    case 1:language = "en_US";
      break;
    case 2:language = "ru_RU";
      break;
  }

  QFile settings_file("settings.json");
  QJsonObject new_json_obj;

  new_json_obj["language"] = language;
  new_json_obj["charge_line"] = charge_line_shown_;
  new_json_obj["music_enabled"] = music_enabled_;
  new_json_obj["fps"] = fps_option_;

  QJsonDocument new_json_document(new_json_obj);
  QString new_json_string = new_json_document.toJson();

  settings_file.open(QIODevice::WriteOnly);
  settings_file.write(new_json_string.toUtf8());
  settings_file.close();

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

Direction MainWindow::DetermineDirection(const QString& start_direction) const {
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

void MainWindow::RandomBonus(Bonus bonus) {
  for (size_t i = 0; i < obstacles_and_bonuses_.size(); i++) {
    for (size_t j = 0; j < obstacles_and_bonuses_[i].size(); j++) {
      if (bonus == Bonus::TypeMedicalKit) {
        if (std::dynamic_pointer_cast<MedicalKit>(
            obstacles_and_bonuses_[i][j]) != nullptr) {
          obstacles_and_bonuses_[i][j] = nullptr;
        }
      } else if (bonus == Bonus::TypeCharge) {
        if (std::dynamic_pointer_cast<Charge>(obstacles_and_bonuses_[i][j]) !=
            nullptr) {
          obstacles_and_bonuses_[i][j] = nullptr;
        }
      }
    }
  }

  int temp = 100;
  bool flag = true;
  while (temp > 0) {
    auto x = qrand() % (map_->GetNumberOfCellsHorizontally() - 1);
    auto y = qrand() % (map_->GetNumberOfCellsVertically() - 1);
    for (auto& object : tanks_) {
      if (object->GetCellX() == x && object->GetCellY() == y) {
        flag = false;
        break;
      }
    }
    if (obstacles_and_bonuses_[x][y] == nullptr &&
        flag && map_->GetField(x, y) != CellType::Wall) {
      if (bonus == Bonus::TypeMedicalKit) {
        obstacles_and_bonuses_[x][y] =
            std::make_shared<MedicalKit>(map_, x, y);
      } else if (bonus == Bonus::TypeCharge) {
        obstacles_and_bonuses_[x][y] =
            std::make_shared<Charge>(map_, x, y);
      }
      return;
    }
    --temp;
  }
}
