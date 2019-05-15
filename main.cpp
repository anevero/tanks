#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStyleFactory>
#include <QTranslator>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setStyle(QStyleFactory::create("Fusion"));

  QString language{};
  bool charge_line = true;
  bool music_enabled = true;
  int fps_option;

#ifdef Q_OS_ANDROID
  qputenv("QT_QPA_NO_TEXT_HANDLES", "1");
  fps_option = 3;
#else
  fps_option = 1;
#endif

  QFile settings_file("settings.json");
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

  if (!translator.load(":/translations/tanks_" + language)) {
    translator.load(":/translations/tanks_en_US");
    language = "en_US";
  }
  a.installTranslator(&translator);

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

  return a.exec();
}
