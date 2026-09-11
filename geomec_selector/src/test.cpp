#include "test.h"

#include <QApplication>
#include <QProcessEnvironment>
#include <QStringList>

#include "SelectorHelper.h"

#include "QUtil.h"

////////////////////////////////////////////////////////
//			TESTS
//
///////////////////////////////////////////////////////

// C:\Program Files\Geomec\5.7.0.10_build_20020.2
// to solve that always Geomec.exe in goemec_selector.exe location is launched, instead of the specified one
//
// -> SOLVED !!!
//
// Solution not present in distributed version: '5.7.0.10_build_20020.3'
void TEST_07() {
  QString version_ = "5.7.0.10_build_20020.2";

  QString path = QUtil::url(app::paths::get(version_), version_);
  QString bin = "Geomec.exe";
  QString params; // = file;
  TMap envvars_m = app::envvars(path.toStdString());

  // this doesn't work
  // app::launch( path, bin,	params,	envvars_m );

  // this is Ok
  app::launch(path, path + "\\" + bin, params, envvars_m);
}

// test get version from geomec_shel..exe using launch method (instead of app::exec)
void TEST_01() { std::pair<std::string, std::string> ver = version::get__(); }

// test get and add path to the config file
void TEST_02() {
  QStringList paths = app::paths::get();

  app::paths::add("C:\\Users\\Manel.CarreraRuibal\\OneDrive - Shell\\Documents");
  QStringList paths2 = app::paths::get();
}

// tests when QProcess was not executing the binary
// CONCLUSION: start( process_name not process_full_path )
int TEST_03(int argc, char *argv[]) {
  QApplication a(argc, argv);

  //--------------------------------------------------- TEST
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

  env.insert("Path", "C:\\Program Files\\GEOMEC - SVS\\5.7.0.4_build_19245.2;\
C:\\Program Files\\GEOMEC - SVS\\5.7.0.4_build_19245.2\\Diana\\bin;\
C:\\Program Files\\GEOMEC - SVS\\5.7.0.4_build_19245.2\\Diana\\binseg\\ap;\
C:\\Program Files\\GEOMEC - SVS\\5.7.0.4_build_19245.2\\dll\\flex;\
C:\\Program Files\\GEOMEC - SVS\\5.7.0.4_build_19245.2\\dll\\oiv;\
C:\\Program Files\\GEOMEC - SVS\\5.7.0.4_build_19245.2\\dll\\qt;\
C:\\Program Files\\GEOMEC - SVS\\5.7.0.4_build_19245.2\\dll\\qt\\platforms;\
C:\\Program Files\\GEOMEC - SVS\\5.7.0.4_build_19245.2\\dll\\qwt;\
C:\\Program Files\\GEOMEC - SVS\\5.7.0.4_build_19245.2\\dll\\tbb;\
C:\\Program Files\\GEOMEC - SVS\\5.7.0.4_build_19245.2\\dll\\usr;\
C:\\Program Files\\GEOMEC - SVS\\5.7.0.4_build_19245.2\\dll\\other");

  env.insert("QT_QPA_PLATFORM_PLUGIN_PATH",
             "C:\\Program Files\\GEOMEC - SVS\\5.7.0.4_build_19245.2\\dll\\qt\\platforms");

  // QString exe = "C:\\Program Files\\GEOMEC - SVS\\5.7.0.4_build_19245.2\\Geomec.exe";
  QString exe = "Geomec.exe";
  QProcess *p2 = new QProcess();
  p2->setProcessEnvironment(env);
  p2->start(exe);
  bool res = p2->waitForFinished(-1); // if nothing then 30s. by default
  return a.exec();
  //---------------------------------------------------
}

// installed versions
void TEST_04() {
  QStringList paths = app::paths::get();       //{C:\Users\Manel.CarreraRuibal\AppData\Roaming\Geomec, ...}
  QStringList versions = version::list(paths); //{5.7.0.4_build_19245.2, ...}
}

void TEST_05() { QStringList paths = app::paths::default_(); }

// Force dlg
// 2020-01-17
void TEST_06() {
  QString version_;
  bool checked = false;

  QStringList paths = app::paths::get() << app::paths::default_();
  paths.removeDuplicates();

  std::pair<std::vector<QString>, std::vector<QString>> versions = version::list_ext(paths);
  std::tie(version_, checked) = version::get_GUI(versions);
}

///////////////////////////////////////////////
//
//			DRAFT
//
//////////////////////////////////////////////////////

// no longer needed but I didn't make it work
/*bool is_set_reg_test = false;
if( is_set_reg_test )
{
  //"C:\Program Files\GEOMEC - SVS\selector\geomec_selector.exe" "%1"

  //Computer\HKEY_LOCAL_MACHINE\SOFTWARE\Classes\Geomec5.Document\shell\open\command
  QSettings settings(
    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Classes\\Geomec5.Document\\shell\\open\\command",
    QSettings::NativeFormat);

  QString value_1 = "\"C:\\Program Files\\GEOMEC - SVS\\selector\\geomec_selector.exe\" \"%1\"";
  QString value_2 = "\"C:\\Program Files\\GEOMEC - SVS\\5.7.0.4_build_19241.1\\Geomec.exe\" \"%1\"";

  settings.setValue("", value_2 );

  return 0;
}*/