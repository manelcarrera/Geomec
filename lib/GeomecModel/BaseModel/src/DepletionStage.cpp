// DepletionStage.cpp: implementation of the CDepletionStage class.
//
//////////////////////////////////////////////////////////////////////
#include "ModelBase.h"
#include "DepletionStage.h"
#include "ResultRegister.h"
#include "BranchState.h"
#include "resourceIDI.h"
#include "resourceIDS.h"
#include "StreamVersion.h"

/*!
\class CDepletionStage
\brief The depletion stage represents a time step in the analyis

The depletion stage is a time step defined by a name, a year and month. Depletion stages are organized
as linked list. The function CDepletionStage::Next() and CDepletionStage::Previous() give acces to decending and 
ascending depletion stages. Non-linear parameter with the prefix NL.. are also stored on the CDepletionStage. 
*/

const char* CDepletionStage::m_output_type_cp[] = { "None", "Results", "Branch", "Phase" };

CDepletionStage::CDepletionStage(CDepletionStageEntry& entry)
: CStorageNode(entry.Model()), 
  m_pPrevious(0), 
  m_pNext(0), 
  m_Time(QDateTime::currentDateTime().date().year(), QDateTime::currentDateTime().date().month()),
  m_entry(entry),
  m_bMarkedAsInitial(false),
  m_output_type(DefaultOutputType())
{
  SetDefaultNonLinParams();
  Index(-1);
}

//##ModelId=3BC55D630297
CDepletionStage::CDepletionStage(const QString& strName, 
                 CDepletionStageEntry& entry,
                 int nSteps,
                 const double& dStepScalingFactor,
                 const double& dConvNormVal,
                 int nIterationSteps,
                 eConvergenceType convergence_type,
                 eIterationScheme iteration_scheme,
                 eOutputType output_type,
                 bool bAutoStepSizes,
                 double dMaxAutoStepSize)
: CStorageNode(strName, entry.Model()), 
  m_pPrevious(0),
  m_pNext(0),
  m_Time(QDateTime::currentDateTime().date().year(), QDateTime::currentDateTime().date().month()),
  m_entry(entry),
  m_bMarkedAsInitial(false),
  m_nSteps(nSteps),
  m_dStepScalingFactor(dStepScalingFactor),
  m_dConvNormVal(dConvNormVal),
  m_nIterationSteps(nIterationSteps),
  m_convergence_type(convergence_type),
  m_iteration_scheme(iteration_scheme),
  m_output_type(output_type),
  m_bAutoStepSizes(bAutoStepSizes),
  m_dMaxAutoStepSize(dMaxAutoStepSize)
{
  // Creation of the first depletion stage (initial)
  Index(0);

  // Register first depletionstage in the entry
  m_entry.m_pFirstStage = this;

  // Link to the entry
  reParent(&m_entry);

  assert(Initial());
}

//##ModelId=3BC55D63029A
CDepletionStage::CDepletionStage(const QString& strName, 
                 CDepletionStage& previous, 
                 int nSteps,
                 const double& dStepScalingFactor,
                 const double& dConvNormVal,
                 int nIterationSteps,
                 eConvergenceType convergence_type,
                 eIterationScheme iteration_scheme,
                 eOutputType output_type,
                 bool bAutoStepSizes,
                 double dMaxAutoStepSize)
: CStorageNode(strName, previous.Model()), 
  m_pPrevious(&previous), 
  m_pNext(0), 
  m_Time(previous.Time().GetYear() + 1, previous.Time().GetMonth()),
  m_entry(previous.Entry()),
  m_bMarkedAsInitial(false),
  m_nSteps(nSteps),
  m_dStepScalingFactor(dStepScalingFactor),
  m_dConvNormVal(dConvNormVal),
  m_nIterationSteps(nIterationSteps),
  m_convergence_type(convergence_type),
  m_iteration_scheme(iteration_scheme),
  m_output_type(output_type),
  m_bAutoStepSizes(bAutoStepSizes),
  m_dMaxAutoStepSize(dMaxAutoStepSize)
{
// The previous depletion stage should be valid ...
  assert(Previous().Index() > -1);
  Index(Previous().Index() + 1);

// Link to previous entry
  assert(Previous().Last());
  Previous().m_pNext = this;
  LinkTo(Previous());

// Link to entry
  reParent(&m_entry);
}

