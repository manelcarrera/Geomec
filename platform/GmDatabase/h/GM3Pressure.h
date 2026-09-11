// Pressure.h: interface for the CPressure class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRESSURE_H__91DBB18F_D6A7_46E4_91F8_8AA7C1BE69F7__INCLUDED_)
#define AFX_PRESSURE_H__91DBB18F_D6A7_46E4_91F8_8AA7C1BE69F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "point.h"
#include "GMPressure.h"
class IProgressBase;

namespace gm
{
#ifdef _WIN64
class CDaoRecordset;
#endif

class C3DDatabase;

//##ModelId=3BC55D4C02E4
class C3DPressure : public CPressure  
{
  void ReadSinglePressure(C3DDatabase& db, CDaoRecordset &rs);
  // Distributed
  geo::CPoint				m_ptMin;
  geo::CPoint				m_ptMax;

public:
  // Construction / Destruction
  C3DPressure();
  virtual ~C3DPressure();

  void ReadPressure(C3DDatabase &db,
            const long lFormationKey,
            const int nTimeStep,
            IProgressBase &callback);
  

  const geo::IPoint& Max() const;
  const geo::IPoint& Min() const;
};

}

#endif // !defined(AFX_PRESSURE_H__91DBB18F_D6A7_46E4_91F8_8AA7C1BE69F7__INCLUDED_)
