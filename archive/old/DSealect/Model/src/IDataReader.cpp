#include "stdafx.h"
#include "IDataReader.h"

CImportInfo::CImportInfo()
: m_iHeaders(-1),
  m_iUnits(-1),
  m_iDataStart(-1),
  m_iDataEnd(-1),
  m_nNumColumns(0)
{
}

void CImportInfo::AddLine(const CString& strLine)
{
  CString s(strLine);
  std::vector<CString> vcColumns;

  // check tab delimiter first
  CString strSearch;
  if(strLine.Find('\t') != -1)
    strSearch = "\t\r\n";
  else
    strSearch = " \r\n";

  for(int pos = 0; pos !=-1;)
  {
    CString sToken = strLine.Tokenize(strSearch, pos);
    sToken.Trim("\"");

    if(pos >= 0)
      vcColumns.push_back(sToken);
  }
  m_vcLines.push_back(vcColumns);
  if(vcColumns.size() > m_nNumColumns)
    m_nNumColumns = vcColumns.size();
}

int CImportInfo::LineSize() const
{
  return int(m_vcLines.size());
}

const std::vector<CString>& CImportInfo::LineVec(int i) const
{
  assert(i >= 0 && i < LineSize());
  return m_vcLines[size_t(i)];
}

void CImportInfo::SetColumnNumber(TTagID tagID, int iColumn)
{
  m_mpColumns[tagID] = iColumn;
}

int CImportInfo::GetColumnNumber(TTagID tagID) const
{
  TColumnMap::const_iterator it = m_mpColumns.find(tagID);
  if(it != m_mpColumns.end())
    return it->second;

  return -1;
}

void CImportInfo::SetUnit(TTagID tagID, TUnitID unitID)
{
  m_mpUnits[tagID] = unitID;
}

CImportInfo::TUnitID CImportInfo::GetUnit(CImportInfo::TTagID tagID) const
{
  TUnitMap::const_iterator it = m_mpUnits.find(tagID);
  if(it != m_mpUnits.end())
    return it->second;

  // default to 0
  return (TUnitID)0;
}

void CImportInfo::Headers(int nLine)
{
  m_iHeaders = nLine;
}

int CImportInfo::Headers() const
{
  return m_iHeaders;
}

void CImportInfo::Units(int nLine)
{
  m_iUnits = nLine;
}

int CImportInfo::Units() const
{
  return m_iUnits;
}

void CImportInfo::DataStart(int nLine)
{
  m_iDataStart = nLine;
}

int CImportInfo::DataStart() const
{
  return m_iDataStart;
}

void CImportInfo::DataEnd(int nLine)
{
  m_iDataEnd = nLine;
}

int CImportInfo::DataEnd() const
{
  return m_iDataEnd;
}

int CImportInfo::MaxNumColumns() const
{
  return m_nNumColumns;
}


///// IDataReader
bool IDataReader::Import(const CString& strFilePath)
{
  if(!ReadContents(strFilePath))
    return false;

  GuessLineNumbers();
  GuessColumns();
  GuessUnits();

  // allow import dialog to be shown
  if(!OnPresentImportDialog(strFilePath))
    return false;

  // and do the import
  if(!OnImportData())
    return false;

  return true;
}

void IDataReader::AddTag(CImportInfo::TTagID tagID, const CString& strTag)
{
  TTagMap::iterator it;
  it = m_mpTags.insert(TTagMap::value_type(tagID, std::vector<CString>())).first;
  it->second.push_back(strTag);
}

void IDataReader::AddUnit(CImportInfo::TUnitID unitID, const CString& strUnit)
{
  TUnitMap::iterator it;
  it = m_mpUnits.insert(TUnitMap::value_type(unitID, std::vector<CString>())).first;
  it->second.push_back(strUnit);
}

CImportInfo& IDataReader::ImportInfo()
{
  return m_ImportInfo;
}

const CImportInfo& IDataReader::ImportInfo() const
{
  return m_ImportInfo;
}

bool IDataReader::OnPresentImportDialog(const CString& strFilePath)
{
  return true;
}

