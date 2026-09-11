#ifndef _FORMATIONLOADCOMPONENT_H_
#define _FORMATIONLOADCOMPONENT_H_

#include "FormationPlane.h"
#include "HexaFormation.h"
#include "FormationLoadBase.h"
#include "Box.h"
#include "ConvexHull.h"
#include "ConvexHull_2D.h"
#include "ValueTypeFactory.h"
#include "IPointSet.h"
#include "TetraMesh.h"
#include "HexaMesh.h"

template <class VALUE_TYPE>
class CFormationLoadComponent : public IValueComponentBase
{
	bool m_bEmpty;

public:
  typedef IValueDataInterfaceScalarTempl<CFormationLoadComponent<VALUE_TYPE> > IValueDataInterfaceFormationLoadScalar;

	// Construction
	CFormationLoadComponent(CFormationLoadBase<VALUE_TYPE> &formationload, unsigned int uMode, bool bEmpty = false);
	CFormationLoadComponent(const CFormationLoadComponent &rhs);

  virtual TValueComponentType Type() const;

  virtual const IValueDataInterfaceScalar& ScalarData() const;
  virtual IValueDataInterfaceScalar& ScalarData();

  // Access to the formation load 
	const CFormationLoadBase<VALUE_TYPE>& FormationLoadBase() const;
	CFormationLoadBase<VALUE_TYPE>& FormationLoadBase();

