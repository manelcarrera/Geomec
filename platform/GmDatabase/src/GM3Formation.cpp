// Formation.cpp: implementation of the C3DFormation class.
//
//////////////////////////////////////////////////////////////////////

#include "GM3Formation.h"
#include "GM3Database.h"
#include "GM3Model.h"
#include "GM3Pressure.h"
#include "GM3WellPoint.h"
#include "GM3tabledef.h"
#include "StdAfx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace gm {

// ##ModelId=3BC55D4D022C
C3DFormation::C3DFormation()

{}

// ##ModelId=3BC55D4D022D
C3DFormation::~C3DFormation() {}

// ##ModelId=3BC55D4D0238
void C3DFormation::ReadFormation(C3DDatabase &db, const C3DModel &model, const long lFormationKey,
                                 IProgressBase &callback) {
#ifndef _WIN64
  assert(m_vcPressure.size() == 0);

  CDaoRecordset rs(&db);
  CString strQuery, strNewQuery;
  strQuery = _T("SELECT * FROM ");
  strQuery += TB_FORMATION;
  strQuery += _T(" WHERE ");
  strQuery += FD_FORMATION_INDEX;
  strQuery += _T("=%d");

  strNewQuery.Format(strQuery, lFormationKey);
  rs.Open(dbOpenDynaset, strNewQuery, dbReadOnly);

  rs.MoveFirst();

  callback.Step();

  // Get name, color and nr of elements
  m_strName = rs.GetFieldValue(FD_FORMATION_NAME).pcVal;
  m_lColor = rs.GetFieldValue(FD_FORMATION_COLOR).lVal;
  m_lElement = rs.GetFieldValue(FD_FORMATION_ELEMENT).lVal;
  m_bReservoir = rs.GetFieldValue(FD_FORMATION_RESERVOIR).boolVal;

  if (db.Version() < 2.0) {
    // Read the old material ...
  } else {
    m_strMaterial = rs.GetFieldValue(FD_MATERIAL_NAME).pcVal;
  }

  // Get the pressures if this is a reservoir
  if (m_bReservoir) {
    // This is a reservoir -> Get the pressures ...
    rs.Close();

    m_vcPressure.resize(model.TimestepSize());
    for (int i = 0; i < model.TimestepSize(); ++i) {
      // Start loading of pressures
      m_vcPressure[i].ReadPressure(db, lFormationKey, i, callback);
    }
  } else {
    // We're not a reservoir
    m_vcPressure.resize(1);
    m_vcPressure[0].ReadPressure(db, lFormationKey, 0, callback);
  }

#endif
}

int C3DFormation::Element() const { return (int)m_lElement; }

const CPressure &C3DFormation::Pressure(const int nDepletionStage) const { return m_vcPressure[nDepletionStage]; }

} // namespace gm
