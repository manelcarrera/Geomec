// IDianaRunner.cpp: implementation of the IDianaRunner class.
//
//////////////////////////////////////////////////////////////////////
#include <sstream>

#include <QDir>

#include "IDCElementProperty.h"
#include "DCLoadManager.h"
#include "IDCDianaRunner.h"
#include "IProgressBase.h"

#include "DCLoadManager.h"
#include "IDCTemperatureTable.h"
#include "IDCConcentrationTable.h"

#include "MeshNodeGroup.h"
#include "IElement.h"
#include "INode.h"
#include "ISupport.h"

#include <qstring.h>
#include "dimple.h"
#include "lbcx.h"
#include "lbfl.h"

#include "comfile.h"
#include "DCRangeBuilder.h"
#include "DCFixedPotential.h"
#include "ElementGroup.h"
#include "IDCAnalysisStatus.h"

#include "Printer.h"
#include "GlobalMessage.h"

#include "FilosFile.h" //OpenFilos()


#include <algorithm>

#define BS 2048

#if !defined(WIN32) && !defined(WIN64)
#define _stat stat
#endif  // !defined(WIN32) && !defined(WIN64)

#ifndef _S_IFDIR
#define _S_IFDIR S_IFDIR
#endif  // _S_IFDIR

#if !defined(WIN32) && !defined(WIN64)
#define _snprintf snprintf
#endif  // !defined(WIN32) && !defined(WIN64)

#if !defined(WIN32) && !defined(WIN64)
#define _getcwd getcwd
#endif  // !defined(WIN32) && !defined(WIN64)

