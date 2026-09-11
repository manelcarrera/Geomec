// gm3WellPath.cpp: implementation of the C3DWellPath class.
//
//////////////////////////////////////////////////////////////////////
#include "gm3WellPath.h"
#include "GM3TableDef.h"
#include "gm3Database.h"
#include "gm3wellpoint.h"
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace gm {

C3DWellPath::C3DWellPath() : m_dElevation(0) {}

C3DWellPath::~C3DWellPath() {}

void C3DWellPath::ReadWellPath(C3DDatabase &db, const long lWellPathKey, IProgressBase &callback) {
#ifndef _WIN64
  // Well Path data ...
  CDaoRecordset rs(&db);
  CString strQuery = _T("SELECT * FROM ");
  strQuery += TB_LINER;
  strQuery += _T(" WHERE ");
  strQuery += FD_LINER_INDEX;
  strQuery += _T("=%d");
  CString strNewQuery;
  strNewQuery.Format(strQuery, lWellPathKey);
  rs.Open(dbOpenDynaset, strNewQuery, dbReadOnly);

  rs.MoveFirst();
  callback.Step();
  m_strName = rs.GetFieldValue(FD_LINER_NAME).pcVal;

  m_RefPoint = geo::CPoint(rs.GetFieldValue(FD_LINER_YPOINT).dblVal, rs.GetFieldValue(FD_LINER_XPOINT).dblVal);

  m_bIsVertical = (rs.GetFieldValue(FD_LINER_ISVERTICAL).boolVal != 0);

  if (db.Version() >= 2.8)
    m_dElevation = rs.GetFieldValue(FD_LINER_ELEVATION).dblVal;

  rs.Close();

  strQuery = _T("SELECT * FROM ");
  strQuery += TB_LINERPOINT;
  strQuery += _T(" WHERE ");
  strQuery += FD_INLINER_INDEX;
  strQuery += _T("=%d");
  strNewQuery.Format(strQuery, lWellPathKey);
  rs.Open(dbOpenDynaset, strNewQuery, dbReadOnly);

  // Read the coordinates
  rs.MoveFirst();

  while (!rs.IsEOF()) {
    // Update progress ctrl
    callback.Step();

    // Read point field

    if (rs.GetFieldValue(FD_POINTTYPE).lVal == 0) {
      C3DWellPoint *pWellPoint = new C3DWellPoint(
          rs.GetFieldValue(FD_AHD).dblVal, rs.GetFieldValue(FD_INCLINATION).dblVal, rs.GetFieldValue(FD_AZIMUTH).dblVal,
          rs.GetFieldValue(FD_NORTHING).dblVal, rs.GetFieldValue(FD_EASTING).dblVal, rs.GetFieldValue(FD_TVD).dblVal);
      PushBack(*pWellPoint);
    }

    rs.MoveNext();
  }
#endif
}

bool C3DWellPath::PushBack(const geo::IPoint &point) {
  if (!(m_PolyLine.PushBack(point))) // points for the polyline...
    return false;                    // point  has already been inserted...

  m_PointArr.PushBack((C3DWellPoint &)point); // the well points...

  return true;
}

} // namespace gm
