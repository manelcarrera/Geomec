// DerivedResult.h: interface for the CDerivedResult class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DERIVEDRESULT_H__71B97812_1F04_4D96_B01F_D047D7002780__INCLUDED_)
#define AFX_DERIVEDRESULT_H__71B97812_1F04_4D96_B01F_D047D7002780__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// The CDerivedResult allows the user to derive a user defined result with the RPN Calculator.
// The CDerivedResult can be stored in the registry or in the Geomec Project File TM. The user
// can access point sets, results and derived results when the result is stored in the Geomec
// Project File TM. When the Derived Result is stored on the registry the only the results
// available in each app. are accessible. The CResultSelectDlg is used to add operands to the
// RPN stack.

#include "IVectorResult.h"
#include "Result.h"
#include "rpnstack.h"

class CResultInfo;
class CDerivedResultGroup;
class CDerivedResult : public IResult {
  CDerivedResultGroup *m_pGroup;
  QString m_sResultKey;
  rpn::CRpnStack *m_pStack;

public:
  typedef IValueDomainScalar::TValueVec TValueVec; // Vector of return values
  typedef IValueDomainScalar::TValue TValue;
  typedef IValueComponentBase::UNIT UNIT;
  // The coordinate proxy
  class CResultCoordinateProxy : public rpn::CRpnStack::CCoordinateProxy {
  protected:
    virtual int TypeId() const;

  public:
    CResultCoordinateProxy(unsigned int uName, rpn::CRpnStack &stack, COORDINATE coor, const QString &sProxyId);
    CResultCoordinateProxy(unsigned int uName, rpn::CRpnStack &stack, COORDINATE coor);
    CResultCoordinateProxy(const QString &sName, rpn::CRpnStack &stack, COORDINATE coor, const QString &sProxyId);
    CResultCoordinateProxy(rpn::CRpnStack &stack, const QString &sProxyId); // Constructor for loading
    virtual rpn::CRpnOperand::IValueProxy *Clone(rpn::CRpnStack &NewStack) const;
    virtual TValue Value(const geo::IPoint &pt, UNIT unit) const;
    virtual unsigned int IconId() const;
    virtual bool Recursive(TParentSet stParent) const;
    virtual bool Defined() const { return true; }
    virtual bool exists() const { return true; }
  };

