#include "InterfaceDiagnostics.h"
#include "ArchiveQDataStream.h"
#include "ArgumentParser.h"
#include "BaseEntryTypes.h"
#include "GeomecShellUtils.h"
#include "GeomecUtils.h"
#include "Global.h"
#include "IProgressFactory.h"
#include "InterfaceContext.h"
#include "MeshDataCacher.h"
#include "ModelBase.h"
#include "ResultCache.h"
#include "ResultRegister.h"
#include "ZoominModelEntry.h"
#include "ZoominModelPlaceHolder.h"
#include "geomec_shell.h"

#include <QTextStream>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "ModelDiagnostics.h"

namespace {

class CArchiveQTextStream : public CArchiveInterface {
public:
  CArchiveQTextStream(QIODevice *device);
  ~CArchiveQTextStream();

  // Attributes

  virtual bool IsLoading() const;
  virtual bool IsStoring() const;
  virtual bool Exists() const;

  // Position

  virtual quint64 GetPosition();
  virtual void SetPosition(quint64 position);

  // Operations

  virtual void Flush();
  virtual void Close();
  virtual void Abort();

  // Insertion

  virtual CArchiveQTextStream &operator<<(char c);
  virtual CArchiveQTextStream &operator<<(int i);
  virtual CArchiveQTextStream &operator<<(unsigned int ui);
  virtual CArchiveQTextStream &operator<<(long l);
  virtual CArchiveQTextStream &operator<<(unsigned long ul);
  virtual CArchiveQTextStream &operator<<(double d);
  virtual CArchiveQTextStream &operator<<(const QString &s);
  virtual CArchiveQTextStream &operator<<(const QDateTime &dt);

  // Extraction

  virtual CArchiveQTextStream &operator>>(char &c);
  virtual CArchiveQTextStream &operator>>(int &i);
  virtual CArchiveQTextStream &operator>>(unsigned int &ui);
  virtual CArchiveQTextStream &operator>>(long &l);
  virtual CArchiveQTextStream &operator>>(unsigned long &ul);
  virtual CArchiveQTextStream &operator>>(double &d);
  virtual CArchiveQTextStream &operator>>(QString &s);
  virtual CArchiveQTextStream &operator>>(QDateTime &dt);

