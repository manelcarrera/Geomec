// ITensorGroup.h: interface for the ITensorGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITENSORGROUP_H__6395E266_6FF5_4A48_A12F_6C339966C2DF__INCLUDED_)
#define AFX_ITENSORGROUP_H__6395E266_6FF5_4A48_A12F_6C339966C2DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IVectorResult.h"
#include "IParallelInitializationCallback.h"

class CResultInfo;

class CDepletionStage;
// The ITensor group supplies the components, the principal directions and the invariants of the vector.
// Every sub group is an IResult
class ITensorGroup : public CResultGroup  
{
public:
  class CTensorData : public IValueDataInterfaceTensor
  {
    ITensorGroup* m_Group;
    const IResultComponent& m_Parent;

    mutable double m_Min;
    mutable double m_Max;

    // TODO: check whether this is a meaningful way to determine some "min" and "max" values of tensors
    void SetMinMax(const TValue& t) const;

  public:
    CTensorData(const IResultComponent& parent);

    void ResetMinMax() const;

    using IValueDataInterfaceTempl<IValueDataInterface::TENSOR, IValueDomainTensor>::MinMax;
    virtual TMinMax MinMax(IProgressBase &,const IValueDataInterface::UNIT = IQuantityDouble::SI_UNIT) const;
    virtual TValue ValuePoint(const geo::IPoint &, const IValueDataInterface::UNIT = IQuantityDouble::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
    virtual TValueVec ValueElement(const geo::IElement &, const IValueDataInterface::UNIT = IQuantityDouble::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
    virtual TValue Average(IProgressBase&, const IValueDataInterface::UNIT = IQuantityDouble::SI_UNIT) const;
  };

  typedef enum { TT_STRESS = 0, TT_STRAIN } TTensorType;
	// The component composite class supplies the tensor components xx, yy, zz, xy, yz and zx to
	// the user interface. 
	class CComponentComposite : public IResult
	{
	public:
		enum TENSOR_COMPONENT { TC_XX = 0, TC_YY, TC_ZZ, TC_XY, TC_YZ, TC_ZX, TC_FT };
		// The tensor component represents the 6 different tensor components in 6 instances,
		// + (waij TFS 101672) an entry for Full Tensor.

		class CComponent : public IResultComponent
		{
			TENSOR_COMPONENT m_component;
			geo::CValue ComponentAt(const ITensor& tensor, UNIT unit) const;
		public:
			CComponent(unsigned int uComponentName,				// Name from resource
					   CComponentComposite& parent,		// The tensor composite parent
					   enum TENSOR_COMPONENT component,	// Component of the vector 	
					   const CDepletionStage& stage,	// Depletion stage of the component
					   const CAnalysisType& antype,					// Analysis type
					   int nRegister);					// Register index
			virtual unsigned int IconId() const;
			virtual unsigned int TypeId() const;
			virtual	QString UnitName(const UNIT unit) const;
      virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
      virtual void MapValueElement(const geo::IElement& elm, IValueDomainScalar::TValueVec& values, TMapType map_type, UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
			
      virtual bool CanComputeOnPoints() const { return true; }
      TENSOR_COMPONENT Component() const;

      virtual bool NeedParallelInitializationCallback() const;
      virtual geo::IParallelInitializationCallback *GetParallelInitializationCallback();
      virtual bool PrepareMapping(const geo::IElementSet *);
      virtual void FinishMapping();

		};
		class CFullTensorComponent : public IResultComponent
		{
		public:
			CFullTensorComponent(CComponentComposite& parent,		// The tensor composite parent
					   const CDepletionStage& stage,	// Depletion stage of the component
					   const CAnalysisType& antype,					// Analysis type
					   int nRegister);					// Register index
			virtual unsigned int IconId() const;
			virtual unsigned int TypeId() const;
			virtual	QString UnitName(const UNIT unit) const;

      // abstract in IResultComponent, so we need dummy implementations here
      virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
      virtual void MapValueElement(const geo::IElement& elm, IValueDomainScalar::TValueVec& values, TMapType map_type, UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;

      virtual IValueComponentBase::TValueComponentType Type() const;

      virtual const IValueDataInterfaceTensor* TensorData() const;
      virtual IValueDataInterfaceTensor* TensorData();

      virtual void ResetMinMax() const;
			
      virtual bool CanComputeOnPoints() const { return true; }

      virtual bool NeedParallelInitializationCallback() const;
      virtual geo::IParallelInitializationCallback *GetParallelInitializationCallback();
      virtual bool PrepareMapping(const geo::IElementSet *);
      virtual void FinishMapping();

    private:
      CTensorData m_Data;
		};
	protected:
		virtual void BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister);
	public:
		CComponentComposite(unsigned int uName, ITensorGroup& group);
		CComponentComposite(const QString& sName, ITensorGroup& group);
		virtual unsigned int IconId() const;
		virtual bool OnDefined(const IResultComponent& component) const;
		virtual QString ExportLabel(int nComponent) const;
    virtual QString ExportLabel() const;
		virtual bool CanMap(const COpenGLNode& node, int nRegister) const;
    virtual bool ValidName (const std::string &name, CResultInfo &resultInfo) const;
	};

	// The CVectorComposite class is three times instantiated. 
	// I.   Representing the maximum principal stress direction.
	// II.  Representing the medium principal stress direction.
	// III. Representing the minimum principal stress direction.
  //
  // TODO: redesign
  // For now, we add a 4th
	class CVectorComposite : public IVectorResult
	{
        public:
          bool ValidName (const std::string &name, CResultInfo &resultInfo) const;

	protected:
		virtual bool OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister) const;
		virtual void BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister);
	public:
    class CVectorComponent : public IVectorResult::CVectorComponent
    {
    public:
      CVectorComponent(unsigned int uComponentName, // Name from resource
               IVectorResult& parent, // The vector composite parent
		 const CDepletionStage& stage, // Depletion stage of the component
		 const CAnalysisType& antype, // Analysis type
		 int nRegister, // Register index
               VECTOR_COMPONENT uComponent = VC_LENGTH);
      virtual const IValueDataInterfaceTensor* TensorData() const;
      virtual IValueDataInterfaceTensor* TensorData();

      virtual void ResetMinMax() const;

      virtual bool NeedParallelInitializationCallback() const;
      virtual geo::IParallelInitializationCallback *GetParallelInitializationCallback();
      virtual bool PrepareMapping(const geo::IElementSet *);
      virtual void FinishMapping();

    private:
      CTensorData m_TensorData;
    };                       

		class CLengthComponent : public CVectorComponent
		{
		public:
			CLengthComponent(IVectorResult& parent,				// The vector composite parent
							 const CDepletionStage& stage,		// Depletion stage of the component
							 const CAnalysisType& antype,						// Analysis type
							 int nRegister,					// Register index
               VECTOR_COMPONENT uComponent = VC_LENGTH);
      virtual geo::CValue ValuePoint(const geo::IPoint& pt, const UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
      virtual void MapValueElement(const geo::IElement& elm, IValueDomainScalar::TValueVec& values, TMapType map_type, UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
		};

    class CFullVectorComponent : public IVectorResult::CFullVectorComponent
    {
    public:
      CFullVectorComponent(IVectorResult& parent,				// The vector composite parent
							 const CDepletionStage& stage,		// Depletion stage of the component
							 const CAnalysisType& antype,						// Analysis type
							 int nRegister);					// Register index

      virtual const IValueDataInterfaceTensor* TensorData() const;
      virtual IValueDataInterfaceTensor* TensorData();

      virtual void ResetMinMax() const;

      virtual bool NeedParallelInitializationCallback() const;
      virtual geo::IParallelInitializationCallback *GetParallelInitializationCallback();
      virtual bool PrepareMapping(const geo::IElementSet *);
      virtual void FinishMapping();

    private:
      CTensorData m_TensorData;
    };                       
    
    
    typedef enum { PD_MAX = 0, PD_MED, PD_MIN, CYLINDRICAL } PRINCIPAL_DIRECTION;
		CVectorComposite(unsigned int uName, CResultGroup& group, PRINCIPAL_DIRECTION direction);
		CVectorComposite(const QString& sName, CResultGroup& group, PRINCIPAL_DIRECTION direction);

    virtual const geo::IVector& VectorPoint(const geo::IPoint& point, const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;
		virtual const geo::IVector& VectorElement(const geo::IElement& element, 
											      int nNodeIndex, 
												  TMapType map_type,
												  const IResultComponent& component, geo::IParallelInitializationCallback *callback = 0) const;

    virtual bool IsTensorVector  () const
    {
        return m_direction != CYLINDRICAL;
    }
      
		virtual const ITensor & TensorElement(const geo::IElement& element,
			int nNodeIndex,
			TMapType map_type,
      const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const;
       
		virtual double ConvertToField(const double& dValue) const;
		virtual unsigned int IconId() const;
		virtual bool OnDefined(const IResultComponent& component) const;
		virtual QString ExportLabel(int nComponent) const;
    virtual QString ExportLabel() const;
    virtual QString VectorExportLabel() const;
		virtual bool CanMap(const COpenGLNode& node, int nRegister) const;
		virtual	QString UnitName(UNIT unit) const;
		PRINCIPAL_DIRECTION Direction() const;

    const ITensorGroup *TensorGroup();
    const ITensorGroup *TensorGroup() const;

	private:
		PRINCIPAL_DIRECTION m_direction;
		const ITensorGroup* m_pTensorGroup;
	};

private:
	// Tensor representations
	CComponentComposite* m_pComponent;
	CVectorComposite* m_pPrincipal[3];
  CVectorComposite* m_pCylindric;
//	CInvariantComposite* m_pInVariant;
private:
  void SetSignConvention(geo::IVector& vec, double dLength) const;
protected:
	virtual void CreateComposite();
  void CreatePrincipalDirections();
	void PrincipalDirections(geo::CVector& vcmax,
		                     double& max_length,
						     geo::CVector& vcmed,
							 double& med_length,
							 geo::CVector& vcmin,
							 double& min_length,
							 const ITensor& tensor) const;
  virtual double GetTypicalEpsilon() const = 0;

  void CreateCylindricalComposite();

public:
	// Construction
	ITensorGroup(const QString& sName, CResultGroup& parent);
	ITensorGroup(unsigned int uName, CResultGroup& parent);
	virtual ~ITensorGroup();

	// Getting the components
	typedef CValueType::TMapEnum TMapType;
  virtual const ITensor& TensorPoint(const geo::IPoint& point, const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const = 0;
	virtual const ITensor& TensorElement(const geo::IElement& element, 
									     int nNodeIndex, 
										 TMapType map_type,
                     const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const = 0;

    typedef std::pair<geo::CVector, double> TPrincipal;
	virtual const TPrincipal& VectorPoint  (const geo::IPoint& point, 
							  			    CVectorComposite::PRINCIPAL_DIRECTION direction,
                          const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const = 0;
	virtual const TPrincipal& VectorElement(const geo::IElement& element, 
											CVectorComposite::PRINCIPAL_DIRECTION direction,
										    int nNodeIndex, 
											TMapType map_type,
                      const IResultComponent& component, geo::IParallelInitializationCallback *cb = 0) const = 0;

	virtual bool OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister) const;

	// Direct X
	const CComponentComposite& Components() const { return *m_pComponent; }
	const CVectorComposite *MaxDirection() const;
	const CVectorComposite *MedDirection() const;
	const CVectorComposite *MinDirection() const;
  const CVectorComposite& Direction(CVectorComposite::PRINCIPAL_DIRECTION direction) const;
	virtual TTensorType Type() const = 0;
	virtual bool OnDefined(const IResultComponent& component) const;

	// Label for export
	virtual unsigned int PreExportNameId() const = 0;
	virtual unsigned int TypeId() const = 0;
	virtual unsigned int ExportNameId() const = 0;
	virtual unsigned int PostExportNameId() const = 0;

	// Index for referencing ..
	virtual unsigned int ComponentIndex() const = 0;
	virtual unsigned int VectorIndex(CVectorComposite::PRINCIPAL_DIRECTION direction) const = 0;
	virtual unsigned int InvariantIndex() const = 0;
	virtual double ConvertToField(const double& dDouble) const = 0;

	virtual bool CanMap(const COpenGLNode& node, int nRegister) const = 0;
	virtual	QString UnitName(CQuantity::UNIT unit) const = 0;

  virtual bool NeedParallelInitializationCallback() const = 0;
  virtual geo::IParallelInitializationCallback *GetParallelInitializationCallback() = 0;
  virtual bool PrepareMapping(const geo::IElementSet *, const IValueComponentBase *) = 0;
  virtual void FinishMapping() = 0;

  ACCEPT_GEOMECMODELVISITORS(VisitITensorGroup);
};


#endif // !defined(AFX_ITENSORGROUP_H__6395E266_6FF5_4A48_A12F_6C339966C2DF__INCLUDED_)
