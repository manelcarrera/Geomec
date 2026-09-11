#include "ISubListObject.h"

const int MAX_COLUMNS = 20;

class CPreviewListObject : public IListObject
{
  class CPreviewColumnSubListObject : public ISubListObject
  {
  public:
    CPreviewColumnSubListObject(int nColumn, const QString& strLine, CPreviewListObject& parent);
    virtual QString Text() const;

  private:
    QString m_strLine;
  };

public:
  CPreviewListObject(CListCtrl& ctrl, int nLineNr, const std::vector<CString>& vcColumns, int nMaxNumColumns);
  virtual unsigned int Icon() const;
  virtual QString Text() const;

private:
  int m_nLineNr;
};