  // The derived result component contains the rpn stack
  class CDerivedResultComponent : public IResultComponent {
  public:
    void RecursiveExpand(std::list<std::string> &lstMessages);
    void MapValueElement(const geo::IElement &elm, TValueVec &values, IVectorResult::TMapType map_type,
                         UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
    class CValueComponentProxy : public rpn::CRpnOperand::IValueProxy {
      // Valuecomponent observer observes inserted value components and when the
      // valuecomponent observer disconnects the valuecomponent distroys.
      class CValueComponentObserver : public CGraphNode {
        friend class CValueComponentProxy;
        CValueComponentProxy *m_proxy;
        IValueComponentBase &m_value_component;

      public:
        // Construction
        CValueComponentObserver(IValueComponentBase &value_component, CValueComponentProxy &proxy);
        virtual ~CValueComponentObserver();
        const IValueComponentBase &ValueComponent() const;
        IValueComponentBase &ValueComponent();
        virtual void OnNeighbourDeleted(const CGraphNode &item);
        virtual unsigned int IconId() const;
        virtual unsigned int TypeId() const;
        rpn::CRpnObject *GetExpandedRpnObject(rpn::CRpnStack &targetstack, std::list<std::string> &lstMessages) const;
      };
      friend class CValueComponentObserver;

    protected:
      virtual int TypeId() const;

    private:
      CValueComponentObserver *m_pObserver; // The reference to the IValueComponentBase object
                                            // what is destroys when the value components
                                            // disconnects.
      QString m_sName;                      // Name of the observer cq operand
      // These variables are used to read the IValueComponentBase reference from file and intialize
      // the m_pObserver member when the enable function is called.
      int m_nIndex;           // Index to the IResultComposite or IValueComponentBase
      int m_nComponent;       // Component of result Composite
      int m_nTimeStep;        // Timestep of the result -1 when we have a plain component
      CAnalysisType m_antype; // Linear or non-linear
      CModelBase *m_pModel;   // Model ptr
      bool Connect();         // Function to connect to object
      bool m_bStoreOnFile;    // Is the value component stored on file of globally
      bool m_bFixedLinStage;  // Are the linearity and stage fixed?

    public:
      // Construction with a name, an IValueComponentBase and a RpnStack
      CValueComponentProxy(const QString &sName, IValueComponentBase &value_component, rpn::CRpnStack &NewStack,
                           bool bFixedLinStage);
      // Constructor for file loading. After the constuctor is called the load function should
      // called after construction. When the model is fully loaded the cross references are
      // resolved by calling the Enable function.
      CValueComponentProxy(rpn::CRpnStack &NewStack, const QString &sProxyId);
      CValueComponentProxy(const CValueComponentProxy &rhs, rpn::CRpnStack &NewStack);
      virtual ~CValueComponentProxy();
      virtual rpn::CRpnOperand::IValueProxy *Clone(rpn::CRpnStack &NewStack) const;
      const IValueComponentBase &ValueComponent() const;
      IValueComponentBase &ValueComponent();
      void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version);
      //			void SaveStream(CStorageNode::TSTREAM& stream);
      virtual void SaveStream(std::stringstream &stream);
      virtual void LoadStream(std::stringstream &stream, CStreamVersion &version);
      virtual void AttachToModel(CFemAppModel &model);
      virtual QString TextTag() const;
      virtual TValue Value(const geo::IPoint &pt, UNIT unit) const;
      virtual TValueVec Value(const geo::IElement &el, UNIT unit) const;
      virtual unsigned int IconId() const;
      bool IsResult() const;
      virtual bool CanDelete() const;
      virtual bool HasProperties() const;
      virtual bool Properties(bool fixedLinStage, const IValueComponentBase *selection, const QString &name);
      void StoreOnFile(bool bStoreOnFile);
      virtual bool Recursive(TParentSet stParent) const;
      void SetAnalysisTypeAndStage(const CAnalysisType &antype, int nStage);
      virtual bool Defined() const;
      virtual bool exists() const;

      bool FixedLinearityAndStage() const { return m_bFixedLinStage; }
      const CAnalysisType &AnalysisType() const { return m_antype; }
      int Stage() const { return m_nTimeStep; }
      bool StoreOnFile() const;
      virtual rpn::CRpnObject *GetExpandedRpnObject(rpn::CRpnStack &targetstack,
                                                    std::list<std::string> &lstMessages) const;
      bool HasValueComponent() const;

    private:
      IValueComposite *FindResultByIndex(CResultGroup &rg) const;
    };

    void UpdateFormations();
    QString LoadFromString(std::stringstream &stream);
    void SaveString(std::stringstream &stream, const QString &string);
    void RpnVersionFromFileVersion(TSTREAM &stream, const CStreamVersion &version,
                                   std::vector<rpn::CRpnVersion> &vcRpnVersion) const;

  public:
    // Construction ...
    CDerivedResultComponent(CDerivedResult &parent, const CDepletionStage &stage, const CAnalysisType &m_antype,
                            int m_nRegister);
    virtual ~CDerivedResultComponent();
    virtual unsigned int IconId() const;
    virtual unsigned int TypeId() const;
    virtual bool Empty() const;
    virtual const QString &Name() const;
    virtual void Name(const QString &strName);
    virtual long SavedItems() const;
    virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
    virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
    void LoadStream(std::stringstream &stream, rpn::CRpnVersion &version);
    void SaveStream(std::stringstream &stream);

    void RpnStack(const rpn::CRpnStack &stack);
    const rpn::CRpnStack &RpnStack() const;

    // set the linearity and stage of this component in the stack's proxies
    // return false if not possible (not defined)
    bool MakeCurrent(bool bSendModified = true);

