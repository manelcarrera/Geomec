#include "stdafx.h"
#include "geomec.h"

#include "BinaryResultsImport.h"
#include "ImportFormat.h"
#include "FvInputStream.h"

#include <fstream>

CBinaryResultsImport::CBinaryResultsImport(CModelBase& model)
: m_model(model),
  m_pFormat(0),
  m_nProgress(0)
{
}

CBinaryResultsImport::~CBinaryResultsImport()
{
  Close();
}

bool CBinaryResultsImport::Open(const CString& strPath, IProgressBase& progress)
{
  assert(!m_pFormat); // already open?
  bool bRet = true;

  try
  {
  std::ifstream s(strPath, std::ios::in | std::ios::binary);
  if(!s)
      throw CReadException(QString("Unable to open '%1'").arg(QString(strPath)));

  s.seekg(0, std::ios_base::end);
  m_endpos = s.tellg();
  s.seekg(0, std::ios_base::beg);

  progress.AddSteps(100);
  progress.StatusMessage(QString("Reading '%1'").arg(QString(strPath)));

  m_pFormat = new CResultImportFormat(m_model);

  QString str;

  // magic
  str = GetLine(s);
  if(str != "BINARY RESULTS")
      throw CReadException("This is not a valid binary results file");

  SetProgress(progress, s);

  // unit
  str = GetLine(s);
  QStringList sl = str.split(QRegExp("[\\t ]"), QString::SkipEmptyParts);
  if(sl.size() != 2)
      throw CReadException("Invalid UNIT specification encountered");

  if(sl[0] != "UNIT")
      throw CReadException("Invalid UNIT specification encountered");

  if(sl[1] == "SI")
      m_pFormat->Unit(CQuantity::SI_UNIT);
  else if(sl[1] == "FIELD")
      m_pFormat->Unit(CQuantity::FIELD_UNIT);
  else
      throw CReadException(QString("UNIT should be 'SI' or 'FIELD', found '%1'").arg(sl[1]));

  SetProgress(progress, s);

  // header
  str = GetLine(s);
  sl = str.split(QRegExp("[\\t ]"), QString::SkipEmptyParts);
  QStringList::iterator it;
  int nColumns = sl.size();
  for(it = sl.begin(); it != sl.end(); ++it)
      m_pFormat->AppendColumn(*it);

  SetProgress(progress, s);

  // data
  assert(sizeof(int) == sizeof(float));
  assert(sizeof(int) == 4);

  typedef union
  {
      char* cval;
      int* ival;
      float* fval;
  } TReadValue;

  TReadValue v;
  v.cval = new char[4 * nColumns];

  while(true)
  {
      s.read(v.cval, 4 * nColumns);
      if(s.eof())
    break;

      SetProgress(progress, s);

      CResultImportFormat::TValueVec vcValues(nColumns);
      int i;
      for(i = 0; i < nColumns; ++i)
      {
    if(i < 2)
          vcValues[i] = double(v.ival[i]);
    else
          vcValues[i] = double(v.fval[i]);
      }
      m_pFormat->AppendRow(vcValues);
  }

  delete[] v.cval;
  }
  catch(CReadException e)
  {
  m_strErrorMessage = e.ErrorMessage();
  bRet = false;
  }

  Close();

  return bRet;
}

void CBinaryResultsImport::Close()
{
  delete m_pFormat;
  m_pFormat = 0;
}

QString CBinaryResultsImport::ErrorMessage() const
{
  return m_strErrorMessage;
}

void CBinaryResultsImport::SetProgress(IProgressBase& progress, std::ifstream& s)
{
  assert(s.is_open());
  std::ifstream::pos_type pos = s.tellg();

  int nProgress = int((double(pos) / double(m_endpos)) * 100.);
  if(nProgress - m_nProgress > 0)
  {
  TRACE("Progress: %d steps (%d to %d), %d of %d\n", nProgress - m_nProgress, m_nProgress, nProgress, pos, m_endpos);
  progress.Step(nProgress - m_nProgress);
  m_nProgress = nProgress;
  }
}

QString CBinaryResultsImport::GetLine(std::ifstream& s) const
{
  QString sRet;
  char c;
  do
  {
  c = s.get();
  sRet += c;
  } while(c != '\n');

  return sRet.trimmed();
}
