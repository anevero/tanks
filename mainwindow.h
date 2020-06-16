#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QEvent>
#include <QFileInfo>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLCDNumber>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMessageBox>
#include <QPainter>
#include <QPalette>
#include <QPushButton>
#include <QSize>
#include <QString>
#include <QTextBrowser>
#include <QTimer>
#include <QToolTip>
#include <QTouchDevice>
#include <QTranslator>
#include <QUrl>
#include <QVector>
#include <QtGlobal>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "boom.h"
#include "bot.h"
#include "cleverbot.h"
#include "improvedbot.h"
#include "map.h"
#include "objectonmap.h"
#include "portal.h"
#include "rocket.h"
#include "tank.h"

struct GameOptions {
  int map_number;
  int tank_number;
  int difficulty_level_number;
};

class MainWindow : public QMainWindow {
 Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow() override = default;

 private:
  [[maybe_unused]] void mouseReleaseEvent(QMouseEvent* event) override;
  [[maybe_unused]] void keyReleaseEvent(QKeyEvent* event) override;
  [[maybe_unused]] void paintEvent(QPaintEvent*) override;
  [[maybe_unused]] void resizeEvent(QResizeEvent*) override;
  [[maybe_unused]] void timerEvent(QTimerEvent*) override;

 private slots:
  // Runs new game dialog and switches options inside it.
  void NewGame();
  // Pauses or continues the game (depending on its current state).
  void PauseOrContinue();
  // Runs settings dialog and switches options inside it.
  void Settings();
  // Runs about dialog.
  void About();

  // Creates KeyEvent with specified key. It's used for virtual keys buttons.
  void PressVirtualKey(Qt::Key key);
  // Switches charge button on the screen and type of charge of the player's
  // tank.
  void ChangeChargeButton(int type);

 private:
  // Clears all the data connected with the current round and initializes
  // objects with values necessary to start the new round. After that repaints
  // everything on the screen.
  void RedrawContent();
  // Updates info about size of indents in the current window.
  void UpdateIndents();
  // Changes buttons coordinates in the window according to the window size.
  void RedrawButtons();
  // Updates charge buttons color according to the state of charge of the
  // player's tank.
  void RedrawChargeButtons();
  void UpdateScreenTimer();
  // Workaround for the bug with not-adjusting fonts on buttons (Android).
  void AdjustFont(QWidget* widget);

  // Checks if rockets are interacting with the tanks at the moment and
  // destroys rockets / reduces tanks' health.
  void FindInteractingObjects();

  // Checks if given objects have been collided, i.e. they have common points
  // on the screen. Compares its coordinates for that.
  bool HaveObjectsCollided(const std::shared_ptr<Movable>& obj1,
                           const std::shared_ptr<Movable>& obj2) const;
  // Checks if given rocket has been released by the given tank. The result of
  // this function is used when we decide if we need to reduce tank's health
  // (rockets released by the tank can't reduce its own health).
  bool IsRocketByThisTank(const std::shared_ptr<Movable>& rocket,
                          const std::shared_ptr<Movable>& tank) const;
  // Checks if some objects are 'dead', i.e. searches for the tanks with 0
  // health, rockets, which need to be destroyed. Erases such objects.
  // If player's tank is 'dead', calls GameOver().
  void CheckDeadObjects();
  // Runs game over dialog with the appropriate content (i.e 'you win' or
  // 'you lose').
  void GameOver(bool win);

  // Creates a rocket which begins to move from the given tank. Rocket's
  // type is defined by the tank current type of charge. Rocket's geometry on
  // the map is defined by tank's geometry.
  void ShootRocket(const std::shared_ptr<Tank>& tank);
  // Creates and initializes explosion. Its geometry is defined by the
  // given object.
  void MakeBoom(const std::shared_ptr<Movable>& object);
  // Generates bonus of the given type. Erases old bonuses of such type from
  // the map.
  void RandomBonus(Bonus bonus);

  int GetTimerDuration() const;
  void ToggleVirtualKeys();
  void SwitchVirtualButtonsLayout();
  void ToggleMusic();
  void ChangeFPSOption(int new_option, bool start_timer = false);
  // Initializes new game dialog with buttons and menus. Runs once (!) in the
  // constructor of main window.
  void InitializeNewGameDialog();
  // Initializes settings dialog with buttons and menus. Runs once (!) in the
  // constructor of main window.
  void InitializeSettingsDialog();
  // Initializes About dialog with necessary content. Runs once (!) in the
  // constructor of main window.
  void InitializeAboutDialog();
  // Updates variables which store settings state with the information from
  // settings file.
  void DetermineCurrentSettings();
  // Calls functions to change game settings according to variables state and
  // writes new info about settings to the settings file.
  void ChangeCurrentSettings();
  QJsonObject GetJsonObjectFromFile(const QString& filepath);
  // Returns direction object corresponding to the string.
  Direction DetermineDirection(
      const QString& start_direction) const;