extern "C"
{
  void ThrowException();
  void DebugPrint(const char *fmt, va_list args);
  void Fatal(int erno, const char *errm, const char *proc, va_list args);
  void InitializeFilos();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace{ Printer* printer = Printer::instance(Printer::RunR); }

namespace dia {

IDianaRunner::IDianaRunner(const geo::IMesh &mesh)
:	m_pTempTable(0),
  m_pConcenTable(0),
  m_pFixedPotential(0),
  m_bCalculationResultExpectOKMessage(false),
  m_bCanceled(false),
  m_nLicenseRetry(0),
  m_bHaveResults(false),
  m_bCalculationResult(false),
  m_LoadManager(mesh, *this),
  m_pAnalysisStatus(0),
  m_FF_(nullptr),
  m_progress(nullptr)
{
  printer->info("new >>");
  m_pmpDirections = new TDirectionMap;
}

IDianaRunner::~IDianaRunner()
{
  OnDeleteTemperatureTable();
  OnDeleteConcentrationTable();
  delete m_pmpDirections;

  if( m_progress )
  {
    delete m_progress;
    m_progress=nullptr;
  }

  if (m_FF_)
  {
    delete m_FF_;
    m_FF_ = nullptr;
  }
  printer->info("delete <<");
}

const geo::IMesh &IDianaRunner::Mesh() const
{
  return m_LoadManager.Mesh();
}

const std::string &IDianaRunner::FFDIR() const
{
  return m_FFDIR;
}

bool IDianaRunner::CreateSubdir(const std::string &rootdir)
{
  struct stat buf;

  if(stat(rootdir.c_str(), &buf) != 0) return false; // does not exist
  if(!(buf.st_mode & _S_IFDIR)) return false; // not a directory

  char *copy = DiStrsave(rootdir.c_str());
  char last = copy[strlen(copy) - 1];
  if(last != '/' && last != '\\') STRAPPEND(copy, "/");

  NativeStylePath(copy);

  STRAPPEND(copy, "drXXXXXX");
  if(!mktemp(copy)) return false;

#if defined(WIN32) || defined(WIN64)
  if(mkdir(copy) != 0) return false;
#else // !defined(WIN32) && !defined(WIN64)
  if(mkdir(copy, S_IFDIR | S_IRWXU) != 0) return false;
#endif  // defined(WIN32) || defined(WIN64)

  m_FFDIR = copy;

  DiFree(copy, "IDianaRunner::CreateSubdir");

  return true;
}

// initialize the Filos file for a calculation
// several items have to be set
void IDianaRunner::InitializeFile()
{
  InitializeFilos();
}

void IDianaRunner::InitDirectionsMapFromFilos()
{
  if(XistIndexed("/DIRECT", 0))
  {
  PushDir();
  ChangeDir("/DIRECT");

  ftn_int_t idx = InquireIndex(".");
  do
  {
      if(XistIndexed("XYZ", 0))
      {
    ftn_double_t vec[3];
    GetItemLength("XYZ", vec, 3);
    geo::CVector cvec(vec[0], vec[1], vec[2]);
    cvec = cvec.UnitVector();
    m_pmpDirections->insert(TDirectionMap::value_type(cvec, (int)idx));
      }
  } while(NextIndex(&idx, 0));

  PopDir();
  }
}

IDianaRunner::TDirectionMap* IDianaRunner::DirectionMap()
{
  return m_pmpDirections;
}

void IDianaRunner::DirectionMap(TDirectionMap* pMap)
{
  m_pmpDirections = pMap;
}

IAnalysisStatus *IDianaRunner::AnalysisStatus()
{
  return m_pAnalysisStatus;
}

void IDianaRunner::AnalysisStatus(IAnalysisStatus *pAnalysisStatus)
{
  m_pAnalysisStatus = pAnalysisStatus;
}

int IDianaRunner::MeshProgressSize() const
{
  return Mesh().ElementSize() + Mesh().PointSize();
}

int IDianaRunner::GroupsProgressSize() const
{
  int sz = 0;

  int i;
  for(i = 0; i < Mesh().ElementGroupSize(); i++) sz += Mesh().ElementGroup(i).ElementSize();
  for(i = 0; i < Mesh().NodeGroupSize(); i++) sz += Mesh().NodeGroup(i).NodeSize();

  return sz;
}

int IDianaRunner::SupportProgressSize() const
{
  return Mesh().SupportSize();
}

int IDianaRunner::TyingsProgressSize() const
{
  return Mesh().TyingSize();
}

int IDianaRunner::LoadsProgressSize() const
{
  return 0;
}

int IDianaRunner::CommandsProgressSize() const
{
  return 1;
}

// Return true if you want to export .com and .dat files
// instead of actually running the analysis.
// The analysis will not be performed, but input
// will be run to generate the data file.
bool IDianaRunner::WriteInputFiles(std::string &/*comfilename*/, std::string &/*datfilename*/) const
{
  return false;
}

int IDianaRunner::ProgressSize() const
{
  int sz = 0;

  sz += MeshProgressSize();
  sz += GroupsProgressSize();
  sz += SupportProgressSize();
  sz += TyingsProgressSize();
  sz += LoadsProgressSize();
  sz += CommandsProgressSize();

  if(m_pTempTable) sz += Mesh().ElementSize();
  if(m_pConcenTable) sz += Mesh().ElementSize();

  if(m_pFixedPotential)
  sz += m_pFixedPotential->NodeSize();

  return sz;
}

bool IDianaRunner::WriteDatabase(
  const std::string &title,
  IRetrieveDianaFileNames& retrieveDianaFileNames)
{
  printer->info("write db");

  if(!OnInitialize(title, retrieveDianaFileNames) || m_bCanceled) 
    return false;

  if (!OnWriteMesh() || m_bCanceled)
    return false;

  if (!OnWriteTyings() || m_bCanceled)
    return false;

  if (!OnApplyLoads(m_LoadManager) || m_bCanceled)
    return false;
  
  if (!OnWriteLoads() || m_bCanceled)
    return false;

  if (!OnWriteSupports() || m_bCanceled)
    return false;

  if (!WriteCommands() || m_bCanceled)
    return false;

  if(!OnEndWriteCommands(title) || m_bCanceled) 
    return false;

  if(m_pTempTable)
  {
    m_progress->StatusMessage("Writing temperature table");
    m_pTempTable->Write(*m_progress);
  }

  if(m_pConcenTable)
  {
    m_progress->StatusMessage("Writing concentration table");
    m_pConcenTable->Write(*m_progress);
  }

  if(m_pFixedPotential)
  {
    m_progress->StatusMessage("Writing fixed potentials");
    m_pFixedPotential->WriteFilos(*m_progress);
  }

  if(!OnWriteMiscelaneous() || m_bCanceled) 
    return false;

  return true;
}

bool IDianaRunner::clean(bool bForce)
{
  if( bForce || CanCleanupAfterRun() )
  {
    Cleanup(m_FFDIR);
    return true;
  }
  return false;
}

//
// TODO: return int with error code instead of just true or false 
//
bool IDianaRunner::end()
{
  printer->info("--end--");

  const RunParams& p = m_params;

  CCurDirSaver cwdsaver;		// stores current directory, and restores it when destroyed
  chdir(m_FFDIR.c_str()); // needs to be done again, even if it was already done in DoRun_START

  bool bHaveResults, bCalcResults;
  GetResultInfo(bHaveResults, bCalcResults, false);
  AnalysisSuccess(bCalcResults);

  // Use O_RDONLY whenever possible, as otherwise Diana may try to compress the Filos File which can take a long time
  int open_mode = OpenFilosfileReadOnlyForResult() ? O_RDONLY : O_RDWR;
  OpenFilos(m_FF.c_str(), open_mode);

  std::string sComfileName;
  std::string sDatfileName;

  bool bWriteInputFiles = WriteInputFiles(sComfileName, sDatfileName);
  OnModifyInputFileNames(sComfileName, sDatfileName);

  if (bWriteInputFiles)
  {
    if (!PostprocessWriteInputFiles(sComfileName, sDatfileName))
    {
      OnError(PostProcessWriteInputFiles);
      return false;
    }
  }

  if (!OnEndAnalysis(p.path,*p.save)) // implemented in CDianaRunner
  {
    OnError(EndAnalysis);
    return false;
  }

  return true;
}

void IDianaRunner::OnError( eError id, const std::string& msg2 )
{
  std::string msg;
  switch( id )
  {
    case WriteDB: msg="write db"; break;
    case WriteRunInfo_: msg="write run info"; break;
    case RunAnalysis: msg = "run analysis"; break;
    case UserDefinedFF: msg = "usr defined FF"; break;
    case CopyMaterials: msg = "copy materials"; break;
    case Running: msg = "running"; break;
    case PreProcessWriteInputFiles: msg = "pre process write unput files"; break;
    case PostProcessWriteInputFiles: msg = "post process write unput files"; break;
    case EndAnalysis: msg = "end analysis"; break;
    case CreateSubDir: msg = "create subdir"; break; // called when no subdirectory could be created as working directory (temp dir read-only?)

    default: msg = "error type not defined"; break;
  }

  if(!msg2.empty())
    msg+=(" : "+msg2);

  printer->error((char*)msg.c_str());
}

//
// sets 'm_FFDIR' member
//
bool IDianaRunner::subdir(const std::string &rootdir)
{
  m_FFDIR = std::string();

  std::string strUserDefinedLocation = UserDefinedLocation();
  if (!strUserDefinedLocation.empty())
  {
    struct _stat buf;
    if (_stat(strUserDefinedLocation.c_str(), &buf) == 0 && buf.st_mode & S_IFDIR)
      m_FFDIR = strUserDefinedLocation;
  }

  if (m_FFDIR.empty())
  {
    if (UseRootDirAsWorkDir())
      m_FFDIR = rootdir;
    else
    {
      if (!CreateSubdir(rootdir))
        return false;
    }
  }
  return true;
}


bool IDianaRunner::create_FF()
{
  const RunParams& p = m_params;

  printer->info("create FF");

  //
  //clear all maps
  //
  for (int i = 0; i < 3; i++)
    m_mpElementProperty[i].clear();
  m_pmpDirections->clear();


  // stores current directory, and restores it when destroyed
  CCurDirSaver cwdsaver;
  if (!subdir(p.rootdir))
  {
    OnError(CreateSubDir, p.rootdir);
    return false;
  }
  chdir(m_FFDIR.c_str());



  if (m_pTempTable)
    OnExtendTemperatureTable(m_pTempTable);
  else
    m_pTempTable = OnCreateTemperatureTable();

  if (m_pConcenTable)
    OnExtendConcentrationTable(m_pConcenTable);
  else
    m_pConcenTable = OnCreateConcentrationTable();

  int progsz = ProgressSize();

  m_progress->AddSteps(progsz + 2);
  m_progress->StatusMessage("Initializing database");

  m_progress->Step(); // 1

            //
            // create and init FF
            //

  m_FF = std::string();
  bool bInitFilos = true;

  if (!UserDefinedFilosFile(p.path, m_FF, bInitFilos))
  {
    OnError(UserDefinedFF);
    return false;
  }

  m_FF_ = new CFilosFile(m_FF, bInitFilos);
  printer->info("--FFDIR--: %s --FF--: %s", m_FFDIR.c_str(),m_FF.c_str());

  if (bInitFilos)
    InitializeFile();
  else
    InitDirectionsMapFromFilos();

  //
  // model from memory to FF
  //

  if (!WriteDatabase(p.title, *p.retrieve))
  {
    OnError(WriteDB);
    return false;
  }

  m_progress->StatusMessage("Copying materials");

  if (!OnCopyMaterials())
  {
    OnError(CopyMaterials);
    return false;
  }

  m_progress->Step();

  std::string sComfileName;
  std::string sDatfileName;

  bool bWriteInputFiles = WriteInputFiles(sComfileName, sDatfileName);

  // Below we give a final status message that should be visible just long enough for the user to read, until we hide the window

  if (bWriteInputFiles)
  {
    m_progress->StatusMessage("Generating input files");

    assert(!sComfileName.empty());
    assert(!sDatfileName.empty());

    if (!PreprocessWriteInputFiles(sComfileName, sDatfileName))
    {
      OnError(PreProcessWriteInputFiles);
      return false;
    }
  }
  else
  {
    m_progress->StatusMessage("Running analysis");
    CalculationResultExpectOKMessage(true);
  }

  if (!WriteRunInfo())
  {
    OnError(WriteRunInfo_);
    return false;
  }

  //
  // temporarily close Filos for DIANA run
  //
  ff::close();

  return true;
}

bool IDianaRunner::run()
{
  printer->info("run");

  const RunParams& p = m_params;

  m_progress->Enable(false);

  if (!OnRunAnalysis(p.dxw, p.bSilent))
  {
  if (!p.bSilent)
      OnError(RunAnalysis);
    return false;
  }

  return true;
}

//
// TODO: verify params have been set
//
bool IDianaRunner::start()
{
  printer->info("start (%s)", m_params.dsa ? "dsa":"non-dsa");

  bool res = create_FF();

  if (!res)
    return false;

  //
  if(m_bCanceled)
    return false;
  //

  res = run();
  if (!res)
    return false;

  //
  if(m_bCanceled)
    return false;
  //

  if (m_params.dsa)
    return true;
  else
  {
    res = end();
    if (!res)
      return false;

    clean();
  }

  return true;
}

bool IDianaRunner::WriteNodes()
{
  assert(!XistIndexed("/NODES", 0));

  //m_progress->StatusMessage("Writing nodes");
  m_progress->StatusMessage("Writing nodes");

  PushDir();
  for(int i = 0; i < Mesh().PointSize(); i++)
  {
    ftn_int_t idx = i + 1;
    ChangeIndexedDir("/NODES/", &idx);

    const geo::IPoint &pt = Mesh().Point(i);

    ftn_double_t coord[3];
  coord[0] = (ftn_double_t)pt.X();
  coord[1] = (ftn_double_t)pt.Y();
  coord[2] = (ftn_double_t)pt.Z();

  if (TranslateElementsToXYOrigin())
  {
      double transX = (Mesh().Min().X() + Mesh().Max().X()) / 2;
      double transY = (Mesh().Min().Y() + Mesh().Max().Y()) / 2;

      coord[0] -= transX;
      coord[1] -= transY;
  }

    PutItemLength("COORD", coord, 3);

    //m_progress->Step();
    m_progress->Step();

  }
  PopDir();

  return true;
}


bool IDianaRunner::OnCopyMaterials()
{
  // Copy all created materials in /MATERI to /INPUT/MATERI
  char materi[] = "/MATERI";
  char inputmateri[] = "/INPUT/MATERI";
  DSCopyTree(materi, strlen(materi), 0, inputmateri, strlen(inputmateri), 0, 1);

  return true;
}

bool IDianaRunner::OnWriteMesh()
{
  assert(Mesh().PointSize() > 0);
  assert(Mesh().ElementSize() > 0);

  if (!WriteNodes())
    return false;

  if (!WriteElements())
    return false;

  return true;
}


static void WriteSupportItems(ftn_int_t nodeidx, const char* type, ftn_int_t diridx)
{
  int iType = Inquire("NODES", "TYPE");
  if(iType >= 100)
  PutItemLength("NODES", &nodeidx, 1);
  else
  PutItem("NODES", &nodeidx);

  PutCharItem("TYPE", type);

  if(diridx > 0)
  PutItem("DIRECT", &diridx);
}

void IDianaRunner::WriteSupport(int nodeindex, const char* type, const geo::IVector& vecDirection)
{
  ftn_int_t diridx = 0;
  if(!vecDirection.Empty())
  diridx = (ftn_int_t) AddDirection(vecDirection);
  ftn_int_t nodeidx = (ftn_int_t) nodeindex + 1;

  int offset = 0;
  if(XistIndexed("/SUPPOR", 0))
  offset = Inquire("/SUPPOR", "DIM");

  ftn_int_t idx = offset + 1;

  PushDir();
  ChangeIndexedDir("/SUPPOR/", &idx);
  WriteSupportItems(nodeidx, type, diridx);
  PopDir();

  offset = 0;
  if(XistIndexed("/INPUT/SUPPOR", 0))
  offset = Inquire("/INPUT/SUPPOR", "DIM");

  idx = offset + 1;

  PushDir();
  ChangeIndexedDir("/INPUT/SUPPOR/", &idx);
  PutCharItem("NAME", "support");
  if(XistIndexed("USRDOF", 0))
  RemoveIndexedItem("USRDOF", 0);
  ChangeDir("USRDOF");
  WriteSupportItems(nodeidx, type, diridx);
  PopDir();
}

bool IDianaRunner::OnWriteSupports()
{
  m_progress->StatusMessage("Writing supports");

  for(int i = 0; i < Mesh().SupportSize(); i++)
  {
    const geo::ISupport &support = Mesh().Support(i);

    assert(support.DirectionSize() > 0);
    for(size_t j = 0; j < support.DirectionSize(); j++)
      WriteSupport(support.NodeIndex(), support.Type().c_str(), support.Direction(j));

    m_progress->Step();
  }

  return true;
}

bool IDianaRunner::OnApplyLoads(CLoadManager &/*manager*/)
{
  return true;
}

void IDianaRunner::WriteTyingInfo(const geo::CTying& tying, const char* type, bool bDirected)
{
  ftn_int_t nodeidx1 = (ftn_int_t) (tying.SlaveIndex() + 1);
  PutItem("NODES", &nodeidx1);

  if(bDirected)
  {
    ftn_int_t diridx = (ftn_int_t) AddDirection(tying.SlaveDirection());
    PutItem("DIRECT", &diridx);
  }

  if(type)
    PutCharItem("TYPE", type);
  else
  PutCharItem("TYPE", tying.SlaveTypeString().c_str());

  for(int j = 0; j < tying.MasterSize(); j++)
  {
    PushDir();
    ftn_int_t idx = (ftn_int_t) (j + 1);
    ChangeIndexedDir("MASTER", &idx);

    ftn_int_t nodeidx2 = (ftn_int_t) (tying.MasterIndex(j) + 1);
    PutItem("NODES", &nodeidx2);

  if(bDirected)
  {
      ftn_int_t diridx = (ftn_int_t) AddDirection(tying.MasterDirection(j));
      PutItem("DIRECT", &diridx);
  }

  if(type)
      PutCharItem("TYPE", type);
  else
      PutCharItem("TYPE", tying.MasterTypeString(j).c_str());

    ftn_double_t factor = (ftn_double_t) tying.MasterFactor(j);
    PutItem("FACTOR", &factor);

    PopDir();
  }
}

void IDianaRunner::WriteTying(const geo::CTying& tying, const char* type, bool bDirected)
{
  int offset = 0;
  if(XistIndexed("/TYINGS", 0))
  offset = Inquire("/TYINGS", "DIM");

  ftn_int_t idx = offset + 1;

  PushDir();

  ChangeIndexedDir("/TYINGS/", &idx);
  WriteTyingInfo(tying, type, bDirected);

  // create symbolic link from /NODES/TYINGS to this index in /TYINGS
#if 0
  PushDir();

  ftn_int_t nodeidx = (ftn_int_t) (tying.SlaveIndex() + 1);
  assert(XistIndexed("/NODES/", &nodeidx));
  ChangeIndexedDir("/NODES/", &nodeidx);
  ftn_int_t numlinks = 0;
  if(XistIndexed("TYINGS", 0)) numlinks = Inquire("TYINGS", "DIM");
  ftn_int_t linkidxs[2];

  linkidxs[0] = nodeidx;
  linkidxs[1] = numlinks + 1;

//	MakeSymLink("/TYINGS/", &idx, "/NODES/TYINGS/", linkidxs);

  PopDir();
#endif

  PopDir();

  offset = 0;
  if(XistIndexed("/INPUT/TYINGS", 0))
  offset = Inquire("/INPUT/TYINGS", "DIM");

  idx = offset + 1;

  PushDir();
  ChangeIndexedDir("/INPUT/TYINGS/", &idx);
  PutCharItem("NAME", "tying");
  if(XistIndexed("USERTY", 0))
  RemoveIndexedItem("USERTY", 0);
  ChangeDir("USERTY");
  WriteTyingInfo(tying, type, bDirected);
  PopDir();
}

bool IDianaRunner::OnWriteTyings()
{
  m_progress->StatusMessage("Writing tyings");

  for(int i = 0; i < Mesh().TyingSize(); i++)
  {
    const geo::CTying &tying = Mesh().Tying(i);
  WriteTying(tying);
    m_progress->Step();
  }

  return true;
}

bool IDianaRunner::OnWriteLoads()
{
  return m_LoadManager.WriteFilos();
}

bool IDianaRunner::WriteCommands()
{
  bool bRet = true;

  int svfi = SetFunctionInfoFlags(1, DS_FUNC_WRITE);
  PushDir();

  if(XistIndexed("/CMNDS/SEGINF", 0))
    RemoveIndexedItem("/CMNDS/SEGINF", 0);

  ftn_int_t idx = 10;
  ChangeIndexedDir("/CMNDS/SEGINF/", &idx);

  //if(!OnWriteCommands(progress)) 
  if (!OnWriteCommands())
    bRet = false;;

  PopDir();
  SetFunctionInfoFlags(svfi, DS_FUNC_WRITE);

  return bRet;
}

bool IDianaRunner::OnWriteCommands()
{
  // to be written by derived class
  return true;
}

bool IDianaRunner::EndWriteCommands()
{
  PushDir();

  ChangeDir("/CMNDS");
  ftn_int_t idx = Inquire("SEGINF", "DIM");

  if(idx < 0) idx = 0;

  // write END segment 10 higher than DIM
  idx += 10;

  assert(!XistIndexed("SEGINF/", &idx));
  ChangeIndexedDir("SEGINF/", &idx);

  PutCharItem("MODULE", "DC");
  PutCharItem("SEGMEN", "END");

  PopDir();

  int svfi = SetFunctionInfoFlags(1, DS_FUNC_WRITE);
  idx = 10;
  PutItem( "/CMNDS/JCLPOS", &idx );
  SetFunctionInfoFlags(svfi, DS_FUNC_WRITE);

  return true;
}

bool IDianaRunner::WriteRunInfo()
{
  PushDir();

  // create a new instance of /jobinf/run()
  ChangeDir("/JOBINF");
  ftn_int_t idx = Inquire("RUN", "DIM");
  if(idx < 0) idx = 0;
  ++idx;

  assert(!XistIndexed("RUN/", &idx));
  ChangeIndexedDir("RUN/", &idx);

  ftn_double_t value = 0;
  PutItem("ICPU", &value);
  PutItem("IIO", &value);

  PopDir();

  return true;
}

void IDianaRunner::AnalysisSuccess(bool bAnalysisSuccess)
{
}

bool IDianaRunner::OnEndWriteCommands(const std::string &/*title*/)
{
  return EndWriteCommands();
}

bool IDianaRunner::OnRunAnalysis(IDianaXWrapper* /*dianaXWrapper*/, bool /* bSilent */)
{
  return true;
}

bool IDianaRunner::OnEndAnalysis(const QString& /*getPathName*/,
  ISaveModel& /*saveModel*/)
{
  return true;
}

// group names
std::string IDianaRunner::GroupName(const geo::IElementSet &/*group*/, int nIndex)
{
  std::stringstream stream;
  stream << "ELMSET" << (nIndex + 1);

  return stream.str();
}

std::string IDianaRunner::GroupName(const geo::CMeshNodeGroup &group)
{
  std::stringstream stream;
  stream << "NODESET" << (group.Index() + 1);

  return stream.str();
}

static const char* elment_property_inq[] = { "/MATERI", "/GEOMET", "/DATA" };
static const char* elment_property_dir[] = { "/MATERI/", "/GEOMET/", "/DATA/" };

// returns the 1-based index of the element property, for use in Filos
// returns 0 on error
int IDianaRunner::AddElementProperty(const IElementProperty &prop, int type, bool comparePointers)
{
  if (comparePointers)
  {
  bool bSuccess = true;

  std::pair<IElementProperty::TElementPropertyPointerMap::iterator, bool> prInsert =
      m_mpElementPropertyPointer[type].insert(std::make_pair(&prop, 0));

  if (prInsert.second)
  {
      // material did not exist yet, write it to the database
      ftn_int_t idx = Inquire(elment_property_inq[type], "DIM");

      if (idx < 0) idx = 0;
      idx++;

      prInsert.first->second = (int)idx;

      PushDir();

      assert(!XistIndexed(elment_property_dir[type], &idx));
      ChangeIndexedDir(elment_property_dir[type], &idx);
      if (!prop.WriteFilos(*this)) bSuccess = false;

      PopDir();
  }

  if (!bSuccess) return 0;

  assert(prInsert.first->second > 0);
  return prInsert.first->second;

  }
  else
  {
  bool bSuccess = true;

  std::pair<IElementProperty::TElementPropertyMap::iterator, bool> prInsert =
      m_mpElementProperty[type].insert(std::make_pair(&prop, 0));

  if (prInsert.second)
  {
      // material did not exist yet, write it to the database
      ftn_int_t idx = Inquire(elment_property_inq[type], "DIM");

      if (idx < 0) idx = 0;
      idx++;

      prInsert.first->second = (int)idx;

      PushDir();

      assert(!XistIndexed(elment_property_dir[type], &idx));
      ChangeIndexedDir(elment_property_dir[type], &idx);
      if (!prop.WriteFilos(*this)) bSuccess = false;

      PopDir();
  }

  if (!bSuccess) return 0;

  assert(prInsert.first->second > 0);
  return prInsert.first->second;
  }
}

// returns the 1-based index of the direction, for use in Filos
int IDianaRunner::AddDirection(const geo::IVector &dir)
{
  geo::CVector unitdir = dir.UnitVector();
  std::pair<TDirectionMap::iterator, bool> prInsert =
    m_pmpDirections->insert(TDirectionMap::value_type(unitdir, 0));

  if(prInsert.second)
  {
    // direction did not exist yet, write it to the database
    ftn_int_t idx = Inquire("/DIRECT", "DIM");
    
    if(idx < 0) idx = 0;
    idx++;

    prInsert.first->second = (int) idx;

    PushDir();

    assert(!XistIndexed("/DIRECT/", &idx));
    ChangeIndexedDir("/DIRECT/", &idx);

    ftn_double_t vec[3];
    vec[0] = unitdir.X();
    vec[1] = unitdir.Y();
    vec[2] = unitdir.Z();

    PutItemLength("XYZ", vec, 3);

    PopDir();
  }

  assert(prInsert.first->second > 0);
  return prInsert.first->second;
}

void IDianaRunner::Cleanup(const std::string &subdir)
{
  printer->info("--cleanup-- %s", subdir.c_str());

  bool result = true;
  QDir dir(subdir.c_str());

  delete m_FF_;
  m_FF_ = nullptr;

  if (dir.exists(subdir.c_str()))
  {
  Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot |
      QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files,
      QDir::DirsFirst))
  {
      if (info.isDir())
      {
    Cleanup(info.absoluteFilePath().toStdString());
      }
      else
      {
    result = QFile::remove(info.absoluteFilePath());
      }

      if (!result)
      {
    return;
      }
  }

  result = dir.rmdir(subdir.c_str());
  }
}