  // Compression
  virtual CArchiveQTextStream &CreateCompressedStream();
  virtual void DestroyCompressedStream();
  virtual bool IsCompressed() const;

private:
  QTextStream archive;
  bool compressed;
};

CArchiveQTextStream::CArchiveQTextStream(QIODevice *device) : archive(device), compressed(false) {}

CArchiveQTextStream::~CArchiveQTextStream() {}

bool CArchiveQTextStream::IsLoading() const { return archive.device()->isReadable(); }

bool CArchiveQTextStream::IsStoring() const { return archive.device()->isWritable(); }

bool CArchiveQTextStream::Exists() const { return true; }

quint64 CArchiveQTextStream::GetPosition() { return archive.device()->pos(); }

void CArchiveQTextStream::SetPosition(quint64 position) { archive.device()->seek(position); }

void CArchiveQTextStream::Flush() {}

void CArchiveQTextStream::Close() { archive.device()->close(); }

void CArchiveQTextStream::Abort() { archive.device()->close(); }

// write -----------------------------------------------------------------------

CArchiveQTextStream &CArchiveQTextStream::operator<<(char c) {
  assert(sizeof(char) == sizeof(qint8));

  qint8 q = c;
  archive << q << " ";

  return *this;
}

CArchiveQTextStream &CArchiveQTextStream::operator<<(int i) {
  assert(sizeof(int) == sizeof(qint32));

  qint32 q = i;
  archive << q << " ";

  return *this;
}

CArchiveQTextStream &CArchiveQTextStream::operator<<(unsigned int ui) {
  assert(sizeof(unsigned int) == sizeof(quint32));

  quint32 q = ui;
  archive << q << " ";

  return *this;
}

CArchiveQTextStream &CArchiveQTextStream::operator<<(long l) {
  qint32 q = l;
  archive << q << " ";

  return *this;
}

CArchiveQTextStream &CArchiveQTextStream::operator<<(unsigned long ul) {
  quint32 q = ul;
  archive << q << " ";

  return *this;
}

CArchiveQTextStream &CArchiveQTextStream::operator<<(double d) {
  archive << d << " ";

  return *this;
}

CArchiveQTextStream &CArchiveQTextStream::operator<<(const QString &s) {
  archive << s << " ";

  return *this;
}

CArchiveQTextStream &CArchiveQTextStream::operator<<(const QDateTime & /*dt*/) {
  // archive << dt;

  return *this;
}

// read ------------------------------------------------------------------------

CArchiveQTextStream &CArchiveQTextStream::operator>>(char &c) {
  assert(sizeof(char) == sizeof(qint8));

  archive >> c;

  return *this;
}

CArchiveQTextStream &CArchiveQTextStream::operator>>(int &i) {
  assert(sizeof(int) == sizeof(qint32));

  qint32 q;
  archive >> q;
  i = q;

  return *this;
}

CArchiveQTextStream &CArchiveQTextStream::operator>>(unsigned int &ui) {
  assert(sizeof(unsigned int) == sizeof(quint32));

  quint32 q;
  archive >> q;
  ui = q;

  return *this;
}

CArchiveQTextStream &CArchiveQTextStream::operator>>(long &l) {
  qint32 q;
  archive >> q;
  l = q;

  return *this;
}

CArchiveQTextStream &CArchiveQTextStream::operator>>(unsigned long &ul) {
  quint32 q;
  archive >> q;
  ul = q;

  return *this;
}

CArchiveQTextStream &CArchiveQTextStream::operator>>(double &d) {
  archive >> d;

  return *this;
}

CArchiveQTextStream &CArchiveQTextStream::operator>>(QString &s) {
  archive >> s;

  return *this;
}

CArchiveQTextStream &CArchiveQTextStream::operator>>(QDateTime & /*dt*/) {
  // archive >> dt;

  return *this;
}

// Compression
CArchiveQTextStream &CArchiveQTextStream::CreateCompressedStream() {
  compressed = true;
  return *this;
}

void CArchiveQTextStream::DestroyCompressedStream() { compressed = false; }

bool CArchiveQTextStream::IsCompressed() const { return compressed; }

} // namespace

class CModelDiagnosticsHookResultRegister : public IModelDiagnosticsHook {
  QHash<int, QString> m_storeInfo;
  int m_order;
  int m_exit;

public:
  CModelDiagnosticsHookResultRegister();
  virtual void start(CModelBase *model);
  virtual void end(CModelBase *model);

  void SetStoreInfo(int order, QString &file, bool bExit);
};

class CModelDiagnostics : public IModelDiagnostics {
  QHash<QString, bool> m_onOffSettings;
  QHash<QString, IModelDiagnosticsHook *> m_hooks;

public:
  CModelDiagnostics();
  virtual bool Setting(const QString &name) const;
  virtual void Setting(const QString &name, bool value);
  virtual void Hook(const QString &name, IModelDiagnosticsHook &hook);
  virtual IModelDiagnosticsHook *Hook(const QString &name) const;
};

CModelDiagnosticsHookResultRegister::CModelDiagnosticsHookResultRegister() : m_order(0), m_exit(-1) {}

void CModelDiagnosticsHookResultRegister::start(CModelBase * /*model*/) { ++m_order; }

void CModelDiagnosticsHookResultRegister::end(CModelBase *model) {
  if (m_storeInfo.contains(m_order)) {
    QFile exportFile(m_storeInfo[m_order]);

    if (!exportFile.open(QIODevice::WriteOnly)) {
      std::cerr << "Failed to open file '" << m_storeInfo[m_order].toStdString() << "' for writing" << std::endl;
    } else {
      IProgressBase *prog = _g->prog()->create(eProgress::Geo, "Saving results");

      prog->AddSteps(model->ResultRegister().SavedItems());

      CArchiveQDataStream exportStream(&exportFile);

      model->ResultRegister().SaveStream(exportStream, *prog);

      delete prog;
    }

    if (m_exit == m_order)
      exit(0);
  }
}

void CModelDiagnosticsHookResultRegister::SetStoreInfo(int order, QString &file, bool bExit) {
  m_storeInfo[order] = file;
  if (bExit)
    m_exit = order;
}