 private:
  bool paused_ = false;
  bool virtual_keys_shown_ = true;
  bool new_virtual_keys_enabled_ = true;
  bool charge_line_shown_;
  bool music_enabled_;
  int fps_option_;
  int timer_duration_;

  GameOptions current_game_options_{0, 0, 0};
  const int minutes_per_round_ = 10;

  QLCDNumber* screen_timer_;
  QPalette standart_lcdnumber_palette_;
  QPalette red_lcdnumber_palette_;
  int screen_timer_ms_ = 0;
  int screen_timer_sec_ = 0;
  int screen_timer_min_ = 0;

  QDialog* new_game_dialog_;
  QDialogButtonBox* new_game_dialog_buttons_;
  QVBoxLayout* new_game_dialog_layout_;
  QComboBox* switch_map_menu_;
  QComboBox* switch_tank_menu_;
  QComboBox* switch_difficulty_menu_;
  QLabel* new_game_info_label_;
  QLabel* switch_map_label_;
  QLabel* switch_tank_label_;
  QLabel* switch_difficulty_label_;

  QDialog* settings_dialog_;
  QDialogButtonBox* settings_dialog_buttons_;
  QVBoxLayout* settings_dialog_layout_;
  QCheckBox* virtual_keys_checkbox_;
  QCheckBox* new_virtual_keys_checkbox_;
  QCheckBox* charge_line_checkbox_;
  QCheckBox* music_checkbox_;
  QComboBox* fps_menu_;
  QComboBox* language_menu_;
  QLabel* fps_menu_label_;
  QLabel* language_menu_label_;
  QLabel* language_menu_restart_label_;

  QDialog* about_dialog_;
  QDialogButtonBox* about_dialog_buttons_;
  QVBoxLayout* about_dialog_layout_;
  QTextBrowser* html_widget_;
  QLabel* how_to_scroll_label_;

  QVBoxLayout* main_buttons_layout_;
  QPushButton* new_game_button_;
  QPushButton* pause_continue_button_;
  QPushButton* settings_button_;
  QPushButton* about_button_;

  QHBoxLayout* charge_buttons_layout_;
  QVector<QPushButton*> charge_buttons_;
  QPalette standart_button_palette_;
  QVector<Qt::GlobalColor> charge_colors_{Qt::red, Qt::yellow, Qt::green};
  QVector<QPalette> charge_palettes_;

  QGridLayout* virtual_buttons_layout_;
  QVector<QPushButton*> virtual_keys_buttons_;
  QVector<Qt::Key> virtual_keys_encodings_;
  const int number_of_virtual_keys_in_first_row_ = 2;
  const int number_of_virtual_keys_in_second_row_ = 3;

  QVBoxLayout* new_virtual_buttons_layout_left_;
  QHBoxLayout* new_virtual_buttons_layout_right_;

  QMediaPlayer music_player_;
  QMediaPlaylist music_playlist_;

  std::shared_ptr<Map> map_;
  // List of all the tanks (including player's tank and all the types of bots).
  // The player's tank, if exists, is always the first item in the list.
  QList<std::shared_ptr<Movable>> tanks_;
  // List of rockets and booms.
  QList<std::shared_ptr<Movable>> rockets_;
  std::vector<std::vector<std::shared_ptr<ObjectOnMap>>> obstacles_and_bonuses_;
  // List of 'copies' of tanks. Every item is just a pointer to the real tank
  // and coordinates of the copy. It's used while a tank is moving through the
  // portal (while it must be shown in two places at the same time).
  QList<QPair<std::shared_ptr<Movable>, Coordinates>> objects_copies_;

  const int number_of_player_tanks_ = 1;
  const QVector<QString> difficulty_levels_names_ = {tr("Easy"), tr("Normal"),
                                                     tr("Hard")};
  const QVector<QPair<QString, int>> available_fps_options_ = {
      {"240", 4}, {"120", 8}, {"90", 11}, {"60", 17}, {"50", 20}, {"40", 25}};
  QVector<TankQualities> available_tank_types_;
  const QVector<RocketParameters> types_of_rockets_;

  int timer_id_ = 0;
  int time_since_last_medicalkit_ = 0;
  int time_since_last_charge_ = 0;
  int sq_width_;
  int sq_height_;
  int w_indent_;
  int h_indent_;
};

#endif  // MAINWINDOW_H_
