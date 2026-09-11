// DDXQuantity.h: interface for the CDDXQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DDXQUANTITY_H__632224BD_3907_403F_AEA0_46B576C3092F__INCLUDED_)
#define AFX_DDXQUANTITY_H__632224BD_3907_403F_AEA0_46B576C3092F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CDependenciesManager.h"
#include "stdafx.h"
#include <QString>

class CQuantityReciever : public QObject {
  Q_OBJECT;

public:
  CQuantityReciever(CDoubleQuantity *pQ);
  ~CQuantityReciever() {}
  bool Refused() const { return m_bRefused; }
  const QString &Description() { return m_Description; }
private slots:
  void OnRefuseValue(const CDoubleQuantity &Quantity, const QString &Description);

private:
  bool m_bRefused;
  QString m_Description;
};

void AFXAPI DDV_Quantity(CDataExchange *pDX, int nIDC, CDoubleQuantity *pQ, CDependenciesManager *pDM,
                         CString strQuantityName);
void AFXAPI DDV_Quantity(CDataExchange *pDX, int nIDC, CDoubleQuantity *pQ, CDoubleQuantity::UNIT us,
                         CString sQuantityName);

CString GetUnitString(CDoubleQuantity *pQ, CDoubleQuantity::UNIT us);
void SetUnitlabel(CStatic *pUnitLabel, CDoubleQuantity *pQ, CDoubleQuantity::UNIT us);

#endif // !defined(AFX_DDXQUANTITY_H__632224BD_3907_403F_AEA0_46B576C3092F__INCLUDED_)
