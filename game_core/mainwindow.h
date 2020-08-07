#ifndef GAME_CORE_MAINWINDOW_H_
#define GAME_CORE_MAINWINDOW_H_

#include <QDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLCDNumber>
#include <QLabel>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMessageBox>
#include <QPainter>
#include <QPalette>
#include <QPushButton>
#include <QSettings>
#include <QString>
#include <QTimer>
#include <QToolTip>
#include <QTranslator>
#include <QUrl>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <list>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "game_core/about_dialog.h"
#include "game_core/constants.h"
#include "game_core/map.h"
#include "game_core/new_game_dialog.h"
#include "game_core/settings_dialog.h"
#include "static_objects/static_object.h"
#include "static_objects/portal.h"
#include "movable_objects/boom.h"
#include "movable_objects/bot.h"
#include "movable_objects/clever_bot.h"
#include "movable_objects/improved_bot.h"
#include "movable_objects/rocket.h"
#include "movable_objects/tank.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow() override;

 private:
  [[maybe_unused]] void mouseReleaseEvent(QMouseEvent* event) override;
  [[maybe_unused]] void keyReleaseEvent(QKeyEvent* event) override;
  [[maybe_unused]] void paintEvent(QPaintEvent*) override;
  [[maybe_unused]] void resizeEvent(QResizeEvent*) override;
  [[maybe_unused]] void timerEvent(QTimerEvent*) override;

 private:
  void PauseOrContinue();
  void ExecNewGameDialog();
  void ExecSettingsDialog();
  void ExecAboutDialog();

 private:
  void LoadApplicationSettings();
  void LoadTanksTypesInfo();
  void LoadRocketsTypesInfo();

  void SetVirtualKeysEnabled(bool enabled);
  void SetMobileVirtualKeysStyleEnabled(bool enabled);
  void SetMusicEnabled(bool enabled);
  void SetFpsOption(int index, bool start_timer = false);

 private:
  // Clears all the data connected with the current round and initializes
  // objects with values necessary to start the new round.
  void LoadRoundInfo();
  // Resets the state of all Qt objects to start the new round, calls
  // LoadRoundInfo for resetting state of game objects, and then starts a new
  // round.
  void StartRound();
  // Runs game over dialog with the appropriate content (i.e 'you win' or
  // 'you lose').
  void FinishRound(bool win);

 private:
  // Updates info about size of indents in the current window.
  void UpdateIndents();
  // Changes buttons coordinates in the window according to the window size.
  void RedrawButtons();
  // Scales buttons fonts (it is not scaled automatically).
  void SetButtonsFontPixelSize(int pixel_size);
  // Updates charge buttons color according to the state of charge of the
  // player's tank.
  void RedrawChargeButtons();
  // Updates screen timer according to the current time.
  void UpdateScreenTimer();

 private:
  // Checks if rockets are interacting with the tanks at the moment and
  // destroys rockets / reduces tanks' health.
  void FindInteractingObjects();
  // Checks if some objects are 'dead', i.e. searches for the tanks with 0
  // health, rockets, which need to be destroyed. Erases such objects.
  // If player's tank is 'dead', calls FinishRound().
  void CheckDeadObjects();
  // Creates a rocket which begins to move from the given tank. Rocket's
  // type is defined by the tank current type of charge. Rocket's geometry on
  // the map is defined by tank's geometry.
  void ShootRocket(const std::shared_ptr<Tank>& tank);
  // Creates and initializes explosion. Its geometry is defined by the
  // given object.
  void MakeBoom(const std::shared_ptr<Movable>& object);
  // Generates bonus of the given type. Erases old bonuses of such type from
  // the map.
  template<class Bonus>
  void RandomBonus();
  // Switches charge button on the screen and type of charge of the player's
  // tank.
  void SwitchCharge(int type);

 private:
  static QJsonObject GetJsonObjectFromFile(const QString& filepath);

 private:
  int current_map_number_ = 0;
  int current_tank_number_ = 0;
  int current_difficulty_level_ = 0;

  std::vector<TankParameters> tanks_types_ = {};
  std::vector<RocketParameters> rockets_types_ = {};

 private:
  std::shared_ptr<Map> map_;
  // List of all the tanks (including player's tank and all the types of bots).
  // The player's tank, if exists, is always the first item in the list.
  std::list<std::shared_ptr<Tank>> tanks_ = {};
  std::list<std::shared_ptr<Rocket>> rockets_ = {};
  std::list<std::shared_ptr<Boom>> booms_ = {};
  std::vector<std::vector<std::shared_ptr<StaticObject>>>
      obstacles_and_bonuses_ = {};
  // List of 'copies' of tanks. Every item is just a pointer to the real tank
  // and coordinates of the copy. It's used while a tank is moving through the
  // portal (while it must be shown in two places at the same time).
  std::list<std::pair<std::shared_ptr<Tank>, Coordinates>>
      objects_copies_ = {};

  int timer_id_ = 0;
  int time_since_last_medical_kit_ = 0;
  int time_since_last_charge_ = 0;
  int timer_duration_;
  bool paused_ = false;

 private:
  bool virtual_keys_enabled_;
  bool mobile_virtual_keys_style_enabled_;
  bool charge_line_enabled_;
  bool music_enabled_;
  int current_fps_option_;

 private:
  QLCDNumber* screen_timer_;
  QPalette standard_lcdnumber_palette_;
  QPalette red_lcdnumber_palette_;
  int screen_timer_ms_ = 0;
  int screen_timer_sec_ = 0;
  int screen_timer_min_ = 0;

  NewGameDialog* new_game_dialog_;
  SettingsDialog* settings_dialog_;
  AboutDialog* about_dialog_;

  QVBoxLayout* main_buttons_layout_;
  QPushButton* new_game_button_;
  QPushButton* pause_continue_button_;
  QPushButton* settings_button_;
  QPushButton* about_button_;

  QHBoxLayout* charge_buttons_layout_;
  std::vector<QPushButton*> charge_buttons_;
  QPalette standard_button_palette_;
  const std::vector<Qt::GlobalColor> charge_colors_ =
      {Qt::red, Qt::yellow, Qt::green};
  std::vector<QPalette> charge_palettes_;

  QGridLayout* virtual_buttons_layout_;
  std::vector<QPushButton*> virtual_keys_buttons_;

  const std::vector<Qt::Key> virtual_keys_encodings_ =
      {Qt::Key_Q, Qt::Key_W, Qt::Key_A, Qt::Key_S, Qt::Key_D};
  const int number_of_virtual_keys_in_first_row_ = 2;

  QVBoxLayout* mobile_virtual_buttons_layout_left_;
  QHBoxLayout* mobile_virtual_buttons_layout_right_;

  QMediaPlayer* music_player_;
  QMediaPlaylist* music_playlist_;

  int view_width_;
  int view_height_;
  int width_indent_;
  int height_indent_;

  static std::mt19937 random_generator_;
};

#endif  // GAME_CORE_MAINWINDOW_H_