// use this constructor to insert a stage in between two stages.
CDepletionStage::CDepletionStage(const QString& strName, 
                 CDepletionStage *previous, 
                 CDepletionStage *next,
                 const CGeomecTime &time,
                 int nSteps,
                 const double& dStepScalingFactor,
                 const double& dConvNormVal,
                 int nIterationSteps,
                 eConvergenceType convergence_type,
                 eIterationScheme iteration_scheme,
                 eOutputType output_type,
                 bool bAutoStepSizes,
                 double dMaxAutoStepSize)
: CStorageNode(strName, next->Model()), 
  m_pPrevious(previous), 
  m_pNext(next), 
  m_Time(time),
  m_entry(next->Entry()),
  m_bMarkedAsInitial(false),
  m_nSteps(nSteps),
  m_dStepScalingFactor(dStepScalingFactor),
  m_dConvNormVal(dConvNormVal),
  m_nIterationSteps(nIterationSteps),
  m_convergence_type(convergence_type),
  m_iteration_scheme(iteration_scheme),
  m_output_type(output_type),
  m_bAutoStepSizes(bAutoStepSizes),
  m_dMaxAutoStepSize(dMaxAutoStepSize)
{
  // set the index
  if(!previous)
  {
  Index(0);

  if(next->m_bMarkedAsInitial)
  {
      m_bMarkedAsInitial = true;
      next->m_bMarkedAsInitial = false;
  }

  m_entry.m_pFirstStage = this;
  }
  else
  Index(Previous().Index() + 1);
  
  if(previous)
  {
  Next().UnLink(*previous);
  // Link to previous entry
    Previous().m_pNext = this;
    LinkTo(Previous());
  }

  Next().m_pPrevious = this;
  Next().LinkTo(*this);

  CDepletionStage *nextStage = this;
  int currentIndex = Index();
  // Fix indices 
  do
  {
  nextStage = &nextStage->Next();
  nextStage->Index(++currentIndex);
  }while(!nextStage->Last());
  
  // Link to entry
  reParent(&m_entry);
}

CDepletionStage::CDepletionStage(const CDepletionStage& rhs)
: CStorageNode(rhs),
  m_pPrevious(rhs.m_pPrevious), 
  m_pNext(rhs.m_pNext), 
  m_Time(rhs.m_Time),
  m_entry(rhs.m_entry),
  m_bMarkedAsInitial(rhs.m_bMarkedAsInitial),
  m_nSteps(rhs.m_nSteps),
  m_dStepScalingFactor(rhs.m_dStepScalingFactor),
  m_dConvNormVal(rhs.m_dConvNormVal),
  m_nIterationSteps(rhs.m_nIterationSteps),
  m_convergence_type(rhs.m_convergence_type),
  m_iteration_scheme(rhs.m_iteration_scheme),
  m_output_type(rhs.m_output_type),
  m_bAutoStepSizes(rhs.m_bAutoStepSizes),
  m_dMaxAutoStepSize(rhs.m_dMaxAutoStepSize)
{
}

const CDepletionStageEntry& CDepletionStage::Entry() const
{
  return m_entry;
}

CDepletionStageEntry& CDepletionStage::Entry()
{
  return m_entry;
}

int CDepletionStage::Index() const
{
  int idx = 0;
  const CDepletionStage* pStage = this;
  while((pStage != 0) && !pStage->Initial())
  {
    pStage = &pStage->Previous();
    ++idx;
  }

  return idx;
}

CDepletionStage& CDepletionStage::operator=(const CDepletionStage& rhs)
{
  CStorageNode::operator =(rhs);

  CModelBase& model = dynamic_cast<CModelBase&>(Model());
  if((m_Time != rhs.m_Time) ||
     (m_nSteps != rhs.m_nSteps) ||
     (m_dStepScalingFactor != rhs.m_dStepScalingFactor) ||
     (m_dConvNormVal != rhs.m_dConvNormVal) ||
     (m_nIterationSteps != rhs.m_nIterationSteps) ||
     (m_convergence_type != rhs.m_convergence_type) ||
     (m_iteration_scheme != rhs.m_iteration_scheme) ||
     (m_bAutoStepSizes != rhs.m_bAutoStepSizes) ||
     (fabs(m_dMaxAutoStepSize - rhs.m_dMaxAutoStepSize) >= 1e-6))
  {
    model.ResultRegister().ClearNonLinear(true);
  }
  
  m_Time = rhs.m_Time;
  m_nSteps = rhs.m_nSteps;
  m_dStepScalingFactor = rhs.m_dStepScalingFactor;
  m_dConvNormVal = rhs.m_dConvNormVal;
  m_nIterationSteps = rhs.m_nIterationSteps;
  m_convergence_type = rhs.m_convergence_type;
  m_iteration_scheme = rhs.m_iteration_scheme;
  m_bAutoStepSizes = rhs.m_bAutoStepSizes;
  m_dMaxAutoStepSize = rhs.m_dMaxAutoStepSize;

  return *this;
}