const std::string& IDianaRunner::FF() const
{
  return m_FF;
}

CFixedPotential& IDianaRunner::FixedPotentialNodeSet()
{
  if(!m_pFixedPotential)
  m_pFixedPotential = new CFixedPotential;

  return *m_pFixedPotential;
}

bool IDianaRunner::CalculationResultExpectOKMessage() const
{
  return m_bCalculationResultExpectOKMessage;
}

void IDianaRunner::CalculationResultExpectOKMessage(bool flag)
{
  m_bCalculationResultExpectOKMessage = flag;
}

void IDianaRunner::GetResultInfo(bool& bHaveResults, bool& bCalculationResult, bool bSave)
{
  printer->info("get results info : %s", bSave ? "from FF" : "saved values");

  if (bSave)
  {
  GetResultInfo( 
    FFDIR().c_str(), 
    FF().c_str(), 
    bHaveResults, 
    bCalculationResult, 
    CalculationResultExpectOKMessage());

  m_bHaveResults = bHaveResults;
  m_bCalculationResult = bCalculationResult;
  }
  else
  {
  bHaveResults = m_bHaveResults;
  bCalculationResult = m_bCalculationResult;
  }
}

void IDianaRunner::GetResultInfo(
  const char *workingdir, 
  const char *filosfile, 
  bool& bHaveResults, 
  bool& bCalculationResult, 
  bool expectOkMessage)
{
  //printer->info("get results info : FFDIR:%s : FF:%s", workingdir, filosfile);

  bHaveResults = bCalculationResult = false;

  	QString current = QDir::currentPath();  
  QDir::setCurrent( workingdir );

#ifdef _WIN32
  struct _stat64 buf;


  if (_stat64(filosfile, &buf) == 0)
#else
  struct stat buf;

  if (stat(filosfile, &buf) == 0)
#endif
  {
    // 
    // mcr 2020-07-14
    //
    // TODO: what if we close it just in case?
    //
  assert(!fcisop_()); // Filos must have been closed
  ff::open(filosfile, O_RDONLY);

  if (XistIndexed("/JOBINF/RUN", 0))
  {
      ftn_int_t index = Inquire("/JOBINF/RUN", "DIM");

      PushDir();
      ChangeIndexedDir("/JOBINF/RUN/", &index);

      if (expectOkMessage)
      {

    const char *OKMessage = "Geomec job finished";
    char buffer[1024];
    ftn_int_t status_index = 0;

    if (GetIndexedCharItemIfExist("STATUS", &status_index, buffer, 1024))
    {
          bCalculationResult = !strncmp(buffer, OKMessage, 20);
    }

    if (bCalculationResult)
    {
          if (!XistIndexed("/RESULT", 0))
      bCalculationResult = false;
          else
      bHaveResults = true;
    }
      }
      else
      {
    bCalculationResult = !XistIndexed("STATUS", 0);
      }

      PopDir();
  }
  ff::close();
  }
  QDir::setCurrent( current );
  printer->info("get results info : FFDIR:%s : FF:%s -> have_results:%d : calculation_results:%d", workingdir, filosfile, bHaveResults, bCalculationResult);
  //printer->info("get results info : have_results:%d : calculation_results:%d", bHaveResults, bCalculationResult);
}


