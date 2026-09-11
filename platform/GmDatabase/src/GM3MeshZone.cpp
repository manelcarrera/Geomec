// GM3MeshZone.cpp: implementation of the C3DMeshZone class.
//
//////////////////////////////////////////////////////////////////////
#include "GM3MeshZone.h"
#include "GM3TableDef.h"
#include "Gm3Database.h"
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace gm {

C3DMeshZone::C3DMeshZone() {}

C3DMeshZone::~C3DMeshZone() {}

int C3DMeshZone::Index() const { return m_nIndex; }

int C3DMeshZone::ExponentX() const { return m_nExponentX; }

int C3DMeshZone::ExponentY() const { return m_nExponentY; }

int C3DMeshZone::Size() const { return m_vcPoint.size(); }

const geo::IPoint &C3DMeshZone::Point(int nIndex) const { return m_vcPoint[nIndex]; }

void C3DMeshZone::ReadMeshZone(C3DDatabase &db, const C3DModel &model, const long lMeshZoneKey,
                               IProgressBase &callback) {
#ifndef _WIN64
  // Mesh zone data
  CDaoRecordset rs(&db);
  CString strQuery = _T("SELECT * FROM ");
  strQuery += TB_MESH_ZONE;
  strQuery += _T(" WHERE ");
  strQuery += FD_MESH_ZONE_ID;
  strQuery += _T("=%d");
  CString strNewQuery;
  strNewQuery.Format(strQuery, lMeshZoneKey);
  rs.Open(dbOpenDynaset, strNewQuery, dbReadOnly);

  rs.MoveFirst();
  callback.Step();

  m_nExponentX = rs.GetFieldValue(FD_MESH_ZONE_EXP_Y).lVal;
  m_nExponentY = rs.GetFieldValue(FD_MESH_ZONE_EXP_X).lVal;
  m_nIndex = rs.GetFieldValue(FD_MESH_ZONE_INDEX).lVal;

  rs.Close();

  strQuery = _T("SELECT ");
  strQuery += FD_MESH_ZONE_NODE_X;
  strQuery += _T(", ");
  strQuery += FD_MESH_ZONE_NODE_Y;
  strQuery += _T(" FROM ");
  strQuery += TB_MESH_ZONE_NODE;
  strQuery += _T(" WHERE ");
  strQuery += FD_MESH_ZONE_NR;
  strQuery += _T("=%d");

  strNewQuery.Format(strQuery, lMeshZoneKey);
  rs.Open(dbOpenDynaset, strNewQuery, dbReadOnly);
  rs.MoveFirst();
  int iCount = 0;
  while (!rs.IsEOF()) {
    if (iCount++ != 0) // the first record is invalid (bug from Geomec2.x)
    {
      callback.Step();
      //		COleVariant vVariant;
      //		vVariant=rs.GetFieldValue("X");
      geo::CPoint pt(rs.GetFieldValue(FD_MESH_ZONE_NODE_Y).dblVal, rs.GetFieldValue(FD_MESH_ZONE_NODE_X).dblVal);

      m_vcPoint.push_back(pt);
    }
    rs.MoveNext();
  }

  rs.Close();
#endif
}

} // namespace gm
