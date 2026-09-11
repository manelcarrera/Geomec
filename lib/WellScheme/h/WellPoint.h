// WellPoint.h: interface for the WellPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLPOINT_H__1C2D3E8A_A9EF_4DD7_B0CD_B038B9C3D4F4__INCLUDED_)
#define AFX_WELLPOINT_H__1C2D3E8A_A9EF_4DD7_B0CD_B038B9C3D4F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//*******************************************************************
//
//  FILE:       WellPoint.h
//  AUTHOR:     htg
//  PROJECT:    WellScheme
//  COMPONENT:  CWellPoint
//  DATE:       17.09.2003
//  COPYRIGHT:  TNO-DIANA BV 2003
//  COMMENTS:   -
//
//*******************************************************************

#include "WellSchemeInclude.h"

namespace well {

class CWellPathBase;
class CWellPoint;

class IWellPointFactory {
public:
  virtual ~IWellPointFactory();
  virtual CWellPoint *CreateWellPoint(const CWellPathBase &wellpath, const double &tmd) const = 0;
};

class CWellPoint : public QObject, public geo::IPoint {
  Q_OBJECT;

  friend class CWellPathBase;

public:
signals:
  void OnTMDChanged();
  void OnDestroy(const well::CWellPoint &wellpoint); // use namespace in signals/slots

public:
  CWellPoint(const CWellPathBase &WellPath, double TMD);
  CWellPoint(const CWellPoint &rhs);

  virtual ~CWellPoint();

  // IPoint interface ...
  virtual const double &X() const;
  virtual const double &Y() const;
  virtual const double &Z() const;

  // no direct accses to IPoint
  virtual void X(const double & /*dX*/) { assert(false); }
  virtual void Y(const double & /*dY*/) { assert(false); }
  virtual void Z(const double & /*dZ*/) { assert(false); }

  virtual void AssertValid() const;
  virtual bool Empty() const;

  const CLengthQuantity &Northing() const;
  const CLengthQuantity &Easting() const;
  const CAngleQuantity &Azimuth() const;
  const CAngleQuantity &Inclination() const;
  const CLengthQuantity &TVD() const;
  const CLengthQuantity &TMD() const;

  // only change the TMD the rest is recalculated
  CLengthQuantity &TMD();

  CWellPoint &operator=(const CWellPoint &rhs);

  bool operator==(const CWellPoint &rhs) const;
  bool operator!=(const CWellPoint &rhs) const;

  bool operator<(const CWellPoint &rhs) const;
  bool operator>(const CWellPoint &rhs) const;

  virtual bool Defined() const;
  virtual void Invalidate();

  CWellPathBase &WellPath();
  const CWellPathBase &WellPath() const;

  // direction vector in this point
  geo::CVector GetDirection() const;

  void addElement(const geo::IElement *pElement);
  void setElements(const std::set<const geo::IElement *> &stElements);
  const std::set<const geo::IElement *> &getElements() const;

private slots:
  void OnQuantityChanged(const CDoubleQuantity &pQ);
  void OnWellDefPointsChanged();

protected:
  CWellPoint(); // default constructor is only for a wellpath object

private:
  void Connect();
  const CWellPathBase *m_pWellPath;
  CLengthQuantity m_Northing;
  CLengthQuantity m_Easting;
  CAngleQuantity m_Azimuth;
  CAngleQuantity m_Inclination;
  CLengthQuantity m_TMD;
  CLengthQuantity m_TVD;
  std::set<const geo::IElement *> m_stElements;

  // for IPoint interface
  mutable double m_X, m_Y, m_Z;
};

} // namespace well

#endif // !defined(AFX_WELLPOINT_H__1C2D3E8A_A9EF_4DD7_B0CD_B038B9C3D4F4__INCLUDED_)
