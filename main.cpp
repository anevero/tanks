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

  QFile settings_file("settings.json");
  if (settings_file.exists()) {
    settings_file.open(QIODevice::ReadOnly);
    QString text = settings_file.readAll();
    settings_file.close();
    QJsonDocument json_document(QJsonDocument::fromJson(text.toUtf8()));
    QJsonObject json = json_document.object();
    language = json["language"].toString();
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