void IDianaRunner::Cancel()
{
  m_bCanceled = true;
}

bool IDianaRunner::Canceled() const
{
  return m_bCanceled;
}

void IDianaRunner::LicenseRetry(int nLicenseRetry)
{
  m_nLicenseRetry = nLicenseRetry;
}

int IDianaRunner::LicenseRetry() const
{
  return m_nLicenseRetry;
}

bool IDianaRunner::PreprocessWriteInputFiles(std::string &sComfileName, std::string &sDatfileName)
{
  // allow modification of the filenames
  if(!OnModifyInputFileNames(sComfileName, sDatfileName))
  return false;

  // first save the comfile
  std::string sInitialComFileText = InitialComFileText();
  bool bExportComFileWithDefaults = ExportCommandFileWithDefaults();
  if(!SaveComFile(sComfileName.c_str(), sInitialComFileText.empty() ? 0 : sInitialComFileText.c_str(), bExportComFileWithDefaults ? 1 : 0)) return false;

  int svfi = SetFunctionInfoFlags(1, DS_FUNC_WRITE);
  PushDir();

  // remove the actual commands and replace them for input
  ChangeDir("/CMNDS");
  RemoveIndexedItem("SEGINF", 0);

  ftn_int_t idx = 10;

  ChangeIndexedDir("/CMNDS/SEGINF/", &idx);
  ChangeDir("INPUT");
  ChangeDir("REMAKE");

  char *filename = DiStrsave(sDatfileName.c_str());
  char *pfname = strrchr(filename, '\\');
  if(!pfname) pfname = strrchr(filename, '/');

  if(pfname)
  {
    *pfname = 0;
    pfname++;
    m_sDatFileDir = filename;
    m_sDatFileName = pfname;
  }
  else
  {
    m_sDatFileName = filename;
    m_sDatFileDir = ".";
  }

  PutCharItem("FILE", m_sDatFileName.c_str());

  DiFree(filename, "IDianaRunner::PreprocessWriteInputFiles");

  if(!EndWriteCommands()) return false;

  PopDir();

  if(!OnEndPreprocessWriteInputFiles())
  return false;

  SetFunctionInfoFlags(svfi, DS_FUNC_WRITE);

  return true;
}

