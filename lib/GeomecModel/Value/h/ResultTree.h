// ResultTree.h: interface for the CResultTree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESULTTREE_H__F5E7E64B_F6B3_4C59_A180_1A8301652AFC__INCLUDED_)
#define AFX_RESULTTREE_H__F5E7E64B_F6B3_4C59_A180_1A8301652AFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CoordinateMap.h"
#include "FieldFactor.h"
#include "FormationVolume.h"
#include "IStrainTensorGroup.h"
#include "IStressTensorGroup.h"
#include "Result.h"

#include <tbb/concurrent_vector.h>

namespace geo {
class IInterfaceElement;
}

class CDerivedResultGroup;
class CFormationBase;
class CStaborResult;

// The ResultTree class represents the result tree as it is displayed in the tree.
//
// These defines are used for references to results of the result tree in the file storage.
// New results can only added, but this must always supported ... A reference to a result
// component consist of storing the following data. For saving a result component you must
// save the following values :
// I.	IResult index (The value mentioned here, retieve : int IResult::Index() const)
// II.	Component index (Retrieve : int IResultComponent::ComponentIndex() const)
// III.	Timestep index (Retrieve: int IDepletionStage::Index(), using const CDepeltionStage&
//		IResultComponent::Stage() const to retrieve the depletion stage)
// IV.	Linear / Non Linear (Retrieve: bool IResultComponent::Linear() const)
// For loading the results simply use :
//	const IResultComponent& ResultComponent(int nResultIndex, int nComponent, int nTimeStep,
//	const CAnalysisType& antype) const;
// and the result component is returned ...

#define RC_TOTAL_STRESS_COMPONENT 1
#define RC_TOTAL_STRESS_PRINC_MIN 2
#define RC_TOTAL_STRESS_PRINC_MED 3
#define RC_TOTAL_STRESS_PRINC_MAX 4
#define RC_TOTAL_STRESS_INVARIANT 5
#define RC_TOTAL_STRESS_WPSTRESS 6
#define RC_TOTAL_STRESS_WPSTRESS_CHANGE 9
#define RC_TOTAL_STRESS_SURFACESTRESS 7
#define RC_TOTAL_STRESS_SURFACESTRESS_CHANGE 8

#define RC_TOTAL_STRESS_COMPONENT_CHANGE 11
#define RC_TOTAL_STRESS_PRINC_MIN_CHANGE 12
#define RC_TOTAL_STRESS_PRINC_MED_CHANGE 13
#define RC_TOTAL_STRESS_PRINC_MAX_CHANGE 14
#define RC_TOTAL_STRESS_INVARIANT_CHANGE 15

#define RC_EFF_STRESS_COMPONENT 21
#define RC_EFF_STRESS_PRINC_MIN 22
#define RC_EFF_STRESS_PRINC_MED 23
#define RC_EFF_STRESS_PRINC_MAX 24
#define RC_EFF_STRESS_INVARIANT 25
#define RC_EFF_STRESS_WPSTRESS 26
#define RC_EFF_STRESS_WPSTRESS_CHANGE 27

#define RC_EFF_STRESS_COMPONENT_CHANGE 31
#define RC_EFF_STRESS_PRINC_MIN_CHANGE 32
#define RC_EFF_STRESS_PRINC_MED_CHANGE 33
#define RC_EFF_STRESS_PRINC_MAX_CHANGE 34
#define RC_EFF_STRESS_INVARIANT_CHANGE 35

#define RC_TOTAL_STRAIN_COMPONENT 41
#define RC_TOTAL_STRAIN_PRINC_MIN 42
#define RC_TOTAL_STRAIN_PRINC_MED 43
#define RC_TOTAL_STRAIN_PRINC_MAX 44
#define RC_TOTAL_STRAIN_INVARIANT 45
#define RC_TOTAL_STRAIN_WPAXIALSTRAIN 46

#define RC_PLASTIC_STRAIN_COMPONENT 51
#define RC_PLASTIC_STRAIN_PRINC_MIN 52
#define RC_PLASTIC_STRAIN_PRINC_MED 53
#define RC_PLASTIC_STRAIN_PRINC_MAX 54
#define RC_PLASTIC_STRAIN_INVARIANT 55
#define RC_PLASTIC_STRAIN_WPAXIALSTRAIN 56

#define RC_FRACTURE_MATRIX_PRESSURE 58
#define RC_FRACTURE_MATRIX_PRESSURE_CHANGE 59

#define RC_PORE_PRESSURE 61
#define RC_PORE_PRESSURE_CHANGE 62
#define RC_SHEAR_CAPACITY 63
#define RC_GAMMA_VERTICAL 64
#define RC_GAMMA_MAX 65
#define RC_GAMMA_MED 66
#define RC_GAMMA_MIN 67
#define RC_GAMMA_VOLUME 68
#define RC_VOLUME_AVERAGE 69

#define RC_DISPLACEMENT 70
#define RC_COMPACTION_CAPACITY 71

#define RC_FAULT_DISPLACEMENT 80
#define RC_FAULT_DILATATION 81
#define RC_FAULT_TOTAL_NORMAL_STRESS 82
// #define RC_FAULT_TOTAL_SHEAR_STRESS			83
#define RC_FAULT_EFF_NORMAL_STRESS 84
#define RC_FAULT_SHEAR_STRESS 85
#define RC_FAULT_TOTAL_NORMAL_STRESS_CHANGE 86
// #define RC_FAULT_TOTAL_SHEAR_STRESS_CHANGE	87
#define RC_FAULT_EFF_NORMAL_STRESS_CHANGE 88
#define RC_FAULT_SHEAR_STRESS_CHANGE 89
#define RC_FAULT_SHEAR_CAPACITY 90
#define RC_FAULT_PRESSURE 91
#define RC_FAULT_PRESSURE_CHANGE 92
#define RC_FAULT_PLASTIC_SLIP 93
#define RC_FAULT_PLASTIC_ENERGY 94
#define RC_FAULT_PRESSURE_INPUT 95
#define RC_FAULT_PRESSURE_CHANGE_INPUT 96