bool CDepletionStage::operator==(const CDepletionStage& rhs) const
{
  if(!CStorageNode::operator ==(rhs))
    return false;

  return ((m_Time == rhs.m_Time) &&
      (m_nSteps == rhs.m_nSteps) &&
      (m_dStepScalingFactor == rhs.m_dStepScalingFactor) &&
      (m_dConvNormVal == rhs.m_dConvNormVal) &&
      (m_nIterationSteps == rhs.m_nIterationSteps) &&
      (m_convergence_type == rhs.m_convergence_type) &&
      (m_iteration_scheme == rhs.m_iteration_scheme) &&
      (m_bAutoStepSizes == rhs.m_bAutoStepSizes) &&
      (fabs(m_dMaxAutoStepSize - rhs.m_dMaxAutoStepSize) < 1e-6));
}

void CDepletionStage::SetNonLinParams(bool bAutoStepSizes,
                   double dMaxAutoStepSize,
                   int nSteps,
                   const double& dStepScalingFactor,
                   const double& dConvNormVal,
                   int nIterationSteps,
                   eConvergenceType convergence_type,
                   eIterationScheme iteration_scheme)
{
  m_bAutoStepSizes = bAutoStepSizes;
  m_dMaxAutoStepSize = dMaxAutoStepSize;
  m_nSteps = nSteps;
  m_dStepScalingFactor = dStepScalingFactor;
  m_dConvNormVal = dConvNormVal;
  m_nIterationSteps = nIterationSteps;
  m_convergence_type = convergence_type;
  m_iteration_scheme = iteration_scheme;
}

void CDepletionStage::SetDefaultNonLinParams()
{
  m_nSteps = Initial() ? 1 : 10;
  m_dStepScalingFactor = 1.;
  m_dConvNormVal = 0.0001;
  m_nIterationSteps = 5;
  m_convergence_type = FORCE;
  m_iteration_scheme = REGULAR;
  m_bAutoStepSizes = false;
  m_dMaxAutoStepSize = 0.3;
}	

int CDepletionStage::NLSteps() const
{
  return m_nSteps;
}

const double& CDepletionStage::NLStepScalingFactor() const
{
  return m_dStepScalingFactor;
}

const double& CDepletionStage::NLConvNormVal() const
{
  return m_dConvNormVal;
}

int CDepletionStage::NLIterationSteps() const
{
  return m_nIterationSteps;
}

CDepletionStage::eConvergenceType  CDepletionStage::NLConvergenceType() const
{
  return m_convergence_type;
}

CDepletionStage::eIterationScheme CDepletionStage::NLIterationScheme() const
{
  return m_iteration_scheme;
}

void CDepletionStage::setNLIterationScheme(eIterationScheme itScheme)
{
  m_iteration_scheme = itScheme;
}

bool CDepletionStage::AutomaticStepSizes() const
{
  return (/*Initial() ? false : */m_bAutoStepSizes);
}

double CDepletionStage::MaxAutomaticStepSize() const
{
  return m_dMaxAutoStepSize;
}

void CDepletionStage::setOutputType(eOutputType output_type)
{
  m_output_type = output_type;
  m_entry.Modified();
}

CDepletionStage::eOutputType CDepletionStage::OutputType() const
{
  return m_output_type;
}

bool CDepletionStage::IsPhaseStartStage() const
{
  return Initial() || Previous().OutputType() == BRANCH || Previous().OutputType() == PHASE;
}