bool IDianaRunner::OnEndPreprocessWriteInputFiles()
{
  return true;
}

bool IDianaRunner::PostprocessWriteInputFiles(const std::string& sComfileName, const std::string &sDatfileName)
{
  // we need to move the generated .dat file to the desired location
  // unless the desired location is the generated one
  if ( strcmp( sDatfileName.c_str(), m_sDatFileName.c_str() ) == 0 ) return true;

  FILE *fpin = fopen(m_sDatFileName.c_str(), "r");
  if(!fpin) return false;

  FILE *fpout = fopen(sDatfileName.c_str(), "w");
  if(!fpout)
  {
    fclose(fpin);
    return false;
  }

  char buf[BS];

  while(fgets(buf, BS - 1, fpin)) fputs(buf, fpout);

  fclose(fpin);
  fclose(fpout);

  unlink(m_sDatFileName.c_str());

  if(!OnEndPostprocessWriteInputFiles(sComfileName, sDatfileName))
  return false;

  return true;
}

bool IDianaRunner::OnEndPostprocessWriteInputFiles(const std::string& /*sComFileName*/, const std::string& /*sDatFileName*/)
{
  return true;
}

/*!
 return element data for the given element
 return 0 if no data attached
*/
const IElementData *IDianaRunner::ElementData(const geo::IElement &/*element*/) const
{
  return 0;
}

