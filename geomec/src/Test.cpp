#include "stdafx.h"

#include "Test.h"

#include <QFile>
#include <QTextStream>

#include "GeomecDoc.h"
#include "Printer.h"
#include "QUtil.h"

#include "Events.h" // notity TI (TestInterface)

#include "GmHelper.h"

namespace {
Printer *printer_ = Printer::instance(Printer::Tests); // TESTS.log
Printer *printer = Printer::instance(Printer::Gen);    // gm_5.8.0.0_20105.1.log

static const char *SEP = " ";
} // namespace

const char *CTest::ana_type_cp[] = {"Linear", "Non-linear", "Heat", "Mix",
                                    "Mix-containment"}; // for run and export commands

QString CTest::m_cmd_qs[] = {"open", "run", "export", "goto", "label", "command", "unknown"};

std::map<QString, CTest::eCmd> CTest::m_cmd_map = {
    {"open", CTest::Open},   {"run", CTest::Run},         {"export", CTest::Export},  {"goto", CTest::Goto},
    {"label", CTest::Label}, {"command", CTest::Command}, {"unknown", CTest::Unknown}};

std::vector<CTest::TCmd> CTest::parse(const QString &url) {
  std::vector<TCmd> cmd_v;

  TCmd cmd;

  QFile file(url);
  if (file.open(QIODevice::ReadOnly)) {
    QTextStream in(&file);
    while (!in.atEnd()) {
      QString line = in.readLine();

      //
      // empty line
      //
      if (line.isEmpty())
        continue;

      line = line.trimmed(); // in case '# 'is not the first char

      //
      // full line commented
      //
      bool skip = false;
      for (auto token : {"#", "//"}) {
        if (line.startsWith(token)) {
          skip = true;
          break;
        }
        //
        // remove after command comments
        //
        if (line.contains(token))
          line = line.left(line.indexOf(token));

        line = line.trimmed(); // remove spaces or tabs after last param and comment token
      }
      if (skip)
        continue;
      //
      // empty line
      //
      QStringList items = line.split(SEP);
      if (!items.length())
        continue;

      cmd.id = m_cmd_map[items.takeFirst()]; // removed the first one

      if (cmd.id == Unknown)
        continue;

      cmd.params = items;

      cmd_v.push_back(cmd);
    }
    file.close();
  }
  return cmd_v;
};

CTest::CTest(const QString &url_) : m_idx(0) {
  m_url = url_;

  //
  // default input file
  //
  if (m_url.isEmpty())
    m_url = QString::fromStdString(QUtil::url(QUtil::eUsrDir::Goemec, "TEST"));

  printer_->url((m_url + ".log").toStdString());
  //
  printer_->info(">>>>>>>>>>>>>>>");
  printer->info("--Test: START--");

  m_cmd_v = parse(m_url);
}

CTest::~CTest() {
  printer->info("--Test: END--");
  printer_->info("<<<<<<<<<<<<<<<");
}

QString CTest::cmd_s(int idx) {
  TCmd &cmd = m_cmd_v[idx];
  return m_cmd_qs[cmd.id] + SEP + cmd.params.join(SEP);
}

bool CTest::execute(int idx) { return execute(m_cmd_v[idx]); }