#define RC_CREEP_STRAIN_COMPONENT 100
#define RC_CREEP_STRAIN_PRINC_MIN 101
#define RC_CREEP_STRAIN_PRINC_MED 102
#define RC_CREEP_STRAIN_PRINC_MAX 103
#define RC_CREEP_STRAIN_INVARIANT 104

#define RC_MR_NORTHING 108
#define RC_MR_EASTING 109
#define RC_MR_DEPTH 110
#define RC_MR_MAX_EDGE 111
#define RC_MR_MEAN_EDGE 112
#define RC_MR_SURFACE 113
#define RC_MR_VOLUME 114
#define RC_MR_EDGE_RATIO 115
#define RC_MR_MAX_ANGLE 116
#define RC_MR_MIN_ANGLE 117
#define RC_MR_SHAPE_FACTOR 118

#define RC_TEMPERATURE 119
#define RC_TEMPERATURE_CHANGE 120
#define RC_CONSOL 121
#define RC_CONSOL_CHANGE 122
#define RC_POROSITY 123
#define RC_POROSITY_CHANGE 124
#define RC_SHTOTAZIMUTH 125 // wjrx mantis 2870
#define RC_SHTOT_SvTOT 126  // wjrx mantis 2870
#define RC_ShTOT_SvTOT 127  // wjrx mantis 2870

#define RC_THICKNESS 128
#define RC_THICKNESS_CHANGE 129

#define RC_APERTURE_HI 140
#define RC_APERTURE_ME 141
#define RC_APERTURE_LO 142
#define RC_APERTURE_HI_CHANGE 143
#define RC_APERTURE_ME_CHANGE 144
#define RC_APERTURE_LO_CHANGE 145
#define RC_PERMEABILITY_MULTIPLIER_HI 146
#define RC_PERMEABILITY_MULTIPLIER_ME 147
#define RC_PERMEABILITY_MULTIPLIER_LO 148
#define RC_PLASTIC_ENERGY 149

#define RC_DSTABOR 150

#define RC_VERTICAL_STRESS_GRADIENT 151
#define RC_PORE_PRESSURE_GRADIENT 152
#define RC_TEMPERATURE_GRADIENT 153

#define RC_PORE_PRESSURE_INPUT 154
#define RC_PORE_PRESSURE_CHANGE_INPUT 155
#define RC_PORE_PRESSURE_GRADIENT_INPUT 156
#define RC_TEMPERATURE_INPUT 157
#define RC_TEMPERATURE_CHANGE_INPUT 158
#define RC_TEMPERATURE_GRADIENT_INPUT 159
#define RC_VOLUMETRIC_STRAIN_INPUT 160
#define RC_VOLUMETRIC_STRAIN_CHANGE_INPUT 161

#define RC_NORMAL_STRAIN_INPUT 162
#define RC_LATERAL_STRAIN_INPUT 163

#define RC_MR_AHD 164
#define RC_MR_AZIMUTH 165
#define RC_MR_INCLINATION 166

#define RC_BOUNDARY_DISPLACEMENT 167
#define RC_BOUNDARY_PRESSURE 168

#define REGISTRY_RESULT_BASE 400
#define DERIVED_RESULT_BASE 700
#define COMPOSITE_BASE 1000

// NOTE : The value composites start at 1000 with numbering. (See modelbase.h NumberValueComposites()
//		  for further details.

// class CDepletionStage;
//

class CResultInfo;

class CResultTree : public CResultGroup {
public:
  // The effective stress tensor class is a representation of the Diana stress.
  // Derived from the tensor
  class CEffectiveStressResult : public IStressTensorGroup {
    double Convert(const double &dValue, CQuantity::UNIT unit) const;

  public:
    // Construction
    CEffectiveStressResult(unsigned int uName, CResultGroup &group, bool bChange = false);
    CEffectiveStressResult(const QString &sName, CResultGroup &group, bool bChange = false);

    virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;

    virtual unsigned int PreExportNameId() const;
    virtual unsigned int ExportNameId() const;
    virtual unsigned int TypeId() const;
    virtual QString TypeName() const;
    virtual unsigned int PostExportNameId() const;

    // Index for referencing ..
    virtual unsigned int ComponentIndex() const;
    virtual unsigned int VectorIndex(CVectorComposite::PRINCIPAL_DIRECTION direction) const;
    virtual unsigned int InvariantIndex() const;
    virtual bool CanMap(const COpenGLNode &node, int nRegister) const;
    virtual unsigned int WellPathStressIndex() const;
  };

  // The total stress tensor equipment
  class CTotalStressResult : public IStressTensorGroup {
  public:
    // surface stress can only be displayed for total stress
    class CSurfaceStressComposite : public IResult {
    public:
      class CSurfaceStressComponent : public IResultComponent {
      public:
        typedef enum { SSTC_NORMAL = 0, SSTC_SHEAR_PLANE = 1, SSTC_SHEAR_DIP_ANGLE = 2 } TComponent;

        CSurfaceStressComponent(unsigned int uName, CSurfaceStressComposite &parent, TComponent comp,
                                const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);
        virtual unsigned int IconId() const;
        virtual unsigned int TypeId() const;
        virtual QString UnitName(const UNIT unit) const;
        virtual bool CanComputeOnPoints() const;
        virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint &pt, const UNIT unit = CQuantity::SI_UNIT,
                                                      geo::IParallelInitializationCallback *cb = 0) const;
        virtual void MapValueElement(const geo::IElement &elm, IValueDomainScalar::TValueVec &values, TMapType map_type,
                                     UNIT unit = CQuantity::SI_UNIT,
                                     geo::IParallelInitializationCallback *cb = 0) const;
        virtual IValueDomainScalar::TValue Value(const geo::IElement & /*mesh_element*/, int /*nNodeIndex*/,
                                                 UNIT /*unit*/) const {
          assert(false);
          return geo::CValue();
        }