class CModelDiagnosticsHookAnalysisLog : public IModelDiagnosticsHook {
  QString m_file;
  bool m_bExit;

public:
  CModelDiagnosticsHookAnalysisLog(QString &file, bool bExit);
  virtual void end(CModelBase *model);
};

CModelDiagnosticsHookAnalysisLog::CModelDiagnosticsHookAnalysisLog(QString &file, bool bExit)
    : m_file(file), m_bExit(bExit) {}

void CModelDiagnosticsHookAnalysisLog::end(CModelBase *model) {
  model->Logger().ExportToFile(m_file.toStdString(), true);

  if (m_bExit)
    exit(0);
}

CModelDiagnostics::CModelDiagnostics() {}

bool CModelDiagnostics::Setting(const QString &name) const {
  return m_onOffSettings.contains(name) ? m_onOffSettings[name] : false;
}

void CModelDiagnostics::Setting(const QString &name, bool value) { m_onOffSettings[name] = value; }

void CModelDiagnostics::Hook(const QString &name, IModelDiagnosticsHook &hook) { m_hooks[name] = &hook; }

IModelDiagnosticsHook *CModelDiagnostics::Hook(const QString &name) const {
  return m_hooks.contains(name) ? m_hooks[name] : 0;
}

namespace gm_shell {

bool CInterfaceDiagnostics::Expose() const { return false; }

const char *CInterfaceDiagnostics::ShortDescription() const { return "Geomec Diagnostics"; }

bool CInterfaceDiagnostics::AcceptParameters(CArgumentParser &argParser, bool bForced) {
  if (bForced) {
    std::string key;
    std::string value;

    m_argParser = &argParser;

    int nFound = 0;

    for (size_t i = 0; i < argParser.ArgumentSize(); ++i) {
      argParser.GetArgument(i, key, value);
      if (value.length() > 4) {
        std::string ext = value.substr(value.length() - 4, 4);

        if (ext == ".gm4" || ext == ".gm5") {
          if (!nFound)
            m_model = value.c_str();
          ++nFound;
        }
      }
    }

    if (nFound == 1) {
      m_commands.push(DIAGNOSTICS_PARSE);
    } else {
      m_error = 1;
      m_commands.push(IInterfaceBase::USAGE);
      m_commands.push(IInterfaceBase::ABORT);
    }

    return true;
  }

  return false;
}

void CInterfaceDiagnostics::PrintUsage() const {
  std::cerr << std::endl
            << "geomec_shell /interface diagnostics <model>" << std::endl
            << "  the following commands are recognized:" << std::endl
            << "    help" << std::endl
            << "    quit" << std::endl
            << std::endl;
}

void CInterfaceDiagnostics::PrintHelp() const {
  std::cout << "load [main]" << std::endl
            << "\tload model; option to stop before loading zoomin models" << std::endl
            << "ls depletions|analyses|zoomin" << std::endl
            << "\tlist" << std::endl
            << "cd ..|<zoomin model>" << std::endl
            << "\tswitch to parent or child model" << std::endl
            << "ignore [main|zoomin|wellzoomin] results" << std::endl
            << "\tdon't store read results in memory" << std::endl
            << "save <file>" << std::endl
            << "\tsave model" << std::endl
            << "flush results" << std::endl
            << "\tflush result cache" << std::endl
            << "save results [txt] <file>" << std::endl
            << "\tsave results of current model (unencrypted)" << std::endl
            << "sethook resultregister [exit] # <file>" << std::endl
            << "\tsave #th register that is loaded to file (and exit thereafter)" << std::endl
            << "sethook analysislog [exit] <file>" << std::endl
            << "\tsave the (main) analysis log to file (and exit)" << std::endl
            << "tmp <dir>" << std::endl
            << "\tuse <dir> as temporary directory" << std::endl
            << "quit" << std::endl
            << "\tquit the program" << std::endl
            << std::endl
            << "use ignore and sethook before loading; don't use quotes or extra spaces" << std::endl
            << std::endl;
}

void CInterfaceDiagnostics::Startup() {
  if (IsSlave()) {
    m_pModelContext = m_channel->Master()->GetModelContext();

    if (m_pModelContext)
      m_model = m_pModelContext->ModelPath();
  }

  gm_shell::MakeAbsolutePath(m_model);

  std::cout << "using interface Geomec Diagnostics for model " << m_model.toStdString() << std::endl
            << std::endl
            << "WARNING: extremely experimental and undocumented" << std::endl
            << "there is no guarantee that this interface will remain the same" << std::endl
            << "use help to view current commands" << std::endl
            << std::endl
            << "$ ";

  if (!m_pModelContext)
    m_pModelContext = new CInterfaceModelContext(AppVersion());

  if (!m_pModelDiagnostics)
    m_pModelDiagnostics = new CModelDiagnostics();

  m_initialized = true;
}

void CInterfaceDiagnostics::HandleLastCommand() {
  int command = m_commands.front();
  m_commands.pop();

  int retval = 0;

  switch (command) {
  case DIAGNOSTICS_PARSE:
    retval = ParseAndEvaluate();
    break;
  case DIAGNOSTICS_HELP:
    PrintHelp();
    break;
  }

  if (retval != 0) {
    m_error = retval;
    m_commands.push(DIAGNOSTICS_HELP);
  } else if (m_commands.empty()) {
    m_commands.push(DIAGNOSTICS_PARSE);
    std::cout << "$ ";
  }
}

// Extremely simplistic parser
int CInterfaceDiagnostics::ParseAndEvaluate() {
  int retval = 0;

  QTextStream qin(stdin);
  QString orig = qin.readLine().trimmed();
  QString line = orig.toLower();

  if (line == "quit")
    m_commands.push(IInterfaceBase::ABORT);
  else if (line.startsWith("load"))
    retval = ParseAndEvaluateLoad(line);
  else if (line.startsWith("ls ") && m_pModelContext->Model())
    retval = ParseAndEvaluateList(line);
  else if (line.startsWith("cd "))
    retval = ParseAndEvaluateChangeDir(orig);
  else if (line.startsWith("ignore "))
    retval = ParseAndEvaluateIgnore(line);
  else if (line.startsWith("flush results"))
    retval = ParseAndEvaluateFlushResults(line);
  else if (line.startsWith("save results ") && m_pModelContext->Model())
    retval = ParseAndEvaluateSaveResults(line);
  else if (line.startsWith("save "))
    retval = ParseAndEvaluateSave(line);
  else if (line.startsWith("sethook "))
    retval = ParseAndEvaluateSetHook(line);
  else if (line.startsWith("tmp "))
    retval = ParseAndEvaluateSetTemp(line);
  else if (line != "")
    retval = 1;

  return retval;
}

int CInterfaceDiagnostics::ParseAndEvaluateLoad(QString &line) {
  if (line == "load main")
    m_pModelDiagnostics->Setting("LoadMainModelOnly", true);
  try {
    m_pModelContext->Load(m_model, m_pModelDiagnostics);
  } catch (std::exception const &e) {
    std::cerr << "caught exception: " << e.what() << std::endl;
  }
  std::cout << std::endl;
  if (!m_pModelContext->Model())
    std::cerr << "Failed to load model" << std::endl;

  return 0;
}

int CInterfaceDiagnostics::ParseAndEvaluateList(QString &line) {
  if (line == "ls depletions") {
    CDepletionStage *pStage = &m_pModelContext->Model()->DepletionStageEntry().LastStage();
    while (pStage && !pStage->Initial())
      pStage = &pStage->Previous();
    while (pStage) {
      std::cout << pStage->Index() << "\t" << pStage->Name().toStdString() << "\t";
      std::cout << (m_pModelContext->Model()->ResultRegister().ResultsAvailable(CAnalysisType::AT_LINEAR, *pStage)
                        ? "l"
                        : ".");
      std::cout << (m_pModelContext->Model()->ResultRegister().ResultsAvailable(CAnalysisType::AT_NONLIN, *pStage)
                        ? "n"
                        : ".");
      std::cout << (m_pModelContext->Model()->ResultRegister().ResultsAvailable(CAnalysisType::AT_HEAT, *pStage) ? "h"
                                                                                                                 : ".");
      std::cout << (m_pModelContext->Model()->ResultRegister().ResultsAvailable(CAnalysisType::AT_MIXTURE, *pStage)
                        ? "m"
                        : ".");
      std::cout << (m_pModelContext->Model()->ResultRegister().ResultsAvailable(CAnalysisType::AT_MIXTURE_CONTAINMENT,
                                                                                *pStage)
                        ? "c"
                        : ".")
                << "\t";
      std::cout << CDepletionStage::OutputTypeTag(pStage->OutputType());
      std::cout << std::endl;
      pStage = pStage->Last() ? 0 : &pStage->Next();
    }
  } else if (line == "ls analyses") {
    if (m_pModelContext->Model()->ResultRegister().ResultsAvailable(CAnalysisType::AT_LINEAR))
      std::cout << "l\tlinear" << std::endl;
    if (m_pModelContext->Model()->ResultRegister().ResultsAvailable(CAnalysisType::AT_NONLIN))
      std::cout << "n\tnon-linear" << std::endl;
    if (m_pModelContext->Model()->ResultRegister().ResultsAvailable(CAnalysisType::AT_HEAT))
      std::cout << "h\theat flow" << std::endl;
    if (m_pModelContext->Model()->ResultRegister().ResultsAvailable(CAnalysisType::AT_MIXTURE))
      std::cout << "m\tmixture" << std::endl;
    if (m_pModelContext->Model()->ResultRegister().ResultsAvailable(CAnalysisType::AT_MIXTURE_CONTAINMENT))
      std::cout << "c\tmixture with containment" << std::endl;
    if (m_pModelContext->Model()->ResultRegister().HasSmoothedResults())
      std::cout << "using averaged results" << std::endl;
  } else if (line == "ls zoomin") {
    CZoominModelEntry *pZIEntry =
        static_cast<CZoominModelEntry *>(m_pModelContext->Model()->GraphEntry(MD_BASE_ZOOMIN_MODEL));
    CGraphEntry::TEntryNodeSet zoominNodes = pZIEntry->GraphEntryNodes();
    for (CGraphEntry::TEntryNodeSet::iterator it = zoominNodes.begin(); it != zoominNodes.end(); ++it)
      std::cout << (*it)->Name().toStdString() << std::endl;
  }

  return 0;
}

int CInterfaceDiagnostics::ParseAndEvaluateChangeDir(QString &line) {
  QString name = line.right(line.length() - 3).trimmed();
  if (name == "..") {
    m_pModelContext->SwitchToParent();
  } else {
    CZoominModelEntry *pZIEntry =
        static_cast<CZoominModelEntry *>(m_pModelContext->Model()->GraphEntry(MD_BASE_ZOOMIN_MODEL));
    CGraphEntry::TEntryNodeSet zoominNodes = pZIEntry->GraphEntryNodes();
    for (CGraphEntry::TEntryNodeSet::iterator it = zoominNodes.begin(); it != zoominNodes.end(); ++it)
      if ((*it)->Name() == name) {
        if (dynamic_cast<CModelBase *>(*it))
          m_pModelContext->SwitchTo(static_cast<CModelBase *>(*it));
        else if (dynamic_cast<CZoominModelPlaceHolder *>(*it))
          m_pModelContext->SwitchTo(&static_cast<CZoominModelPlaceHolder *>(*it)->ChildModel());
      }
  }

  return 0;
}

int CInterfaceDiagnostics::ParseAndEvaluateIgnore(QString &line) {
  if (line == "ignore results") {
    std::cout << "ignoring results" << std::endl;
    m_pModelDiagnostics->Setting(gm_diagnostics::Setting_LoadModelNoResults, true);
  } else if (line == "ignore main results") {
    std::cout << "ignoring main results" << std::endl;
    m_pModelDiagnostics->Setting(gm_diagnostics::Setting_LoadMainModelNoResults, true);
  } else if (line == "ignore zoomin results") {
    std::cout << "ignoring zoomin results" << std::endl;
    m_pModelDiagnostics->Setting(gm_diagnostics::Setting_LoadZoominModelNoResults, true);
  } else if (line == "ignore wellzoomin results") {
    std::cout << "ignoring wellzoomin results" << std::endl;
    m_pModelDiagnostics->Setting(gm_diagnostics::Setting_LoadWellZoominModelNoResults, true);
  } else {
    std::cerr << "unrecognized command: '" << line.toStdString() << "'" << std::endl;
  }

  return 0;
}

int CInterfaceDiagnostics::ParseAndEvaluateFlushResults(QString & /*line*/) {
  m_pModelContext->Model()->ResultRegister().Cache().RawCacher().Flush();

  return 0;
}

int CInterfaceDiagnostics::ParseAndEvaluateSaveResults(QString &line) {
  int nNr = 13;
  bool bText = line.right(line.length() - nNr).startsWith("txt ");
  if (bText)
    nNr += 4;

  QString exportPath = line.right(line.length() - nNr).trimmed();

  QFile exportFile(exportPath);
  if (!exportFile.open(QIODevice::WriteOnly)) {
    std::cerr << "Failed to open file '" << exportPath.toStdString() << "' for writing" << std::endl;
  } else {
    IProgressBase *prog = _g->prog()->create(eProgress::Geo, "Saving results");

    prog->AddSteps(m_pModelContext->Model()->ResultRegister().SavedItems());

    if (bText) {
      CArchiveQTextStream exportStream(&exportFile);

      m_pModelContext->Model()->ResultRegister().SaveStream(exportStream, *prog);
    } else {
      CArchiveQDataStream exportStream(&exportFile);

      m_pModelContext->Model()->ResultRegister().SaveStream(exportStream, *prog);
    }

    delete prog;
  }

  return 0;
}

int CInterfaceDiagnostics::ParseAndEvaluateSave(QString &line) {
  int nNr = 5;

  QString exportPath = line.right(line.length() - nNr).trimmed();

  m_pModelContext->SaveAs(exportPath);

  return 0;
}

int CInterfaceDiagnostics::ParseAndEvaluateSetHook(QString &line) {
  if (line.startsWith("sethook resultregister")) {
    int nNr = 23;
    int nNrOrder = 0;
    int nOrder = 0;

    bool bExit = false;

    if (line.mid(nNr, 4) == "exit") {
      nNr += 5;
      bExit = true;
    }

    while (line.at(nNr + nNrOrder).isDigit())
      ++nNrOrder;

    nOrder = line.mid(nNr, nNrOrder).toInt();

    QString exportPath = line.right(line.length() - nNr - nNrOrder).trimmed();

    IModelDiagnosticsHook *hook = m_pModelDiagnostics->Hook(gm_diagnostics::Hook_ResultRegister_LoadCacheResults);
    if (!hook) {
      hook = new CModelDiagnosticsHookResultRegister();
      m_pModelDiagnostics->Hook(gm_diagnostics::Hook_ResultRegister_LoadCacheResults, *hook);
    }

    static_cast<CModelDiagnosticsHookResultRegister *>(hook)->SetStoreInfo(nOrder, exportPath, bExit);

    std::cout << "hook resultregister " << nOrder << " to " << exportPath.toStdString();
    if (bExit)
      std::cout << " and exiting right after";
    std::cout << std::endl;

  } else if (line.startsWith("sethook analysislog")) {
    int nNr = 20;

    bool bExit = false;
    if (line.mid(nNr, 4) == "exit") {
      nNr += 5;
      bExit = true;
    }

    QString exportPath = line.right(line.length() - nNr).trimmed();

    IModelDiagnosticsHook *hook = m_pModelDiagnostics->Hook(gm_diagnostics::Hook_ModelBase_LoadAnalysisLog);
    if (!hook) {
      hook = new CModelDiagnosticsHookAnalysisLog(exportPath, bExit);
      m_pModelDiagnostics->Hook(gm_diagnostics::Hook_ModelBase_LoadAnalysisLog, *hook);
    }

    std::cout << "hook analysislog to " << exportPath.toStdString();
    if (bExit)
      std::cout << " and exiting right after";
    std::cout << std::endl;
  }

  return 0;
}

int CInterfaceDiagnostics::ParseAndEvaluateSetTemp(QString &line) {
  QString dir = line.right(line.length() - 4);
  ForceGeomecTempPathExt(CTempPath::TEMP_GENERAL, dir);

  std::cout << "temporary directory is now: " << dir.toStdString() << std::endl;
  return 0;
}

} // namespace gm_shell