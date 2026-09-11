// DCasingPointResult.h: interface for the CDCasingPointResult class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCASINGPOINTRESULT_H__DF8A3ED6_7B28_48AF_9547_9453BB92FFD4__INCLUDED_)
#define AFX_DCASINGPOINTRESULT_H__DF8A3ED6_7B28_48AF_9547_9453BB92FFD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #include <QValueVector.h>
#include "DCasingPointResultData.h"

class CDCasingPointResult {

public:
  CDCasingPointResult();
  virtual ~CDCasingPointResult();

  // interface
  DCasingPointSupportedResultMap &GetDCasingPointSupportedResultMap();
  DCasingPointUnsupportedResultMap &GetDCasingPointUnsupportedResultMap();

  CDCasingPointResult &operator=(const CDCasingPointResult &right);

  const DCasingPointSupportedResultMap &GetDCasingPointSupportedResultMap() const;
  const DCasingPointUnsupportedResultMap &GetDCasingPointUnsupportedResultMap() const;

private:
  DCasingPointSupportedResultMap m_DCasingPointSupportedResultMap;
  DCasingPointUnsupportedResultMap m_DCasingPointUnsupportedResultMap;
};

#endif // !defined(AFX_DCASINGPOINTRESULT_H__DF8A3ED6_7B28_48AF_9547_9453BB92FFD4__INCLUDED_)
