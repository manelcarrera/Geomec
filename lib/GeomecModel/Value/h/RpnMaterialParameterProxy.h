// RpnMaterialParameterProxy.h: interface for the CRpnMaterialParameterProxy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RPNMATERIALPARAMETERPROXY_H__DB2BB1CD_36AD_4BA9_8F3A_E1ECA71727B2__INCLUDED_)
#define AFX_RPNMATERIALPARAMETERPROXY_H__DB2BB1CD_36AD_4BA9_8F3A_E1ECA71727B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HorizonBase.h"
#include "ifaceelmmat.h"
#include "InterfaceElement.h"
#include "rpn_version.h"
#include "RpnOperand.h"
#include "ModelBase.h"
#include "FormationBase.h"
#include "MeshBase.h"

class CMeshBase;
class CDepletionStage;

class IRpnMaterialParameterProxy : public rpn::CRpnOperand::IValueProxy 
{
  CMeshBase* m_pMesh;
  const CDepletionStage *m_DeplStage;
  bool m_bFixedStage;
  bool m_bObsolete; // waij TFS 77889
 
protected:
	virtual int TypeId() const { return MAT_PARAM_ID; }
public:
  IRpnMaterialParameterProxy
  ( CMeshBase& mesh
  , rpn::CRpnStack& NewStack
  , bool bObsolete= false
  )
  : rpn::CRpnOperand::IValueProxy(NewStack)
  , m_pMesh(&mesh)
  , m_DeplStage(0)
  , m_bFixedStage(false)
  , m_bObsolete( bObsolete) {}

  IRpnMaterialParameterProxy
  ( CMeshBase& mesh
  , const CDepletionStage *pStage
  , rpn::CRpnStack& NewStack
  , const QString& sProxyId
  , bool bFixedStage
  , bool bObsolete= false
  )
  : rpn::CRpnOperand::IValueProxy(NewStack, sProxyId)
  , m_pMesh(&mesh)
  , m_DeplStage(pStage)
  , m_bFixedStage(bFixedStage)
  , m_bObsolete(bObsolete)
  {}

  IRpnMaterialParameterProxy(rpn::CRpnStack& stack, const QString& sProxyId)
  : rpn::CRpnOperand::IValueProxy(stack, sProxyId)
  , m_pMesh(0), m_DeplStage(0)
  , m_bFixedStage(false)
  , m_bObsolete(false)
  {}

  void Mesh(CMeshBase& mesh) { m_pMesh = &mesh; }
  const CMeshBase *Mesh() const { return m_pMesh; }
  CMeshBase *Mesh() { return m_pMesh; }
  bool FixedStage() const { return m_bFixedStage; }
  void FixedStage(bool bFixed) { m_bFixedStage = bFixed; }

  virtual void AttachToModel(CFemAppModel& model)
	{ 	CModelBase& base_model = dynamic_cast<CModelBase&>(model); m_pMesh = &base_model.Mesh(); }

  virtual bool Recursive(TParentSet /*stParent*/) const { return false; }

  virtual bool Defined() const { return true; }
  virtual bool exists() const { return true; }

  void TimeStep(const CDepletionStage *stage) { m_DeplStage = stage; }
  const CDepletionStage *TimeStep() const { return m_DeplStage; }

  virtual QString DisplayName() const = 0;
  virtual unsigned int MaterialTypeID() const = 0;
  virtual bool Obsolete() const { return m_bObsolete; } // waij TFS 77889
};

template< class T >
class CRpnMaterialParameterProxy : public IRpnMaterialParameterProxy//public rpn::CRpnOperand::IValueProxy 
{
public:
  // Pre-defined return values
  typedef IValueDomainScalar::TValueVec TValueVec;			// Vector of return values
  typedef IValueDomainScalar::TValue TValue;
  typedef IValueComponentBase::UNIT UNIT;

  // Construction with a name, an IValueComponentBase and a RpnStack
  CRpnMaterialParameterProxy
  ( CMeshBase& mesh
  , rpn::CRpnStack& NewStack
  , bool bObsolete=false
  )
  : IRpnMaterialParameterProxy(mesh, NewStack,bObsolete)
  {}

  CRpnMaterialParameterProxy
  ( CMeshBase& mesh
  , const CDepletionStage *pStage
  , rpn::CRpnStack& NewStack
  , const QString& sProxyId
  , bool bFixedStage
  )
  : IRpnMaterialParameterProxy
  ( mesh
  , pStage
  , NewStack
  , sProxyId.toStdString().c_str()
  , bFixedStage
  )
  {}

  CRpnMaterialParameterProxy
  ( rpn::CRpnStack& stack
  , const QString& sProxyId
  )
  : IRpnMaterialParameterProxy
  (stack
  , sProxyId.toStdString().c_str()
  )
  {}

