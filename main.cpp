#include <iostream>

#include <QApplication>
#include <QLibraryInfo>
#include <QProcess>
#include <QSettings>
#include <QStyleFactory>
#include <QTranslator>

#include "game_core/constants.h"
#include "game_core/mainwindow.h"

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras>
#include <QDateTime>
#endif

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

#ifdef Q_OS_ANDROID
  QtAndroid::runOnAndroidThread([] {
    auto activity = QtAndroid::androidActivity();
    auto window =
        activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
    const int FLAG_KEEP_SCREEN_ON = 128;
    window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);

    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
      env->ExceptionClear();
    }
  });
#endif

  MainWindow main_window;
  main_window.setWindowTitle("Tanks");

#ifdef Q_OS_ANDROID
  main_window.showFullScreen();
#else
  main_window.show();
#endif

  int return_code = QApplication::exec();
  if (return_code != constants::kApplicationRestartCode) {
    return return_code;
  }

#ifdef Q_OS_ANDROID
  // Code after won't work for SDK >= 28/29
  qDebug() << QtAndroid::androidSdkVersion();

  QtAndroid::runOnAndroidThread([] {
    auto activity = QtAndroid::androidActivity();

    auto packageManager = activity.callObjectMethod(
        "getPackageManager",
        "()Landroid/content/pm/PackageManager;");

    auto activityIntent = packageManager.callObjectMethod(
        "getLaunchIntentForPackage",
        "(Ljava/lang/String;)Landroid/content/Intent;",
        activity.callObjectMethod("getPackageName",
                                  "()Ljava/lang/String;").object());

    auto pendingIntent = QAndroidJniObject::callStaticObjectMethod(
        "android/app/PendingIntent",
        "getActivity",
        "(Landroid/content/Context;ILandroid/content/Intent;I)"
        "Landroid/app/PendingIntent;",
        activity.object(),
        jint(0),
        activityIntent.object(),
        QAndroidJniObject::getStaticField<jint>("android/content/Intent",
                                                "FLAG_ACTIVITY_CLEAR_TOP"));

    auto alarmManager = activity.callObjectMethod(
        "getSystemService",
        "(Ljava/lang/String;)Ljava/lang/Object;",
        QAndroidJniObject::getStaticObjectField(
            "android/content/Context",
            "ALARM_SERVICE",
            "Ljava/lang/String;").object());

    alarmManager.callMethod<void>(
        "setExact",
        "(IJLandroid/app/PendingIntent;)V",
        QAndroidJniObject::getStaticField<jint>(
            "android/app/AlarmManager", "RTC"),
        jlong(QDateTime::currentMSecsSinceEpoch() + 100),
        pendingIntent.object());

    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
      env->ExceptionClear();
    }
  });

  return 0;
#else
  QString executable_path = QApplication::applicationFilePath();
  if (!QProcess::startDetached(executable_path, {})) {
    std::cerr << "Starting a new process failed!" << std::endl;
    return 1;
  }
  return 0;
#endif
}
