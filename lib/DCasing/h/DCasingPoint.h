// DCasingPoint.h: interface for the CDCasingPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCASINGPOINT_H__0EBF1053_79BA_4562_AD85_958C1A20ECA4__INCLUDED_)
#define AFX_DCASINGPOINT_H__0EBF1053_79BA_4562_AD85_958C1A20ECA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DCasingPointResult.h"
#include "QuantityInclude.h"
#include <qobject.h>

class CSinglePressure;
class CNoneQuantity;
class CSinglePressure;
class CDCasing;

class CDCasingPoint //: public QObject
{
  // friend CDCasing;
  // Q_OBJECT

private:
  // member attributes
  //	CDCasing* m_pDCasing;
  CSinglePressure m_ElasticSupportYoungsMod;
  CNoneQuantity m_ElasticSupportPoissonRatio;
  CSinglePressure m_InternalPressure;
  CSinglePressure m_ExternalPressure;
  CNoneQuantity m_Epscom;
  CDCasingPointResult m_CDCasingPointResult;

public:
  // construction / destruction
  CDCasingPoint();
  virtual ~CDCasingPoint();

  // interface
  bool Defined();

  CSinglePressure &ElasticSupportYoungsMod();
  CNoneQuantity &ElasticSupportPoissonRatio();
  CSinglePressure &InternalPressure();
  CSinglePressure &ExternalPressure();
  CNoneQuantity &Epscom();
  CDCasingPointResult &GetCDCasingPointResult();
  const CDCasingPointResult &GetCDCasingPointResult() const;
};

#endif // !defined(AFX_DCASINGPOINT_H__0EBF1053_79BA_4562_AD85_958C1A20ECA4__INCLUDED_)