    // Get values from the component
    virtual geo::CValue ValuePoint(const geo::IPoint &pt, const UNIT unit = CQuantity::SI_UNIT,
                                   geo::IParallelInitializationCallback *cb = 0) const;
    virtual TValueVec ValueElement(const geo::IElement &elm, const UNIT unit = CQuantity::SI_UNIT,
                                   geo::IParallelInitializationCallback *cb = 0) const;

    virtual QString ExportLabel() const;
    virtual QString UnitName(const UNIT unit = IQuantityDouble::SI_UNIT) const;
    virtual bool Defined() const;
    virtual IValueDomainScalar::TMinMax MinMax(IProgressBase &progressBase,
                                               const UNIT unit = IQuantityDouble::SI_UNIT) const;
    void AttachToModel(CModelBase &model);

    const rpn::CRpnStack *Stack() const;
    rpn::CRpnStack *Stack();
    void CreateStack(const rpn::CRpnStack *pSource = 0);

  private:
    template <class T> void CollectProxies(std::vector<T *> &vcProxies) const {
      rpn::CRpnObject::TObjectSet stRpnObjects;
      RpnStack().CollectUsedObjects(stRpnObjects);

      rpn::CRpnObject::TObjectSet::iterator it;
      for (it = stRpnObjects.begin(); it != stRpnObjects.end(); ++it) {
        const rpn::CRpnOperand *operand = dynamic_cast<const rpn::CRpnOperand *>(*it);
        if (operand) {
          const T *pTProxy = dynamic_cast<const T *>(operand->Proxy());
          if (pTProxy)
            vcProxies.push_back(const_cast<T *>(pTProxy));
        }
      }
    }
    void MakeCurrentMaterialParameterProxies(bool &bModified);
    void MakeCurrentValueComponentProxies(bool &bModified);
    void AppendMaterialParameterExportLabel(bool &bAppend) const;
    void AppendValueComponentExportLabel(bool &bAppend) const;
  };

  // Construction / Destruction
  CDerivedResult(CDerivedResultGroup &group);
  virtual ~CDerivedResult();
  bool StoreOnFile() const;

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual bool Empty() const;
  virtual long SavedItems() const;
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  void LoadStream(std::stringstream &stream);
  void SaveStream(std::stringstream &stream);
  void SaveToRegistry();
  void LoadFromRegistry(const QString &sResultKey);
  void AttachToModel(CModelBase &model);
  virtual bool Destroy();
  bool RegisterResult();

  // Export functions
  bool CanExport() const;

  virtual bool CanMap(const COpenGLNode &node, int nRegister) const;

  const rpn::CRpnStack *RpnStack() const;
  rpn::CRpnStack *RpnStack();
  void CreateStack(const rpn::CRpnStack *pSource = 0);

  ACCEPT_GEOMECMODELVISITORS(VisitDerivedResult);

protected:
  void BuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);
  virtual IResultComponent *OnGetResultComponent(int nTimeStep, const CAnalysisType &antype, int nRegister,
                                                 unsigned int nComponent) const;

public:
  virtual QString ExportLabel(int nComponent) const;
  virtual bool ValidName(const std::string &name, CResultInfo &resultInfo) const;

  virtual bool PrepareMapping(const geo::IElementSet *pElementSet, const IValueComponentBase *pValueComponent);
};

class CDerivedResultGroup : public CResultGroup {
public:
  CDerivedResultGroup(unsigned int uGroupName, CResultGroup &parent, bool bStoreOnFile);

  void LoadResultFromRegistry();
  bool StoreOnFile() const;
  void AttachToModel();
  int UniqueIndex() const;

  void detach();

  long SavedItems() const;
  void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog);
  void SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &prog);

  ACCEPT_GEOMECMODELVISITORS(VisitDerivedResultGroup);

private:
  bool m_bStoreOnFile;
};

#endif // !defined(AFX_DERIVEDRESULT_H__71B97812_1F04_4D96_B01F_D047D7002780__INCLUDED_)
