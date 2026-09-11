// DistributedQuantity.h: interface for the CDistributedQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISTRIBUTEDQUANTITY_H__E7746C19_DD33_4BD1_9D31_612E6CC025FE__INCLUDED_)
#define AFX_DISTRIBUTEDQUANTITY_H__E7746C19_DD33_4BD1_9D31_612E6CC025FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "geomec.h"
#include "ValueSet.h"
#include "QuantityNode.h"
#include "EntryTypes.h"
#include "PointSet.h"

//##ModelId=3BE7AEC401AB
class CDistributedQuantity : public CQuantityNode
{
  //##ModelId=3BE7AEC401DA
  std::vector<CValueSet*> m_vcValueSet;
  //##ModelId=3BE7AEC401EA
  const CPointSet& m_point_set;
public:
  // Construction / Destruction
  //##ModelId=3BE7AEC401F9
  CDistributedQuantity(CPointSet &point_set, const int nComponentSize = 1);
  CDistributedQuantity(const CDistributedQuantity &quantity);
  //##ModelId=3BE7AEC40208
  virtual ~CDistributedQuantity();

  // Access to assigned value sets
  //##ModelId=3BE7AEC4020A
  const CValueSet& ValueSet(const int nIndex) const;
  //##ModelId=3BE7AEC40218
  CValueSet& ValueSet(const int nIndex);
  //##ModelId=3BE7AEC40228
  BOOL IsValueSet(const int nIndex) const;
  //##ModelId=3BE7AEC4022B
  void ValueSet(const int nIndex, CValueSet &value);
  //##ModelId=3BE7AEC40238
  int ValueSetSize() const;
  bool Undefined() const;
  // Point set access ...
  //##ModelId=3BE7AEC4023A
  const CPointSet& PointSet() const;

  // Interface of CGraphNode
  //##ModelId=3BE7AEC4023C
  virtual void OnNewNeighbour(const CGraphNode &node);
  //##ModelId=3BE7AEC40249
  virtual void OnNeighbourDeleted(const CGraphNode &node);
  //##ModelId=3BE7AEC4024C
  virtual UINT TypeNameId() const;

  // Interface of IQuantity
  //##ModelId=3BE7AEC40256
  virtual int ComponentSize() const;
  //##ModelId=3BE7AEC40258
  virtual double MaxValue(const int nComponent = 0, const UNIT unit = SI_UNIT) const;
  //##ModelId=3BE7AEC40266
  virtual double MinValue(const int nComponent = 0, const UNIT unit = SI_UNIT) const;
  //##ModelId=3BE7AEC4026A
  virtual double Min(const geo::IBox &box, const int nComponent = 0, const UNIT unit = SI_UNIT) const;
  //##ModelId=3BE7AEC40278
  virtual double Max(const geo::IBox &box, const int nComponent = 0, const UNIT unit = SI_UNIT) const;
  //##ModelId=3BE7AEC40285
  virtual double Value(const geo::IPoint& pt, 
             const int nComponent = 0, 
             const UNIT unit = SI_UNIT) const;
  virtual	double Convert(const double &value, const UNIT out, const UNIT in) const;
  virtual const IQuantityEntry& QuantityEntry() const;
  virtual UINT IconId() const;
  virtual const COpenGLNode* ProjectOn() const;
  virtual std::string QuantityName() const;
  virtual std::string UnitName(const UNIT unit = SI_UNIT) const;

  virtual BOOL operator==(const CDistributedQuantity &rhs) const;
  CDistributedQuantity& operator=(const CDistributedQuantity &rhs);
  virtual void Properties();
};

template<UINT ENTRY_ID,				// Identifier to register globally the variable
     UINT ICON_ID,				// Resource identifier of the icon
     int  COMPONENTS,			// Nr of components 
     UINT QUANTITY_NAME_ID,		// Resource identifier of the quantity name
     UINT SI_UNIT_NAME_ID,		// Resource identifier of the SI unit name
     UINT FIELD_UNIT_NAME_ID,	// Resource identifier of the field unit name
     double FIELD_UNIT_FACTOR>	// Factor to calculate to SI to FIELD