//##ModelId=3BC55D6302A8
CDepletionStage::~CDepletionStage()
{
  if(!IsCopy())
  {
    if(m_bMarkedAsInitial && !Initial())
      InitialStage().MarkAsInitial();

    if(m_pNext)
    {
      UnLink(*m_pNext);
      m_pNext->m_pPrevious = m_pPrevious;
    }

    if(m_pPrevious)
    {
      UnLink(*m_pPrevious);
      m_pPrevious->m_pNext = m_pNext;
    }
    
    if(m_pNext && m_pPrevious)
      m_pNext->LinkTo(*m_pPrevious);

    // can not have the last stage marked as initial
    if(m_entry.LastStage().IsMarkedAsInitial())
      m_entry.InitialStage().MarkAsInitial();

  CModelBase& model = dynamic_cast<CModelBase&>(Model());

  if (model.BranchState().IsBranch() && &model.BranchState().BranchStage() == this)
  {
      CDepletionStage *findBranch = m_pPrevious;
      while (findBranch != 0 && findBranch->OutputType() !=  CDepletionStage::BRANCH)
    findBranch = findBranch->m_pPrevious;

      if (findBranch)
    model.BranchState().SetBranchStage(*findBranch);
      else
    model.BranchState().ClearBranch();
  }
  }
}

//##ModelId=3BC55D6302C7
bool CDepletionStage::Initial() const
{
  return !m_pPrevious;
}

//##ModelId=3BC55D6302C5
bool CDepletionStage::Last() const
{
  return !m_pNext;
}

//##ModelId=3BC55D6302BA
const CDepletionStage& CDepletionStage::Next() const
{
  assert(!Last());
  return *m_pNext;
}

//##ModelId=3BC55D6302BC
CDepletionStage& CDepletionStage::Next()
{
  assert(!Last());
  return *m_pNext;
}

const CDepletionStage* CDepletionStage::Next_(){ return m_pNext; }
const CDepletionStage* CDepletionStage::Previous_(){ return m_pPrevious; }

//##ModelId=3BC55D6302B7
const CDepletionStage& CDepletionStage::Previous() const
{
  assert(!Initial());
  return *m_pPrevious;
}

//##ModelId=3BC55D6302B9
CDepletionStage& CDepletionStage::Previous()
{
  assert(!Initial());
  return *m_pPrevious;
}

//##ModelId=3BC55D6302B5
unsigned int CDepletionStage::IconId() const
{
  CModelBase& model = (CModelBase&)Model();

  if(m_bMarkedAsInitial)
  {
    if(model.BranchState().IsBranch() && &model.BranchState().BranchStage() == this)
      return IDI_DEPLETION_STAGE_INITIAL_BRANCH;
    else if(OutputType() == NONE)
      return IDI_DEPLETION_STAGE_INITIAL_NONE;
    else
      return IDI_DEPLETION_STAGE_INITIAL;
  }

  if(model.BranchState().IsBranch() && &model.BranchState().BranchStage() == this)
    return IDI_DEPLETION_STAGE_BRANCH;

  if(OutputType() == NONE)
    return IDI_DEPLETION_STAGE_NONE;

  return IDI_DEPLETION_STAGE;
}

//##ModelId=3BC55D630287
unsigned int CDepletionStage::TypeId() const
{
  return 0;
}

//##ModelId=3BE7AEC5011E
const CGeomecTime &CDepletionStage::Time() const
{
  return m_Time;
}

//##ModelId=3BE7AEC500FF
void CDepletionStage::Time(const CGeomecTime &time)
{
  CModelBase& model = dynamic_cast<CModelBase&>(Model());
  if((m_Time != time) && !IsCopy())
    model.ResultRegister().ClearNonLinear();
  m_Time = time;
  Modified();
}

//##ModelId=3BE7AEC5013D
bool CDepletionStage::Less(const CGraphNode &node) const
{
  const CDepletionStage* pStage = dynamic_cast<const CDepletionStage*>(&node);
  if(pStage)
  {
    if(pStage == this)
      return false;

    // Try to find lower
    const CDepletionStage *pLower = pStage;
    while((this != pLower) && (!pLower->Initial()))
      pLower = &pLower->Previous();

    if(pLower == this)
      return true;

    return false;
  }
    
  return CGraphNode::Less(node);
}

bool CDepletionStage::Empty() const
{
  return ((m_pPrevious == 0) && (m_pNext == 0));
}

