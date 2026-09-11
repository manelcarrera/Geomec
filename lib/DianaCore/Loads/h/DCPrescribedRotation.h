// DCPrescribedRotation.h: interface for the CPrescribedRotation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPRESCRIBEDROTATION_H__2F64BEED_986E_47D0_BB72_0439780FB40B__INCLUDED_)
#define AFX_IPRESCRIBEDROTATION_H__2F64BEED_986E_47D0_BB72_0439780FB40B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace geo {
class CRotationSupport;
}

namespace dia {

class CLoadManager;

class CPrescribedRotation : public ISupportLoad  
{
public:
  CPrescribedRotation(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::CRotationSupport &support);
  virtual ~CPrescribedRotation();

  virtual bool WriteFilos() const;
};

}

#endif // !defined(AFX_IPRESCRIBEDROTATION_H__2F64BEED_986E_47D0_BB72_0439780FB40B__INCLUDED_)