        virtual bool NeedParallelInitializationCallback() const;
        virtual geo::IParallelInitializationCallback *GetParallelInitializationCallback();
        virtual bool PrepareMapping(const geo::IElementSet *pElementSet);
        virtual void FinishMapping();

      private:
        TComponent m_comp;
      };

    public:
      CSurfaceStressComposite(unsigned int uName, CResultGroup &group, int storageNodeIndex);
      CSurfaceStressComposite(const QString &sName, CResultGroup &group, int storageNodeIndex);

      virtual unsigned int IconId() const;
      virtual bool OnDefined(const IResultComponent &component) const;
      virtual QString ExportLabel(int nComponent) const;
      virtual bool CanMap(const COpenGLNode &node, int nRegister) const;

      bool ValidName(const std::string &name, CResultInfo &resultInfo) const;

    protected:
      virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;
      virtual void BuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);

    private:
      const IStressTensorGroup *m_pTensorGroup;
    };

  private:
    double Convert(const double &dValue, CQuantity::UNIT unit) const;

    CSurfaceStressComposite *m_pSurfaceStressComposite;

  protected:
    virtual void CreateComposite();

  public:
    // Construction
    CTotalStressResult(unsigned int uName, CResultGroup &group, bool bChange = false);
    CTotalStressResult(const QString &sName, CResultGroup &group, bool bChange = false);

    virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;

    virtual unsigned int PreExportNameId() const;
    virtual unsigned int TypeId() const;
    virtual QString TypeName() const;
    virtual unsigned int ExportNameId() const;
    virtual unsigned int PostExportNameId() const;

    // Index for referencing ..
    virtual unsigned int ComponentIndex() const;
    virtual unsigned int VectorIndex(CVectorComposite::PRINCIPAL_DIRECTION direction) const;
    virtual unsigned int InvariantIndex() const;
    virtual bool CanMap(const COpenGLNode &node, int nRegister) const;
    virtual unsigned int WellPathStressIndex() const;
  };

  class CParentStressInputResult : public CEffectiveStressResult {
    double Convert(const double &dValue, CQuantity::UNIT unit) const;

  protected:
    virtual double GetTypicalEpsilon() const { return 1e-4; }

  public:
    // Construction
    CParentStressInputResult(unsigned int uName, CResultGroup &group, bool bChange = false);
    CParentStressInputResult(const QString &sName, CResultGroup &group, bool bChange = false);

    virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;

    virtual unsigned int PreExportNameId() const;
    virtual unsigned int ExportNameId() const;
    virtual unsigned int TypeId() const;
    virtual QString TypeName() const;
    virtual unsigned int PostExportNameId() const;

    virtual bool OnDefined(const IResultComponent &component) const;

    virtual const ITensor &TensorPoint(const geo::IPoint &point, const IResultComponent &component,
                                       geo::IParallelInitializationCallback *cb) const;
    virtual const ITensor &TensorElement(const geo::IElement &element, int nNodeIndex, TMapType map_type,
                                         const IResultComponent &component,
                                         geo::IParallelInitializationCallback *cb) const;

    void TensorElementFromParentModel(_ResultCache<CStressTensor> &cache, const geo::IElement &element,
                                      TMapType map_type, const CDepletionStage &stage,
                                      const CAnalysisType &antype) const;

#if 0
  // Index for referencing ..
  virtual unsigned int ComponentIndex() const;
  virtual TTensorType Type() const { return TT_STRESS; }
  virtual unsigned int VectorIndex(CVectorComposite::PRINCIPAL_DIRECTION direction) const;
  virtual unsigned int InvariantIndex() const;
#endif
    virtual bool CanMap(const COpenGLNode &node, int nRegister) const;
#if 0
  virtual unsigned int WellPathStressIndex() const;
  virtual bool Change() const { return false; }
  virtual double ConvertToField(const double& dDouble) const { return dDouble * FF_FACTOR_STRESS; }
  virtual	QString UnitName(CQuantity::UNIT unit) const
  {
      QString sUnit;
      if (unit == CQuantity::SI_UNIT)
    sUnit = getStringTableEntry(IDS_UNIT_SI_STRESS);
      else
    sUnit = getStringTableEntry(IDS_UNIT_FIELD_STRESS);
      return sUnit;
  }
#endif
    virtual bool PrepareMapping(const geo::IElementSet *, const IValueComponentBase *);
  };

  // The total strain tensor equipment
  class CTotalStrainResult : public IStrainTensorGroup {

  public:
    // Construction
    CTotalStrainResult(unsigned int uName, CResultGroup &group);
    CTotalStrainResult(const QString &sName, CResultGroup &group);

    virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;

    virtual unsigned int PreExportNameId() const;
    virtual unsigned int TypeId() const;
    virtual QString TypeName() const;
    virtual unsigned int ExportNameId() const;
    virtual unsigned int PostExportNameId() const;

    // Index for referencing ..
    virtual unsigned int ComponentIndex() const;
    virtual unsigned int VectorIndex(CVectorComposite::PRINCIPAL_DIRECTION direction) const;
    virtual unsigned int InvariantIndex() const;
    virtual bool CanMap(const COpenGLNode &node, int nRegister) const;
    virtual unsigned int WellPathAxialStrainIndex() const { return RC_TOTAL_STRAIN_WPAXIALSTRAIN; }
    virtual bool OnDefined(const IResultComponent &component) const;
  };

  // The plactic strain tensor equipment
  class CPlasticStrainResult : public IStrainTensorGroup {

  public:
    // Construction
    CPlasticStrainResult(unsigned int uName, CResultGroup &group);
    CPlasticStrainResult(const QString &sName, CResultGroup &group);

    virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;

    virtual unsigned int PreExportNameId() const;
    virtual unsigned int ExportNameId() const;
    virtual unsigned int TypeId() const;
    virtual QString TypeName() const;
    virtual unsigned int PostExportNameId() const;

    // Index for referencing ..
    virtual unsigned int ComponentIndex() const;
    virtual unsigned int VectorIndex(CVectorComposite::PRINCIPAL_DIRECTION direction) const;
    virtual unsigned int InvariantIndex() const;
    virtual bool CanMap(const COpenGLNode &node, int nRegister) const;
    virtual unsigned int WellPathAxialStrainIndex() const { return RC_PLASTIC_STRAIN_WPAXIALSTRAIN; }
    virtual bool PrepareMapping(const geo::IElementSet *, const IValueComponentBase *);
  };

  /*
    // The creep strain tensor equipment
    class CCreepStrainResult : public IStrainTensorGroup
    {

    public:
      // Construction
      CCreepStrainResult(unsigned int uName, CResultGroup& group);
      CCreepStrainResult(const QString& sName, CResultGroup& group);

      virtual bool OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister) const;

      virtual unsigned int PreExportNameId() const;
      virtual unsigned int ExportNameId() const;
      virtual unsigned int TypeId() const;
      virtual QString TypeName() const;
      virtual unsigned int PostExportNameId() const;

      // Index for referencing ..
      virtual unsigned int ComponentIndex() const;
      virtual unsigned int VectorIndex(CVectorComposite::PRINCIPAL_DIRECTION direction) const;
      virtual unsigned int InvariantIndex() const;
      virtual bool CanMap(const COpenGLNode& node, int nRegister) const;
    };
  */
  // The scalar results
  class CScalarResult : public IResult {
  public:
    typedef enum {
      PORE_PRESSURE,
      PORE_PRESSURE_CHANGE,
      FRACTURE_MATRIX_PRESSURE,
      FRACTURE_MATRIX_PRESSURE_CHANGE,
      SHEAR_CAPACITY,
      GAMMA_VERTICAL,
      GAMMA_MAX,
      GAMMA_MED,
      GAMMA_MIN,
      GAMMA_VOLUME,
      VOLUME_AVERAGE,
      TEMPERATURE,
      TEMPERATURE_CHANGE,
      CONSOL,
      CONSOL_CHANGE,
      SHTOTAZIMUTH,
      SHTOT_SvTOT,
      ShTOT_SvTOT,
      POROSITY,
      POROSITY_CHANGE,
      THICKNESS,
      THICKNESS_CHANGE,
      APERTURE_HI,
      APERTURE_ME,
      APERTURE_LO,
      APERTURE_HI_CHANGE,
      APERTURE_ME_CHANGE,
      APERTURE_LO_CHANGE,
      PERMEABILITY_MULTIPLIER_HI,
      PERMEABILITY_MULTIPLIER_ME,
      PERMEABILITY_MULTIPLIER_LO,
      COMPACTION_CAPACITY,
      PLASTIC_ENERGY,
      PORE_PRESSURE_GRADIENT,
      TEMPERATURE_GRADIENT,
      PORE_PRESSURE_INPUT,
      PORE_PRESSURE_CHANGE_INPUT,
      PORE_PRESSURE_GRADIENT_INPUT,
      TEMPERATURE_INPUT,
      TEMPERATURE_CHANGE_INPUT,
      TEMPERATURE_GRADIENT_INPUT,
      VOLUMETRIC_STRAIN_INPUT,
      VOLUMETRIC_STRAIN_CHANGE_INPUT,
      NORMAL_STRAIN_INPUT,
      LATERAL_STRAIN_INPUT
    } TResultType;

    class CScalarResultComponent : public IScalarResultComponent {
      friend class CThicknessParallelInitializationCallback;
      friend class CAvgVolumeParallelInitializationCallback;
      friend class CValueMapper;

      typedef std::map<const CFormationBase *, geo::CValue> TFormAvgVolMap;
      mutable TFormAvgVolMap m_mpValues;

      // used to store top and bottom displacement values for formation thickness
      typedef std::map<std::pair<CAnalysisType::TAnalysisType, const CDepletionStage *>, geo::CValue>
          TDisplacementValueMap;
      typedef geo::CCoordinateMap<geo::CPoint, TDisplacementValueMap> TDisplacementMap;
      mutable TDisplacementMap m_mpDisplacementMap;

      void AddToCache(TDisplacementMap *cache);
      void AddToCache(TFormAvgVolMap *cache);

    public:
      CScalarResultComponent(CScalarResult &parent,        // The tensor composite composite parent
                             const CDepletionStage &stage, // Depletion stage of the component
                             const CAnalysisType &antype,  // Analysis type
                             int nRegister);               // ResultRegister
      virtual unsigned int IconId() const;
      virtual unsigned int TypeId() const;
      virtual QString UnitName(const UNIT unit) const;

      void ClearMap();
      virtual bool Defined() const;

      void calcValues(const geo::CValue &sEE, const geo::CValue &sNN, const geo::CValue &sVV, const geo::CValue &sEN,
                      geo::CValue &SHTotazimuth, geo::CValue &SHTot_SvTot,
                      geo::CValue &ShTot_SvTot) const; // wjrx mantis 2870

      virtual void ElementValues(IValueDomainScalar::TValueVec &values, const geo::IElement &element, UNIT unit,
                                 geo::IParallelInitializationCallback *cb) const;
      void PorePressure(IValueDomainScalar::TValueVec &values, const geo::IElement &element,
                        const CDepletionStage &stage) const;
      void PorePressureInput(IValueDomainScalar::TValueVec &values, const geo::IElement &element,
                             const CDepletionStage &stage) const;
      void FractureMatrixPressure(IValueDomainScalar::TValueVec &values, const geo::IElement &element,
                                  const CDepletionStage &stage) const;
      void Temperature(IValueDomainScalar::TValueVec &values, const geo::IElement &element,
                       const CDepletionStage &stage) const;
      void TemperatureInput(IValueDomainScalar::TValueVec &values, const geo::IElement &element,
                            const CDepletionStage &stage) const;
      void Consolidation(IValueDomainScalar::TValueVec &values, const geo::IElement &element,
                         const CDepletionStage &stage) const;
      void StressRatios(IValueDomainScalar::TValueVec &values, const geo::IElement &element,
                        const CDepletionStage &stage) const; // wjrx mantis 2870
      void Porosity(IValueDomainScalar::TValueVec &values, const geo::IElement &element, const CDepletionStage &stage,
                    geo::IParallelInitializationCallback *cb) const;
      void GammaValues(IValueDomainScalar::TValueVec &values, const geo::IElement &element, TResultType type, UNIT unit,
                       geo::IParallelInitializationCallback *cb) const;
      void Thickness(IValueDomainScalar::TValueVec &values, const geo::IElement &element, const CDepletionStage &stage,
                     geo::IParallelInitializationCallback *cb) const;
      void ShearCapacityMC(IValueDomainScalar::TValueVec &values, const geo::IElement &element,
                           const CDepletionStage &stage) const;
      void CompactionCapacity(IValueDomainScalar::TValueVec &values, const geo::IElement &element,
                              const CDepletionStage &stage) const;
      void ApertureValues(TResultType result_type, IValueDomainScalar::TValueVec &values, const CDepletionStage &stage,
                          const geo::IElement &mesh_element, UNIT unit) const;
      void permeabilityMultiplierValues(TResultType result_type, IValueDomainScalar::TValueVec &values,
                                        const geo::IElement &mesh_element, UNIT unit) const;

      virtual bool Gradient() const;

      virtual geo::IParallelInitializationCallback *GetParallelInitializationCallback();
      virtual bool NeedParallelInitializationCallback() const { return Parent().NeedParallelInitializationCallback(); }

    private:
      geo::CValue FetchDisplacementValueFor(const geo::IPoint &pt, const CDepletionStage &stage,
                                            geo::IParallelInitializationCallback *cb) const;
      void PorePressureUndrained(IValueDomainScalar::TValueVec &values, const geo::IElement &element,
                                 const CDepletionStage &stage, const CFormationBase &formation) const;
      void apertureValues(IValueDomainScalar::TValueVec &fractureAperture, const geo::IElement &mesh_element,
                          const CDepletionStage &depletionStage, UNIT unit, TResultType resultType) const;
      void PlasticEnergy(const CDepletionStage &stage, IValueDomainScalar::TValueVec &values,
                         const geo::IElement &element, UNIT unit, geo::IParallelInitializationCallback *cb) const;
    };

  private:
    TResultType m_result_type;
    unsigned int m_uLabel;
    CScalarResultComponent *m_pVolumeAverage;
    void SetIndex();

  protected:
    virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;
    virtual void BuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);

  public:
    // Construction
    CScalarResult(unsigned int uName, unsigned int uLabelId, CResultGroup &group, TResultType result_type);
    CScalarResult(const QString &sName, unsigned int uLabelId, CResultGroup &group, TResultType result_type);
    bool ValidName(const std::string &name, CResultInfo &resultInfo) const;
    TResultType ResultType() const;
    virtual QString ExportLabel(int nComponent) const;
    virtual bool CanMap(const COpenGLNode &node, int nRegister) const;
    virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
    virtual bool OnDefined(const IResultComponent &component) const;

    class CTensorParallelInitializationCallback
        : public ITensorGroupTemplate<CStrainTensor>::CParallelInitializationCallback {
      ITensorGroupTemplate<CStressTensor>::CParallelInitializationCallback m_cb;

      std::vector<IValueDomainScalar::TValueVec> m_ScalarVectors;
      std::vector<CStressTensorValueSet::TValueVec> m_StressTensorVectors;

      template <class Value> void initVector(std::vector<std::vector<Value>> &vectors, size_t index, size_t size);

    public:
      CTensorParallelInitializationCallback(CModelBase &model);
      virtual ~CTensorParallelInitializationCallback();
      virtual _ITensorResultCache &GetCache(const ITensorGroup *caller, const IResultComponent &component) const;

      virtual IValueDomainScalar::TValueVec *getScalarVector(size_t index, size_t size);
      virtual CStressTensorValueSet::TValueVec *getStressTensorVector(size_t index, size_t size);
    };

    virtual bool NeedParallelInitializationCallback() const;

    virtual bool PrepareMapping(const geo::IElementSet *, const IValueComponentBase *);

  private:
    bool m_bNeedTensorParallelInitializationCallback;
  };

  // The displacement result represents the displacemnt vector vor
  class CDisplacementResult : public CVectorResult {
  protected:
    virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;

  public:
    // Construction
    CDisplacementResult(unsigned int uName, CResultGroup &group);
    CDisplacementResult(const QString &sName, CResultGroup &group);

    bool ValidName(const std::string &name, CResultInfo &resultInfo) const;

    virtual double ConvertToField(const double &dValue) const;

    // Displacements are only in the interesting in non-initial depletionstages.
    virtual bool CanMap(const COpenGLNode &node, int nRegister) const;
    virtual QString ExportLabel(int nComponent) const;
    virtual QString VectorExportLabel() const;
    virtual QString UnitName(UNIT unit) const;

    virtual bool PrepareMapping(const geo::IElementSet *, const IValueComponentBase *);
  };

  // Scalar results for fault computing
  class CFaultScalarResult : public IResult {
  public:
    typedef enum {
      DILATATION,
      SHEAR_CAPACITY,
      PORE_PRESSURE,
      PORE_PRESSURE_CHANGE,
      PLASTIC_ENERGY,
      PORE_PRESSURE_INPUT,
      PORE_PRESSURE_CHANGE_INPUT,
      BOUNDARY_DISPLACEMENT_INPUT,
      BOUNDARY_PRESSURE_INPUT
    } TResultType;
    class CFaultScalarResultComponent : public IScalarResultComponent {
    public:
      CFaultScalarResultComponent(CFaultScalarResult &parent,   // The tensor composite composite parent
                                  const CDepletionStage &stage, // Depletion stage of the component
                                  const CAnalysisType &antype,  // Analysis type
                                  int nRegister);               // Register index
      virtual unsigned int IconId() const;
      virtual unsigned int TypeId() const;
      virtual QString UnitName(const UNIT unit) const;
      virtual void ElementValues(IValueDomainScalar::TValueVec &values, const geo::IElement &element, UNIT unit,
                                 geo::IParallelInitializationCallback *cb) const;
      virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement &elm, const UNIT unit = CQuantity::SI_UNIT,
                                                         geo::IParallelInitializationCallback *cb = 0) const;

    private:
      void DilatationValues(const CDepletionStage &stage, IValueDomainScalar::TValueVec &values,
                            const geo::IElement &mesh_element, UNIT unit) const;
      void PlasticEnergy(const CDepletionStage &stage, IValueDomainScalar::TValueVec &values,
                         const geo::IElement &element, UNIT unit) const;
    };

  private:
    TResultType m_result_type;
    unsigned int m_uLabel;
    void SetIndex();

  protected:
    virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;
    virtual void BuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);

  public:
    // Construction
    CFaultScalarResult(unsigned int uName, unsigned int uLabelId, CResultGroup &group, TResultType result_type);
    CFaultScalarResult(const QString &sName, unsigned int uLabelId, CResultGroup &group, TResultType result_type);
    bool ValidName(const std::string &name, CResultInfo &resultInfo) const;
    TResultType ResultType() const;
    virtual QString ExportLabel(int nComponent) const;
    virtual bool CanMap(const COpenGLNode &node, int nRegister) const;
    virtual bool OnDefined(const IResultComponent &component) const;
    virtual bool isFaultResult() const;
    virtual bool PrepareMapping(const geo::IElementSet *, const IValueComponentBase *);
  };

  // The displacement result represents the displacemnt vector for the fault
  class CFaultDisplacementResult : public CVectorResult {
  protected:
    virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;

  public:
    // Construction
    CFaultDisplacementResult(unsigned int uName, CResultGroup &group);
    CFaultDisplacementResult(const QString &sName, CResultGroup &group);

    bool ValidName(const std::string &name, CResultInfo &resultInfo) const;
    // Displacements are only in the interesting in non-initial depletionstages.
    virtual bool CanMap(const COpenGLNode &node, int nRegister) const;
    virtual QString ExportLabel(int nComponent) const;
    virtual QString VectorExportLabel() const;
    virtual QString UnitName(UNIT unit) const;
    virtual double ConvertToField(const double &dValue) const;
    virtual bool isFaultResult() const;
    virtual bool PrepareMapping(const geo::IElementSet *, const IValueComponentBase *);
  };

  class CFaultPlasticSlipResult : public IVectorResult {
  public:
    // Construction
    CFaultPlasticSlipResult(unsigned int uName, CResultGroup &group);
    CFaultPlasticSlipResult(const QString &sName, CResultGroup &group);

    virtual const geo::IVector &VectorPoint(const geo::IPoint &point, const IResultComponent &component,
                                            geo::IParallelInitializationCallback *cb = 0) const;
    virtual const geo::IVector &VectorElement(const geo::IElement &element, int nNodeIndex, TMapType map_type,
                                              const IResultComponent &component,
                                              geo::IParallelInitializationCallback *callback = 0) const;
    virtual bool CanMap(const COpenGLNode &node, int nRegister) const;
    virtual QString ExportLabel(int nComponent) const;
    virtual QString VectorExportLabel() const;
    virtual QString UnitName(UNIT unit) const;
    virtual double ConvertToField(const double &dValue) const;
    virtual bool isFaultResult() const;
    bool ValidName(const std::string &name, CResultInfo &resultInfo) const;

    virtual bool PrepareMapping(const geo::IElementSet *, const IValueComponentBase *);

  protected:
    virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;

  private:
    struct _cache {
      std::vector<geo::CVector> m_vcValues;
      const geo::IElement *pElement;
      const geo::IPoint *pPoint;
      const CDepletionStage *pStage;
      CAnalysisType::TAnalysisType antype;
      int nRegisterIndex;
      std::set<int> stNodeIndex;
    };

    mutable struct _cache m_cache;

    geo::CVector Calculate(const geo::IInterfaceElement &iface, const geo::IPoint &point,
                           const geo::IVector &vecDeformation, double dDilatation, const geo::IVector &vecShearStress,
                           double dElasticShearStiffness) const;
  };

  class CFaultNormalStress : public IResult {
    class CNormalStressComponent : public IScalarResultComponent {
    public:
      CNormalStressComponent(CFaultNormalStress &parent,   // The tensor composite composite parent
                             const CDepletionStage &stage, // Depletion stage of the component
                             const CAnalysisType &antype,  // Analysis type
                             int nRegister);               //
      virtual unsigned int IconId() const;
      virtual unsigned int TypeId() const;
      virtual QString UnitName(const UNIT unit) const;
      virtual void ElementValues(IValueDomainScalar::TValueVec &values, const geo::IElement &element, UNIT unit,
                                 geo::IParallelInitializationCallback *cb) const;
    };
    friend class CNormalStressComponent;
    bool m_bChange;
    unsigned int m_uLabel;
    bool m_bTotal;

  protected:
    bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;
    virtual void BuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);

  public:
    // Construction
    CFaultNormalStress(unsigned int uFileIndex, unsigned int uName, unsigned int uLabelId, CResultGroup &group,
                       bool bTotal, bool bChange);
    bool ValidName(const std::string &name, CResultInfo &resultInfo) const;
    virtual QString ExportLabel(int nComponent) const;
    virtual bool CanMap(const COpenGLNode &node, int nRegister) const;
    virtual bool isFaultResult() const;
    virtual bool PrepareMapping(const geo::IElementSet *, const IValueComponentBase *);
  };

  class CFaultShearStress : public CVectorResult {
    unsigned int m_uLabelId;

  protected:
    virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;

  public:
    CFaultShearStress(unsigned int uFileIndex, unsigned int uLabelId, CResultGroup &group);

    // Displacements are only in the interesting in non-initial depletionstages.
    bool ValidName(const std::string &name, CResultInfo &resultInfo) const;
    virtual double ConvertToField(const double &dValue) const;
    virtual bool CanMap(const COpenGLNode &node, int nRegister) const;
    virtual QString ExportLabel(int nComponent) const;
    virtual QString VectorExportLabel() const;
    virtual QString UnitName(UNIT unit) const;
    virtual bool isFaultResult() const;
    virtual bool PrepareMapping(const geo::IElementSet *, const IValueComponentBase *);
  };

  class CFaultShearStressLengthChangeComposite : public CVectorResult {
  public:
    class CFaultShearStressLengthChangeComponent : public IVectorResult::CVectorComponent {
    public:
      CFaultShearStressLengthChangeComponent(unsigned int uName, CFaultShearStressLengthChangeComposite &parent,
                                             const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);

      virtual geo::CValue ValuePoint(const geo::IPoint &pt,
                                     const IValueComponentBase::UNIT unit = IQuantityDouble::SI_UNIT) const;
      virtual void MapValueElement(const geo::IElement &elm, IValueDomainScalar::TValueVec &values,
                                   IResultComponent::TMapType map_type,
                                   IValueComponentBase::UNIT unit = CQuantity::SI_UNIT) const;

      const CFaultShearStressLengthChangeComposite &Composite() const;

      virtual bool isFaultResult() const;

    private:
      CFaultShearStressLengthChangeComposite &m_parent;
      typedef std::pair<const IResultComponent *, const IResultComponent *> TSourceComponentPair;
      TSourceComponentPair SourceComponents() const;
    };

    CFaultShearStressLengthChangeComposite(unsigned int uName, unsigned int uLabelId, CResultGroup &group);
    CFaultShearStressLengthChangeComposite(const QString &sName, unsigned int uLabelId, CResultGroup &group);
    virtual ~CFaultShearStressLengthChangeComposite();

    virtual bool OnDefined(const IResultComponent &component) const;
    virtual unsigned int IconId() const;

    virtual bool CanMap(const COpenGLNode &node, int nRegister) const;
    virtual double ConvertToField(const double &dValue) const;
    virtual QString ExportLabel(int nComponent) const;
    virtual QString VectorExportLabel() const;
    virtual QString UnitName(UNIT unit) const;
    virtual bool isFaultResult() const;
    bool ValidName(const std::string &name, CResultInfo &resultInfo) const;

    virtual bool PrepareMapping(const geo::IElementSet *, const IValueComponentBase *);

  protected:
    virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;
    virtual void BuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);

  private:
    unsigned int m_uLabelId;
  };

  class CRTCIStrainResult : public IResult {
  public:
    class CRTCIStrainResultComponent : public IResultComponent {
    public:
      CRTCIStrainResultComponent(CRTCIStrainResult &result, const CDepletionStage &stage, const CAnalysisType &antype,
                                 int nRegister);
      virtual unsigned int IconId() const;
      virtual unsigned int TypeId() const;
      virtual QString UnitName(const UNIT unit = IQuantityDouble::SI_UNIT) const;
      virtual geo::CValue ValuePoint(const geo::IPoint &point, const UNIT unit = IQuantityDouble::SI_UNIT,
                                     geo::IParallelInitializationCallback *cb = 0) const;
      virtual void MapValueElement(const geo::IElement &elm, IValueDomainScalar::TValueVec &values, TMapType map_type,
                                   UNIT unit = IQuantityDouble::SI_UNIT,
                                   geo::IParallelInitializationCallback *cb = 0) const;

      virtual bool PrepareMapping(const geo::IElementSet *pElementSet);
      virtual void FinishMapping();
    };

    CRTCIStrainResult(CResultGroup &group);
    bool ValidName(const std::string &name, CResultInfo &resultInfo) const;

    virtual QString ExportLabel(int nComponent) const;
    virtual bool CanMap(const COpenGLNode &node, int nRegister) const;
    virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;

  protected:
    virtual void BuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);
  };