void CDepletionStage::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  CStorageNode::LoadStream(stream, version, progress);

  // Is it the first
  if(m_entry.EntryNodes().size() > 0)
  {
    // Link to the last stage
    m_pPrevious = &m_entry.InitialStage();
    while(!m_pPrevious->Last())
      m_pPrevious = &m_pPrevious->Next();

    assert(m_pPrevious->Index() == (Index() - 1));
    m_pPrevious->m_pNext = this;
    LinkTo(*m_pPrevious);
  }

  // Read the members
  if(version < CStreamVersion(3, 0, 33))
  {
    // older versions only saved years in integers
    // if a year is greater than 2000 it is used, and the data is set to 1 january
    // otherwise it is added to 2000
    int nYear;
    stream >> nYear;
    if(nYear < 1900) nYear += 2000;
    if(!Initial() && nYear <= Previous().Time().GetYear()) nYear = Previous().Time().GetYear() + 1;
    m_Time.Set(nYear, 1);
  }
  else if(version < CStreamVersion(3, 0, 58))
  {
    QDateTime temp;
    stream >> temp;
    m_Time.Set(temp.date().year(), temp.date().month());
  }
  else if(version < CStreamVersion(3, 7, 26) || (version >= CStreamVersion(3, 8, 0) && version < CStreamVersion(3, 8, 3)))
  {
    int nYear, nMonth;
    stream >> nYear;
    stream >> nMonth;
    m_Time.Set(nYear, nMonth);
  }
  else
  {
  int nYear, nMonth, nDay, nHour, nMinute, nSecond;
  stream >> nYear;
  stream >> nMonth;
  stream >> nDay;
  stream >> nHour;
  stream >> nMinute;
  stream >> nSecond;
  m_Time.Set(nYear, nMonth, nDay, nHour, nMinute, nSecond);
  }

  stream >> m_nIterationSteps;
  stream >> m_nSteps;

  if(version > CStreamVersion(3, 0, 105))
  stream >> m_dStepScalingFactor;
  else
  m_dStepScalingFactor = 1.;

  int conv_type;
  stream >> conv_type;
  m_convergence_type = (eConvergenceType)conv_type;
  int it_scheme;
  stream >> it_scheme;
  m_iteration_scheme = (eIterationScheme)it_scheme;
  stream >> m_dConvNormVal;

  if(version < CStreamVersion(3, 0, 79))
  {
    if(Initial())
      m_bMarkedAsInitial = true;
  }
  else
  {
    // new for version 3.0.79
    int nDummy;
    stream >> nDummy;
    m_bMarkedAsInitial = (nDummy != 0);
  }

  if(CStreamVersion(3, 0, 82) < version)
  {
    int output_type;
    stream >> output_type;
    m_output_type = (eOutputType)output_type;
  }

  if(version >= CStreamVersion(4, 1, 16))
  {
  int iAutoStepSizes;
  stream >> iAutoStepSizes;
  m_bAutoStepSizes = (iAutoStepSizes != 0);
  }

  // wedx (mantis #2663)
  // for the initial stage only convergence norm value and maximum number of iterations is valid for modification
  // all other parameters may not be changed
  // if the user has changed them in a previous version they will now be reset
/*  if(Initial())
  {
  // store convergence norm value and maximum number of iterations
  double dConvNormVal = m_dConvNormVal;
  int nIterationSteps = m_nIterationSteps;

  // set the default values
  SetDefaultNonLinParams();

  // restore loaded values
  m_dConvNormVal = dConvNormVal;
  m_nIterationSteps = nIterationSteps;
  }*/

  reParent(&m_entry);
}

void CDepletionStage::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  CStorageNode::SaveStream(stream, progress);

  // Save the members
  stream << Time().GetYear();
  stream << Time().GetMonth();

  stream << Time().GetDay();
  stream << Time().GetHour();
  stream << Time().GetMinute();
  stream << Time().GetSecond();

  stream << m_nIterationSteps;
  stream << m_nSteps;
  stream << m_dStepScalingFactor;
  int conv_type = (int)(m_convergence_type);
  stream << conv_type;
  int it_scheme = (int)(m_iteration_scheme);
  stream << it_scheme;
  stream << m_dConvNormVal;

  // new for version 3.0.79
  stream << (m_bMarkedAsInitial ? (int)1 : (int)0);

  // new for version 3.0.83
  int output_type = (int)(m_output_type);
  stream << output_type;

  // new for version 4.1.16
  int iAutoStepSizes = (m_bAutoStepSizes ? 1 : 0);
  stream << iAutoStepSizes;
}

long CDepletionStage::SavedItems() const
{
  return 1;
}

std::string CDepletionStage::IterationSchemeTag(eIterationScheme scheme)
{
  switch(scheme)
  {
  case CONSTANT:
    return "CONSTA";
  case LINEAR:
    return "LINEAR";
  case REGULAR:
    return "REGULA";
  case MODIFIED:
    return "MODIFI";
  case SECANT:
  return "SECANT";
  default:
    assert(false);
  }
  return "";
}