	// Unit name and overall min /max
//	virtual	QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
	virtual IValueDomainScalar::TMinMax MinMax(IProgressBase& progressBase, const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValue Average(IProgressBase& progressBase, const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;

	// Assignment and so ...
	CFormationLoadComponent& operator=(const CFormationLoadComponent &rhs);
	bool operator==(const CFormationLoadComponent &rhs) const;

	// Save and load
	virtual bool Empty() const;
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
//	virtual QString ExportLabel() const;

	// we only want to show values on formations and formation plane and when we are a depletion stage value we only want
	// to show it on formations to which we belong
	virtual bool CanMap(const COpenGLNode& node) const;

  virtual bool PrepareMapping(const geo::IElementSet *pElementSet) = 0;

protected:
  IValueDataInterfaceFormationLoadScalar m_Data;
};

template <class VALUE_TYPE>
CFormationLoadComponent<VALUE_TYPE>::CFormationLoadComponent(CFormationLoadBase<VALUE_TYPE> &formationload, unsigned int uMode, bool /*bEmpty*/)
: IValueComponentBase(formationload, 0, uMode), m_Data(*this)
{
	// wedx 27062007: Shouldn't the bEmpty flag be captured into m_bEmpty ??
}

template <class VALUE_TYPE>
CFormationLoadComponent<VALUE_TYPE>::CFormationLoadComponent(const CFormationLoadComponent &rhs)
: IValueComponentBase(rhs), m_bEmpty(rhs.m_bEmpty), m_Data(rhs.m_Data)
{
}

template <class VALUE_TYPE>
IValueComponentBase::TValueComponentType CFormationLoadComponent<VALUE_TYPE>::Type() const
{
  return static_cast<IValueComponentBase::TValueComponentType>(ScalarData().Type());
}

template <class VALUE_TYPE>
const IValueDataInterfaceScalar& CFormationLoadComponent<VALUE_TYPE>::ScalarData() const
{
  return m_Data;
}

template <class VALUE_TYPE>
IValueDataInterfaceScalar& CFormationLoadComponent<VALUE_TYPE>::ScalarData()
{
  return m_Data;
}

template <class VALUE_TYPE>
const CFormationLoadBase<VALUE_TYPE>& CFormationLoadComponent<VALUE_TYPE>::FormationLoadBase() const
{
	const CFormationLoadBase<VALUE_TYPE> *pFormationLoad = dynamic_cast<const CFormationLoadBase<VALUE_TYPE>*> (&Parent());
	assert(pFormationLoad);
	return *pFormationLoad;
}

template <class VALUE_TYPE>
CFormationLoadBase<VALUE_TYPE>& CFormationLoadComponent<VALUE_TYPE>::FormationLoadBase()
{
	CFormationLoadBase<VALUE_TYPE> *pFormationLoad = dynamic_cast<CFormationLoadBase<VALUE_TYPE>*> (&Parent());
	assert(pFormationLoad);
	return *pFormationLoad;
}

template <class VALUE_TYPE>
IValueDomainScalar::TMinMax CFormationLoadComponent<VALUE_TYPE>::MinMax(IProgressBase& progressBase, const CQuantity::UNIT unit) const
{
	geo::CBox box(FormationLoadBase().Min(), FormationLoadBase().Max());
	return ScalarData().MinMax(progressBase, box, unit);
}

template <class VALUE_TYPE>
IValueDomainScalar::TValue CFormationLoadComponent<VALUE_TYPE>::Average(IProgressBase& progressBase, const CQuantity::UNIT unit) const
{
  geo::CBox box(FormationLoadBase().Min(), FormationLoadBase().Max());
  return ScalarData().Average(progressBase, box, unit);
}

/*
template <class VALUE_TYPE>
QString CFormationLoadComponent<VALUE_TYPE>::UnitName(const CQuantity::UNIT unit) const
{
	QString sRet;
	if(unit == CQuantity::SI_UNIT)
		sRet = getStringTableEntry(IDS_UNIT_SI_PRESSURE);
	else
		sRet = getStringTableEntry(IDS_UNIT_FIELD_PRESSURE);

	return sRet;
}

template <class VALUE_TYPE>
QString CFormationLoadComponent<VALUE_TYPE>::ExportLabel() const
{
	 return getStringTableEntry(IDS_ET_PRESSURE);
}
*/

template <class VALUE_TYPE>
IValueDomainScalar::TValueVec CFormationLoadComponent<VALUE_TYPE>::ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  IValueDomainScalar::TValueVec vcRet(elm.NrOfPoints());
  std::vector<int> vcNrOfValue(elm.NrOfPoints(), 0);
  if(FormationLoadBase().DistributedSize() > 0)
  {
    std::vector <double> distance(elm.NrOfPoints(), 0);
    bool aPointInConvexHull = false;

    // wjrx mantis 3480
    //
    // Drawing of 'Full Formations' and 'Formation Skin' was different
    // for mapped values from element sets.
    //
    // A 3d Hexa-element consists of sub elements of type Quad.
    // If elm is a Quad, we use the parent element.
    //
    // Similar action for Triangle.
    //
    // This works for 2d elements, because a 2d element is never an IBody.
    //
    const geo::IElement *pElement = dynamic_cast<const geo::IBody *> (&elm);

    const geo::CBodyTriangle *pBodyTriangle = dynamic_cast<const geo::CBodyTriangle *> (&elm);
    if(pBodyTriangle) pElement = pBodyTriangle->Parent();

    const geo::CBodyQuadrilateral *pBodyQuad = dynamic_cast<const geo::CBodyQuadrilateral *> (&elm);
    if(pBodyQuad) pElement = pBodyQuad->Parent();

    if ( ! pElement ) pElement= &elm;

    for(size_t i = 0; i < FormationLoadBase().DistributedSize(); i++)
    {
      const VALUE_TYPE& formationload = FormationLoadBase().DistributedValue(i);

      aPointInConvexHull = aPointInConvexHull ||
      formationload.PointSet().PointInConvexHull(pElement->MidPoint());
    }

    for(size_t i = 0; i < FormationLoadBase().DistributedSize(); i++)
    {
      const VALUE_TYPE& formationload = FormationLoadBase().DistributedValue(i);
      IValueDomainScalar::TValueVec vcValue = formationload.Component().ScalarData().ValueElement(elm, unit, cb);
      bool anElementInConvexHull =
      formationload.PointSet().PointInConvexHull(pElement->MidPoint());

      for (int n = 0; n < elm.NrOfPoints(); n++)
      {
        if (aPointInConvexHull && vcValue[n].Valid())
        {
          if (anElementInConvexHull)
          {
            if (vcNrOfValue[n] == 0)
            vcRet[n] = vcValue[n];
            else
            vcRet[n] = vcRet[n] + vcValue[n];
            vcNrOfValue[n]++;
          }
        }
        else if (FormationLoadBase().DistributedOnly() && vcValue[n].Valid()) // validity necessary?
        {
          // wjrx mantis 3480
          //
          // formationload.PointSet() cal be a CPointSet or an IElementSet.
          //
          const CPointSet* pPointSet = dynamic_cast <const CPointSet*> (&formationload.PointSet());

          geo::CPoint midPoint;
          double representativeLength=-1;

          if ( pPointSet )
          {
            // if we have a convex hull, use it to calc midPoint and representativeLength
            if (  pPointSet->m_pHull_3D != 0 )
            {
              midPoint= pPointSet->m_pHull_3D->MidPoint();
              representativeLength= pPointSet->m_pHull_3D->RepresentativeLength();
            }
            else if (  pPointSet->m_pHull_2D != 0 )
            {
              midPoint= pPointSet->m_pHull_2D->MidPoint();
              representativeLength= pPointSet->m_pHull_2D->RepresentativeLength();
            }
          }

          if ( representativeLength < 0 ) // not CPointSet or no convex hull
          {
            midPoint= (formationload.PointSet().Min()+formationload.PointSet().Max())/2;

            // representativeLength is an approximation of the diameter of the point cloud
            //
            representativeLength= 
              formationload.PointSet().Min().Distance( formationload.PointSet().Max() )/ 2;
          }

          if ( representativeLength>0 )
          {
            double val= midPoint.Distance(elm.Point(n)) / representativeLength;

            if ( distance[n]==0 || val < distance[n] )
            {
                distance[n] = val;
                vcRet[n] = vcValue[n];
                vcNrOfValue[n] = 1;
            }
          }
        }
      }
    }

    for(int n = 0; n < elm.NrOfPoints(); n++)
    {
      if(vcNrOfValue[n] > 1)
        vcRet[n] = vcRet[n] / vcNrOfValue[n];
    }
  }

#ifdef IMPLEMENT_NEW_CH_CHECK
  // test whether new routine is same as old; if satisfactory, replace above with this (but without creating the wrapper all the time)
  FormationBaseTemplDistributedSetWrapper<VALUE_TYPE> wrapper(FormationLoadBase());
  IValueDomainScalar::TValueVec vcRetCheck;
  CConvexHullCheck<IValueDomainScalar::TValueVec>::ValueElement(wrapper, elm, unit, vcRetCheck);

  assert(vcRet.size() == vcRetCheck.size());
  for (size_t i = 0; i < vcRet.size(); ++i)
  {
    assert(vcRet[i].Valid() == vcRetCheck[i].Valid());

    if (vcRet[i].Valid())
    {
      assert(vcRet[i] == vcRetCheck[i]);
    }
  }
#endif

  return vcRet;
}


// PRE	: true
// POST	: Not Valid()	Point is outside hull and not extrapolated
//		  Valid()		A value is taken from the distributed set
template <class VALUE_TYPE>
IValueDomainScalar::TValue CFormationLoadComponent<VALUE_TYPE>::ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
	if(FormationLoadBase().DistributedSize() > 0)
	{
		int n = 0;
		double dResult = 0;
		for(size_t i = 0; i < FormationLoadBase().DistributedSize(); i++)
		{
			const VALUE_TYPE& formationload = FormationLoadBase().DistributedValue(i);
			if(FormationLoadBase().DistributedOnly() || formationload.PointSet().PointInConvexHull(pt))
			{
				geo::CValue value = formationload.Component().ScalarData().ValuePoint(pt, unit, cb);
				if(!value.Valid())
					return geo::CValue();
				dResult += value.Value();
				n++;
			}
		}

		if(n > 0)
			return geo::CValue(dResult / n);
	}