private:
  CFaultNormalStress *m_pFaultEffectiveNormalStressChange;
  CFaultNormalStress *m_pFaultEffectiveNormalStress;
  CFaultNormalStress *m_pFaultTotalNormalStressChange;
  CFaultNormalStress *m_pFaultTotalNormalStress;
  CFaultShearStressLengthChangeComposite *m_pFaultShearStresschange;
  CFaultShearStress *m_pFaultShearStress;

  CDisplacementResult *m_pDisplacement;
  CTotalStressResult *m_pTotalStress;
  CEffectiveStressResult *m_pEffectiveStress;
  CScalarResult *m_pGammaVertical;
  CScalarResult *m_pGammaMax;
  CScalarResult *m_pGammaMed;
  CScalarResult *m_pGammaMin;
  CScalarResult *m_pGammaVolume;
  CScalarResult *m_pVolumeAverage;
  CScalarResult *m_pShearCapacity;
  CScalarResult *m_pCompactionCapacity;
  CTotalStressResult *m_pTotalStressChange;
  CEffectiveStressResult *m_pEffectiveStressChange;
  CScalarResult *m_pConsolidation;
  CScalarResult *m_pConsolidationChange;
  CScalarResult *m_pSHtotAzimuth;
  CScalarResult *m_pSHtotSvtot;
  CScalarResult *m_pShtotSvtot;
  CScalarResult *m_pPorePressure;
  CScalarResult *m_pPorePressureChange;
  CScalarResult *m_pPorePressureGradient;
  CScalarResult *m_pFractureMatrixPressure;
  CScalarResult *m_pFractureMatrixPressureChange;
  CScalarResult *m_pTemperature;
  CScalarResult *m_pTemperatureChange;
  CScalarResult *m_pTemperatureGradient;
  CScalarResult *m_pPorosity;
  CScalarResult *m_pPorosityChange;
  CTotalStrainResult *m_pTotalStrain;
  CPlasticStrainResult *m_pPlasticStrain;
  CScalarResult *m_pPlasticEnergy;
  CScalarResult *m_pThickness;
  CScalarResult *m_pThicknessChange;
  CScalarResult *m_pApertureHi;
  CScalarResult *m_pApertureMe;
  CScalarResult *m_pApertureLo;
  CScalarResult *m_pApertureHiChange;
  CScalarResult *m_pApertureMeChange;
  CScalarResult *m_pApertureLoChange;
  CScalarResult *m_pPermeabilityMultiplierHi;
  CScalarResult *m_pPermeabilityMultiplierMe;
  CScalarResult *m_pPermeabilityMultiplierLo;
  CFaultScalarResult *m_pFaultDilatation;
  CFaultDisplacementResult *m_pFaultDisplacement;
  CFaultPlasticSlipResult *m_pFaultPlasticSlip;
  CFaultScalarResult *m_pFaultPlasticEnergy;
  CFaultScalarResult *m_pFaultShearCapacity;
  CFaultScalarResult *m_pFaultPressure;
  CFaultScalarResult *m_pFaultPressureChange;
  CRTCIStrainResult *m_pRTCIStrain;
  CStaborResult *m_pStaborResult;
  CScalarResult *m_pPorePressureInput;
  CScalarResult *m_pPorePressureChangeInput;
  CScalarResult *m_pPorePressureGradientInput;
  CScalarResult *m_pTemperatureInput;
  CScalarResult *m_pTemperatureChangeInput;
  CScalarResult *m_pTemperatureGradientInput;
  CScalarResult *m_pVolumetricStrainInput;
  CScalarResult *m_pVolumetricStrainChangeInput;
  CFaultScalarResult *m_pFaultPressureInput;
  CFaultScalarResult *m_pFaultPressureChangeInput;
  CFaultScalarResult *m_pBoundaryPressureInput;
  CFaultScalarResult *m_pBoundaryDisplacementInput;
  CParentStressInputResult *m_pParentStressInput;

  CDerivedResultGroup *m_pDerivedResults;
  CDerivedResultGroup *m_pRegistryResults;
  std::map<CScalarResult::TResultType, IResult *> m_pAperture;

