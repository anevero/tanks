#ifndef GAME_CORE_SETTINGS_DIALOG_H_
#define GAME_CORE_SETTINGS_DIALOG_H_

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QString>
#include <QVBoxLayout>
#include <vector>

class SettingsDialog : public QDialog {
  Q_OBJECT

 public:
  explicit SettingsDialog(QWidget* parent = nullptr);
  ~SettingsDialog() override = default;

  void accept() override;
  void reject() override;

  bool AreVirtualKeysEnabled() const;
  bool IsMobileVirtualKeysStyleEnabled() const;
  bool IsChargeLineEnabled() const;
  bool IsMusicEnabled() const;
  int GetCurrentFpsOption() const;

 private:
  void ResetSettings();
  void WriteSettings();
  void SwitchLanguage();

 private:
  bool virtual_keys_enabled_ = false;
  bool mobile_virtual_keys_style_enabled_ = false;
  bool charge_line_enabled_ = false;
  bool music_enabled_ = false;
  int current_fps_option_ = 0;
  int current_language_ = 0;

  QVBoxLayout* layout_;
  QCheckBox* virtual_keys_checkbox_;
  QCheckBox* mobile_virtual_keys_checkbox_;
  QCheckBox* charge_line_checkbox_;
  QCheckBox* music_checkbox_;
  QLabel* fps_menu_label_;
  QComboBox* switch_fps_menu_;
  QLabel* language_menu_label_;
  QComboBox* switch_language_menu_;
  QDialogButtonBox* buttons_;

  const std::vector<QString> kLanguagesNames =
      {tr("en_US"), tr("ru_RU"), tr("be_BY")};
};

#endif  // GAME_CORE_SETTINGS_DIALOG_H_