class CImportTemp : public CDistributedQuantity
{
public:
  typedef CQuantityEntry<CQuantityNode> TQuantityEntry;
private:
  TQuantityEntry *m_pEntry;
public:
  CImportTemp(CPointSet &point_set)
  : CDistributedQuantity(point_set, COMPONENTS)
  {
    // Register the Quantity
    ASSERT(point_set.Model().GraphEntry(ENTRY_ID));
    m_pEntry = (TQuantityEntry*)(point_set.Model().GraphEntry(ENTRY_ID));
    m_pEntry->LinkTo(*this);
  }
  
  double Convert(const double &value, const UNIT out, const UNIT in) const
  {
    if(((CQuantity::SI_UNIT == out) && (CQuantity::SI_UNIT == in)) ||
       ((CQuantity::FIELD_UNIT == out) && (CQuantity::FIELD_UNIT == in)) )
       return value;

    if((CQuantity::FIELD_UNIT == out) && (CQuantity::SI_UNIT == in))
      return FIELD_UNIT_FACTOR * value;

    if((CQuantity::SI_UNIT == out) && (CQuantity::FIELD_UNIT == in))
      return FIELD_UNIT_FACTOR / value;

    ASSERT(FALSE);

    return 0;
  }

  virtual const IQuantityEntry& QuantityEntry() const
  {	return  *m_pEntry; }

  virtual UINT IconId() const
  {	return ICON_ID; }

  virtual const COpenGLNode* ProjectOn() const
  {	return &PointSet();	}

  virtual std::string QuantityName() const
  {
    CString strQuantityName;
    strQuantityName.LoadString(QUANTITY_NAME_ID);
    return std::string((LPCSTR)strQuantityName);
  }

  virtual std::string UnitName(const UNIT unit = SI_UNIT) const
  {
    CString strUnitName;
    if(unit == SI_UNIT)
      strUnitName.LoadString(SI_UNIT_NAME_ID);
    
    if(unit == FIELD_UNIT)
      strUnitName.LoadString(FIELD_UNIT_NAME_ID);

    return std::string((LPCSTR)strUnitName);
  }
};

// Pressure distributed ....
typedef CImportTemp<MD_QUANTITY_PRESSURE,			// Identifier to register globally the variable
          IDI_PRES_DISTRI,				// Resource identifier of the icon
          1,								// Nr of components 
          IDS_QUANTITY_PRESSURE,			// Resource identifier of the quantity name
          IDS_UNIT_SI_PRESSURE,			// Resource identifier of the SI unit name
          IDS_UNIT_FIELD_PRESSURE,		// Resource identifier of the field unit name
          1.000000000> TPressure;			// Factor to calculate to SI to FIELD 

// Stress Tensor distributed ....
typedef CImportTemp<MD_QUANTITY_STRESS,				// Identifier to register globally the variable
          IDI_STRESS_TENSOR,				// Resource identifier of the icon
          6,								// Nr of components 
          IDS_QUANTITY_STRESS_TENSOR,		// Resource identifier of the quantity name
          IDS_UNIT_SI_STRESS,				// Resource identifier of the SI unit name
          IDS_UNIT_FIELD_STRESS,			// Resource identifier of the field unit name
          1.000000000> TStressTensor;		// Factor to calculate to SI to FIELD 

// Minimal principal stress
typedef CImportTemp<MD_QUANTITY_STRESS,						// Identifier to register globally the variable
          IDI_MINIMAL_PRINCIPAL_STRESS,			// Resource identifier of the icon
          1,										// Nr of components 
          IDS_QUANTITY_MINIMAL_PRINCIPAL_STRESS,	// Resource identifier of the quantity name
          IDS_UNIT_SI_STRESS,						// Resource identifier of the SI unit name
          IDS_UNIT_FIELD_STRESS,					// Resource identifier of the field unit name
          1.000000000> TMinimalPrincipalStress;	// Factor to calculate to SI to FIELD 