/*!
 return element geometry for the given element
 return 0 if no geometry attached
*/
const IElementData *IDianaRunner::ElementGeometry(const geo::IElement &/*element*/) const
{
  return 0;
}

// returns the 1 based material index for the given material
int IDianaRunner::FilosMaterialIndex(const IMaterial &material) const
{
  IElementProperty::TElementPropertyMap::const_iterator it;
  it = m_mpElementProperty[0].find(&material);

  assert(it != m_mpElementProperty[0].end());

  return (*it).second;
}


// For a temper table
ITemperatureTable *IDianaRunner::OnCreateTemperatureTable() const
{
  return 0;
}

void IDianaRunner::OnExtendTemperatureTable(ITemperatureTable *) const
{
}

void IDianaRunner::OnDeleteTemperatureTable()
{
  if (m_pTempTable)
  delete m_pTempTable;
}

ITemperatureTable *IDianaRunner::TemperatureTable()
{
  return m_pTempTable;
}

void IDianaRunner::TemperatureTable(ITemperatureTable *pTable)
{
  m_pTempTable = pTable;
  if (m_pTempTable)
  m_pTempTable->SetRunner(this);
}

IConcentrationTable *IDianaRunner::OnCreateConcentrationTable() const
{
  return 0;
}

void IDianaRunner::OnExtendConcentrationTable(IConcentrationTable *) const
{
}

