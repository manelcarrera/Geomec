
#include "AnalysisType.h"
#include "resourceIDI.h"

#include <sstream>

CAnalysisType CAnalysisType::FirstType()
{
  return CAnalysisType(TAnalysisType(AT_FIRST));
}

CAnalysisType CAnalysisType::NextType()
{
  if(m_antype == AT_LAST)
    return CAnalysisType(); // invalid

  return CAnalysisType(TAnalysisType(int(m_antype) + 1));
}

CAnalysisType::CAnalysisType()
: m_bValid(false)
{
}

CAnalysisType::CAnalysisType(TAnalysisType antype)
: m_antype(antype),
  m_bValid(true)
{
}

CAnalysisType::CAnalysisType(const CAnalysisType& rhs)
: m_antype(rhs.m_antype),
  m_bValid(rhs.m_bValid)
{
}

CAnalysisType& CAnalysisType::operator=(const CAnalysisType& rhs)
{
  m_antype = rhs.m_antype;
  m_bValid = rhs.m_bValid;
  return *this;
}

bool CAnalysisType::operator==(const CAnalysisType& rhs) const
{
  if(!m_bValid && !rhs.m_bValid)
    return true;

  return m_bValid == rhs.m_bValid &&
         m_antype == rhs.m_antype;
}

bool CAnalysisType::operator!=(const CAnalysisType& rhs) const
{
  return !operator==(rhs);
}

CAnalysisType& CAnalysisType::operator=(TAnalysisType type)
{
  assert(type >= AT_LINEAR && type <= AT_MIXTURE_CONTAINMENT);

  m_antype = type;
  m_bValid = true;

  return *this;
}

bool CAnalysisType::operator==(TAnalysisType type) const
{
  if(!m_bValid)
    return false;

  return m_antype == type;
}

bool CAnalysisType::operator!=(TAnalysisType type) const
{
  return !operator==(type);
}

bool CAnalysisType::operator<(const CAnalysisType& rhs) const
{
  if(!m_bValid)
  {
    if(!rhs.m_bValid)
      return false;
    return true;
  }

  if(!rhs.m_bValid)
    return false;

  return m_antype < rhs.m_antype;
}

bool CAnalysisType::operator>(const CAnalysisType& rhs) const
{
  if(!m_bValid)
    return false;

  if(!rhs.m_bValid)
    return true;

  return m_antype > rhs.m_antype;
}

bool CAnalysisType::Valid() const
{
  return m_bValid;
}

CAnalysisType::TAnalysisType CAnalysisType::AnalysisType() const
{
  assert(m_bValid);
  return m_antype;
}

bool CAnalysisType::FromExportCharacter(char c)
{
  switch(c)
  {
  case 'L':
  case 'l':
    m_antype = AT_LINEAR;
    m_bValid = true;
    break;
  case 'N':
  case 'n':
    m_antype = AT_NONLIN;
    m_bValid = true;
    break;
  case 'H':
  case 'h':
    m_antype = AT_HEAT;
    m_bValid = true;
    break;
  case 'M':
  case 'm':
    m_antype = AT_MIXTURE;
    m_bValid = true;
    break;
  case 'C':
  case 'c':
    m_antype = AT_MIXTURE_CONTAINMENT;
    m_bValid = true;
	break;
  default:
    m_bValid = false;
  }

  return m_bValid;
}

char CAnalysisType::ExportCharacter() const
{
  assert(m_bValid);

  switch(m_antype)
  {
  case AT_LINEAR:
    return 'L';
  case AT_NONLIN:
    return 'N';
  case AT_HEAT:
    return 'H';
  case AT_MIXTURE:
    return 'M';
  case AT_MIXTURE_CONTAINMENT:
    return 'C';
  }

  assert(false);
  return ' ';
}

QString CAnalysisType::Label() const
{
  assert(m_bValid);
  QString sLabel;
  switch(m_antype)
  {
  case AT_LINEAR:
    sLabel = "Linear";
    break;
  case AT_NONLIN:
    sLabel = "Non-linear";
    break;
  case AT_HEAT:
    sLabel = "Heat transfer";
    break;
  case AT_MIXTURE:
    sLabel = "Mixture";
    break;
  case AT_MIXTURE_CONTAINMENT:
    sLabel = "Mixture Containment";
    break;
  default:
    assert(false);
  }

  return sLabel;
}

unsigned int CAnalysisType::IconId() const
{
  unsigned int id = 0;
  switch(m_antype)
  {
  case AT_LINEAR:
		id = IDI_LINEAR;
    break;
  case AT_NONLIN:
	  id = IDI_NON_LINEAR;
    break;
  case AT_HEAT:
    id = IDI_HEAT;
    break;
  case AT_MIXTURE:
    id = IDI_MIXTURE;
    break;
  case AT_MIXTURE_CONTAINMENT:
    id = IDI_MIXTURE_CONTAINMENT;
    break;
  }

  return id;
}

void CAnalysisType::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  LoadStream<TSTREAM, false>(stream, version);
  progress.Step();
}

void CAnalysisType::SaveStream(TSTREAM& stream, TPROGRESS& progress) const
{
  int nValid = (m_bValid ? 1 : 0);
  stream << nValid;
  if(m_bValid)
    stream << ExportCharacter();

  progress.Step();
}

long CAnalysisType::SavedItems() const
{
  return 1;
}

void CAnalysisType::LoadStream(std::stringstream& stream, CStreamVersion& version)
{
  LoadStream<std::stringstream, true>(stream, version);
}

void CAnalysisType::SaveStream(std::stringstream& stream) const
{
  int nValid = (m_bValid ? 1 : 0);
  stream << nValid << " ";
  if(m_bValid)
    stream << ExportCharacter() << " ";
}

template <class STREAM, bool bStringStream>
void CAnalysisType::LoadStream(STREAM& stream, CStreamVersion& /*version*/)
{
  int nValid;
  stream >> nValid;
  if(nValid)
  {
    char c;
    stream >> c;
    bool succeeded = FromExportCharacter(c);
    assert(succeeded);
  }
}