std::string CDepletionStage::ConvergenceTypeTag(eConvergenceType type)
{
  switch(type)
  {
  case FORCE:
    return "FORCE";
  case DISPLACEMENT:
    return "DISPLA";
  case ENERGY:
    return "ENERGY";
  default:
    assert(false);
  }
  return "";
}

std::string CDepletionStage::OutputTypeTag(eOutputType type)
{
  switch(type)
  {
  case NONE:
    return "None";
  case RESULTS:
    return "Results";
  case BRANCH:
    return "Branch";
  case PHASE:
  return "Phase";
  }

  assert(false);
  return std::string();
}

CDepletionStage::eOutputType CDepletionStage::DefaultOutputType()
{
  return RESULTS;
}

const CDepletionStage &CDepletionStage::InitialStage() const
{
  return m_entry.InitialStage();
}

CDepletionStage &CDepletionStage::InitialStage()
{
  return m_entry.InitialStage();
}

const CDepletionStage &CDepletionStage::LastStage() const
{
  return m_entry.LastStage();
}

CDepletionStage &CDepletionStage::LastStage()
{
  return m_entry.LastStage();
}

void CDepletionStage::MarkAsInitial()
{
  if(!m_bMarkedAsInitial)
  {
    CDepletionStage* pStage = &m_entry.InitialStage();
    while(pStage)
    {
      if(pStage->m_bMarkedAsInitial)
      {
        pStage->m_bMarkedAsInitial = false;
        pStage->Modified();
      }

      if(pStage->Last())
        pStage = 0;
      else
        pStage = &pStage->Next();
    }

    m_bMarkedAsInitial = true;

    Modified();

    m_entry.Modified(MarkedAsInitialChanged);
  }
}

bool CDepletionStage::IsMarkedAsInitial() const
{
  return m_bMarkedAsInitial;
}

bool CDepletionStage::CanMarkAsInitial() const
{
  return !Last() && !m_bMarkedAsInitial;
}

bool CDepletionStage::isAfter(const CDepletionStage& other) const
{
  if(&other == this)
    return false;

  const CDepletionStage* pStage = this;
  while(pStage)
  {
    if(pStage == &other)
      return true;

    if(pStage->Initial())
      pStage = 0;
    else
      pStage = &pStage->Previous();
  }

  return false;
}

bool CDepletionStage::isBefore(const CDepletionStage& other) const
{
  if(&other == this)
    return false;

  return !isAfter(other);
}

const CDepletionStage& CDepletionStage::GetNextBranchStage(
  const CDepletionStage& stage) const
{
  const CDepletionStage* pStage = &stage;

  while (pStage)
  {
  CDepletionStage::eOutputType outputtype = pStage->OutputType();

  if (outputtype == CDepletionStage::BRANCH ||
      outputtype == CDepletionStage::PHASE)
  {
      break;
  }

  if (pStage->Last())
  {
      break; // the last stage is always done
  }
  else
  {
      pStage = &pStage->Next();
  }
  }

  assert(pStage);

  return *pStage;
}

std::string CDepletionStage::ModifiedFileName(const std::string& fileName) const
{
  QString strFile = fileName.c_str();
  QString strPostfix = QString("_D%1").arg(Index());
  int iDot = strFile.lastIndexOf('.');

  if (iDot >= 0)
  {
  return (strFile.left(iDot) + strPostfix +
      strFile.right(strFile.length() - iDot)).toStdString();
  }

  return (strFile + strPostfix).toStdString();
}

///// CConstDepletionStageIterator
CConstDepletionStageIterator::CConstDepletionStageIterator()
: m_pStage(0)
{
}

CConstDepletionStageIterator::CConstDepletionStageIterator(const CDepletionStage& stage)
: m_pStage(&stage)
{
}

CConstDepletionStageIterator::CConstDepletionStageIterator(const CConstDepletionStageIterator& rhs)
: m_pStage(rhs.m_pStage)
{
}

CConstDepletionStageIterator& CConstDepletionStageIterator::operator=(const CConstDepletionStageIterator& rhs)
{
  m_pStage = rhs.m_pStage;
  return *this;
}

bool CConstDepletionStageIterator::operator==(const CConstDepletionStageIterator& rhs) const
{
  return m_pStage == rhs.m_pStage;
}

bool CConstDepletionStageIterator::operator!=(const CConstDepletionStageIterator& rhs) const
{
  return !operator==(rhs);
}

