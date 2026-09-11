// ValueTypeFactory.h: interface for the CValueTypeFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VALUETYPEFACTORY_H__8BBA9A1E_1769_4A91_8C58_5543D9A84770__INCLUDED_)
#define AFX_VALUETYPEFACTORY_H__8BBA9A1E_1769_4A91_8C58_5543D9A84770__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ValueType.h"
#include "ValueTypes.h"
#include "ValueComponent.h"
#include "StringNoCaseLess.h"
#include "resourceIDS.h"
#include "resourceIDI.h"

class CValueTypeBuilder
{
public:
  CValueTypeBuilder() {};
  virtual ~CValueTypeBuilder() = 0;
  virtual CValueType* Build(IPointSet &point_set, const QString &sName) = 0;
  virtual CValueType* Build(IPointSet &point_set, unsigned int uName) = 0;
  virtual unsigned int IconID() const = 0;
};

class CValueTypeFactory  
{
  typedef std::pair<CValueTypeBuilder*, unsigned int> TBuilder;
  typedef std::map<unsigned int, TBuilder> TBuilderMap;
  typedef std::map<QString, unsigned int, CStringNoCaseLess> TImportMap;
  TBuilderMap m_mpBuilder;
  TImportMap m_mpImport;
  typedef std::map <unsigned int, QString> TValueType2ImportTag;
  TValueType2ImportTag m_mpValueType2ImportTag;

// Initialisation 
  void AppendValueTypeBuilder(unsigned int uValueType, unsigned int uNameId, unsigned int uExportId, CValueTypeBuilder *pBuilder);

  CValueTypeFactory();
  CValueTypeFactory(const CValueTypeFactory&);
  CValueTypeFactory& operator=(const CValueTypeFactory&);
public:
// Construction ...
  static const CValueTypeFactory *instance();
  static void reset();

// Get contents library
  typedef std::pair<unsigned int, unsigned int> TValueTypeIdNamePair;
  typedef std::vector<TValueTypeIdNamePair> TValueTypeVec;
  TValueTypeVec ValueTypes() const;
  int NameIndex(unsigned int uValueType) const;	// Return -1 when failed else the resource index is returned
  QString getImportTag(unsigned int valueType) const;

// Function to build a quantity
  bool ValueTypeAvailable(unsigned int uValueType) const;
  CValueType* BuildValueType(IPointSet &point_set, unsigned int uValueType, unsigned int uName) const;
  CValueType* BuildValueType(IPointSet &point_set, unsigned int uValueType, const QString &sName) const;
  CValueType* BuildValueType(IPointSet &point_set, const QString &sFileTag, unsigned int uName) const;
  CValueType* BuildValueType(IPointSet &point_set, const QString &sFileTag, const QString &sName) const;

  unsigned int ValueTypeIconID(unsigned int uValueType) const;

};


template<class T>
class CValueTypeBuilderTemp : public CValueTypeBuilder
{
public:
  CValueTypeBuilderTemp() {};
  virtual CValueType* Build(IPointSet &point_set, const QString &sName)
  {
    CValueType *pValueType = new T(point_set, sName);
    return pValueType;
  }

  virtual CValueType* Build(IPointSet &point_set, unsigned int uName)
  {
    QString sName;
    sName = getStringTableEntry(uName);
    return Build(point_set, sName);
  }

  virtual unsigned int IconID() const
  {
  return T::icon_id();
  }
};

         

// Surface
class CSurfaceBase;

class TLengthComponent : public CComponentTemp<IDS_COMPONENT_DEFAULT, 0>
{
public:
  TLengthComponent(CValueType &value_type, IValueSet &value_set)
  : CComponentTemp<IDS_COMPONENT_DEFAULT, 0>(value_type, value_set, Units::LengthUnit, Ranges::NoRange)
  {
  }

  TLengthComponent(CValueType &value_type)
  : CComponentTemp<IDS_COMPONENT_DEFAULT, 0>(value_type, Units::LengthUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTLengthComponent);
};

class CSurfaceValueType : public CValueTypeTemp<IDT_VALUETYPE_SURFACE, IDI_VALUETYPE_SURFACE, IDS_VALUENAME_SURFACE, IDS_ET_SURFACE>
{
  typedef CValueTypeTemp<IDT_VALUETYPE_SURFACE, IDI_VALUETYPE_SURFACE, IDS_VALUENAME_SURFACE, IDS_ET_SURFACE> TBaseClass;
  CSurfaceBase *m_pSurface;
public:
  CSurfaceValueType(IPointSet& point_set, const QString &strName);
  ~CSurfaceValueType();
  virtual const QString& Name() const;
  virtual void Name(const QString& sName);
  virtual void OnNeighbourDeleted(const CGraphNode& node);
  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);
  const CSurfaceBase& Surface() const;
  CSurfaceBase& Surface();
  virtual void Unit(CQuantity::UNIT unit);
  virtual bool CanDestroy() const;

  ACCEPT_GEOMECMODELVISITORS(VisitSurfaceValueType);
};

// Template single types
template<unsigned int uTypeId,
     unsigned int uIconId,
     unsigned int uExportTagId, 
     unsigned int uValueNameId>
class CSingleComponentTemp : public CValueTypeTemp<uTypeId, uIconId, uValueNameId, uExportTagId>
{
public:
  CSingleComponentTemp(IPointSet& point_set, const QString &strName, const Units::CUnitType& unittype, const Ranges::CRangeType& rangetype)
  : CValueTypeTemp<uTypeId, uIconId, uValueNameId, uExportTagId>(point_set, strName)
  {
    new CComponentTemp<IDS_COMPONENT_DEFAULT, 0>(*this, unittype, rangetype);
  }

  CSingleComponentTemp(const CValueTypeTemp <uTypeId, uIconId, uValueNameId, uExportTagId> & rhs)
  : CValueTypeTemp<uTypeId, uIconId, uValueNameId, uExportTagId>(rhs)
  {
  }
};

// Template single types
template<unsigned int uTypeId,
     unsigned int uIconId,
     unsigned int uExportTagId, 
     unsigned int uValueNameId>
class CMaterialComponentTemp : public CSingleComponentTemp<uTypeId, uIconId, uExportTagId, uValueNameId>
{
public:
  CMaterialComponentTemp(IPointSet& point_set, const QString &strName, const Units::CUnitType& unittype, const Ranges::CRangeType& rangetype)
  : CSingleComponentTemp<uTypeId, uIconId, uExportTagId, uValueNameId>(point_set, strName, unittype, rangetype)
  {
  }

  CMaterialComponentTemp(const CValueTypeTemp <uTypeId, uIconId, uValueNameId, uExportTagId> & rhs)
  : CSingleComponentTemp<uTypeId, uIconId, uExportTagId, uValueNameId>(rhs)
  {
  }

  virtual bool SingleElementValue() const
  {
    return true;
  }
};

// Pressure
class TPressure : public CSingleComponentTemp<IDT_VALUETYPE_PRESSURE, 
                              IDI_COMPONENT_PRESSURE_DISTRIBUTED,
                                              IDS_ET_PRESSURE,
                                              IDS_VALUENAME_PRESSURE>
{
public:
  TPressure(IPointSet& point_set, const QString &strName)
  : CSingleComponentTemp<IDT_VALUETYPE_PRESSURE, IDI_COMPONENT_PRESSURE_DISTRIBUTED, IDS_ET_PRESSURE, IDS_VALUENAME_PRESSURE>(point_set, strName, Units::PressureUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTPressure);
};

// Fracture Matrix Pressure
class TFractureMatrixPressure : public CSingleComponentTemp<IDT_VALUETYPE_FRACTURE_MATRIX_PRESSURE, 
                              IDI_COMPONENT_FRACTURE_MATRIX_PRESSURE_DISTRIBUTED,
                              IDS_ET_FRACTURE_MATRIX_PRESSURE,
                              IDS_VALUENAME_FRACTURE_MATRIX_PRESSURE>
{
public:
  TFractureMatrixPressure(IPointSet& point_set, const QString &strName)
  : CSingleComponentTemp<IDT_VALUETYPE_FRACTURE_MATRIX_PRESSURE, IDI_COMPONENT_FRACTURE_MATRIX_PRESSURE_DISTRIBUTED, IDS_ET_FRACTURE_MATRIX_PRESSURE, IDS_VALUENAME_FRACTURE_MATRIX_PRESSURE>(point_set, strName, Units::PressureUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTFractureMatrixPressure);
};

// Temperature
class TTemperature : public CSingleComponentTemp<IDT_VALUETYPE_TEMPERATURE,
                                                 IDI_COMPONENT_TEMPERATURE_DISTRIBUTED,
                                                 IDS_ET_TEMPERATURE,
                                                 IDS_VALUENAME_TEMPERATURE>
{
public:
  TTemperature(IPointSet& point_set, const QString& strName)
  : CSingleComponentTemp<IDT_VALUETYPE_TEMPERATURE, IDI_COMPONENT_TEMPERATURE_DISTRIBUTED, IDS_ET_TEMPERATURE, IDS_VALUENAME_TEMPERATURE>(point_set, strName, Units::TemperatureUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTTemperature);
};

// Correction strain
class TVolumetricStrain : public CSingleComponentTemp<IDT_VALUETYPE_VOLUMETRICSTRAIN,
                                                      IDI_COMPONENT_VOLUMETRICSTRAIN_DISTRIBUTED,
                                                      IDS_ET_VOLUMETRICSTRAIN,
                                                      IDS_VALUENAME_VOLUMETRICSTRAIN>
{
public:
  TVolumetricStrain(IPointSet& point_set, const QString& strName)
  : CSingleComponentTemp<IDT_VALUETYPE_VOLUMETRICSTRAIN, IDI_COMPONENT_VOLUMETRICSTRAIN_DISTRIBUTED, IDS_ET_VOLUMETRICSTRAIN, IDS_VALUENAME_VOLUMETRICSTRAIN>(point_set, strName, Units::DimensionLessUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTVolumetricStrain);
};

