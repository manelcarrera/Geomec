// Pressure.cpp: implementation of the C3DPressure class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "IProgressBase.h"
#include "Gm3Database.h"
#include "GM3Pressure.h"
#include "GM3TableDef.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace gm 
{
//##ModelId=3BC55D4C0370
C3DPressure::C3DPressure()
{
  assert(m_vcPoint.size() == 0);
}

//##ModelId=3BC55D4C0371
C3DPressure::~C3DPressure()
{

}

//##ModelId=3BC55D4C0373
void C3DPressure::ReadPressure(C3DDatabase &db, const long lFormationKey, const int nTimeStep, IProgressBase &callback)
{
#ifndef _WIN64
  // Caution ....
  // In the older versions of Geomec the initial depletion stage is -1 based. 
  // In the database this is also the case ...

  // Input ... Undefined and initialized pressures 
  assert(m_type == UNDEFINED);
  assert(m_vcGradient.size() == 0);
  assert(m_vcPoint.size() == 0);

  CDaoRecordset rs(&db);
  //Open pressure table
  // We first open the formation table to find our formation
  if(nTimeStep == 0) {
    CString strNewQuery;
    CString strQuery = _T("SELECT * FROM ");
    strQuery += TB_FORMATION;
    strQuery += _T(" WHERE ");
    strQuery += FD_FORMATION_INDEX;
    strQuery += _T("=%d");
    strNewQuery.Format( strQuery, lFormationKey);
    rs.Open(dbOpenDynaset, strNewQuery, dbReadOnly);
    
    assert(!rs.IsEOF());
    rs.MoveFirst();

    // Load initial timestep
    // Temporary variables of the field contents
    if((rs.GetFieldValue(FD_FORMATION_GWC).boolVal) && ( db.Version() > 2.5)) 
    {
      // We have GWC in the first depletion stage (INITIAL)
      m_vcGradient.resize(2);
      m_dReferenceDepth = rs.GetFieldValue(FD_FOR_CONT_DEPTH).dblVal;
      m_dReferencePressure = rs.GetFieldValue(FD_FOR_CONT_PRESS).dblVal;
      m_vcGradient[0] = rs.GetFieldValue(FD_FOR_GRAD_UP).dblVal;
      m_vcGradient[1] = rs.GetFieldValue(FD_FOR_GRAD_DOWN).dblVal;
      m_type = OWCGWC;
      rs.Close();
      return;
    }
    else 
    {
      // Hmmm ... No GWC Must be constant pressure or an distributed
      if (db.Version() == 1.0) 
      {
        // On Old Database ....
        m_type = CONSTANT;
        m_dReferencePressure = rs.GetFieldValue(FD_FOR_INI_PORE_PRESS).dblVal;
        m_dReferenceDepth = 0;
        rs.Close();	// Close and exit
        return;
      } 
      else 
      {
        if(rs.GetFieldValue(FD_FOR_SINGLE_INSITU).boolVal) 
        {
          ReadSinglePressure(db, rs);
          return;
        }
      }	// Closure of the oldstuff
    }// No GWC

  rs.Close();
  } 

  CString strNewQuery;
  CString strQuery = _T("SELECT * FROM ");
  strQuery += TB_PRESSURE;
  strQuery += _T(" WHERE ");
  strQuery += FD_PRESSURE_FOR_INDEX;
  strQuery += _T("=%d AND ");
  strQuery += FD_PRESSURE_TIME_INDEX;
  strQuery += _T("=%d");
  strNewQuery.Format(strQuery, lFormationKey, nTimeStep - 1);
  rs.Open(dbOpenDynaset,strNewQuery,dbReadOnly);
    
  rs.MoveFirst();

  callback.Step();

  //Get the field contents
  m_ptMax = geo::CPoint ( rs.GetFieldValue(FD_PRESSURE_MAX_Y).dblVal,
              rs.GetFieldValue(FD_PRESSURE_MAX_X).dblVal,
              rs.GetFieldValue(FD_PRESSURE_MAX_Z).dblVal);
  m_ptMin = geo::CPoint ( rs.GetFieldValue(FD_PRESSURE_MIN_Y).dblVal,
              rs.GetFieldValue(FD_PRESSURE_MIN_X).dblVal,
              rs.GetFieldValue(FD_PRESSURE_MIN_Z).dblVal);

  // Pressure definition ...
  if (rs.GetFieldValue(FD_PRESSURE_GWC).boolVal)
    m_type = OWCGWC;
  else
    m_type = DISTRIBUTED;

  m_strSourceFileName = rs.GetFieldValue(FD_PRESSURE_FILE_NAME).pcVal;

  if (m_type == OWCGWC) 
  {
    // Database version switch
    if(db.Version() < 2.6) 
    {
      // Read the GWC of the old days
      m_dReferenceDepth = rs.GetFieldValue(FD_PRESSURE_GWC_DEPTH).dblVal;
      m_dReferencePressure = rs.GetFieldValue(FD_PRESSURE_GWC_PRESS).dblVal;
      m_vcGradient.resize(2);
      m_vcGradient[0] = 0;
      m_vcGradient[1] = 0;
    }
    else 
    {
      // Read the advanced GWC ...
    
      m_dReferenceDepth = rs.GetFieldValue(FD_PRESSURE_GWC_DEPTH_R).dblVal;
      m_dReferencePressure = rs.GetFieldValue(FD_PRESSURE_GWC_PRESS_R).dblVal;
      m_vcGradient.resize(2);
      m_vcGradient[0] = rs.GetFieldValue(FD_PRESSURE_GWC_GRAD_UP).dblVal;
      m_vcGradient[1] = rs.GetFieldValue(FD_PRESSURE_GWC_GRAD_DO).dblVal;
    }
  } 
  else 
  {
    assert(m_type == DISTRIBUTED);
    // Hmmm ... No GWC ... must be a pressure distribution!
    //Get the coordinates
    long lPressureKey=rs.GetFieldValue(FD_PRESSURE_PT_INDEX).lVal;
    rs.Close();

    strQuery = _T("SELECT ");
    strQuery += FD_PRESSURE_PT_X;
    strQuery += _T(", ");
    strQuery += FD_PRESSURE_PT_Y;
    strQuery += _T(", ");
    strQuery += FD_PRESSURE_PT_Z;
    strQuery += _T(" FROM ");
    strQuery += TB_PRESSURE_PT;
    strQuery += _T(" WHERE ");
    strQuery += FD_PRESSURE_INDEX;
    strQuery += _T("=%d");
    strNewQuery.Format(strQuery,lPressureKey);
    rs.Open(dbOpenDynaset, strNewQuery,dbReadOnly);

    // Does the recordset contain records?
    if(rs.IsBOF())
    {
      // There are no record for the distributed pressure. This is due to a bug. The pressure
      // type should be constant. So reopen the formation and read the constant pressures.
      rs.Close();
      CString strNewQuery;
      CString strQuery = _T("SELECT * FROM ");
      strQuery += TB_FORMATION;
      strQuery += _T(" WHERE ");
      strQuery += FD_FORMATION_INDEX;
      strQuery += _T("=%d");
      strNewQuery.Format( strQuery, lFormationKey);
      rs.Open(dbOpenDynaset, strNewQuery, dbReadOnly);
      ReadSinglePressure(db, rs);
      return;
    }

    // Read the coordinates
    rs.MoveFirst();

    while (!rs.IsEOF()) 
    {
      // Update progress ctrl
      callback.Step();

      COleVariant vVariant;
      vVariant=rs.GetFieldValue("X");

      m_vcPoint.push_back(TPressurePoint(geo::CPoint(rs.GetFieldValue(FD_PRESSURE_PT_Y).lVal,
                              rs.GetFieldValue(FD_PRESSURE_PT_X).lVal,
                               0),
                               rs.GetFieldValue(FD_PRESSURE_PT_Z).dblVal));
      rs.MoveNext();
    }

    rs.Close();
  }
  
  assert((m_type != CONSTANT) || (m_vcGradient.size() == 1));
  assert((m_type != OWCGWC) || (m_vcGradient.size() == 2));
  assert(m_type != UNDEFINED);
#endif
}

void C3DPressure::ReadSinglePressure(C3DDatabase& db, CDaoRecordset &rs)
{
#ifndef _WIN64
  // Single initial pore pressure
  m_type = CONSTANT;
  if(db.Version() < 2.2) 
  {
    // Get the old stuff
    if(rs.GetFieldValue(FD_FORMATION_RESERVOIR)) 
    {
      // Get PorePressure And Gradient
      m_dReferencePressure = rs.GetFieldValue(FD_FOR_PORE_PRESS).dblVal;
      m_dReferenceDepth = 0;
      m_vcGradient.push_back(rs.GetFieldValue(FD_FOR_PORE_GRAD).dblVal);
      assert(m_vcGradient.size() == 1);
    } 
    else 
    {
      // Get the normal porepressure
      m_dReferencePressure = rs.GetFieldValue(FD_FOR_INI_PORE_PRESS).dblVal;
      m_dReferenceDepth = 0;
      m_vcGradient.push_back(0);
    }

  } 
  else 
  {
    // Get the new stuff
    if(rs.GetFieldValue(FD_FOR_INI_PORE_GRAD_OK).boolVal)
      m_vcGradient.push_back(rs.GetFieldValue( FD_FOR_INI_PORE_GRAD).dblVal);		// Read InSituPressureGradient
    else
      m_vcGradient.push_back(0);

    if(rs.GetFieldValue(FD_FOR_INI_REF_DEPTH_OK).boolVal)
      // Read InSituPressureGradientOK
      m_dReferenceDepth = rs.GetFieldValue(FD_FOR_INI_REF_DEPTH).dblVal;						
    else
      m_dReferenceDepth = 0;
    
    m_dReferencePressure = rs.GetFieldValue(FD_FOR_INI_PORE_PRESS).dblVal;
  }

  assert(m_vcGradient.size() == 1);

  rs.Close();
#endif
}


//##ModelId=3BC55D4C03C6
const geo::IPoint& C3DPressure::Min() const
{
  assert(Distributed());
  return m_ptMin;
}

//##ModelId=3BC55D4C03C4
const geo::IPoint& C3DPressure::Max() const
{
  assert(Distributed());
  return m_ptMax;
}

} // End of namespace gm3
