// GM3CrossSection.cpp: implementation of the C3DCrossSection class.
//
//////////////////////////////////////////////////////////////////////
#include "GM3CrossSection.h"
#include "GM3Database.h"
#include "GM3TableDef.h"
#include "IProgressBase.h"
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace gm {

C3DCrossSection::C3DCrossSection() {}

C3DCrossSection::~C3DCrossSection() {}

const QString &C3DCrossSection::Name() const { return m_strName; }

const geo::IPoint &C3DCrossSection::First() const { return m_point.first; }

const geo::IPoint &C3DCrossSection::Second() const { return m_point.second; }

void C3DCrossSection::ReadCrossSection(C3DDatabase &db, const long lCrossSectionKey, IProgressBase &callback) {
#ifndef _WIN64
  // Horizon data ...
  CDaoRecordset rs(&db);
  QString strQuery = _T("SELECT * FROM ");
  strQuery += TB_XSECTION;
  strQuery += _T(" WHERE ");
  strQuery += FD_XSECTION_INDEX;
  strQuery += _T("=%1");
  QString strNewQuery;
  strNewQuery = QString(strQuery).arg(lCrossSectionKey);
  rs.Open(dbOpenDynaset, strNewQuery.toStdString().c_str(), dbReadOnly);

  rs.MoveFirst();
  callback.Step();
  m_strName = rs.GetFieldValue(FD_XSECTION_NAME).pcVal;

  m_point.first =
      geo::CPoint(rs.GetFieldValue(FD_XSECTION_FIRST_Y).dblVal, rs.GetFieldValue(FD_XSECTION_FIRST_X).dblVal);

  m_point.second =
      geo::CPoint(rs.GetFieldValue(FD_XSECTION_SECOND_Y).dblVal, rs.GetFieldValue(FD_XSECTION_SECOND_X).dblVal);

  rs.Close();
#endif
}

} // namespace gm