public:
  // Constructor
  CResultTree(CModelBase &model);
  virtual ~CResultTree();

  const CDisplacementResult &Displacement() const { return *m_pDisplacement; }
  const CTotalStressResult &TotalStress() const { return *m_pTotalStress; }
  const CTotalStressResult &TotalStressChange() const { return *m_pTotalStressChange; }
  const CTotalStrainResult &TotalStrain() const { return *m_pTotalStrain; }
  const CPlasticStrainResult &PlasticStrain() const { return *m_pPlasticStrain; }
  const CEffectiveStressResult &EffectiveStress() const { return *m_pEffectiveStress; }
  const CEffectiveStressResult &EffectiveStressChange() const { return *m_pEffectiveStressChange; }
  const IResult &PorePressure() const { return *m_pPorePressure; }
  // const IResult& SHtotAzimuth() {*m_pSHtotAzimuth;} //wjrx mantis 2870
  // const IResult& SHtotSvtot()   {*m_pSHtotSvtot;}   //wjrx mantis 2870
  // const IResult& ShtotSvtot()   {*m_pShtotSvtot;}   //wjrx mantis 2870
  const IResult &Temperature() const { return *m_pTemperature; }
  const IResult &Consolidation() const { return *m_pConsolidation; }
  const IResult &Porosity() const { return *m_pPorosity; }
  const CFaultNormalStress &FaultEffectiveNormalStress() const { return *m_pFaultEffectiveNormalStress; }
  const CFaultShearStress &FaultShearStress() const { return *m_pFaultShearStress; }
  const CFaultDisplacementResult &FaultDisplacement() const { return *m_pFaultDisplacement; }
  const CFaultScalarResult &FaultDilatation() const { return *m_pFaultDilatation; }
  const CFaultPlasticSlipResult &FaultPlasticSlip() const { return *m_pFaultPlasticSlip; }
  const IResult &Aperture(CScalarResult::TResultType resultType) const {
    assert(m_pAperture.find(resultType) != m_pAperture.end());
    return *(m_pAperture.find(resultType)->second);
  }
  const CScalarResult &ShearCapacity() const { return *m_pShearCapacity; }

  const CDerivedResultGroup &DerivedResults() const;
  CDerivedResultGroup &DerivedResults();

  const CDerivedResultGroup &RegistryResults() const;
  CDerivedResultGroup &RegistryResults();

  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);

  typedef std::vector<const IValueComponentBase *> TDataVec;

  ACCEPT_GEOMECMODELVISITORS(VisitResultTree);
};