class TNormalStrain : public CSingleComponentTemp<IDT_VALUETYPE_NRMSTRAIN,
                                                      IDI_COMPONENT_NRMSTRAIN_DISTRIBUTED,
                                                      IDS_ET_NRMSTRAIN,
                                                      IDS_VALUENAME_NRMSTRAIN>
{
public:
  TNormalStrain(IPointSet& point_set, const QString& strName)
  : CSingleComponentTemp<IDT_VALUETYPE_NRMSTRAIN, IDI_COMPONENT_NRMSTRAIN_DISTRIBUTED, IDS_ET_NRMSTRAIN, IDS_VALUENAME_NRMSTRAIN>(point_set, strName, Units::DimensionLessUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTNormalStrain);
};

class TLateralStrain : public CSingleComponentTemp<IDT_VALUETYPE_LATSTRAIN,
                                                      IDI_COMPONENT_LATSTRAIN_DISTRIBUTED,
                                                      IDS_ET_LATSTRAIN,
                                                      IDS_VALUENAME_LATSTRAIN>
{
public:
  TLateralStrain(IPointSet& point_set, const QString& strName)
  : CSingleComponentTemp<IDT_VALUETYPE_LATSTRAIN, IDI_COMPONENT_LATSTRAIN_DISTRIBUTED, IDS_ET_LATSTRAIN, IDS_VALUENAME_LATSTRAIN>(point_set, strName, Units::DimensionLessUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTLateralStrain);
};

