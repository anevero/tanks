#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStyleFactory>
#include <QTranslator>
#include "mainwindow.h"

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  QApplication::setStyle(QStyleFactory::create("Fusion"));

  QString language{};
  bool charge_line = true;
  bool music_enabled = true;
  int fps_option;

#ifdef Q_OS_ANDROID
  // 60 FPS is enough for mobile devices.
  fps_option = 3;
#else
  // Desktop devices can deal with more FPS.
  fps_option = 1;
#endif

  QFile settings_file("settings.json");
  // Checks whether settings file has been already created. If not, it will be
  // created later.
  if (settings_file.exists()) {
    settings_file.open(QIODevice::ReadOnly);
    QString text = settings_file.readAll();
    settings_file.close();
    QJsonDocument json_document(QJsonDocument::fromJson(text.toUtf8()));
    QJsonObject json = json_document.object();
    language = json["language"].toString();
    charge_line = json["charge_line"].toBool();
    music_enabled = json["music_enabled"].toBool();
    fps_option = json["fps"].toInt();
  }

  QTranslator translator;

  if (language.isEmpty()) {
    language = QLocale::system().name();
  }

  // Loads english language if system locale name is different from Qt locale
  // format (ISO 639  + ISO 3166).
  if (!translator.load(":/translations/tanks_" + language)) {
    translator.load(":/translations/tanks_en_US");
    language = "en_US";
  }
  QApplication::installTranslator(&translator);

  // Creates new settings file based on current settings.
  QJsonObject new_json_obj;
  new_json_obj["language"] = language;
  new_json_obj["charge_line"] = charge_line;
  new_json_obj["music_enabled"] = music_enabled;
  new_json_obj["fps"] = fps_option;
  QJsonDocument new_json_document(new_json_obj);
  QString new_json_string = new_json_document.toJson();

  settings_file.open(QIODevice::WriteOnly);
  settings_file.write(new_json_string.toUtf8());
  settings_file.close();

  MainWindow w;
  w.setWindowTitle("Tanks");
  w.show();

  return QApplication::exec();
}