CConstDepletionStageIterator& CConstDepletionStageIterator::operator++()
{
  assert(m_pStage);
  if(m_pStage->Last())
  m_pStage = 0;
  else
  m_pStage = &m_pStage->Next();

  return *this;
}

CConstDepletionStageIterator CConstDepletionStageIterator::operator++(int)
{
  CConstDepletionStageIterator itRet(*this);
  ++(*this);
  return itRet;
}

CConstDepletionStageIterator& CConstDepletionStageIterator::operator--()
{
  assert(m_pStage);
  assert(!m_pStage->Initial());
  m_pStage = &m_pStage->Previous();
  return *this;
}

CConstDepletionStageIterator CConstDepletionStageIterator::operator--(int)
{
  CConstDepletionStageIterator itRet(*this);
  --(*this);
  return itRet;
}

const CDepletionStage& CConstDepletionStageIterator::operator*()
{
  assert(m_pStage);
  return *m_pStage;
}

const CDepletionStage* CConstDepletionStageIterator::operator->()
{
  assert(m_pStage);
  return m_pStage;
}


///// CDepletionStageIterator
CDepletionStageIterator::CDepletionStageIterator()
{
}

CDepletionStageIterator::CDepletionStageIterator(CDepletionStage& stage)
: CConstDepletionStageIterator(stage)
{
}

CDepletionStageIterator::CDepletionStageIterator(const CDepletionStageIterator& rhs)
: CConstDepletionStageIterator(rhs)
{
}

CDepletionStageIterator& CDepletionStageIterator::operator=(const CDepletionStageIterator& rhs)
{
  CConstDepletionStageIterator::operator=(rhs);
  return *this;
}

CDepletionStageIterator& CDepletionStageIterator::operator++()
{
  CConstDepletionStageIterator::operator++();
  return *this;
}

CDepletionStageIterator CDepletionStageIterator::operator++(int)
{
  CDepletionStageIterator itRet(*this);
  ++(*this);
  return itRet;
}

CDepletionStageIterator& CDepletionStageIterator::operator--()
{
  CConstDepletionStageIterator::operator--();
  return *this;
}

CDepletionStageIterator CDepletionStageIterator::operator--(int)
{
  CDepletionStageIterator itRet(*this);
  --(*this);
  return itRet;
}

CDepletionStage& CDepletionStageIterator::operator*()
{
  return const_cast<CDepletionStage&>(CConstDepletionStageIterator::operator*());
}

CDepletionStage* CDepletionStageIterator::operator->()
{
  return const_cast<CDepletionStage*>(CConstDepletionStageIterator::operator->());
}


/*!
\class CDepletionStageEntry
\brief The container of CDepletionStage instances.

The CDepletionStageEntry owns the depletion stages of the model. The CDepletionStageEntry, 
the CResultRegister and the CMeshBase form the Trinity of model. Results are associated with a 
mesh and valid for a certain timestep. The depletion stages are edited in the CModelTimestepPg.
*/

CDepletionStageEntry::CDepletionStageEntry(unsigned int uEntryID, CFemAppModel& model)
: CGraphEntryTemp<CDepletionStage>(uEntryID, IDI_DEPLETION_STAGE, IDS_TREE_DEPLETION_STAGES, model),
  m_pFirstStage(0)
{
}

CDepletionStageEntry::~CDepletionStageEntry()
{
  // wedx 20122006:
  // delete depletion stages in defined order (instead of arbitrary order of set in graph entry)
  // to prevent problems in the depletion stage destructor when it's trying to access other stages
  CDepletionStage* pStage = &LastStage();

  while(pStage)
  {
    CDepletionStage* pToDelete = pStage;

    if(pToDelete->Initial())
      pStage = 0;
    else
      pStage = &pToDelete->Previous();

    delete pToDelete;
  }
}

const CDepletionStage& CDepletionStageEntry::InitialStage() const
{
  assert(m_pFirstStage && (m_pFirstStage->Index() == 0));
  return *m_pFirstStage;
}

CDepletionStage& CDepletionStageEntry::InitialStage()
{ 
  assert(m_pFirstStage && (m_pFirstStage->Index() == 0));
  return *m_pFirstStage;
}

CDepletionStage& CDepletionStageEntry::LastStage()
{
  CDepletionStage *p_last_stage = &InitialStage();
  while(!p_last_stage->Last())
    p_last_stage = &p_last_stage->Next();

  return *p_last_stage;
}