// Cohesion
class TCohesion : public CMaterialComponentTemp<IDT_VALUETYPE_COHESION,
                        IDI_VALUETYPE_COHESION,
                        IDS_ET_COHESION,
                        IDS_VALUENAME_COHESION>
{
public:
  TCohesion(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_COHESION, IDI_VALUETYPE_COHESION, IDS_ET_COHESION, IDS_VALUENAME_COHESION>(point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTCohesion);
};

// wjrx mantis 2517
// Bulk Stiffness 
class TBulkStiffness : public CMaterialComponentTemp<IDT_VALUETYPE_BULKSTIFFNESS,
                 IDI_VALUETYPE_BULKSTIFFNESS,
                 IDS_ET_BULKSTIFFNESS,
                 IDS_VALUENAME_BULKSTIFFNESS>
{
public: 
  // stress, mimimum >= 0.0
  TBulkStiffness(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_BULKSTIFFNESS, IDI_VALUETYPE_BULKSTIFFNESS, IDS_ET_BULKSTIFFNESS, IDS_VALUENAME_BULKSTIFFNESS>(point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTBulkStiffness);
};

class TShearModulus : public CMaterialComponentTemp<IDT_VALUETYPE_SHEARMODULUS,
                 IDI_VALUETYPE_SHEARMODULUS,
                 IDS_ET_SHEARMODULUS,
                 IDS_VALUENAME_SHEARMODULUS>
{
public:
  TShearModulus(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_SHEARMODULUS, IDI_VALUETYPE_SHEARMODULUS, IDS_ET_SHEARMODULUS, IDS_VALUENAME_SHEARMODULUS>(point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTShearModulus);
};

// Vp
class TVelocityP : public CMaterialComponentTemp<IDT_VALUETYPE_VELOCITYP,
                                                 IDI_VALUETYPE_VELOCITYP,
                                                 IDS_ET_VELOCITYP,
                                                 IDS_VALUENAME_VELOCITYP>
{
public:
  TVelocityP(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_VELOCITYP, IDI_VALUETYPE_VELOCITYP, IDS_ET_VELOCITYP, IDS_VALUENAME_VELOCITYP>(point_set, strName, Units::VelocityUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTVelocityP);
};

// Vs
class TVelocityS : public CMaterialComponentTemp<IDT_VALUETYPE_VELOCITYS,
                                                 IDI_VALUETYPE_VELOCITYS,
                                                 IDS_ET_VELOCITYS,
                                                 IDS_VALUENAME_VELOCITYS>
{
public:
  TVelocityS(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_VELOCITYS, IDI_VALUETYPE_VELOCITYS, IDS_ET_VELOCITYS, IDS_VALUENAME_VELOCITYS>(point_set, strName, Units::VelocityUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTVelocityS);
};

// Friction Angle
class TFrictionAngle : public CMaterialComponentTemp<IDT_VALUETYPE_FRICTION_ANGLE,
                                                     IDI_VALUETYPE_FRICTION_ANGLE, 
                                                     IDS_ET_FRICTION_ANGLE, 
                                                     IDS_VALUENAME_FRICTION_ANGLE>
{
public:
  TFrictionAngle(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_FRICTION_ANGLE, IDI_VALUETYPE_FRICTION_ANGLE, IDS_ET_FRICTION_ANGLE, IDS_VALUENAME_FRICTION_ANGLE>(point_set, strName, Units::AngleUnit, Ranges::CRangeType(true, 0.0, false, true, 70.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTFrictionAngle);
};

// Poissons Ratio
class TPoissonsRatio : public CMaterialComponentTemp<IDT_VALUETYPE_POISSONS_RATIO,
                                                     IDI_VALUETYPE_POISSONS_RATIO,
                                                     IDS_ET_POISSON_RATIO,
                                                     IDS_VALUENAME_POISSONS_RATIO>
{
public:
  TPoissonsRatio(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_POISSONS_RATIO, IDI_VALUETYPE_POISSONS_RATIO, IDS_ET_POISSON_RATIO, IDS_VALUENAME_POISSONS_RATIO>(point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, true, true, 0.5, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTPoissonsRatio);
};

// Youngs Modulus
class TYoungsModulus : public CMaterialComponentTemp<IDT_VALUETYPE_YOUNGS_MODULUS,
                                                     IDI_VALUETYPE_YOUNGS_MODULUS,
                                                     IDS_ET_YOUNG_MODULUS,
                                                     IDS_VALUENAME_YOUNGS_MODULUS>
{
public:
  TYoungsModulus(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_YOUNGS_MODULUS, IDI_VALUETYPE_YOUNGS_MODULUS, IDS_ET_YOUNG_MODULUS, IDS_VALUENAME_YOUNGS_MODULUS>(point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, false, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTYoungsModulus);
};

// Density 
class TDensity : public CMaterialComponentTemp<IDT_VALUETYPE_RHOB,
                                               IDI_VALUETYPE_RHOB,
                                               IDS_ET_RHOB, 
                                               IDS_VALUENAME_RHOB>
{
public:
  TDensity(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RHOB, IDI_VALUETYPE_RHOB, IDS_ET_RHOB, IDS_VALUENAME_RHOB>(point_set, strName, Units::DensityUnit, Ranges::CRangeType(true, 0.0, false, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTDensity);
};


// Porosity 
class TPorosity : public CMaterialComponentTemp<IDT_VALUETYPE_POROSITY,
                                                IDI_VALUETYPE_POROSITY, 
                                                IDS_ET_POROSITY, 
                                                IDS_VALUENAME_POROSITY>
{
public:
  TPorosity(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_POROSITY, IDI_VALUETYPE_POROSITY, IDS_ET_POROSITY, IDS_VALUENAME_POROSITY>(point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, true, true, 1.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTPorosity);
};

// Initial friction angle
class TInitFriction : public CMaterialComponentTemp<IDT_VALUETYPE_INITFRICTION,
                                                    IDI_VALUETYPE_INITFRICTION,
                                                    IDS_ET_INITFRICTION,
                                                    IDS_VALUENAME_INITFRICTION>
{
public:
  TInitFriction(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_INITFRICTION, IDI_VALUETYPE_INITFRICTION, IDS_ET_INITFRICTION, IDS_VALUENAME_INITFRICTION>(point_set, strName, Units::AngleUnit, Ranges::CRangeType(true, 0.0, true, true, 70.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTInitFriction);
};

// Hardening parameter
class THardening : public CMaterialComponentTemp<IDT_VALUETYPE_HARDENING,
                                                 IDI_VALUETYPE_HARDENING,
                                                 IDS_ET_HARDENING,
                                                 IDS_VALUENAME_HARDENING>
{
public:
  THardening(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_HARDENING, IDI_VALUETYPE_HARDENING, IDS_ET_HARDENING, IDS_VALUENAME_HARDENING>(point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, false, true, 1.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTHardening);
};

// Pre consolidation pressure
class TPreConsolidation : public CMaterialComponentTemp<IDT_VALUETYPE_PRECONSOLIDATION,
                                                        IDI_VALUETYPE_PRECONSOLIDATION,
                                                        IDS_ET_PRECONSOLIDATION,
                                                        IDS_VALUENAME_PRECONSOLIDATION>
{
public:
  TPreConsolidation(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_PRECONSOLIDATION, IDI_VALUETYPE_PRECONSOLIDATION, IDS_ET_PRECONSOLIDATION, IDS_VALUENAME_PRECONSOLIDATION>(point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, false, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTPreConsolidation);
};

// Cap shape factor
class TCapShape : public CMaterialComponentTemp<IDT_VALUETYPE_CAPSHAPE,
                              IDI_VALUETYPE_CAPSHAPE,
                              IDS_ET_CAPSHAPE,
                              IDS_VALUENAME_CAPSHAPE>
{
public:
  TCapShape(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_CAPSHAPE, IDI_VALUETYPE_CAPSHAPE, IDS_ET_CAPSHAPE, IDS_VALUENAME_CAPSHAPE>(point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.1, true, true, 5.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTCapShape);
};
 
// Dilatation angle
class TDilatation : public CMaterialComponentTemp<IDT_VALUETYPE_DILATATION,
                                IDI_VALUETYPE_DILATATION,
                                IDS_ET_DILATATION,
                                IDS_VALUENAME_DILATATION>
{
public:
  TDilatation(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_DILATATION, IDI_VALUETYPE_DILATATION, IDS_ET_DILATATION, IDS_VALUENAME_DILATATION>(point_set, strName, Units::AngleUnit, Ranges::CRangeType(true, 0.0, true, true, 90.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTDilatation);
};

// SHtot / Svtot
class TLatRatioMax : public CMaterialComponentTemp<IDT_VALUETYPE_LATRATIO_MAX,
                                                   IDI_VALUETYPE_LATRATIO_MAX,
                                                   IDS_ET_LATRATIO_MAX,
                                                   IDS_VALUENAME_LATRATIO_MAX>
{
public:
  TLatRatioMax(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_LATRATIO_MAX, IDI_VALUETYPE_LATRATIO_MAX, IDS_ET_LATRATIO_MAX, IDS_VALUENAME_LATRATIO_MAX>(point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTLatRatioMax);
};

// Shtot / Svtot
class TLatRatioMin : public CMaterialComponentTemp<IDT_VALUETYPE_LATRATIO_MIN,
                                                   IDI_VALUETYPE_LATRATIO_MIN,
                                                   IDS_ET_LATRATIO_MIN,
                                                   IDS_VALUENAME_LATRATIO_MIN>
{
public:
  TLatRatioMin(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_LATRATIO_MIN, IDI_VALUETYPE_LATRATIO_MIN, IDS_ET_LATRATIO_MIN, IDS_VALUENAME_LATRATIO_MIN>(point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTLatRatioMin);
};

// SHtot Azimuth
class TAzimuth : public CMaterialComponentTemp<IDT_VALUETYPE_AZIMUTH,
                                               IDI_VALUETYPE_AZIMUTH,
                                               IDS_ET_AZIMUTH,
                                               IDS_VALUENAME_AZIMUTH>
{
public:
  TAzimuth(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_AZIMUTH, IDI_VALUETYPE_AZIMUTH, IDS_ET_AZIMUTH, IDS_VALUENAME_AZIMUTH>(point_set, strName, Units::AngleUnit, Ranges::CRangeType(true, 0.0, true, true, 360.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTAzimuth);
};

// A1
class TCreepA1 : public CMaterialComponentTemp<IDT_VALUETYPE_CREEP_A1,
                                               IDI_VALUETYPE_CREEP_A1,
                                               IDS_ET_CREEP_A1,
                                               IDS_VALUENAME_CREEP_A1>
{
public:
  TCreepA1(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_CREEP_A1, IDI_VALUETYPE_CREEP_A1, IDS_ET_CREEP_A1, IDS_VALUENAME_CREEP_A1>(point_set, strName, Units::PerTimeUnit, Ranges::CRangeType(true, 0, true, false, 0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTCreepA1);
};

// n1
class TCreepN1 : public CMaterialComponentTemp<IDT_VALUETYPE_CREEP_N1,
                                               IDI_VALUETYPE_CREEP_N1,
                                               IDS_ET_CREEP_N1,
                                               IDS_VALUENAME_CREEP_N1>
{
public:
  TCreepN1(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_CREEP_N1, IDI_VALUETYPE_CREEP_N1, IDS_ET_CREEP_N1, IDS_VALUENAME_CREEP_N1>(point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, true, true, 5.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTCreepN1);
};

// A2
class TCreepA2 : public CMaterialComponentTemp<IDT_VALUETYPE_CREEP_A2,
                                               IDI_VALUETYPE_CREEP_A2,
                                               IDS_ET_CREEP_A2,
                                               IDS_VALUENAME_CREEP_A2>
{
public:
  TCreepA2(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_CREEP_A2, IDI_VALUETYPE_CREEP_A2, IDS_ET_CREEP_A2, IDS_VALUENAME_CREEP_A2>(point_set, strName, Units::PerTimeUnit, Ranges::CRangeType(true, 0, true, false, 0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTCreepA2);
};

// n2
class TCreepN2 : public CMaterialComponentTemp<IDT_VALUETYPE_CREEP_N2,
                                               IDI_VALUETYPE_CREEP_N2, 
                                               IDS_ET_CREEP_N2, 
                                               IDS_VALUENAME_CREEP_N2>
{
public:
  TCreepN2(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_CREEP_N2, IDI_VALUETYPE_CREEP_N2, IDS_ET_CREEP_N2, IDS_VALUENAME_CREEP_N2>(point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, true, true, 5.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTCreepN2);
};

class TFluidBulkModulus : public CMaterialComponentTemp<IDT_VALUETYPE_FLUID_BULK_MOD,
                                  IDI_VALUETYPE_FLUID_BULK_MOD,
                                  IDS_ET_VALUETYPE_FLUID_SHEAR_MOD,
                                  IDS_VALUENAME_FLUID_BULK_MOD>
{
public:
  TFluidBulkModulus(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_FLUID_BULK_MOD, IDI_VALUETYPE_FLUID_BULK_MOD, IDS_ET_VALUETYPE_FLUID_SHEAR_MOD, IDS_VALUENAME_FLUID_BULK_MOD>(point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, false, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTFluidBulkModulus);
};


// Cohesion hardening
class THardCohesion1 : public CMaterialComponentTemp<IDT_VALUETYPE_HARD_COHESION1,
                                                     IDI_VALUETYPE_HARD_COHESION1, 
                                                     IDS_ET_HARD_COHESION1, 
                                                     IDS_VALUENAME_HARD_COHESION1>
{
public:
  THardCohesion1(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_HARD_COHESION1, IDI_VALUETYPE_HARD_COHESION1, IDS_ET_HARD_COHESION1, IDS_VALUENAME_HARD_COHESION1>(point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTHardCohesion1);
};

class THardCohesion2 : public CMaterialComponentTemp<IDT_VALUETYPE_HARD_COHESION2,
                                                     IDI_VALUETYPE_HARD_COHESION2, 
  							                                     IDS_ET_HARD_COHESION2, 
    						                                     IDS_VALUENAME_HARD_COHESION2>
{
public:
  THardCohesion2(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_HARD_COHESION2, IDI_VALUETYPE_HARD_COHESION2, IDS_ET_HARD_COHESION2, IDS_VALUENAME_HARD_COHESION2>(point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTHardCohesion2);
};

class THardCohesion3 : public CMaterialComponentTemp<IDT_VALUETYPE_HARD_COHESION3,
                                                     IDI_VALUETYPE_HARD_COHESION3, 
                                                     IDS_ET_HARD_COHESION3,
                                                     IDS_VALUENAME_HARD_COHESION3>
{
public:
  THardCohesion3(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_HARD_COHESION3, IDI_VALUETYPE_HARD_COHESION3, IDS_ET_HARD_COHESION3, IDS_VALUENAME_HARD_COHESION3>(point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTHardCohesion3);
};

// Friction Angle hardening
class THardFriction1 : public CMaterialComponentTemp<IDT_VALUETYPE_HARD_FRICTION1,
                                                     IDI_VALUETYPE_HARD_FRICTION1, 
                                                     IDS_ET_HARD_FRICTION1, 
                                                     IDS_VALUENAME_HARD_FRICTION1>
{
public:
  THardFriction1(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_HARD_FRICTION1, IDI_VALUETYPE_HARD_FRICTION1, IDS_ET_HARD_FRICTION1, IDS_VALUENAME_HARD_FRICTION1>(point_set, strName, Units::AngleUnit, Ranges::CRangeType(true, 0.0, true, true, 70.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTHardFriction1);
};

class THardFriction2 : public CMaterialComponentTemp<IDT_VALUETYPE_HARD_FRICTION2,
                                                     IDI_VALUETYPE_HARD_FRICTION2, 
                                                     IDS_ET_HARD_FRICTION2,
                                                     IDS_VALUENAME_HARD_FRICTION2>
{
public:
  THardFriction2(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_HARD_FRICTION2, IDI_VALUETYPE_HARD_FRICTION2, IDS_ET_HARD_FRICTION2, IDS_VALUENAME_HARD_FRICTION2>(point_set, strName, Units::AngleUnit, Ranges::CRangeType(true, 0.0, true, true, 70.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTHardFriction2);
};

class THardFriction3 : public CMaterialComponentTemp<IDT_VALUETYPE_HARD_FRICTION3,
                                                     IDI_VALUETYPE_HARD_FRICTION3,
                                                     IDS_ET_HARD_FRICTION3,
                                                     IDS_VALUENAME_HARD_FRICTION3>
{
public:
  THardFriction3(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_HARD_FRICTION3, IDI_VALUETYPE_HARD_FRICTION3, IDS_ET_HARD_FRICTION3, IDS_VALUENAME_HARD_FRICTION3>(point_set, strName, Units::AngleUnit, Ranges::CRangeType(true, 0.0, true, true, 70.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTHardFriction3);
};

// Equivalent Plastic Strain
class TEquivalentPlasticStrain1 : public CMaterialComponentTemp<IDT_VALUETYPE_EQUIV_PLAST_STRAIN1,
                                      IDI_VALUETYPE_EQUIV_PLAST_STRAIN1,
                                      IDS_ET_EQUIV_PLAST_STRAIN1,
                                      IDS_VALUENAME_EQUIV_PLAST_STRAIN1>
{
public:
  TEquivalentPlasticStrain1(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_EQUIV_PLAST_STRAIN1, IDI_VALUETYPE_EQUIV_PLAST_STRAIN1, IDS_ET_EQUIV_PLAST_STRAIN1, IDS_VALUENAME_EQUIV_PLAST_STRAIN1>(point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, true, true, 1.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTEquivalentPlasticStrain1);
};

class TEquivalentPlasticStrain2 : public CMaterialComponentTemp<IDT_VALUETYPE_EQUIV_PLAST_STRAIN2,
                                                                IDI_VALUETYPE_EQUIV_PLAST_STRAIN2,
                                                                IDS_ET_EQUIV_PLAST_STRAIN2,
                                                                IDS_VALUENAME_EQUIV_PLAST_STRAIN2>
{
public:
  TEquivalentPlasticStrain2(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_EQUIV_PLAST_STRAIN2, IDI_VALUETYPE_EQUIV_PLAST_STRAIN2, IDS_ET_EQUIV_PLAST_STRAIN2, IDS_VALUENAME_EQUIV_PLAST_STRAIN2>(point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, true, true, 1.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTEquivalentPlasticStrain2);
};

class TEquivalentPlasticStrain3 : public CMaterialComponentTemp<IDT_VALUETYPE_EQUIV_PLAST_STRAIN3,
                                                                IDI_VALUETYPE_EQUIV_PLAST_STRAIN3,
                                                                IDS_ET_EQUIV_PLAST_STRAIN3,
                                                                IDS_VALUENAME_EQUIV_PLAST_STRAIN3>
{
public:
  TEquivalentPlasticStrain3(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_EQUIV_PLAST_STRAIN3, IDI_VALUETYPE_EQUIV_PLAST_STRAIN3, IDS_ET_EQUIV_PLAST_STRAIN3, IDS_VALUENAME_EQUIV_PLAST_STRAIN3>(point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, true, true, 1.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTEquivalentPlasticStrain3);
};

// Q1/R
class TCreepQR1 : public CMaterialComponentTemp<IDT_VALUETYPE_CREEP_QR1, 
                                                IDI_VALUETYPE_CREEP_QR1,
    							                              IDS_ET_CREEP_QR1,
        					                              IDS_VALUENAME_CREEP_QR1>
{
public:
  TCreepQR1(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_CREEP_QR1, IDI_VALUETYPE_CREEP_QR1, IDS_ET_CREEP_QR1, IDS_VALUENAME_CREEP_QR1>(point_set, strName, Units::TemperatureUnit, Ranges::CRangeType(false, 0.0, true, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTCreepQR1);
};


// Q2/R
class TCreepQR2 : public CMaterialComponentTemp<IDT_VALUETYPE_CREEP_QR2, 
                                                IDI_VALUETYPE_CREEP_QR2,
                              IDS_ET_CREEP_QR2,
                                                IDS_VALUENAME_CREEP_QR2>
{
public:
  TCreepQR2(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_CREEP_QR2, IDI_VALUETYPE_CREEP_QR2, IDS_ET_CREEP_QR2, IDS_VALUENAME_CREEP_QR2>(point_set, strName, Units::TemperatureUnit, Ranges::CRangeType(false, 0.0, true, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTCreepQR2);
};

// alpha
class TCreepAlpha : public CMaterialComponentTemp<IDT_VALUETYPE_CREEP_ALPHA,
                                                  IDI_VALUETYPE_CREEP_ALPHA,
                                                  IDS_ET_CREEP_ALPHA,
                                                  IDS_VALUENAME_CREEP_ALPHA>
{
public:
  TCreepAlpha(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_CREEP_ALPHA, IDI_VALUETYPE_CREEP_ALPHA, IDS_ET_CREEP_ALPHA, IDS_VALUENAME_CREEP_ALPHA>(point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 1e-3, true, true, 1e3, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTCreepAlpha);
};

class TTensileStretch : public CMaterialComponentTemp<IDT_VALUETYPE_TENSILE_STRETCH, 
                                IDI_VALUETYPE_TENSILE_STRETCH,
                                  IDS_ET_TENSILE_STRETCH,
                                  IDS_VALUENAME_TENSILE_STRETCH>
{
public:
  TTensileStretch(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_TENSILE_STRETCH, IDI_VALUETYPE_TENSILE_STRETCH, IDS_ET_TENSILE_STRETCH, IDS_VALUENAME_TENSILE_STRETCH>(point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(false, 0.0, true, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTTensileStretch);
};

class TThermalExpansion : public CMaterialComponentTemp<IDT_VALUETYPE_THERMAL_EXPANSION,
                                                        IDI_VALUETYPE_THERMAL_EXPANSION,
                                                        IDS_ET_THERMAL_EXPANSION,
                                                        IDS_VALUENAME_THERMAL_EXPANSION>
{
public:
  TThermalExpansion(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_THERMAL_EXPANSION, IDI_VALUETYPE_THERMAL_EXPANSION, IDS_ET_THERMAL_EXPANSION, IDS_VALUENAME_THERMAL_EXPANSION>(point_set, strName, Units::PerTemperUnit, Ranges::CRangeType(true, 0.0, true, true, 1.E-3, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTThermalExpansion);
};

// Homogenization rigidity parameters
class TRigidParam1 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM1,
                                                   IDI_VALUETYPE_RIGID_1,
                                                   IDS_RIGI_PARAM1,
                                                   IDS_RIGI_PARAM1>
{
public:
  TRigidParam1(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM1, IDI_VALUETYPE_RIGID_1, IDS_RIGI_PARAM1, IDS_RIGI_PARAM1>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam1);
};

class TRigidParam2 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM2,
                                                   IDI_VALUETYPE_RIGID_2,
                                                   IDS_RIGI_PARAM2,
                                                   IDS_RIGI_PARAM2>
{
public:
  TRigidParam2(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM2, IDI_VALUETYPE_RIGID_2, IDS_RIGI_PARAM2, IDS_RIGI_PARAM2>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam2);
};

class TRigidParam3 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM3,
                                                   IDI_VALUETYPE_RIGID_3,
                                                   IDS_RIGI_PARAM3,
                                                   IDS_RIGI_PARAM3>
{
public:
  TRigidParam3(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM3, IDI_VALUETYPE_RIGID_3, IDS_RIGI_PARAM3, IDS_RIGI_PARAM3>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam3);
};

class TRigidParam4 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM4,
                                                   IDI_VALUETYPE_RIGID_4,
                                                   IDS_RIGI_PARAM4,
                                                   IDS_RIGI_PARAM4>
{
public:
  TRigidParam4(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM4, IDI_VALUETYPE_RIGID_4, IDS_RIGI_PARAM4, IDS_RIGI_PARAM4>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam4);
};

class TRigidParam5 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM5,
                                                   IDI_VALUETYPE_RIGID_5,
                                                   IDS_RIGI_PARAM5,
                                                   IDS_RIGI_PARAM5>
{
public:
  TRigidParam5(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM5, IDI_VALUETYPE_RIGID_5, IDS_RIGI_PARAM5, IDS_RIGI_PARAM5>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam5);
};

class TRigidParam6 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM6,
                                                   IDI_VALUETYPE_RIGID_6,
                                                   IDS_RIGI_PARAM6,
                                                   IDS_RIGI_PARAM6>
{
public:
  TRigidParam6(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM6, IDI_VALUETYPE_RIGID_6, IDS_RIGI_PARAM6, IDS_RIGI_PARAM6>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam6);
};

class TRigidParam7 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM7,
                                                   IDI_VALUETYPE_RIGID_7,
                                                   IDS_RIGI_PARAM7,
                                                   IDS_RIGI_PARAM7>
{
public:
  TRigidParam7(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM7, IDI_VALUETYPE_RIGID_7, IDS_RIGI_PARAM7, IDS_RIGI_PARAM7>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam7);
};

class TRigidParam8 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM8,
                                                   IDI_VALUETYPE_RIGID_8,
                                                   IDS_RIGI_PARAM8,
                                                   IDS_RIGI_PARAM8>
{
public:
  TRigidParam8(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM8, IDI_VALUETYPE_RIGID_8, IDS_RIGI_PARAM8, IDS_RIGI_PARAM8>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam8);
};

class TRigidParam9 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM9,
                                                   IDI_VALUETYPE_RIGID_9,
                                                   IDS_RIGI_PARAM9,
                                                   IDS_RIGI_PARAM9>
{
public:
  TRigidParam9(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM9, IDI_VALUETYPE_RIGID_9, IDS_RIGI_PARAM9, IDS_RIGI_PARAM9>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam9);
};

class TRigidParam10 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM10,
                                                   IDI_VALUETYPE_RIGID_10,
                                                   IDS_RIGI_PARAM10,
                                                   IDS_RIGI_PARAM10>
{
public:
  TRigidParam10(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM10, IDI_VALUETYPE_RIGID_10, IDS_RIGI_PARAM10, IDS_RIGI_PARAM10>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam10);
};

class TRigidParam11 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM11,
                                                   IDI_VALUETYPE_RIGID_11,
                                                   IDS_RIGI_PARAM11,
                                                   IDS_RIGI_PARAM11>
{
public:
  TRigidParam11(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM11, IDI_VALUETYPE_RIGID_11, IDS_RIGI_PARAM11, IDS_RIGI_PARAM11>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam11);
};

class TRigidParam12 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM12,
                                                   IDI_VALUETYPE_RIGID_12,
                                                   IDS_RIGI_PARAM12,
                                                   IDS_RIGI_PARAM12>
{
public:
  TRigidParam12(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM12, IDI_VALUETYPE_RIGID_12, IDS_RIGI_PARAM12, IDS_RIGI_PARAM12>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam12);
};

class TRigidParam13 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM13,
                                                   IDI_VALUETYPE_RIGID_13,
                                                   IDS_RIGI_PARAM13,
                                                   IDS_RIGI_PARAM13>
{
public:
  TRigidParam13(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM13, IDI_VALUETYPE_RIGID_13, IDS_RIGI_PARAM13, IDS_RIGI_PARAM13>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam13);
};

class TRigidParam14 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM14,
                                                   IDI_VALUETYPE_RIGID_14,
                                                   IDS_RIGI_PARAM14,
                                                   IDS_RIGI_PARAM14>
{
public:
  TRigidParam14(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM14, IDI_VALUETYPE_RIGID_14, IDS_RIGI_PARAM14, IDS_RIGI_PARAM14>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam14);
};

class TRigidParam15 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM15,
                                                   IDI_VALUETYPE_RIGID_15,
                                                   IDS_RIGI_PARAM15,
                                                   IDS_RIGI_PARAM15>
{
public:
  TRigidParam15(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM15, IDI_VALUETYPE_RIGID_15, IDS_RIGI_PARAM15, IDS_RIGI_PARAM15>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam15);
};

class TRigidParam16 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM16,
                                                   IDI_VALUETYPE_RIGID_16,
                                                   IDS_RIGI_PARAM16,
                                                   IDS_RIGI_PARAM16>
{
public:
  TRigidParam16(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM16, IDI_VALUETYPE_RIGID_16, IDS_RIGI_PARAM16, IDS_RIGI_PARAM16>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam16);
};

class TRigidParam17 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM17,
                                                   IDI_VALUETYPE_RIGID_17,
                                                   IDS_RIGI_PARAM17,
                                                   IDS_RIGI_PARAM17>
{
public:
  TRigidParam17(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM17, IDI_VALUETYPE_RIGID_17, IDS_RIGI_PARAM17, IDS_RIGI_PARAM17>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam17);
};

class TRigidParam18 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM18,
                                                   IDI_VALUETYPE_RIGID_18,
                                                   IDS_RIGI_PARAM18,
                                                   IDS_RIGI_PARAM18>
{
public:
  TRigidParam18(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM18, IDI_VALUETYPE_RIGID_18, IDS_RIGI_PARAM18, IDS_RIGI_PARAM18>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam18);
};

class TRigidParam19 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM19,
                                                   IDI_VALUETYPE_RIGID_19,
                                                   IDS_RIGI_PARAM19,
                                                   IDS_RIGI_PARAM19>
{
public:
  TRigidParam19(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM19, IDI_VALUETYPE_RIGID_19, IDS_RIGI_PARAM19, IDS_RIGI_PARAM19>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam19);
};

class TRigidParam20 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM20,
                                                   IDI_VALUETYPE_RIGID_20,
                                                   IDS_RIGI_PARAM20,
                                                   IDS_RIGI_PARAM20>
{
public:
  TRigidParam20(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM20, IDI_VALUETYPE_RIGID_20, IDS_RIGI_PARAM20, IDS_RIGI_PARAM20>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam20);
};

class TRigidParam21 : public CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM21,
                                                   IDI_VALUETYPE_RIGID_21,
                                                   IDS_RIGI_PARAM21,
                                                   IDS_RIGI_PARAM21>
{
public:
  TRigidParam21(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_RIGI_PARAM21, IDI_VALUETYPE_RIGID_21, IDS_RIGI_PARAM21, IDS_RIGI_PARAM21>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTRigidParam21);
};

class TElasticHardening : public CMaterialComponentTemp<IDT_VALUETYPE_ELASTIC_HARDENING,
                            IDI_VALUETYPE_ELASTIC_HARDENING,
                            IDS_ET_ELASTIC_HARDENING,
                            IDS_VALUENAME_ELASTIC_HARDENING>
{
public:
  TElasticHardening(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_ELASTIC_HARDENING, IDI_VALUETYPE_ELASTIC_HARDENING, IDS_ET_ELASTIC_HARDENING, IDS_VALUENAME_ELASTIC_HARDENING>(point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0, false, false, 0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTElasticHardening);
};

class TSecondaryPreconsolidation : public CMaterialComponentTemp<IDT_VALUETYPE_SEC_PRECON,
                                                                 IDI_VALUETYPE_SEC_PRECON,
                                                                 IDS_ET_SEC_PRECON,
                                                                 IDS_VALUENAME_SEC_PRECON>
{
public:
  TSecondaryPreconsolidation(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_SEC_PRECON, IDI_VALUETYPE_SEC_PRECON, IDS_ET_SEC_PRECON, IDS_VALUENAME_SEC_PRECON>(point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, false, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTSecondaryPreconsolidation);
};

class TSecondaryHardening : public CMaterialComponentTemp<IDT_VALUETYPE_SEC_HARDENING,
                                                          IDI_VALUETYPE_SEC_HARDENING,
                                                          IDS_ET_SEC_HARDENING,
                                                          IDS_VALUENAME_SEC_HARDENING>
{
public:
  TSecondaryHardening(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_SEC_HARDENING, IDI_VALUETYPE_SEC_HARDENING, IDS_ET_SEC_HARDENING, IDS_VALUENAME_SEC_HARDENING>(point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, true, true, 1.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTSecondaryHardening);
};

class TUniaxialElasticCompressibility : public CMaterialComponentTemp<IDT_VALUETYPE_ELASCOMPRES,
                                                                      IDI_VALUETYPE_ELASCOMPRES,
                                                                      IDS_ET_ELASCOMPRES,
                                                                      IDS_VALUENAME_ELASCOMPRES>
{
public:
  TUniaxialElasticCompressibility(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_ELASCOMPRES, IDI_VALUETYPE_ELASCOMPRES, IDS_ET_ELASCOMPRES, IDS_VALUENAME_ELASCOMPRES>(point_set, strName, Units::PerStressUnit, Ranges::CRangeType(true, 0, false, false, 0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTUniaxialElasticCompressibility);
};

class TUniaxialPlasticCompressibility : public CMaterialComponentTemp<IDT_VALUETYPE_PLASCOMPRES,
                                                                      IDI_VALUETYPE_PLASCOMPRES,
                                                                      IDS_ET_PLASCOMPRES,
                                                                      IDS_VALUENAME_PLASCOMPRES>
{
public:
  TUniaxialPlasticCompressibility(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_PLASCOMPRES, IDI_VALUETYPE_PLASCOMPRES, IDS_ET_PLASCOMPRES, IDS_VALUENAME_PLASCOMPRES>(point_set, strName, Units::PerStressUnit, Ranges::CRangeType(true, 0, false, false, 0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTUniaxialPlasticCompressibility);
};

class TYoungModulusNormal : public CMaterialComponentTemp<IDT_VALUETYPE_YOUNGMODULUS_NORM,
                                                          IDI_VALUETYPE_YOUNGMODULUS_NORM,
                                                          IDS_ET_YOUNGMODULUS_NORM,
                                                          IDS_VALUENAME_YOUNGMODULUS_NORM>
{
public:
  TYoungModulusNormal(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_YOUNGMODULUS_NORM,
                             IDI_VALUETYPE_YOUNGMODULUS_NORM,
                             IDS_ET_YOUNGMODULUS_NORM,
                             IDS_VALUENAME_YOUNGMODULUS_NORM>
                             (point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTYoungModulusNormal);
};

class TYoungModulusTransverse : public CMaterialComponentTemp<IDT_VALUETYPE_YOUNGMODULUS_TRANS,
                                                              IDI_VALUETYPE_YOUNGMODULUS_TRANS,
                                                              IDS_ET_YOUNGMODULUS_TRANS,
                                                              IDS_VALUENAME_YOUNGMODULUS_TRANS>
{
public:
  TYoungModulusTransverse(IPointSet& point_set, const QString& strName) 
  : CMaterialComponentTemp<IDT_VALUETYPE_YOUNGMODULUS_TRANS,
                             IDI_VALUETYPE_YOUNGMODULUS_TRANS,
                             IDS_ET_YOUNGMODULUS_TRANS,
                             IDS_VALUENAME_YOUNGMODULUS_TRANS>
                             (point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTYoungModulusTransverse);
};

class TPoissonRatioNormal : public CMaterialComponentTemp<IDT_VALUETYPE_POISSONRATIO_NORM,
                                                          IDI_VALUETYPE_POISSONRATIO_NORM,
                                                          IDS_ET_POISSONRATIO_NORM,
                                                          IDS_VALUENAME_POISSONRATIO_NORM>
{
public:
  TPoissonRatioNormal(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_POISSONRATIO_NORM,
                             IDI_VALUETYPE_POISSONRATIO_NORM,
                             IDS_ET_POISSONRATIO_NORM,
                             IDS_VALUENAME_POISSONRATIO_NORM>
                             (point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, true, true, 0.5, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTPoissonRatioNormal);
};

class TPoissonRatioTransverse : public CMaterialComponentTemp<IDT_VALUETYPE_POISSONRATIO_TRANS,
                                                          IDI_VALUETYPE_POISSONRATIO_TRANS,
                                                          IDS_ET_POISSONRATIO_TRANS,
                                                          IDS_VALUENAME_POISSONRATIO_TRANS>
{
public:
  TPoissonRatioTransverse(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_POISSONRATIO_TRANS,
                             IDI_VALUETYPE_POISSONRATIO_TRANS,
                             IDS_ET_POISSONRATIO_TRANS,
                             IDS_VALUENAME_POISSONRATIO_TRANS>
                             (point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, true, true, 0.5, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTPoissonRatioTransverse);
};

class TLayerNormalIncl : public CMaterialComponentTemp<IDT_VALUETYPE_LAYER_NORMAL_INCL,
                            IDI_VALUETYPE_LAYER_NORMAL_INCL,
                            IDS_ET_LAYER_NORMAL_INCL,
                            IDS_VALUENAME_LAYER_NORMAL_INCL>
{
public:
  TLayerNormalIncl(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_LAYER_NORMAL_INCL,
              IDI_VALUETYPE_LAYER_NORMAL_INCL,
              IDS_ET_LAYER_NORMAL_INCL,
              IDS_VALUENAME_LAYER_NORMAL_INCL>
              (point_set, strName, Units::AngleUnit, Ranges::CRangeType(true, 0.0, true, true, 90.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTLayerNormalIncl);
};

class TLayerNormalAzi : public CMaterialComponentTemp<IDT_VALUETYPE_LAYER_NORMAL_AZI,
                            IDI_VALUETYPE_LAYER_NORMAL_AZI,
                            IDS_ET_LAYER_NORMAL_AZI,
                            IDS_VALUENAME_LAYER_NORMAL_AZI>
{
public:
  TLayerNormalAzi(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_LAYER_NORMAL_AZI,
              IDI_VALUETYPE_LAYER_NORMAL_AZI,
              IDS_ET_LAYER_NORMAL_AZI,
              IDS_VALUENAME_LAYER_NORMAL_AZI>
              (point_set, strName, Units::AngleUnit, Ranges::CRangeType(true, 0.0, true, true, 360.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTLayerNormalAzi);
};

class TFractStiffNormal : public CMaterialComponentTemp<IDT_VALUETYPE_FRACT_STIFF_NORMAL,
                            IDI_VALUETYPE_FRACT_STIFF_NORMAL,
                            IDS_ET_FRACT_STIFF_NORMAL,
                            IDS_VALUENAME_FRACT_STIFF_NORMAL>
{
public:
  TFractStiffNormal(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_FRACT_STIFF_NORMAL,
                              IDI_VALUETYPE_FRACT_STIFF_NORMAL,
                              IDS_ET_FRACT_STIFF_NORMAL,
                              IDS_VALUENAME_FRACT_STIFF_NORMAL>
                              (point_set, strName, Units::StressGradientUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTFractStiffNormal);
};

class TFractStiffShear : public CMaterialComponentTemp<IDT_VALUETYPE_FRACT_STIFF_SHEAR,
                            IDI_VALUETYPE_FRACT_STIFF_SHEAR,
                            IDS_ET_FRACT_STIFF_SHEAR,
                            IDS_VALUENAME_FRACT_STIFF_SHEAR>
{
public:
  TFractStiffShear(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_FRACT_STIFF_SHEAR,
                              IDI_VALUETYPE_FRACT_STIFF_SHEAR,
                              IDS_ET_FRACT_STIFF_SHEAR,
                              IDS_VALUENAME_FRACT_STIFF_SHEAR>
                              (point_set, strName, Units::StressGradientUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTFractStiffShear);
};

class TThermLinExpNormal : public CMaterialComponentTemp<IDT_VALUETYPE_THERM_LIN_EXP_NORM,
                            IDI_VALUETYPE_THERM_LIN_EXP_NORM,
                            IDS_ET_THERM_LIN_EXP_NORM,
                            IDS_VALUENAME_THERM_LIN_EXP_NORM>
{
public:
  TThermLinExpNormal(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_THERM_LIN_EXP_NORM,
              IDI_VALUETYPE_THERM_LIN_EXP_NORM,
              IDS_ET_THERM_LIN_EXP_NORM,
              IDS_VALUENAME_THERM_LIN_EXP_NORM>
              (point_set, strName, Units::PerTemperUnit, Ranges::CRangeType(true, 0.0, true, true, 1E-3, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTThermLinExpNormal);
};

class TThermLinExpLateral : public CMaterialComponentTemp<IDT_VALUETYPE_THERM_LIN_EXP_LAT,
                            IDI_VALUETYPE_THERM_LIN_EXP_LAT,
                            IDS_ET_THERM_LIN_EXP_LAT,
                            IDS_VALUENAME_THERM_LIN_EXP_LAT>
{
public:
  TThermLinExpLateral(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_THERM_LIN_EXP_LAT,
              IDI_VALUETYPE_THERM_LIN_EXP_LAT,
              IDS_ET_THERM_LIN_EXP_LAT,
              IDS_VALUENAME_THERM_LIN_EXP_LAT>
              (point_set, strName, Units::PerTemperUnit, Ranges::CRangeType(true, 0.0, true, true, 1E-3, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTThermLinExpLateral);
};

class TThermalConductivity : public CMaterialComponentTemp<IDT_VALUETYPE_THERM_CONDUCT,
                            IDI_VALUETYPE_THERM_CONDUCT,
                            IDS_ET_THERM_CONDUCT,
                            IDS_VALUENAME_THERM_CONDUCT>
{
public:
  TThermalConductivity(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_THERM_CONDUCT,
              IDI_VALUETYPE_THERM_CONDUCT,
              IDS_ET_THERM_CONDUCT,
              IDS_VALUENAME_THERM_CONDUCT>
              (point_set, strName, Units::ThermalConductUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTThermalConductivity);
};

class TThermalCapacity : public CMaterialComponentTemp<IDT_VALUETYPE_THERM_CAPACI,
                            IDI_VALUETYPE_THERM_CAPACI,
                            IDS_ET_THERM_CAPACI,
                            IDS_VALUENAME_THERM_CAPACI>
{
public:
  TThermalCapacity(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_THERM_CAPACI,
              IDI_VALUETYPE_THERM_CAPACI,
              IDS_ET_THERM_CAPACI,
              IDS_VALUENAME_THERM_CAPACI>
              (point_set, strName, Units::ThermalCapaciUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTThermalCapacity);
};

class THighFractDens : public CMaterialComponentTemp<IDT_VALUETYPE_HIGH_FRACT_DENS,
                                                      IDI_VALUETYPE_HIGH_FRACT_DENS,
                                                      IDS_ET_HIGH_FRACT_DENS,
                                                      IDS_VALUENAME_HIGH_FRACT_DENS>
{
public:
  THighFractDens(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_HIGH_FRACT_DENS,
              IDI_VALUETYPE_HIGH_FRACT_DENS,
              IDS_ET_HIGH_FRACT_DENS,
              IDS_VALUENAME_HIGH_FRACT_DENS>
              (point_set, strName, Units::PerLengthUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTHighFractDens);
};

class THighFractDensIncl : public CMaterialComponentTemp<IDT_VALUETYPE_HIGH_FRACT_DENS_INCL,
                                                      IDI_VALUETYPE_HIGH_FRACT_DENS_INCL,
                                                      IDS_ET_HIGH_FRACT_DENS_INCL,
                                                      IDS_VALUENAME_HIGH_FRACT_DENS_INCL>
{
public:
  THighFractDensIncl(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_HIGH_FRACT_DENS_INCL,
              IDI_VALUETYPE_HIGH_FRACT_DENS_INCL,
              IDS_ET_HIGH_FRACT_DENS_INCL,
              IDS_VALUENAME_HIGH_FRACT_DENS_INCL>
              (point_set, strName, Units::AngleUnit, Ranges::CRangeType(true, 0.0, true, true, 90.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTHighFractDensIncl);
};

class THighFractDensAzi : public CMaterialComponentTemp<IDT_VALUETYPE_HIGH_FRACT_DENS_AZI,
                                                      IDI_VALUETYPE_HIGH_FRACT_DENS_AZI,
                                                      IDS_ET_HIGH_FRACT_DENS_AZI,
                                                      IDS_VALUENAME_HIGH_FRACT_DENS_AZI>
{
public:
  THighFractDensAzi(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_HIGH_FRACT_DENS_AZI,
              IDI_VALUETYPE_HIGH_FRACT_DENS_AZI,
              IDS_ET_HIGH_FRACT_DENS_AZI,
              IDS_VALUENAME_HIGH_FRACT_DENS_AZI>
              (point_set, strName, Units::AngleUnit, Ranges::CRangeType(true, 0.0, true, true, 360.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTHighFractDensAzi);
};

class TLowFractDens : public CMaterialComponentTemp<IDT_VALUETYPE_LOW_FRACT_DENS,
                                                      IDI_VALUETYPE_LOW_FRACT_DENS,
                                                      IDS_ET_LOW_FRACT_DENS,
                                                      IDS_VALUENAME_LOW_FRACT_DENS>
{
public:
  TLowFractDens(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_LOW_FRACT_DENS,
              IDI_VALUETYPE_LOW_FRACT_DENS,
              IDS_ET_LOW_FRACT_DENS,
              IDS_VALUENAME_LOW_FRACT_DENS>
              (point_set, strName, Units::PerLengthUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTLowFractDens);
};

class TLowFractDensIncl : public CMaterialComponentTemp<IDT_VALUETYPE_LOW_FRACT_DENS_INCL,
                                                      IDI_VALUETYPE_LOW_FRACT_DENS_INCL,
                                                      IDS_ET_LOW_FRACT_DENS_INCL,
                                                      IDS_VALUENAME_LOW_FRACT_DENS_INCL>
{
public:
  TLowFractDensIncl(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_LOW_FRACT_DENS_INCL,
              IDI_VALUETYPE_LOW_FRACT_DENS_INCL,
              IDS_ET_LOW_FRACT_DENS_INCL,
              IDS_VALUENAME_LOW_FRACT_DENS_INCL>
              (point_set, strName, Units::AngleUnit, Ranges::CRangeType(true, 0.0, true, true, 90.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTLowFractDensIncl);
};

class TLowFractDensAzi : public CMaterialComponentTemp<IDT_VALUETYPE_LOW_FRACT_DENS_AZI,
                                                      IDI_VALUETYPE_LOW_FRACT_DENS_AZI,
                                                      IDS_ET_LOW_FRACT_DENS_AZI,
                                                      IDS_VALUENAME_LOW_FRACT_DENS_AZI>
{
public:
  TLowFractDensAzi(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_LOW_FRACT_DENS_AZI,
              IDI_VALUETYPE_LOW_FRACT_DENS_AZI,
              IDS_ET_LOW_FRACT_DENS_AZI,
              IDS_VALUENAME_LOW_FRACT_DENS_AZI>
              (point_set, strName, Units::AngleUnit, Ranges::CRangeType(true, 0.0, true, true, 360.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTLowFractDensAzi);
};

class TInterFractDens : public CMaterialComponentTemp<IDT_VALUETYPE_INTER_FRACT_DENS,
                                                      IDI_VALUETYPE_INT_FRACT_DENS,
                                                      IDS_ET_INTER_FRACT_DENS,
                                                      IDS_VALUENAME_INTER_FRACT_DENS>
{
public:
  TInterFractDens(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_INTER_FRACT_DENS,
              IDI_VALUETYPE_INT_FRACT_DENS,
              IDS_ET_INTER_FRACT_DENS,
              IDS_VALUENAME_INTER_FRACT_DENS>
              (point_set, strName, Units::PerLengthUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTInterFractDens);
};

class TPermeability : public CMaterialComponentTemp<IDT_VALUETYPE_PERMEA,
                          IDI_VALUETYPE_PERMEA,
                          IDS_ET_PERMEA,
                          IDS_VALUENAME_PERMEA>
{
public:
  TPermeability(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_PERMEA,
              IDI_VALUETYPE_PERMEA,
              IDS_ET_PERMEA,
              IDS_VALUENAME_PERMEA>
              (point_set, strName, Units::PermeaUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTPermeability);
};

class TViscosity : public CMaterialComponentTemp<IDT_VALUETYPE_VISCOSITY,
                                                 IDI_VALUETYPE_VISCOSITY,
                                                 IDS_ET_VISCOSITY,
                                                 IDS_VALUENAME_VISCOSITY>
{
public:
  TViscosity(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_VISCOSITY,
                             IDI_VALUETYPE_VISCOSITY,
                             IDS_ET_VISCOSITY,
                             IDS_VALUENAME_VISCOSITY>
                             (point_set, strName, Units::ViscosityUnit, Ranges::CRangeType(true, 0.0, false, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTViscosity);
};

class TYoungsModulusDecompaction : public CMaterialComponentTemp<IDT_VALUETYPE_YOUNG_DECOMP,
                                IDI_VALUETYPE_YOUNG_DECOMP,
                                IDS_ET_YOUNG_DECOMP,
                                IDS_VALUENAME_YOUNG_DECOMP>
{
public:
  TYoungsModulusDecompaction(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_YOUNG_DECOMP,
                             IDI_VALUETYPE_YOUNG_DECOMP,
                             IDS_ET_YOUNG_DECOMP,
                             IDS_VALUENAME_YOUNG_DECOMP>
                             (point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTYoungsModulusDecompaction);
};

class TPoissonsRatioDecompaction : public CMaterialComponentTemp<IDT_VALUETYPE_POISSON_DECOMP,
                                IDI_VALUETYPE_POISSON_DECOMP,
                                IDS_ET_POISSON_DECOMP,
                                IDS_VALUENAME_POISSON_DECOMP>
{
public:
  TPoissonsRatioDecompaction(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_POISSON_DECOMP,
                             IDI_VALUETYPE_POISSON_DECOMP,
                             IDS_ET_POISSON_DECOMP,
                             IDS_VALUENAME_POISSON_DECOMP>
                             (point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, true, true, 0.5, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTPoissonsRatioDecompaction);
};


class TFluidThermalExpansionCoefficient : public CMaterialComponentTemp<IDT_VALUETYPE_FLUIDX,
                                  IDI_VALUETYPE_CAPSHAPE, // both use the alpha symbol
                                  IDS_ET_VALUETYPE_FLUIDX,
                                  IDS_VALUENAME_FLUIDX>
{
public:
  TFluidThermalExpansionCoefficient(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_FLUIDX, 
                           IDI_VALUETYPE_CAPSHAPE, 
                           IDS_ET_VALUETYPE_FLUIDX, 
                           IDS_VALUENAME_FLUIDX>
                           (point_set, strName, Units::PerTemperUnit, Ranges::CRangeType(true, 0.0, false, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTFluidThermalExpansionCoefficient);
};

class TFluidDensity : public CMaterialComponentTemp<IDT_VALUETYPE_FLUID_DENSITY,
                          IDI_VALUETYPE_FLUID_DENSITY, // both use the rho symbol
                          IDS_ET_VALUETYPE_FLUID_DENSITY,
                          IDS_VALUENAME_FLUID_DENSITY>
{
public:
  TFluidDensity(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_FLUID_DENSITY,
                           IDI_VALUETYPE_FLUID_DENSITY,
                           IDS_ET_VALUETYPE_FLUID_DENSITY,
                           IDS_VALUENAME_FLUID_DENSITY>
                           (point_set, strName, Units::DensityUnit, Ranges::CRangeType(true, 0, false, true, 10000, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTFluidDensity);
};

class TReferenceStress : public CMaterialComponentTemp<IDT_VALUETYPE_REFERENCESTRESS,
                                                       IDI_VALUETYPE_REFERENCESTRESS,
                                                       IDS_ET_VALUETYPE_REFERENCESTRESS,
                                                       IDS_VALUENAME_REFERENCESTRESS>
{
public:
  TReferenceStress(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_REFERENCESTRESS,
                           IDI_VALUETYPE_REFERENCESTRESS,
                           IDS_ET_VALUETYPE_REFERENCESTRESS,
                           IDS_VALUENAME_REFERENCESTRESS>
                           (point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0, true, true, 1000, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTReferenceStress);
};

class TApertureParameter : public CMaterialComponentTemp<IDT_VALUETYPE_APERTUREPARAM,
                                                         IDI_VALUETYPE_APERTUREPARAM,
                                                         IDS_ET_VALUETYPE_APERTUREPARAM,
                                                         IDS_VALUENAME_APERTUREPARAM>
{
public:
  TApertureParameter(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_APERTUREPARAM,
                           IDI_VALUETYPE_APERTUREPARAM,
                           IDS_ET_VALUETYPE_APERTUREPARAM,
                           IDS_VALUENAME_APERTUREPARAM>
                           (point_set, strName, Units::PerStressUnit, Ranges::CRangeType(true, 0, true, true, 1000, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTApertureParameter);
};

class TGrainStiffnessParameter : // wjrx mantis 3314
public CMaterialComponentTemp<IDT_VALUETYPE_GRAINSTIFFNESS
                             ,IDI_VALUETYPE_GRAINSTIFFNESS
                             ,IDS_ET_VALUETYPE_GRAINSTIFFNESS
                             ,IDS_VALUENAME_GRAINSTIFFNESS>
{
public:
  TGrainStiffnessParameter(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_GRAINSTIFFNESS,
                           IDI_VALUETYPE_GRAINSTIFFNESS,
                           IDS_ET_VALUETYPE_GRAINSTIFFNESS,
                           IDS_VALUENAME_GRAINSTIFFNESS>
                           (point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 10, true, true, 1000000, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTGrainStiffnessParameter);
};

class TYoungsModulusNormalDecomp : public CMaterialComponentTemp<IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP,
                                                                 IDI_VALUETYPE_YOUNGMODULUS_NORM_DECOMP,
                                                                 IDS_ET_VALUETYPE_YOUNGMODULUS_NORM_DECOMP,
                                                                 IDS_VALUENAME_YOUNGMODULUS_NORM_DECOMP>
{
public:
  TYoungsModulusNormalDecomp(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP,
                           IDI_VALUETYPE_YOUNGMODULUS_NORM_DECOMP,
                           IDS_ET_VALUETYPE_YOUNGMODULUS_NORM_DECOMP,
                           IDS_VALUENAME_YOUNGMODULUS_NORM_DECOMP>
                           (point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTYoungsModulusNormalDecomp);
};

class TYoungModulusTransDecomp : public CMaterialComponentTemp<IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP,
                                                               IDI_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP,
                                                               IDS_ET_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP,
                                                               IDS_VALUENAME_YOUNGMODULUS_TRANS_DECOMP>
{
public:
  TYoungModulusTransDecomp(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP,
                           IDI_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP,
                           IDS_ET_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP,
                           IDS_VALUENAME_YOUNGMODULUS_TRANS_DECOMP>
                           (point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTYoungModulusTransDecomp);
};

class TPoissonRatioNormalDecomp : public CMaterialComponentTemp<IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP,
                                IDI_VALUETYPE_POISSONRATIO_NORM_DECOMP,
                                IDS_ET_VALUETYPE_POISSONRATIO_NORM_DECOMP,
                                IDS_VALUENAME_POISSONRATIO_NORM_DECOMP>
{
public:
  TPoissonRatioNormalDecomp(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP,
                           IDI_VALUETYPE_POISSONRATIO_NORM_DECOMP,
                           IDS_ET_VALUETYPE_POISSONRATIO_NORM_DECOMP,
                           IDS_VALUENAME_POISSONRATIO_NORM_DECOMP>
                           (point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, true, true, 0.5, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTPoissonRatioNormalDecomp);
};

class TPoissonRatioTransDecomp : public CMaterialComponentTemp<IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP,
                                                               IDI_VALUETYPE_POISSONRATIO_TRANS_DECOMP,
                                                               IDS_ET_VALUETYPE_POISSONRATIO_TRANS_DECOMP,
                                                               IDS_VALUENAME_POISSONRATIO_TRANS_DECOMP>
{
public:
  TPoissonRatioTransDecomp(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP,
                           IDI_VALUETYPE_POISSONRATIO_TRANS_DECOMP,
                           IDS_ET_VALUETYPE_POISSONRATIO_TRANS_DECOMP,
                           IDS_VALUENAME_POISSONRATIO_TRANS_DECOMP>
                           (point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, true, true, 0.5, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTPoissonRatioTransDecomp);
};

class TAnisotropicShearModulus : public CMaterialComponentTemp<IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS,
                                                               IDI_VALUETYPE_ANISOTROPIC_SHEARMODULUS,
                                                               IDS_ET_VALUETYPE_ANISOTROPIC_SHEARMODULUS,
                                                               IDS_VALUENAME_ANISOTROPIC_SHEARMODULUS>
{
public:
  TAnisotropicShearModulus(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS,
                           IDI_VALUETYPE_ANISOTROPIC_SHEARMODULUS,
                           IDS_ET_VALUETYPE_ANISOTROPIC_SHEARMODULUS,
                           IDS_VALUENAME_ANISOTROPIC_SHEARMODULUS>
                           (point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTAnisotropicShearModulus);
};

class TAnisotropicShearModulusDecomp : public CMaterialComponentTemp<IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP,
                                                                     IDI_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP,
                                                                     IDS_ET_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP,
                                                                     IDS_VALUENAME_ANISOTROPIC_SHEARMODULUS_DECOMP>
{
public:
  TAnisotropicShearModulusDecomp(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP,
                           IDI_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP,
                           IDS_ET_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP,
                           IDS_VALUENAME_ANISOTROPIC_SHEARMODULUS_DECOMP>
                           (point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTAnisotropicShearModulusDecomp);
};

class TThomsenEpsilon : public CMaterialComponentTemp<IDT_VALUETYPE_THOMSEN_EPSILON,
                                                      IDI_VALUETYPE_THOMSEN_EPSILON,
                                                      IDS_ET_VALUETYPE_THOMSEN_EPSILON,
                                                      IDS_VALUENAME_THOMSEN_EPSILON>
{
public:
  TThomsenEpsilon(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_THOMSEN_EPSILON,
                           IDI_VALUETYPE_THOMSEN_EPSILON,
                           IDS_ET_VALUETYPE_THOMSEN_EPSILON,
                           IDS_VALUENAME_THOMSEN_EPSILON>
                           (point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTThomsenEpsilon);
};

class TThomsenGamma : public CMaterialComponentTemp<IDT_VALUETYPE_THOMSEN_GAMMA,
                          IDI_VALUETYPE_THOMSEN_GAMMA,
                          IDS_ET_VALUETYPE_THOMSEN_GAMMA,
                          IDS_VALUENAME_THOMSEN_GAMMA>
{
public:
  TThomsenGamma(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_THOMSEN_GAMMA,
                           IDI_VALUETYPE_THOMSEN_GAMMA,
                           IDS_ET_VALUETYPE_THOMSEN_GAMMA,
                           IDS_VALUENAME_THOMSEN_GAMMA>
                           (point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTThomsenGamma);
};

class TThomsenDelta : public CMaterialComponentTemp<IDT_VALUETYPE_THOMSEN_DELTA,
                                                      IDI_VALUETYPE_THOMSEN_DELTA,
                                                      IDS_ET_VALUETYPE_THOMSEN_DELTA,
                                                      IDS_VALUENAME_THOMSEN_DELTA>
{
public:
  TThomsenDelta(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_THOMSEN_DELTA,
                           IDI_VALUETYPE_THOMSEN_DELTA,
                           IDS_ET_VALUETYPE_THOMSEN_DELTA,
                           IDS_VALUENAME_THOMSEN_DELTA>
                           (point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, false))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTThomsenDelta);
};

class TInitialFractureApertureHigh : public CMaterialComponentTemp<IDT_VALUETYPE_V0_HI,
                                                                   IDI_VALUETYPE_V0_HI,
                                                                   IDS_ET_VALUETYPE_V0_HI,
                                                                   IDS_VALUENAME_V0_HI>
{
public:
  TInitialFractureApertureHigh(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_V0_HI,
                           IDI_VALUETYPE_V0_HI,
                           IDS_ET_VALUETYPE_V0_HI,
                           IDS_VALUENAME_V0_HI>
                           (point_set, strName, Units::LengthUnit, Ranges::CRangeType(true, 0, true, true, 0.01, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTInitialFractureApertureHigh);
};

class TInitialFractureApertureInter : public CMaterialComponentTemp<IDT_VALUETYPE_V0_ME,
                                  IDI_VALUETYPE_V0_ME,
                                  IDS_ET_VALUETYPE_V0_ME,
                                  IDS_VALUENAME_V0_ME>
{
public:
  TInitialFractureApertureInter(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_V0_ME,
                           IDI_VALUETYPE_V0_ME,
                           IDS_ET_VALUETYPE_V0_ME,
                           IDS_VALUENAME_V0_ME>
                           (point_set, strName, Units::LengthUnit, Ranges::CRangeType(true, 0, true, true, 0.01, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTInitialFractureApertureInter);
};

class TInitialFractureApertureLow : public CMaterialComponentTemp<IDT_VALUETYPE_V0_LO,
                                                                  IDI_VALUETYPE_V0_LO,
                                                                  IDS_ET_VALUETYPE_V0_LO,
                                                                  IDS_VALUENAME_V0_LO>
{
public:
  TInitialFractureApertureLow(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_V0_LO,
                           IDI_VALUETYPE_V0_LO,
                           IDS_ET_VALUETYPE_V0_LO,
                           IDS_VALUENAME_V0_LO>
                           (point_set, strName, Units::LengthUnit, Ranges::CRangeType(true, 0, true, true, 0.01, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTInitialFractureApertureLow);
};

class TReferenceAperture : public CMaterialComponentTemp<IDT_VALUETYPE_REFERENCE_APERTURE,
                                                         IDI_VALUETYPE_REFERENCE_APERTURE,
                                                         IDS_ET_VALUETYPE_REFERENCE_APERTURE,
                                                         IDS_VALUENAME_REFERENCE_APERTURE>
{
public:
  TReferenceAperture(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_REFERENCE_APERTURE,
                           IDI_VALUETYPE_REFERENCE_APERTURE,
                           IDS_ET_VALUETYPE_REFERENCE_APERTURE,
                           IDS_VALUENAME_REFERENCE_APERTURE>
                           (point_set, strName, Units::LengthUnit, Ranges::CRangeType(true, 0, true, true, 0.01, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTReferenceAperture);
};

class TDynamicUniaxialStiffness : public CMaterialComponentTemp<IDT_VALUETYPE_DYNUNISTIFFNESS,
                                IDI_VALUETYPE_DYNUNISTIFFNESS,
                                IDS_ET_DYNUNISTIFFNESS,
                                IDS_VALUENAME_DYNUNISTIFFNESS>
{
public:
  TDynamicUniaxialStiffness(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_DYNUNISTIFFNESS,
                           IDI_VALUETYPE_DYNUNISTIFFNESS,
                           IDS_ET_DYNUNISTIFFNESS,
                           IDS_VALUENAME_DYNUNISTIFFNESS>
                           (point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTDynamicUniaxialStiffness);
};

class TDynamicShearStiffness : public CMaterialComponentTemp<IDT_VALUETYPE_DYNSHEARSTIFFNESS,
                                                             IDI_VALUETYPE_DYNSHEARSTIFFNESS,
                                                             IDS_ET_DYNSHEARSTIFFNESS,
                                                             IDS_VALUENAME_DYNSHEARSTIFFNESS>
{
public:
  TDynamicShearStiffness(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_DYNSHEARSTIFFNESS,
                           IDI_VALUETYPE_DYNSHEARSTIFFNESS,
                           IDS_ET_DYNSHEARSTIFFNESS,
                           IDS_VALUENAME_DYNSHEARSTIFFNESS>
                           (point_set, strName, Units::StressUnit, Ranges::CRangeType(true, 0.0, true, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTDynamicShearStiffness);
};


class TNormalStress : public CSingleComponentTemp<IDT_VALUETYPE_NRMSTRESS,
  IDI_VALUETYPE_NRMSTRESS,
  IDS_ET_NRMSTRESS,
  IDS_VALUENAME_NRMSTRESS>
{
public:
  TNormalStress(IPointSet& point_set, const QString& strName)
  : CSingleComponentTemp<IDT_VALUETYPE_NRMSTRESS, IDI_VALUETYPE_NRMSTRESS, IDS_ET_NRMSTRESS, IDS_VALUENAME_NRMSTRESS>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTNormalStress);
};

class TMeanStress : public CSingleComponentTemp<IDT_VALUETYPE_MEANSTRESS,
  IDI_VALUETYPE_MEANSTRESS,
  IDS_ET_MEANSTRESS,
  IDS_VALUENAME_MEANSTRESS>
{
public:
  TMeanStress(IPointSet& point_set, const QString& strName)
  : CSingleComponentTemp<IDT_VALUETYPE_MEANSTRESS, IDI_VALUETYPE_MEANSTRESS, IDS_ET_MEANSTRESS, IDS_VALUENAME_MEANSTRESS>(point_set, strName, Units::StressUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTMeanStress);
};

class TTimeStrain : public CSingleComponentTemp<IDT_VALUETYPE_TIMESTRAIN,
  IDI_VALUETYPE_TIMESTRAIN,
  IDS_ET_TIMESTRAIN,
  IDS_VALUENAME_TIMESTRAIN>
{
public:
  TTimeStrain(IPointSet& point_set, const QString& strName)
  : CSingleComponentTemp<IDT_VALUETYPE_TIMESTRAIN, IDI_VALUETYPE_TIMESTRAIN, IDS_ET_TIMESTRAIN, IDS_VALUENAME_TIMESTRAIN>(point_set, strName, Units::DimensionLessUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTTimeStrain);
};

class TDeltaV : public CSingleComponentTemp<IDT_VALUETYPE_DELTAV,
  IDI_VALUETYPE_DELTAV,
  IDS_ET_DELTAV,
  IDS_VALUENAME_DELTAV>
{
public:
  TDeltaV(IPointSet& point_set, const QString& strName)
  : CSingleComponentTemp<IDT_VALUETYPE_DELTAV, IDI_VALUETYPE_DELTAV, IDS_ET_DELTAV, IDS_VALUENAME_DELTAV>(point_set, strName, Units::VelocityUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTDeltaV);
};

class TDeltaT : public CSingleComponentTemp<IDT_VALUETYPE_DELTAT,
  IDI_VALUETYPE_DELTAT,
  IDS_ET_DELTAT,
  IDS_VALUENAME_DELTAT>
{
public:
  TDeltaT(IPointSet& point_set, const QString& strName)
  : CSingleComponentTemp<IDT_VALUETYPE_DELTAT, IDI_VALUETYPE_DELTAT, IDS_ET_DELTAT, IDS_VALUENAME_DELTAT>(point_set, strName, Units::TimeUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTDeltaT);
};

class TDisplacementZ : public CSingleComponentTemp<IDT_VALUETYPE_DISPLACEMENTZ,
  IDI_VALUETYPE_DISPLACEMENTZ,
  IDS_ET_DISPLACEMENTZ,
  IDS_VALUENAME_DISPLACEMENTZ>
{
public:
  TDisplacementZ(IPointSet& point_set, const QString& strName)
  : CSingleComponentTemp<IDT_VALUETYPE_DISPLACEMENTZ, IDI_VALUETYPE_DISPLACEMENTZ, IDS_ET_DISPLACEMENTZ, IDS_VALUENAME_DISPLACEMENTZ>(point_set, strName, Units::LengthUnit, Ranges::NoRange)
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTDisplacementZ);
};


class TDummyMaterialParameter : public CMaterialComponentTemp<IDT_VALUETYPE_DUMMY,
  IDI_VALUETYPE_DUMMY,
  IDS_ET_DUMMY,
  IDS_VALUENAME_DUMMY>
{
public:
  TDummyMaterialParameter(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_DUMMY, IDI_VALUETYPE_DUMMY, IDS_ET_DUMMY, IDS_VALUENAME_DUMMY>(point_set, strName, Units::DimensionLessUnit, Ranges::CRangeType(false, 0.0, true, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTDummyMaterialParameter);
};


class TBoundaryKradMaterialParameter : public CMaterialComponentTemp<IDT_VALUETYPE_KRAD,
  IDI_VALUETYPE_KRAD,
  IDS_ET_KRAD,
  IDS_VALUENAME_KRAD>
{
public:
  TBoundaryKradMaterialParameter(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_KRAD, IDI_VALUETYPE_KRAD, IDS_ET_KRAD, IDS_VALUENAME_KRAD>(point_set, strName, Units::StressGradientUnit, Ranges::CRangeType(false, 0.0, true, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTBoundaryKradMaterialParameter);
};

class TBoundaryKtanMaterialParameter : public CMaterialComponentTemp<IDT_VALUETYPE_KTAN,
  IDI_VALUETYPE_KTAN,
  IDS_ET_KTAN,
  IDS_VALUENAME_KTAN>
{
public:
  TBoundaryKtanMaterialParameter(IPointSet& point_set, const QString& strName)
  : CMaterialComponentTemp<IDT_VALUETYPE_KTAN, IDI_VALUETYPE_KTAN, IDS_ET_KTAN, IDS_VALUENAME_KTAN>(point_set, strName, Units::StressGradientUnit, Ranges::CRangeType(false, 0.0, true, false, 0.0, true))
  {
  }

  ACCEPT_GEOMECMODELVISITORS(VisitTBoundaryKtanMaterialParameter);
};



#endif // !defined(AFX_VALUETYPEFACTORY_H__8BBA9A1E_1769_4A91_8C58_5543D9A84770__INCLUDED_)
