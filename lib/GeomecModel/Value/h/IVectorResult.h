// IVectorResult.h: interface for the IVectorResult class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IVECTORRESULT_H__60D22A84_F672_4FE8_B6DD_1ED427713652__INCLUDED_)
#define AFX_IVECTORRESULT_H__60D22A84_F672_4FE8_B6DD_1ED427713652__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IParallelInitializationCallback.h"
#include "Result.h"
#include "ResultRegister.h"

class IVectorResult : public IResult {
protected:
  virtual void BuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);
  virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const = 0;

public:
  enum VECTOR_COMPONENT { VC_X = 0, VC_Y, VC_Z, VC_LENGTH, VC_FULLVECTOR };

  // The vector component can represent the x, y, z coordinate or the length of the vector. The
  // unit is m for si and ft for field.
  typedef std::vector<geo::CVector> TVectorVec;

  class IVectorComponent : public IResultComponent {
  public:
    IVectorComponent(unsigned int uComponentName, IVectorResult &parent, enum VECTOR_COMPONENT component,
                     const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);
    IVectorComponent(IVectorResult &parent, const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);
    virtual void MapValueElement(const geo::IElement &elm, TVectorVec &vectors, TMapType map_type,
                                 UNIT unit = CQuantity::SI_UNIT,
                                 geo::IParallelInitializationCallback *cb = 0) const = 0;
    virtual void MapValueElementCB(const geo::IElement &elm, TVectorVec &vectors, TMapType map_type,
                                   UNIT unit = CQuantity::SI_UNIT,
                                   geo::IParallelInitializationCallback *cb = 0) const = 0;
  };

  class CVectorComponent : public IVectorComponent {
    enum VECTOR_COMPONENT m_component;
    geo::CValue VectorToValue(const geo::IVector &vector, UNIT unit) const;
    geo::CValue ValueToValue(const geo::CValue &value, UNIT unit) const;

  public:
    CVectorComponent(unsigned int uComponentName,     // Name from resource
                     IVectorResult &parent,           // The vector composite parent
                     enum VECTOR_COMPONENT component, // Component of the vector
                     const CDepletionStage &stage,    // Depletion stage of the component
                     const CAnalysisType &antype,     // Analysis type
                     int nRegister);                  // Register index
    virtual unsigned int IconId() const;
    virtual unsigned int TypeId() const;
    virtual QString UnitName(const UNIT unit) const;
    virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint &pt, const UNIT unit = CQuantity::SI_UNIT,
                                                  geo::IParallelInitializationCallback *callback = 0) const;
    virtual void MapValueElement(const geo::IElement &elm, IValueDomainScalar::TValueVec &values, TMapType map_type,
                                 UNIT unit = CQuantity::SI_UNIT,
                                 geo::IParallelInitializationCallback *callback = 0) const;

    virtual void MapTensorElement(const geo::IElement &elm, std::vector<CTensor> &values, TMapType map_type,
                                  geo::IParallelInitializationCallback *callback = 0) const;
    virtual void MapValueElement(const geo::IElement &elm, TVectorVec &vectors, TMapType map_type,
                                 UNIT unit = CQuantity::SI_UNIT,
                                 geo::IParallelInitializationCallback *callback = 0) const;
    virtual void MapValueElementCB(const geo::IElement &elm, TVectorVec &vectors, TMapType map_type,
                                   UNIT unit = CQuantity::SI_UNIT,
                                   geo::IParallelInitializationCallback *callback = 0) const;
    VECTOR_COMPONENT Component() const;

    virtual bool NeedParallelInitializationCallback() const;
  };

  class CFullVectorComponent : public IVectorComponent {
  public:
    class CVectorData : public IValueDataInterfaceVector {
      const CFullVectorComponent *m_Parent;

      mutable double m_Min;
      mutable double m_Max;

      void SetMinMax(const TValue &t) const;

    public:
      CVectorData(const CFullVectorComponent &parent);

      void ResetMinMax() const;

      using IValueDataInterfaceTempl<IValueDataInterface::VECTOR, IValueDomainVector>::MinMax;
      virtual TMinMax MinMax(IProgressBase &, const IValueDataInterface::UNIT = IQuantityDouble::SI_UNIT) const;
      virtual TValue ValuePoint(const geo::IPoint &, const IValueDataInterface::UNIT = IQuantityDouble::SI_UNIT,
                                geo::IParallelInitializationCallback *cb = 0) const;
      virtual TValueVec ValueElement(const geo::IElement &, const IValueDataInterface::UNIT = IQuantityDouble::SI_UNIT,
                                     geo::IParallelInitializationCallback *cb = 0) const;
      virtual TValue Average(IProgressBase &, const IValueDataInterface::UNIT = IQuantityDouble::SI_UNIT) const;
    };

    CFullVectorComponent(IVectorResult &parent,        // The vector composite parent
                         const CDepletionStage &stage, // Depletion stage of the component
                         const CAnalysisType &antype,  // Analysis type
                         int nRegister);               // Register index
    virtual unsigned int IconId() const;
    virtual unsigned int TypeId() const;
    virtual QString UnitName(const UNIT unit) const;
    virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint &pt, const UNIT unit = CQuantity::SI_UNIT,
                                                  geo::IParallelInitializationCallback *callback = 0) const;
    virtual void MapValueElement(const geo::IElement &elm, IValueDomainScalar::TValueVec &values, TMapType map_type,
                                 UNIT unit = CQuantity::SI_UNIT,
                                 geo::IParallelInitializationCallback *callback = 0) const;

    // virtual void MapTensorElement(const geo::IElement& elm, std::vector<CTensor> & values, TMapType map_type) const;
    virtual void MapValueElement(const geo::IElement &elm, TVectorVec &vectors, TMapType map_type,
                                 UNIT unit = CQuantity::SI_UNIT,
                                 geo::IParallelInitializationCallback *callback = 0) const;
    virtual void MapValueElementCB(const geo::IElement &elm, TVectorVec &vectors, TMapType map_type,
                                   UNIT unit = CQuantity::SI_UNIT,
                                   geo::IParallelInitializationCallback *callback = 0) const;

    virtual IValueComponentBase::TValueComponentType Type() const;

    virtual const IValueDataInterfaceVector *VectorData() const;
    virtual IValueDataInterfaceVector *VectorData();

    virtual void ResetMinMax() const;

    virtual bool NeedParallelInitializationCallback() const;

  private:
    CVectorData m_Data;
  };

  // Construction
  IVectorResult(unsigned int uName, CResultGroup &group);
  IVectorResult(const QString &sName, CResultGroup &group);
  virtual ~IVectorResult();

  typedef CValueType::TMapEnum TMapType;
  virtual const geo::IVector &VectorPoint(const geo::IPoint &point, const IResultComponent &component,
                                          geo::IParallelInitializationCallback *callback = 0) const = 0;
  virtual const geo::IVector &VectorElement(const geo::IElement &element, int nNodeIndex, TMapType map_type,
                                            const IResultComponent &component,
                                            geo::IParallelInitializationCallback *callback = 0) const = 0;

  virtual const ITensor &TensorElement(const geo::IElement & /*element*/, int /*nNodeIndex*/, TMapType /*map_type*/,
                                       const IResultComponent & /*component*/,
                                       geo::IParallelInitializationCallback * /*callback*/ = 0) const {
    static CTensor zeroTensor(0);
    return zeroTensor;
  }

  virtual double ConvertToField(const double &dValue) const = 0;
  virtual QString ExportLabel(int nComponent) const;
  virtual QString VectorExportLabel() const = 0;
  virtual QString UnitName(UNIT unit) const = 0;

  ACCEPT_GEOMECMODELVISITORS(VisitIVectorResult);
};

