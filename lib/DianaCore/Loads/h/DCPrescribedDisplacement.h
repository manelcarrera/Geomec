// DCPrescribedDisplacement.h: interface for the CPrescribedDisplacement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPRESCRIBEDDISPLACEMENT_H__44B58D29_CE95_41AD_9CD4_E6FA39CC8525__INCLUDED_)
#define AFX_IPRESCRIBEDDISPLACEMENT_H__44B58D29_CE95_41AD_9CD4_E6FA39CC8525__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace geo {
class CTranslationSupport;
}

#include "IDCSupportLoad.h"

namespace dia {

class CLoadManager;

class CPrescribedDisplacement : public ISupportLoad  
{
public:
  CPrescribedDisplacement(CLoadCase &loadcase, double size, const geo::IVector &vecDirection, const geo::CTranslationSupport &support);
  virtual ~CPrescribedDisplacement();

  virtual bool WriteFilos() const;
};

}

#endif // !defined(AFX_IPRESCRIBEDDISPLACEMENT_H__44B58D29_CE95_41AD_9CD4_E6FA39CC8525__INCLUDED_)
