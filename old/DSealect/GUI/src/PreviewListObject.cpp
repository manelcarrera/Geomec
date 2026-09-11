#include "stdafx.h"
#include "dsealect.h"

#include "PreviewListObject.h"

CPreviewListObject::CPreviewColumnSubListObject::CPreviewColumnSubListObject(int nColumn, const QString& strLine, CPreviewListObject& parent)
: ISubListObject(parent, nColumn),
  m_strLine(strLine)
{
}

QString CPreviewListObject::CPreviewColumnSubListObject::Text() const
{
  return m_strLine;
}

CPreviewListObject::CPreviewListObject(CListCtrl& ctrl, int nLineNr, const std::vector<CString>& vcColumns, int nMaxNumColumns)
: IListObject(ctrl),
  m_nLineNr(nLineNr)
{
	Ctrl().SetItemData(Ctrl().GetItemCount() - 1, (DWORD_PTR) this);
  for(int i = 0; i < nMaxNumColumns && i < MAX_COLUMNS; ++i)
    new CPreviewColumnSubListObject(i + 1, (LPCTSTR)(i < vcColumns.size() ? vcColumns[i] : ""), *this);
}

unsigned int CPreviewListObject::Icon() const
{
  return 0;
}

QString CPreviewListObject::Text() const
{
  return QString("%1").arg(m_nLineNr);
}