class CVectorResult : public IVectorResult {
  class _ResultCache {
  public:
    std::vector<geo::CVector> m_value;
    geo::CPoint m_point;
    const geo::IElement *m_element;
    typedef CValueType::TMapEnum TMapType;
    TMapType m_map_type;
    _ResultCache() : m_element(0), m_map_type(CValueType::MT_NONE) {}
  };

  typedef std::vector<_ResultCache> TCache;
  //	typedef std::pair<TCache, TCache> TCachePair;
  typedef std::map<CAnalysisType, TCache> TCacheMap;
  //	typedef std::vector<TCachePair> TCacheVec;
  typedef std::vector<TCacheMap> TCacheVec;
  mutable TCacheVec m_cache;

  class CParallelInitializationCallback : public geo::IParallelInitializationCallback {
    CModelBase &m_model;
    mutable TCacheVec m_cache;

  public:
    CParallelInitializationCallback(CModelBase &model);
    virtual ~CParallelInitializationCallback();
    _ResultCache &GetCache(const IResultComponent &component) const;
  };

  _ResultCache &GetCache(const IResultComponent &component, geo::IParallelInitializationCallback *callback = 0) const;
  void BuildCache() const;
  void ClearCache();

public:
  typedef CVectorValueSet (CResultRegister::*TValueSetFunction)(const CDepletionStage &stage,
                                                                const CAnalysisType &antype, bool bChange) const;
  CVectorResult(unsigned int uName, TValueSetFunction function, CResultGroup &group, bool bChange);
  CVectorResult(const QString &sName, TValueSetFunction function, CResultGroup &group, bool bChange);

  bool Change() const { return m_bChange; }

  virtual const geo::IVector &VectorPoint(const geo::IPoint &point, const IResultComponent &component,
                                          geo::IParallelInitializationCallback *callback = 0) const;
  virtual const geo::IVector &VectorElement(const geo::IElement &element, int nNodeIndex, TMapType map_type,
                                            const IResultComponent &component,
                                            geo::IParallelInitializationCallback *callback = 0) const;
  void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);

  virtual geo::IParallelInitializationCallback *GetParallelInitializationCallback();
  virtual bool PrepareMapping(const geo::IElementSet *, const IValueComponentBase *) = 0;
  virtual void FinishMapping();

  ACCEPT_GEOMECMODELVISITORS(VisitVectorResult);

private:
  TValueSetFunction m_function;
  bool m_bChange;
  CParallelInitializationCallback *m_callback;
};

#endif // !defined(AFX_IVECTORRESULT_H__60D22A84_F672_4FE8_B6DD_1ED427713652__INCLUDED_)
