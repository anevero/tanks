#include <QApplication>
#include <QLibraryInfo>
#include <QProcess>
#include <QSettings>
#include <QStyleFactory>
#include <QTranslator>

#include "game_core/constants.h"
#include "game_core/mainwindow.h"

int main(int argc, char* argv[]) {
#ifdef Q_OS_ANDROID
  // Workaround for the bug with random selection controls appearing on
  // the screen (Android).
  qputenv("QT_QPA_NO_TEXT_HANDLES", "1");
#endif

  QApplication a(argc, argv);
  QApplication::setStyle(QStyleFactory::create("Fusion"));
  QApplication::setOrganizationName(constants::kOrganizationName);
  QApplication::setApplicationName(constants::kApplicationName);

  // Loading language settings. If no settings found, the en_US locale is
  // set as the default.
  QSettings settings;
  QString language;
  if (settings.contains(constants::kLanguageKey)) {
    language = settings.value(constants::kLanguageKey, language).toString();
  } else {
    language = "en_US";
    settings.setValue(constants::kLanguageKey, language);
  }

  // Loading app translations.
  QTranslator app_translator;
  app_translator.load(":/translations/tanks_" + language);
  QApplication::installTranslator(&app_translator);

  // Loading Qt components translations (QMessageBoxes, etc.).
  QTranslator internal_translator;
  internal_translator.load(
      "qtbase_" + language,
      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  QApplication::installTranslator(&internal_translator);

  MainWindow main_window;
  main_window.setWindowTitle("Tanks");

#ifdef Q_OS_ANDROID
  main_window.showFullScreen();
#else
  main_window.show();
#endif

  int return_code = QApplication::exec();

#ifdef Q_OS_ANDROID
  return return_code;
#endif

  if (return_code != constants::kApplicationRestartCode) {
    return return_code;
  }

  return !QProcess::startDetached(QApplication::applicationFilePath(), {});
}
