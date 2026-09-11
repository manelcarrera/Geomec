// PressureBase.h: interface for the CPressureBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRESSUREBASE_H__94D57B09_9388_4D51_BF26_524EF8E35505__INCLUDED_)
#define AFX_PRESSUREBASE_H__94D57B09_9388_4D51_BF26_524EF8E35505__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ComponentConstant.h"
#include "FormationLoadBase.h"
#include "FormationLoadGradientComponent.h"
#include "PressureGradient.h"
#include "SinglePressure.h"

template <typename Pressure> class CPressureComponent : public IValueComponentBase {
  bool m_bEmpty;

public:
  typedef IValueDataInterfaceScalarTempl<CPressureComponent<Pressure>> IValueDataInterfacePressureScalar;

  // Construction
  CPressureComponent(CFormationLoadBase<Pressure> &pressure, unsigned int uMode, bool bEmpty = false);
  CPressureComponent(const CPressureComponent &rhs);

  virtual TValueComponentType Type() const;

  virtual const IValueDataInterfaceScalar &ScalarData() const;
  virtual IValueDataInterfaceScalar &ScalarData();

  // Access to the pressure
  const CFormationLoadBase<Pressure> &PressureBase() const;
  CFormationLoadBase<Pressure> &PressureBase();

  // Unit name and overall min /max
  virtual QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual IValueDomainScalar::TMinMax MinMax(IProgressBase &progressBase,
                                             const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement &elm,
                                                     const CQuantity::UNIT unit = CQuantity::SI_UNIT,
                                                     geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint &pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT,
                                                geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValue Average(IProgressBase &progressBase,
                                             const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;

  // Assignment and so ...
  CPressureComponent &operator=(const CPressureComponent &rhs);
  bool operator==(const CPressureComponent &rhs) const;

  // Save and load
  virtual bool Empty() const;
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  virtual QString ExportLabel() const;

  // we only want to show pressure on formations and formation plane and when we are a porepressure we only want
  // to show it on formations planes
  virtual bool CanMap(const COpenGLNode &node) const;

protected:
  IValueDataInterfacePressureScalar m_Data;
};

template <typename Pressure>
CPressureComponent<Pressure>::CPressureComponent(CFormationLoadBase<Pressure> &pressure, unsigned int uMode,
                                                 bool /*bEmpty*/)
    : IValueComponentBase(pressure, 0, uMode), m_Data(*this) {}

template <typename Pressure>
CPressureComponent<Pressure>::CPressureComponent(const CPressureComponent &rhs)
    : IValueComponentBase(rhs), m_bEmpty(rhs.m_bEmpty), m_Data(rhs.m_Data) {}

template <typename Pressure> IValueComponentBase::TValueComponentType CPressureComponent<Pressure>::Type() const {
  return static_cast<IValueComponentBase::TValueComponentType>(ScalarData().Type());
}

template <typename Pressure> const IValueDataInterfaceScalar &CPressureComponent<Pressure>::ScalarData() const {
  return m_Data;
}

template <typename Pressure> IValueDataInterfaceScalar &CPressureComponent<Pressure>::ScalarData() { return m_Data; }

template <typename Pressure> const CFormationLoadBase<Pressure> &CPressureComponent<Pressure>::PressureBase() const {
  const CFormationLoadBase<Pressure> *pPressure = dynamic_cast<const CFormationLoadBase<Pressure> *>(&Parent());
  assert(pPressure);
  return *pPressure;
}

template <typename Pressure> CFormationLoadBase<Pressure> &CPressureComponent<Pressure>::PressureBase() {
  CFormationLoadBase<Pressure> *pPressure = dynamic_cast<CFormationLoadBase<Pressure> *>(&Parent());
  assert(pPressure);
  return *pPressure;
}

template <typename Pressure>
IValueDomainScalar::TMinMax CPressureComponent<Pressure>::MinMax(IProgressBase &progressBase,
                                                                 const CQuantity::UNIT unit) const {
  geo::CBox box(PressureBase().Min(), PressureBase().Max());
  return ScalarData().MinMax(progressBase, box, unit);
}

template <typename Pressure>
IValueDomainScalar::TValue CPressureComponent<Pressure>::Average(IProgressBase &progressBase,
                                                                 const CQuantity::UNIT unit) const {
  geo::CBox box(PressureBase().Min(), PressureBase().Max());
  return ScalarData().Average(progressBase, box, unit);
}

template <typename Pressure> QString CPressureComponent<Pressure>::UnitName(const CQuantity::UNIT unit) const {
  QString sRet;
  if (unit == CQuantity::SI_UNIT)
    sRet = getStringTableEntry(IDS_UNIT_SI_PRESSURE);
  else
    sRet = getStringTableEntry(IDS_UNIT_FIELD_PRESSURE);

  return sRet;
}

template <typename Pressure> QString CPressureComponent<Pressure>::ExportLabel() const {
  return getStringTableEntry(IDS_ET_PRESSURE);
}

template <typename Pressure>
IValueDomainScalar::TValueVec
CPressureComponent<Pressure>::ValueElement(const geo::IElement &elm, const CQuantity::UNIT unit,
                                           geo::IParallelInitializationCallback *cb) const {
  IValueDomainScalar::TValueVec vcRet(elm.NrOfPoints());
  std::vector<int> vcNrOfValue(elm.NrOfPoints(), 0);
  if (PressureBase().DistributedSize() > 0) {
    for (size_t i = 0; i < PressureBase().DistributedSize(); i++) {
      const Pressure &pressure = PressureBase().DistributedValue(i);
      IValueDomainScalar::TValueVec vcValue = pressure.Component().ScalarData().ValueElement(elm, unit, cb);
      for (int n = 0; n < elm.NrOfPoints(); n++) {
        if ((PressureBase().DistributedOnly() || pressure.PointSet().PointInConvexHull(elm.Point(n))) &&
            vcValue[n].Valid()) {
          if (vcNrOfValue[n] == 0)
            vcRet[n] = vcValue[n];
          else
            vcRet[n] = vcRet[n] + vcValue[n];
          vcNrOfValue[n]++;
        }
      }
    }

    for (int n = 0; n < elm.NrOfPoints(); n++) {
      if (vcNrOfValue[n] > 1)
        vcRet[n] = vcRet[n] / vcNrOfValue[n];
    }
  }

  return vcRet;
}

// PRE	: true
// POST	: Not Valid()	Point is outside hull and not extrapolated
//		  Valid()		A value is taken from the distributed set
template <typename Pressure>
IValueDomainScalar::TValue CPressureComponent<Pressure>::ValuePoint(const geo::IPoint &pt, const CQuantity::UNIT unit,
                                                                    geo::IParallelInitializationCallback *cb) const {
  if (PressureBase().DistributedSize() > 0) {
    int n = 0;
    double dResult = 0;
    for (size_t i = 0; i < PressureBase().DistributedSize(); i++) {
      const Pressure &pressure = PressureBase().DistributedValue(i);
      if (PressureBase().DistributedOnly() || pressure.PointSet().PointInConvexHull(pt)) {
        geo::CValue value = pressure.Component().ScalarData().ValuePoint(pt, unit, cb);
        if (!value.Valid())
          return geo::CValue();
        dResult += value.Value();
        n++;
      }
    }

    if (n > 0)
      return geo::CValue(dResult / n);
  }

  return geo::CValue();
}

template <typename Pressure>
CPressureComponent<Pressure> &CPressureComponent<Pressure>::operator=(const CPressureComponent &rhs) {
  IValueComponentBase::operator=(rhs);
  return *this;
}

template <typename Pressure> bool CPressureComponent<Pressure>::operator==(const CPressureComponent &rhs) const {
  return IValueComponentBase::operator==(rhs);
}

template <typename Pressure> bool CPressureComponent<Pressure>::Empty() const { return m_bEmpty; }

template <typename Pressure>
void CPressureComponent<Pressure>::LoadStream(TSTREAM & /*stream*/, CStreamVersion & /*version*/,
                                              TPROGRESS & /*progress*/) {
  m_bEmpty = false;
}

template <typename Pressure>
void CPressureComponent<Pressure>::SaveStream(TSTREAM & /*stream*/, TPROGRESS & /*progress*/) {}

template <typename Pressure> bool CPressureComponent<Pressure>::CanMap(const COpenGLNode &node) const {
  const CFormationBase *pFormation = dynamic_cast<const CFormationBase *>(&node);
  const CFormationPlane *pFormationPlane = dynamic_cast<const CFormationPlane *>(&node);
  const CFormationLoadBase<Pressure> *pPressure = (&PressureBase());
  if (pPressure) // Pore pressure
  {
    if (pFormation)
      return pFormation == pPressure->Formation();
    if (pFormationPlane)
      return (&pFormationPlane->Formation()) == (pPressure->Formation());
  }

  return pFormation || pFormationPlane;
}

#endif // !defined(AFX_PRESSUREBASE_H__94D57B09_9388_4D51_BF26_524EF8E35505__INCLUDED_)