void IDataReader::GuessLineNumbers()
{
  int maxrefs = 0;
  for(size_t i = 0; i < m_ImportInfo.LineSize(); ++i)
  {
    //CString ll = ToLower(m_ImportInfo.Line(i));
    const std::vector<CString>& vcColumns = m_ImportInfo.LineVec(i);

    // guess column header line
    int refs = 0;

    // see how many times each tag category is referenced
    for(TTagMap::iterator it = m_mpTags.begin(); it != m_mpTags.end(); ++it)
    {
      // iterate this tag category
      for(size_t j = 0; j < it->second.size(); ++j)
      {
        size_t k;
        for(k = 0; k < vcColumns.size(); ++k)
        {
          CString ll = ToLower(vcColumns[k]);
          if(ll.Find(it->second[j]) != -1)
          {
            // one of this category found
            // increase refcount and stop processing this category
            ++refs;
            break;
          }
        }

        if(k < vcColumns.size())
          break;
      }
    }

    // the one with the highest refcount is the winner
    if(refs > maxrefs)
    {
      m_ImportInfo.Headers(i);
      // restart search for data (must be after the header)
      m_ImportInfo.DataStart(-1);
      m_ImportInfo.DataEnd(-1);
      maxrefs = refs;
    }

    // guess start and end of data
    int nDigitChars = 0;
    int nNonDigitChars = 0;
    for(int k = 0; k < vcColumns.size(); ++k)
    {
      const CString &l = vcColumns[k];
      for(int j = 0; j < l.GetLength(); ++j)
      {
        char c = l[j];
        if((unsigned)(c + 1) <= 256 && isdigit(c) ||
           c == '.' ||
           c == '-')
          ++nDigitChars;
        else if((unsigned)(c + 1) <= 256 && isspace(c))
          // do nothing
          ;
        else
          ++nNonDigitChars;
      }
    }

    double dDigitFactor;
    if(nNonDigitChars == 0)
    {
      if(nDigitChars == 0)
        dDigitFactor = 0;
      else
        dDigitFactor = 1;
    }
    else
    {
      dDigitFactor = double(nDigitChars) / double(nNonDigitChars);
    }
    if(dDigitFactor > 0.7) // more than 70% digit characters
    {
      if(m_ImportInfo.DataStart() == -1)
        m_ImportInfo.DataStart(i);
      m_ImportInfo.DataEnd(i);
    }
  }

  // search for the first non-empty line after the headers
  // assume it's the units line
  if(m_ImportInfo.Headers() != -1)
  {
    for(size_t i = m_ImportInfo.Headers() + 1; i < m_ImportInfo.LineSize(); ++i)
    {
      if(!m_ImportInfo.LineVec(i).empty())
      {
        m_ImportInfo.Units(i);
        break;
      }
    }
  }
}

void IDataReader::GuessUnits()
{
  // walk the known tags
  for(TTagMap::iterator itt = m_mpTags.begin(); itt != m_mpTags.end(); ++itt)
  {
    CImportInfo::TTagID tag = itt->first;

    // tag column index
    int iCol = m_ImportInfo.GetColumnNumber(tag);

    // did we find a column for this tag?
    if(iCol != -1)
    {
      // try to get unit at that column
      std::vector<std::vector<CString> > vcColumnVectors;

      // did we find a row with units?
      if(m_ImportInfo.Units() != -1)
        vcColumnVectors.push_back(m_ImportInfo.LineVec(m_ImportInfo.Units()));

      if(m_ImportInfo.Headers() != -1)
        vcColumnVectors.push_back(m_ImportInfo.LineVec(m_ImportInfo.Headers()));

      for(int j = 0; j < vcColumnVectors.size(); ++j)
      {
        const std::vector<CString>& vcColumns = vcColumnVectors[j];

        if(iCol < vcColumns.size())
        {
          CString sToken = ToLower(vcColumns[iCol]);

          // check for braces and strip them if found
          int pos = sToken.FindOneOf("[{(");
          if(pos != -1)
          {
            CString s = sToken.Mid(pos + 1);
            pos = s.FindOneOf("]})");
            if(pos != -1)
            {
              s = s.Left(pos);
              if(!s.IsEmpty())
                sToken = s;
            }
          }

          // sToken contains the unit
          // walk all known units
          for(TUnitMap::iterator it = m_mpUnits.begin(); it != m_mpUnits.end(); ++it)
          {
            // walk all possible strings for this unit
            int i;
            for(i = 0; i < it->second.size(); ++i)
            {
              if(sToken.Find(it->second[i]) != -1)
              {
                // supply the found unit to the import info and stop iterating
                m_ImportInfo.SetUnit(tag, it->first);
                break;
              }
            }

            if(i < it->second.size())
              break;
          }
        }
      }
    }
  }
}

bool IDataReader::ReadContents(const CString& strFilePath)
{
  FILE* fp = fopen(strFilePath, "r");
  if(!fp)
    return false;

  char buf[2048];
  while(fgets(buf, 2048, fp))
    m_ImportInfo.AddLine(buf);

  fclose(fp);

  return true;
}

CString IDataReader::ToLower(const CString& s) const
{
  CString str(s);
  return str.MakeLower();
}

void IDataReader::GuessColumns()
{
  if(m_ImportInfo.Headers() != -1)
  {
    const std::vector<CString>& vcColumns = m_ImportInfo.LineVec(m_ImportInfo.Headers());
    for(int col = 0; col < vcColumns.size(); ++col)
    {
      CString sColumn = ToLower(vcColumns[col]);
      bool bFound = false;
      for(TTagMap::iterator it = m_mpTags.begin(); it != m_mpTags.end(); ++it)
      {
        const std::vector<CString>& vcTags = it->second;
        for(size_t i = 0; i < vcTags.size(); ++i)
        {
          if(sColumn.Find(vcTags[i]) == 0)
          {
            // register this column
            m_ImportInfo.SetColumnNumber(it->first, col);
            bFound = true;
            break;
          }
        }

        if(bFound)
          break;
      }
    }
  }
}