  virtual rpn::CRpnOperand::IValueProxy* Clone(rpn::CRpnStack& NewStack) const
  { 
    CMeshBase &mesh= const_cast<CMeshBase&>(*Mesh());
    const CDepletionStage *pTimeStep= TimeStep();
    QString sProxyId= ProxyId();
    bool bFixedStage= FixedStage();

    return new CRpnMaterialParameterProxy
      ( mesh
      , pTimeStep
      , NewStack
      , sProxyId
      , bFixedStage
      ); 
  }

  virtual QString TextTag() const
  { 
    QString sRet;
    sRet = getStringTableEntry(T::export_id());
    if(FixedStage() && TimeStep())
    {
      QString deplName = QString("_D%1").arg(TimeStep()->Index());
      sRet += deplName;
    }  
    return sRet; 
  }

  virtual QString DisplayName() const
  {
    return getStringTableEntry(T::name_id());
  }

  virtual unsigned int IconId() const { return T::icon_id(); }
  virtual void SaveStream(std::stringstream& /*stream*/) { }
  virtual void LoadStream(std::stringstream& /*stream*/, CStreamVersion& /*version*/) {}

  virtual TValue Value(const geo::IPoint &pt, UNIT unit) const;
  virtual TValueVec Value(const geo::IElement &el, UNIT unit) const;

  virtual unsigned int MaterialTypeID() const { return T::type_id(); }

  virtual bool Defined() const { return true; }
  virtual bool exists() const { return true; }
};

template< class T >
IValueDomainScalar::TValue CRpnMaterialParameterProxy<T>::Value(const geo::IPoint &pt, UNIT /*unit*/) const
{
	// Do we have a mesh?
	if(!Mesh()->IsMesh())
		return TValue();	// No valid mesh, no valid value...

	std::vector<int> vcElement = Mesh()->Mesh().ElementsAt(pt);

  if (vcElement.size() > 0)
	{
		const geo::IElement& element = Mesh()->Mesh().Element(vcElement[0]); 
    const CFFMaterial &material  = (TimeStep()) ? Mesh()->Formation(element)->Material(*TimeStep()).Material(element) :
                                                   Mesh()->Formation(element)->ActiveMaterial().Material(element);
		assert(	material.IsParameter(T::type_id()) );
		return TValue( material.ParameterValue(T::type_id()) );
	}

	return TValue();
}

template< class T >
IValueDomainScalar::TValueVec CRpnMaterialParameterProxy<T>::Value(const geo::IElement &el, UNIT unit) const
{
	TValueVec ret(el.NrOfPoints());
	if(Mesh()->IsMesh())
	{

		// Find mesh element 
		const geo::IElement* pElement = 0;
		const geo::IFace* pFace = dynamic_cast<const geo::IFace*>(&el);
		if(pFace)
		{
			if(pFace->Parent()) {
				if(&Mesh()->Mesh() == pFace->Parent()->IndexingElementSet())
				  pElement = pFace->Parent();
			}
		}
		
		if(el.IndexingElementSet() == &Mesh()->Mesh())
			pElement = &el;


		if(pElement)
		{
			//start wjrx mantis 2545
    	const geo::CInterfaceElement* pInterface = dynamic_cast<const geo::CInterfaceElement*>(pElement);
			if ( pInterface )
			{
  			const CHorizonBase *pHorizon= Mesh()->SlipHorizon(*pElement);
        const CModelBase& model = static_cast<const CModelBase&>(pHorizon->Model());
				const dia::IMaterial &mat= pHorizon->InterfaceMaterial(*pElement, model.BranchState().ActiveStage());
				const CInterfaceElementMaterial &elMat= 
					dynamic_cast<const CInterfaceElementMaterial &>( mat );

				if ( elMat.ValidParameterValue(T::type_id()) )
				for(size_t i = 0; i < ret.size(); i++)
					ret[i]= elMat.ParameterValue(T::type_id());
			}
			else
			//end wjrx mantis 2545
			{
        const CModelBase& model = static_cast<const CModelBase&>(Mesh()->Model());
        if(Mesh()->Formation(*pElement)->Material(model.InitialDepletionStage()).LibraryMaterial() != 0)
        {
				  const CFFMaterial &material  = (TimeStep()) ? Mesh()->Formation(*pElement)->Material(*TimeStep()).Material(*pElement) :
                                                         Mesh()->Formation(*pElement)->ActiveMaterial().Material(*pElement);
          bool bIsParameter = material.IsParameter(T::type_id());
				  for(size_t i = 0; i < ret.size(); i++)
          {
				    if(bIsParameter)
					    ret[i] = TValue( material.ParameterValue(T::type_id()) );
            else
              ret[i] = TValue();
          }
        }
      }
		}
		else
		{
			for(int i = 0; i < el.NrOfPoints(); i++)
				ret[i] = Value(el.Point(i), unit);
		}
	}
	
	return ret;
}



#endif // !defined(AFX_RPNMATERIALPARAMETERPROXY_H__DB2BB1CD_36AD_4BA9_8F3A_E1ECA71727B2__INCLUDED_)