// Minimal principal stress
typedef CImportTemp<MD_QUANTITY_STRESS,						// Identifier to register globally the variable
          IDI_MAXIMAL_PRINCIPAL_STRESS,			// Resource identifier of the icon
          1,										// Nr of components 
          IDS_QUANTITY_MAXIMAL_PRINCIPAL_STRESS,	// Resource identifier of the quantity name
          IDS_UNIT_SI_STRESS,						// Resource identifier of the SI unit name
          IDS_UNIT_FIELD_STRESS,					// Resource identifier of the field unit name
          1.000000000> TMaximalPrincipalStress;	// Factor to calculate to SI to FIELD 

// Vertical principal stress
typedef CImportTemp<MD_QUANTITY_STRESS,						// Identifier to register globally the variable
          IDI_VERTICAL_STRESS,					// Resource identifier of the icon
          1,										// Nr of components 
          IDS_QUANTITY_VERTICAL_STRESS,			// Resource identifier of the quantity name
          IDS_UNIT_SI_STRESS,						// Resource identifier of the SI unit name
          IDS_UNIT_FIELD_STRESS,					// Resource identifier of the field unit name
          1.000000000> TVerticalStress;			// Factor to calculate to SI to FIELD 

// Young's Modulus distributed ....
typedef CImportTemp<MD_QUANTITY_YOUNGS_MODULUS,		// Identifier to register globally the variable
          IDI_YOUNGS_MODULUS,				// Resource identifier of the icon
          1,								// Nr of components 
          IDS_QUANTITY_YOUNGS_MODULUS,	// Resource identifier of the quantity name
          IDS_UNIT_DIMENSIONLESS,			// Resource identifier of the SI unit name
          IDS_UNIT_DIMENSIONLESS,			// Resource identifier of the field unit name
          1.000000000> TYoungsModulus;	// Factor to calculate to SI to FIELD 

// Poisson's Ratio
typedef CImportTemp<MD_QUANTITY_POISSONS_RATIO,		// Identifier to register globally the variable
          IDI_POISSONS_RATIO,				// Resource identifier of the icon
          1,								// Nr of components 
          IDS_QUANTITY_POISSONS_RATIO,	// Resource identifier of the quantity name
          IDS_UNIT_DIMENSIONLESS,			// Resource identifier of the SI unit name
          IDS_UNIT_DIMENSIONLESS,			// Resource identifier of the field unit name
          1.000000000> TPoissonsRatio;	// Factor to calculate to SI to FIELD

// Cohesion
typedef CImportTemp<MD_QUANTITY_COHESION,			// Identifier to register globally the variable
          IDI_COHESION,					// Resource identifier of the icon
          1,								// Nr of components 
          IDS_QUANTITY_COHESION,			// Resource identifier of the quantity name
          IDS_UNIT_SI_COHESION,			// Resource identifier of the SI unit name
          IDS_UNIT_FIELD_COHESION,		// Resource identifier of the field unit name
          1.000000000> TCohesion;			// Factor to calculate to SI to FIELD 

// Friction Angle
typedef CImportTemp<MD_QUANTITY_FRICTION_ANGLE,		// Identifier to register globally the variable
          IDI_FRICTION_ANGLE,				// Resource identifier of the icon
          1,								// Nr of components 
          IDS_QUANTITY_FRICTION_ANGLE,	// Resource identifier of the quantity name
          IDS_UNIT_SI_ANGLE,				// Resource identifier of the SI unit name
          IDS_UNIT_FIELD_ANGLE,			// Resource identifier of the field unit name
          1.000000000> TFrictionAngle;	// Factor to calculate to SI to FIELD 

// Porostiy
typedef CImportTemp<MD_QUANTITY_POROSITY,			// Identifier to register globally the variable
          IDI_POROSITY,					// Resource identifier of the icon
          1,								// Nr of components 
          IDS_QUANTITY_POROSITY,			// Resource identifier of the quantity name
          IDS_UNIT_DIMENSIONLESS,			// Resource identifier of the SI unit name
          IDS_UNIT_DIMENSIONLESS,			// Resource identifier of the field unit name
          1.000000000> TPorosity;			// Factor to calculate to SI to FIELD 

