#pragma once

// std
#include <map>
#include <string>
// qt
#include <QtGlobal>

namespace {
static const int MAX_LEN_BUFFER = 4096;
static const int FILE_MAX_LEN = 10 * 1000;
} // namespace

class Printer {
  std::string m_file;
  std::string m_folder;
  std::string m_url;
  std::string m_module_s;

  int m_count;

public:
  enum eModule {
    Sel,
    Gen,
    App,
    Doc,
    DocT,
    Shell,
    Tests,
    Evt,
    RunA,
    RunWF,
    RunC,
    RunR,
    RunRes,
    RunU,
    RunW,
    RunP,
    DiaI,
    DiaH,
    DiaE,
    Mr,
    ModulesNum
  };
  enum eType { Debug, Info, Error };

private:
  //////////////////////////////////
  //
  //		Config
  //
  //////////////////////////////////
  class Config {
  public:
    bool error;
    bool info;
    bool debug;

    double max_file_size;

    struct DIE {
      bool override;
      bool debug;
      bool info;
      bool error;
    };

    // enable Debug/Info/Error per module
    DIE m_module_DIE[ModulesNum];

    bool m_module_enabled[ModulesNum];
    eModule m_module;

    std::map<Printer::eModule, std::pair<std::string, std::string>> modules_s_m;

    Config();
  };
  //////////////////////////////////
  //
  //
  //
  //////////////////////////////////

public:
  Config m_cfg;

  bool m_b_multi_process;
  std::string m_multi_process_s;

protected:
  bool enabled() { return m_cfg.m_module_enabled[m_cfg.m_module]; }

public:
  // static
  static Printer *instance(eModule module = Gen);

  static const std::string DEFAULT_IT_FOLDER;
  static const std::string DEFAULT_FOLDER;
  static const std::string DEFAULT_FILE;

  static qint64 m_pid; // gm pid, only for Printer_MP_BOOST

public:
  std::string file() { return m_file; };
  void file(const std::string val);

  std::string folder() { return m_folder; };
  void folder(const std::string val);

  std::string url() { return m_url; };
  void url(std::string val);

  static qint64 pid() { return m_pid; };
  static void pid(const qint64 val) { m_pid = val; };

private:
  bool swap_file();

protected:
  void print(const char *cp, eType type = Debug);

public:
  Printer();
  ~Printer();

  virtual void debug(const char *fmt, ...) = 0;

  virtual void error(const char *fmt, ...) = 0;
  virtual void info(const char *fmt, ...) = 0;
};