void IDianaRunner::OnDeleteConcentrationTable()
{
  if (m_pConcenTable)
  delete m_pConcenTable;
}

IConcentrationTable *IDianaRunner::ConcentrationTable()
{
  return m_pConcenTable;
}

void IDianaRunner::ConcentrationTable(IConcentrationTable *pTable)
{
  m_pConcenTable = pTable;
  if (m_pConcenTable)
  m_pConcenTable->SetRunner(this);
}

std::string IDianaRunner::UserDefinedLocation() const
{
  return std::string();
}

std::string IDianaRunner::InitialComFileText() const
{
  return "*FILOS\n INITIA\n*INPUT\n";
}

bool IDianaRunner::ExportCommandFileWithDefaults() const
{
  return false;
}

void IDianaRunner::executeCommandInGeomec() const // overrides are for casing (GEOTYP=UPDATE) which should not be combined with the HX24L1 setting
{
  SetActive("HX24L1", FTN_TRUE);
}

//// CCurDirSaver implementation
#define PATHLEN 1024

///////////////////////////////////
//
//			CCurDirSaver [Ini]
//
///////////////////////////////////

IDianaRunner::CCurDirSaver::CCurDirSaver()
{
  m_strOldCwd = GetCurrentDir();
}

IDianaRunner::CCurDirSaver::~CCurDirSaver()
{
  if(m_strOldCwd)
  {
    chdir(m_strOldCwd);
    DiFree(m_strOldCwd, "IDianaRunner::CCurDirSaver::~CCurDirSaver");
  }
}
///////////////////////////////////
//
//			CCurDirSaver [End]
//
///////////////////////////////////


char *IDianaRunner::GetCurrentDir()
{
  char *cwd;
  char *ret;
  int dirsize = PATHLEN;
  cwd = (char *) DiMalloc(dirsize * sizeof (char), "IDianaRunner::CreateSubdir");

  ret = _getcwd(cwd, dirsize);

  while(!ret && errno == ERANGE)
  {
    dirsize *= 2;
    cwd = (char *) DiRealloc(cwd, 0, dirsize * sizeof (char), "IDianaRunner::CreateSubdir");

    ret = _getcwd(cwd, dirsize);
  }

  return ret;
}


} // namespace dia