const CDepletionStage& CDepletionStageEntry::LastStage() const
{
  const CDepletionStage *p_last_stage = &InitialStage();
  while(!p_last_stage->Last())
    p_last_stage = &p_last_stage->Next();

  return *p_last_stage;
}

const CDepletionStage& CDepletionStageEntry::MarkedAsInitialStage() const
{
  const CDepletionStage* pStage = &InitialStage();
  while(pStage)
  {
    if(pStage->IsMarkedAsInitial())
      return *pStage;

    if(pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }

  return InitialStage();
}

CDepletionStage& CDepletionStageEntry::MarkedAsInitialStage()
{
  CDepletionStage* pStage = &InitialStage();
  while(pStage)
  {
    if(pStage->IsMarkedAsInitial())
      return *pStage;

    if(pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }

  return InitialStage();
}

void CDepletionStageEntry::LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress)
{
  int deplCounter;
  stream >> deplCounter;
  
  // Go to last stage
  CDepletionStage *pCurrent = &LastStage();

  // Delete depletion stages till the first deleted
  while(!pCurrent->Initial())
  {
    pCurrent = &pCurrent->Previous();
    delete &pCurrent->Next();
  }

  // Delete first stage
  if (m_pFirstStage) 
  {
    delete m_pFirstStage;
    m_pFirstStage = 0;
  }

  // Start creating and loading the depletion stages
  pCurrent = 0;	
  for(int nStage = 0; nStage < deplCounter; nStage++)
  {
    // Create first
    if(nStage == 0)
    {
      m_pFirstStage = new CDepletionStage(*this);
      pCurrent = m_pFirstStage;
    }
    else
    {
      pCurrent = new CDepletionStage(*this);
    }

    pCurrent->Index(nStage);
    pCurrent->LoadStream(stream,version,progress);
  }
}

void CDepletionStageEntry::SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress) const
{
  int nSize = EntryNodes().size();
  stream << nSize;
  CDepletionStage *pStage = (CDepletionStage*)&InitialStage();
  assert(pStage->Initial());
  while(pStage)
  {
    pStage->SaveStream(stream,progress);
    if(pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }
}

const CDepletionStage& CDepletionStageEntry::StageByIndex(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < EntryNodes().size());

  const CDepletionStage* pStage = &InitialStage();
  while(pStage && nIndex-- > 0)
  {
    if(pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }

  assert(pStage);
  return *pStage;
}

CDepletionStage& CDepletionStageEntry::StageByIndex(int nIndex)
{
  assert(nIndex >= 0 && nIndex < EntryNodes().size());

  CDepletionStage* pStage = &InitialStage();
  while(pStage && nIndex-- > 0)
  {
    if(pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }

  assert(pStage);
  return *pStage;
}

CDepletionStageEntry::const_iterator CDepletionStageEntry::begin() const
{
  return CConstDepletionStageIterator(*m_pFirstStage);
}

CDepletionStageEntry::const_iterator CDepletionStageEntry::end() const
{
  return CConstDepletionStageIterator();
}

CDepletionStageEntry::iterator CDepletionStageEntry::begin()
{
  return CDepletionStageIterator(*m_pFirstStage);
}

CDepletionStageEntry::iterator CDepletionStageEntry::end()
{
  return CDepletionStageIterator();
}

int CDepletionStageEntry::StageIndexFromName(QString strName) const
{
  int idxRet = -1;
  int idxUnderscore = strName.lastIndexOf('_');
  if(idxUnderscore > 0)
  {
  // allow trailing analysis-type suffixes (_L, _N, _M, _H, _C)
  if(idxUnderscore == strName.length() - 2 &&
      (strName[strName.length() - 1] == 'L' ||
       strName[strName.length() - 1] == 'N' ||
       strName[strName.length() - 1] == 'M' ||
       strName[strName.length() - 1] == 'H' ||
       strName[strName.length() - 1] == 'C'))
  {
      strName = strName.left(strName.length() - 2);
      idxUnderscore = strName.lastIndexOf('_');
  }

  QString strStageSuffix;
  strStageSuffix = strName.right(strName.length() - idxUnderscore);
  if(strStageSuffix.left(2) == "_D")
  {
      int idxStage = strStageSuffix.right(strStageSuffix.length() - 2).toInt();
      if(idxStage >= 0 && idxStage < EntryNodes().size())
    idxRet = idxStage;
  }
  }

  return idxRet;
}
