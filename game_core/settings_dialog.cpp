#include "settings_dialog.h"

#include "constants.h"

#include <QApplication>
#include <QMessageBox>
#include <QSettings>

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent),
      layout_(new QVBoxLayout(this)),
      virtual_keys_checkbox_(new QCheckBox(tr("Virtual keys"), this)),
      mobile_virtual_keys_checkbox_(new QCheckBox(tr(
          "Mobile virtual keys layout"), this)),
      charge_line_checkbox_(new QCheckBox(tr("Charge line"), this)),
      music_checkbox_(new QCheckBox(tr("Music"), this)),
      fps_menu_label_(new QLabel(tr("Performance"), this)),
      switch_fps_menu_(new QComboBox(this)),
      language_menu_label_(new QLabel(tr("Language"), this)),
      switch_language_menu_(new QComboBox(this)),
      buttons_(new QDialogButtonBox(
          QDialogButtonBox::Ok, Qt::Horizontal, this)) {
  for (int i = 0; i < constants::kFpsOptions.size(); ++i) {
    switch_fps_menu_->addItem(tr("%1 frames per second").arg(
        constants::kFpsOptions[i]));
  }

  for (int i = 0; i < constants::kLanguages.size(); ++i) {
    switch_language_menu_->addItem(constants::kLanguagesNames[i]);
  }

  {
    using namespace constants;
    QSettings settings;

    virtual_keys_enabled_ = settings.value(kVirtualKeysEnabledKey).toBool();
    mobile_virtual_keys_style_enabled_ = settings.value(
        kMobileVirtualKeysStyleEnabledKey).toBool();
    charge_line_enabled_ = settings.value(kChargeLineEnabledKey).toBool();
    music_enabled_ = settings.value(kMusicEnabledKey).toBool();
    current_fps_option_ = settings.value(kCurrentFpsOptionKey).toInt();

    auto language_string = settings.value(kLanguageKey).toString();
    current_language_ = std::find(
        kLanguages.begin(), kLanguages.end(), language_string)
        - kLanguages.begin();
  }

  ResetSettings();

  layout_->addWidget(virtual_keys_checkbox_);
  layout_->addWidget(mobile_virtual_keys_checkbox_);
  layout_->addWidget(charge_line_checkbox_);
  layout_->addWidget(music_checkbox_);
  layout_->addWidget(fps_menu_label_);
  layout_->addWidget(switch_fps_menu_);
  layout_->addWidget(language_menu_label_);
  layout_->addWidget(switch_language_menu_);
  layout_->addWidget(buttons_);

  connect(virtual_keys_checkbox_, &QCheckBox::stateChanged, [this] {
    mobile_virtual_keys_checkbox_->setEnabled(
        virtual_keys_checkbox_->isChecked());
  });
  connect(buttons_, &QDialogButtonBox::accepted,
          this, &SettingsDialog::accept);
  connect(buttons_, &QDialogButtonBox::rejected,
          this, &SettingsDialog::reject);
}

void SettingsDialog::accept() {
  virtual_keys_enabled_ = virtual_keys_checkbox_->isChecked();
  mobile_virtual_keys_style_enabled_ =
      mobile_virtual_keys_checkbox_->isChecked();
  charge_line_enabled_ = charge_line_checkbox_->isChecked();
  music_enabled_ = music_checkbox_->isChecked();
  current_fps_option_ = switch_fps_menu_->currentIndex();

  WriteSettings();

  if (current_language_ != switch_language_menu_->currentIndex()) {
    SwitchLanguage();
  }

  return QDialog::accept();
}

void SettingsDialog::reject() {
  ResetSettings();
  return QDialog::reject();
}

bool SettingsDialog::AreVirtualKeysEnabled() const {
  return virtual_keys_enabled_;
}

bool SettingsDialog::IsMobileVirtualKeysStyleEnabled() const {
  return mobile_virtual_keys_style_enabled_;
}

bool SettingsDialog::IsChargeLineEnabled() const {
  return charge_line_enabled_;
}

bool SettingsDialog::IsMusicEnabled() const {
  return music_enabled_;
}

int SettingsDialog::GetCurrentFpsOption() const {
  return current_fps_option_;
}

void SettingsDialog::ResetSettings() {
  virtual_keys_checkbox_->setChecked(virtual_keys_enabled_);
  mobile_virtual_keys_checkbox_->setChecked(mobile_virtual_keys_style_enabled_);
  charge_line_checkbox_->setChecked(charge_line_enabled_);
  music_checkbox_->setChecked(music_enabled_);
  switch_fps_menu_->setCurrentIndex(current_fps_option_);
  switch_language_menu_->setCurrentIndex(current_language_);

  mobile_virtual_keys_checkbox_->setEnabled(
      virtual_keys_checkbox_->isChecked());
}

void SettingsDialog::WriteSettings() {
  QSettings settings;
  using namespace constants;

  settings.setValue(kVirtualKeysEnabledKey, virtual_keys_enabled_);
  settings.setValue(kMobileVirtualKeysStyleEnabledKey,
                    mobile_virtual_keys_style_enabled_);
  settings.setValue(kChargeLineEnabledKey, charge_line_enabled_);
  settings.setValue(kMusicEnabledKey, music_enabled_);
  settings.setValue(kCurrentFpsOptionKey, current_fps_option_);
}

void SettingsDialog::SwitchLanguage() {
  QMessageBox::StandardButton response;
  QString message = tr(
      "The app needs to be restarted to change the language.");

#ifdef Q_OS_ANDROID
  response = QMessageBox::question(this, tr("Close application?"), message);
#else
  response = QMessageBox::question(this, tr("Restart application?"), message);
#endif

  if (response != QMessageBox::Yes) {
    switch_language_menu_->setCurrentIndex(current_language_);
    return;
  }

  QSettings settings;
  settings.setValue(
      constants::kLanguageKey,
      constants::kLanguages[switch_language_menu_->currentIndex()]);

  QCoreApplication::exit(constants::kApplicationRestartCode);
}