	return geo::CValue();
}

template <class VALUE_TYPE>
CFormationLoadComponent<VALUE_TYPE>& CFormationLoadComponent<VALUE_TYPE>::operator=(const CFormationLoadComponent &rhs)
{
	IValueComponentBase::operator =(rhs);
	return *this;
}

template <class VALUE_TYPE>
bool CFormationLoadComponent<VALUE_TYPE>::operator==(const CFormationLoadComponent &rhs) const
{
	return IValueComponentBase::operator ==(rhs);
}

template <class VALUE_TYPE>
bool CFormationLoadComponent<VALUE_TYPE>::Empty() const
{
	return m_bEmpty;
}

template <class VALUE_TYPE>
void CFormationLoadComponent<VALUE_TYPE>::LoadStream(TSTREAM &/*stream*/, CStreamVersion &/*version*/, TPROGRESS &/*progress*/)
{
	m_bEmpty = false;
}

template <class VALUE_TYPE>
void CFormationLoadComponent<VALUE_TYPE>::SaveStream(TSTREAM &/*stream*/, TPROGRESS &/*progress*/)
{
}

template <class VALUE_TYPE>
bool CFormationLoadComponent<VALUE_TYPE>::CanMap(const COpenGLNode& node) const
{
	const CFormationBase* pFormation = dynamic_cast<const CFormationBase*>(&node);
	const CFormationPlane* pFormationPlane = dynamic_cast<const CFormationPlane*>(&node);

	if(FormationLoadBase().Formation()) // depletion stage value
	{
		if(pFormation)
			return pFormation == FormationLoadBase().Formation();
		if(pFormationPlane)
			return (&pFormationPlane->Formation()) == (FormationLoadBase().Formation());
	}
	
	return pFormation || pFormationPlane;
}

#endif // _FORMATIONLOADCOMPONENT_H_
