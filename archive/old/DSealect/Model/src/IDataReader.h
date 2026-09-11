

class CImportInfo
{
public:
  typedef int TTagID;
  typedef int TUnitID;

public:
  CImportInfo();

  void AddLine(const CString& strLine);
  int LineSize() const;

  // vector of columns at the given line number
  const std::vector<CString>& LineVec(int i) const;

  void SetColumnNumber(TTagID tagID, int iColumn);
  int GetColumnNumber(TTagID tagID) const;

  void SetUnit(TTagID tagID, TUnitID unitID);
  TUnitID GetUnit(TTagID tagID) const;

  void Headers(int nLine);
  int Headers() const;

  void Units(int nLine);
  int Units() const;

  void DataStart(int nLine);
  int DataStart() const;

  void DataEnd(int nLine);
  int DataEnd() const;

  int MaxNumColumns() const;

private:
  // vector of columns of strings
  std::vector<std::vector<CString> > m_vcLines;

  // line numbers (zero-based!)
  int m_iHeaders;
  int m_iUnits;
  int m_iDataStart;
  int m_iDataEnd;
  int m_nNumColumns;

  // column numbers
  typedef std::map<TTagID, int> TColumnMap;
  TColumnMap m_mpColumns;

  // units
  typedef std::map<TTagID, TUnitID> TUnitMap;
  TUnitMap m_mpUnits;
};

class IDataReader
{
public:
  bool Import(const CString& strFilePath);
  CImportInfo& ImportInfo();
  const CImportInfo& ImportInfo() const;

protected:
  void AddTag(CImportInfo::TTagID tagID, const CString& strTag);
  void AddUnit(CImportInfo::TUnitID unitID, const CString& strUnit);

  virtual bool OnPresentImportDialog(const CString& strFilePath);
  virtual bool OnImportData() = 0;

private:
  bool ReadContents(const CString& strFilePath);
  CString ToLower(const CString& s) const;
  void GuessLineNumbers();
  void GuessColumns();
  void GuessUnits();

private:
  CImportInfo m_ImportInfo;

  // tags
  typedef std::map<CImportInfo::TTagID, std::vector<CString> > TTagMap;
  TTagMap m_mpTags;

  typedef std::map<CImportInfo::TUnitID, std::vector<CString> > TUnitMap;
  TUnitMap m_mpUnits;
};