// Bulk density
typedef CImportTemp<MD_QUANTITY_BULK_DENSITY,		// Identifier to register globally the variable
          IDI_BULK_DENSITY,				// Resource identifier of the icon
          1,								// Nr of components 
          IDS_QUANTITY_POROSITY,			// Resource identifier of the quantity name
          IDS_UNIT_SI_MATERIAL_DENSITY,	// Resource identifier of the SI unit name
          IDS_UNIT_FIELD_MATERIAL_DENSITY,// Resource identifier of the field unit name
          1.000000000> TBulkDensity;		// Factor to calculate to SI to FIELD 

// Compressional veloctiy
typedef CImportTemp<MD_QUANTITY_COMPRESSIONAL_VELOCITY,		// Identifier to register globally the variable
          IDI_COMPRESSIONAL_VELOCITY,				// Resource identifier of the icon
          1,										// Nr of components 
          IDS_QUANTITY_COMPRESSIONAL_VELOCITY,	// Resource identifier of the quantity name
          IDS_UNIT_SI_VELOCITY,					// Resource identifier of the SI unit name
          IDS_UNIT_FIELD_VELOCITY,				// Resource identifier of the field unit name
          1.000000000> TCompressionalVelocity;	// Factor to calculate to SI to FIELD 

// Shear velocity
typedef CImportTemp<MD_QUANTITY_SHEAR_VELOCITY,				// Identifier to register globally the variable
          IDI_SHEAR_VELOCITY,						// Resource identifier of the icon
          1,										// Nr of components 
          IDS_QUANTITY_SHEAR_VELOCITY,			// Resource identifier of the quantity name
          IDS_UNIT_SI_VELOCITY,					// Resource identifier of the SI unit name
          IDS_UNIT_FIELD_VELOCITY,				// Resource identifier of the field unit name
          1.000000000> TShearVelocity;			// Factor to calculate to SI to FIELD 


// Vertical permeability
typedef CImportTemp<MD_QUANTITY_PERMEABILITY,				// Identifier to register globally the variable
          IDI_VERTICAL_PERMEABILITY,				// Resource identifier of the icon
          1,										// Nr of components 
          IDS_QUANTITY_VERTICAL_PERMEABILITY,		// Resource identifier of the quantity name
          IDS_UNIT_DIMENSIONLESS,					// Resource identifier of the SI unit name
          IDS_UNIT_DIMENSIONLESS,					// Resource identifier of the field unit name
          1.000000000> TVerticalPermeability;		// Factor to calculate to SI to FIELD 

// Horizon permeability
typedef CImportTemp<MD_QUANTITY_PERMEABILITY,				// Identifier to register globally the variable
          IDI_HORIZONTAL_PERMEABILITY,			// Resource identifier of the icon
          1,										// Nr of components 
          IDS_QUANTITY_HORIZONTAL_PERMEABILITY,	// Resource identifier of the quantity name
          IDS_UNIT_DIMENSIONLESS,				// Resource identifier of the SI unit name
          IDS_UNIT_DIMENSIONLESS,			// Resource identifier of the field unit name
          1.000000000> THorizontalPermeability;		// Factor to calculate to SI to FIELD 


class CQuantityBuilder
{
public:
  CQuantityBuilder() {};
  virtual CDistributedQuantity* Build(CPointSet &point_set) = 0;
};

template<class T>
class CQuantityBuilderTemp : public CQuantityBuilder
{
public:
  CQuantityBuilderTemp() {};
  virtual CDistributedQuantity* Build(CPointSet &point_set)
  {
    CDistributedQuantity *pQuantity = new T(point_set);
    return pQuantity;
  }
};







#endif // !defined(AFX_DISTRIBUTEDQUANTITY_H__E7746C19_DD33_4BD1_9D31_612E6CC025FE__INCLUDED_)