class CThicknessParallelInitializationCallback : public geo::IParallelInitializationCallback {
  friend class CValueMapper;

  CResultTree::CScalarResult::CScalarResultComponent &m_parent;

  typedef std::map<const CFormationVolume *, CFormationVolume::TThicknessCache *> TFormationVolumeThicknessCacheMap;

  TFormationVolumeThicknessCacheMap m_mpFormationVolumeThicknessCache;
  CResultTree::CScalarResult::CScalarResultComponent::TDisplacementMap *m_pmpDisplacementCache;

public:
  CThicknessParallelInitializationCallback(CResultTree::CScalarResult::CScalarResultComponent &parent);
  virtual ~CThicknessParallelInitializationCallback();

  void GetCache(const CFormationVolume *pVolume, CFormationVolume::TThicknessCache **pCache);
  void GetCache(CResultTree::CScalarResult::CScalarResultComponent::TDisplacementMap **pCache);
};

class CAvgVolumeParallelInitializationCallback
    : public CResultTree::CScalarResult::CTensorParallelInitializationCallback {
  friend class CValueMapper;

  CResultTree::CScalarResult::CScalarResultComponent &m_parent;

  CResultTree::CScalarResult::CScalarResultComponent::TFormAvgVolMap *m_pmpValues;

public:
  CAvgVolumeParallelInitializationCallback(CModelBase &model,
                                           CResultTree::CScalarResult::CScalarResultComponent &parent);
  virtual ~CAvgVolumeParallelInitializationCallback();

  void GetCache(CResultTree::CScalarResult::CScalarResultComponent::TFormAvgVolMap **cache);
};

#endif // !defined(AFX_RESULTTREE_H__F5E7E64B_F6B3_4C59_A180_1A8301652AFC__INCLUDED_)
