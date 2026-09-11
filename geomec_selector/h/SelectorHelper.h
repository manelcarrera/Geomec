#pragma once

#include <QString>
#include <QtGlobal> //QtMsgType

#include <map>
#include <vector>

typedef std::map<std::string, std::string> TMap;
typedef std::pair<std::string, std::string> TPair;

class QProcess;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

void qdebug_msg_handler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

///////////////////////////////////////////////////////////////////////

namespace app {
static const std::string CONFIG_FILE = std::string(getenv("APPDATA")) + "/" + "Geomec" + "/" + "selector.xml";
#ifdef KK
// for test proposal, it never helped me
void read_output();
void read_error();
#endif
TMap envvars(const std::string &path);
// not used
void launch_system_call(const QString &path, const QString &file);
// data request
bool launch(const QString &path, const QString &bin, const QString &param, const TMap &envvars_m, QString &result);
// detached
void launch(const QString &path, const QString &bin, const QString &param, const TMap &envvars_m);

namespace paths {
QStringList get();                    // get paths list from config file
void write(const QStringList &paths); // (over)writes paths in config file
void add(const QString &path);        // add a path to the config file, call every new execution
void tidy();                          // tidy paths in config file, removing the ones with no installations
QString get(const QString &version);  // get the path of a given version
QStringList default_();               // get default paths, possible locations for installed versions
} // namespace paths
} // namespace app

////////////////////////////////////////////////////////////////////////// -> version: get, set, read

namespace version {
enum eType { User, Sys };

QString config_file_url(version::eType type_ = version::eType::User);
void unset();
QString read();
void set(const QString &version);
std::pair<QString, bool> get_GUI(const std::pair<std::vector<QString>, std::vector<QString>> &versions);
// gets version:
// 1) from config file
// 2) usr chooses it by means of a dlg : 'get_GUI' method
std::pair<QString, bool> get();
QStringList list(const QString &path);      // versions list in a given path
QStringList list(const QStringList &paths); // versions list in a paths list
// as the one above but returning a paths vector and a versions vector
std::pair<std::vector<QString>, std::vector<QString>> list_ext(const QStringList &paths);

// functions not used currently but can be useful in the future
TPair get_();
TPair get__();
TPair parse_(const QString &ver);
} // namespace version

////////////////////////////////////////////////////////////////////////// -> migrate: registry -> config file

namespace migrate {
static const QString SETTINGS_SECTION = "Settings";
static const QString PATHS_SECTION = "Paths";
static const QString COLOR_SCALE_SECTION = "ColorScale";
static const QString POINT_SET_DLG_SECTION = "PointSetDlg";
static const QString WINDOW_SIZE_SECTION = "Window size";

static const std::vector<QString> SETTINGS_KEYS = {"XMLMatLib",    "OIVLicenseAttempt", "UNIT",          "UnlockGVT",
                                                   "MAGIC",        "DCASINT_MAGIC",     "MODEL2D_MAGIC", "UserTempPath",
                                                   "CalcTempPath", "UserTemp",          "Build"};

static const std::vector<QString> PATHS_KEYS = {"DIAPATH", "InputExeName", "DianaExeName", "FgvExeName"};

static const std::vector<QString> COLOR_SCALE_KEYS = {"Local"};

static const std::vector<QString> POINT_SET_DLG_KEYS = {"DefaultValueType"};

static const std::vector<QString> WINDOW_SIZE_KEYS = {
    "Rect", "icon", "max", "MainFrameSplitter", "OpenGLSplitViewSplitter", "TreeSplitViewSplitter"};

// user defined results
std::vector<std::string> udr();
void udr(std::vector<std::string> results_v);
// migrate the wheole section
void section(const QString &section_, const std::vector<QString> &keys_v);
void all_but_udr();
bool need();
// Only one time
// To force it do be redone set 'migrate' key in config file to empty, 'Y' or 'YES'
void do_();
} // namespace migrate