//
// TODO: currently always returning true
//
bool CTest::execute(TCmd &cmd) {
  CGeomecDoc *doc = GetGeomecDoc();

  // TCmd& cmd = m_cmd_v[ idx ]; // needs to be a reference because of "goto"

  int num_params = cmd.params.size();

  QString msg = m_cmd_qs[cmd.id] + SEP + cmd.params.join(SEP);

  printer->info("test : %s", msg.toStdString().c_str());

  if (cmd.id == Open || cmd.id == Label || cmd.id == Command) // no params
    printer_->info("%s", msg.toStdString().c_str());

  switch (cmd.id) {
  //
  // a) model related commands: open + Run + Export
  //
  case Open: {
    enum eParams { File, Num };

    if (num_params != Num)
      return false;

    QString model = cmd.params[File].remove("\""); //"C:/aWork/aProjects/2020/09/SynMod_test.gm5";

    doc->OnOpenDocument(model.toStdString().c_str());
    //
    doc->UpdateAllViews(NULL, NEW_MODEL);
    break;
  }
  case Run: {
    enum eParams { Type, Num };

    if (num_params != Num)
      return false;

    int type = cmd.params[Type].toInt();

    printer_->info("%s: %s", msg.toStdString().c_str(), ana_type_cp[type]);

    doc->RunAnalysis((CAnalysisType::TAnalysisType)type, false, false);
    break;
  }
  case Export: {
    enum eParams { Type, QuadFlag, Num };

    if (num_params != 1 && num_params != 2)
      return false;

    bool b_Quad = false;
    if (num_params == 2 && cmd.params[QuadFlag] == "quad")
      b_Quad = true;

    int type = cmd.params[Type].toInt();

    printer_->info("%s: %s %s", msg.toStdString().c_str(), ana_type_cp[type], b_Quad ? "(quad)" : "");

    doc->RunAnalysis((CAnalysisType::TAnalysisType)type, !b_Quad, b_Quad);
    break;
  }
  //
  // b) commands interpreter internal commands: goto + label
  //
  case Goto: {
    enum eParams { Id, Times, Num };

    if (num_params == 0) // id,[times]
      return false;

    //
    // if no Times provided then force it to 1
    //
    if (num_params == 1) {
      cmd.params.append("1");
      num_params = cmd.params.size();
    }

    int times = cmd.params[Times].toInt();

    QString label_id = cmd.params[Id]; // target

    if (!times) {
      printer_->info("%s", msg.toStdString().c_str());
      break;
    }

    //
    // find label idx
    //
    bool found = false;
    for (int i = 0; i < m_cmd_v.size() && !found; i++) {
      TCmd &cmd_ = m_cmd_v[i];
      if (cmd_.id != Label) // command is no Label
        continue;
      if (cmd_.params[Id] != label_id) // label is not the target one
        continue;

      found = true;
      m_idx = i;
    }

    printer_->info("%s: %s", msg.toStdString().c_str(), found ? "label found" : "label not found !!!");

    if (!found) // label not found
      break;

    cmd.params[Times] = QString::number(times - 1);

    break;
  }
  case Label: {
    /*enum eParams{Id=1,Num};

    if(num_params!=1) // id
      return false;

    QString id = cmd[Id];*/
    break;
  }
  //

  //
  // use ON_COMMAND_RANGE
  // https://stackoverflow.com/questions/27622545/is-there-a-way-to-get-the-handler-function-that-a-control-is-mapped-to-in-an-mfc
  // https://docs.microsoft.com/en-us/cpp/error-messages/compiler-errors-1/compiler-error-c2064?view=msvc-160
  //
  case Command: {
    enum eParams { CommandId, Num };

    if (num_params != 1)
      return false;

    UINT command = gm::cmd(cmd.params[0]);
    if (!command)
      return false;

    //
    // 1) executed here
    //
    auto blocking_run = [=]() { (doc->*doc->m_func_map[command])(); };

    //
    // 2) send an event to the MFC queue to be exeucuted later
    //
    // FIXME: don't use by now as a mechanism to wait till the end of the command still needs to be implemented
    //
    // gm::send::msg(ID_ON_EVENT, Test_Command, command);
    auto non_blocking_run_01 = [=]() { gm::send::msg(ID_ON_EVENT, Test_Command, command); };

    //
    // 3) shortcut
    //
    // FIXME: just playing around
    //
    auto k = [](WORD key, int times = 1) { return gm::send::key_ext{key, times}; };
    auto non_blocking_run_02 = [=](const std::vector<gm::send::key_ext> &key_v) { gm::send::key_stroke(key_v); };
    /*if(command==ID_FILE_SAVE_AS)
      //gm::send::key_stroke_ctrl('S'); // save model -> it works ... but it crashes no quit save_as dlg ( hWnd=nullptr,
      why? ) gm::send::key_stroke( std::vector<WORD>{VK_DOWN,VK_DOWN} );*/

    blocking_run();
    // non_blocking_run_02( { k(VK_F10), k(VK_DOWN,13), k(VK_RETURN) } ); // open first file in recent files -> Ok

    break;
  }
  default: {
    printer->error("test : cmd : unknown");
    printer_->error("cmd : unknown");
    break;
  }
  }

  return true;
}