// Tying.h: interface for the CTying class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HEXATYING_H__6FE72FA6_2530_4BA5_BDF0_3305B78310EC__INCLUDED_)
#define AFX_HEXATYING_H__6FE72FA6_2530_4BA5_BDF0_3305B78310EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHexaTying {
public:
  CHexaTying();
  CHexaTying(geo::CPoint &slave, geo::CPoint &master1, double dWeightFactor1, geo::CPoint &master2,
             double dWeightFactor2);
  CHexaTying(const CHexaTying &tying);
  virtual ~CHexaTying();

  void SetSlave(geo::CPoint &slave);
  void SetMaster(int nr, geo::CPoint &master, double dWeightFactor);

  geo::CPoint *GetSlave() const;
  geo::CPoint *GetMaster(int nr) const;
  double GetWeightFactor(int nr) const;

private:
  geo::CPoint *m_pSlave;
  geo::CPoint *m_pMaster1, *m_pMaster2;
  double m_dWeightFactor1, m_dWeightFactor2;
};

#endif // !defined(AFX_HEXATYING_H__6FE72FA6_2530_4BA5_BDF0_3305B78310EC__INCLUDED_)
