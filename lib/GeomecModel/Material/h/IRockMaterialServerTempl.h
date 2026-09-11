#ifndef _IROCKMATERIALSERVERTEMPL_H_
#define _IROCKMATERIALSERVERTEMPL_H_

#include "IMaterialServerTempl.h"
#include "IMaterialRock.h"
#include "MaterialEntry.h"
#include "BaseEntryTypes.h"
#include "MatParamsTensor.h"
#include "ValueTypes.h"
#include "MaterialParameter.h"
#include "LibraryMaterial.h"
#include "Materials.h"

template <class PARENT>
class IRockMaterialServerTempl : public IMaterialServerTempl<IMaterialRock, PARENT>
{
  typedef IMaterialServerTempl<IMaterialRock, PARENT> TBase;

public:
  // A list of material models and parameters that are fixed
  // in restart (branch) situations.
  // -1 means 'any model'.
  // These parameters cannot change in time, this is checked
  // when new materials are added (CanConnectItem).
  // When a distributed set of values is attached, it is
  // automatically attached to all stages.
  typedef struct
  {
  int model;
  QString param;
  } TMatParamPair;

  // terminated by empty string for param (param.isEmpty())
  static TMatParamPair FixedParameters[];
  static TMatParamPair InitialOnlyParameters[];

private:
  virtual bool IsValueTypeValid(const CValueType& value_type) const;
  bool MatchingParameterValues(const IMaterialRock& mat1, const IMaterialRock& mat2, const QString& sParamName) const;
  bool IsFixedParameter(unsigned int typeID) const;
  bool UnlinkFixedParameterFromStages(unsigned int typeId, const CValueType &valueType);
  bool IsInitialOnlyParameter(unsigned int typeID) const;

protected:
  virtual IMaterialRock* FindIndexInEntry(int nIndex) const;

public:
  IRockMaterialServerTempl(PARENT &parent, const CDepletionStage& stage);
  virtual ~IRockMaterialServerTempl();

  int MaterialModel() const;

  virtual bool MatchingMaterials(const IMaterialRock& mat1, const IMaterialRock& mat2) const;
  virtual	bool ConnectItem(const CGraphNode &item);
  virtual bool CanConnectItem(const CGraphNode &item) const;
  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourDeleted(const CGraphNode& node);

  virtual const CFFMaterial &Material(const geo::IElement& element, const CFFMaterial::TValueMap& mpValueInit = CFFMaterial::TValueMap()) const;
  virtual const CFFMaterial &Material(const geo::IPoint& point, const CFFMaterial::TValueMap& mpValueInit = CFFMaterial::TValueMap()) const;

  bool FixedMaterialModel() const;
};

template <class PARENT>
typename IRockMaterialServerTempl<PARENT>::TMatParamPair IRockMaterialServerTempl<PARENT>::FixedParameters[] =
{
  { MM_CAMCLAY,             MLD_CAPSHAPE      },
  { MM_CAMCLAY,             MLD_GAMMA         },
  { MM_DUALCAP_LINELA,      MLD_CAPSHAPE      },
  { MM_DUALCAP_LINELA,      MLD_GAMMA         },
  { MM_MODIFIEDMOHRCOULOMB, MLD_CAPSHAPEPARAM },
  { -1,                     QString()         } // terminator (param.isEmpty())
};

template <class PARENT>
typename IRockMaterialServerTempl<PARENT>::TMatParamPair IRockMaterialServerTempl<PARENT>::InitialOnlyParameters[] =
{
  { -1, MLD_LATRATIO_MAX },
  { -1, MLD_LATRATIO_MIN },
  { -1, MLD_AZIMUTH      },
  { -1, QString()        } // terminator (param.isEmpty())
};

template <class PARENT>
IRockMaterialServerTempl<PARENT>::IRockMaterialServerTempl(PARENT &parent, const CDepletionStage& stage)
: TBase(parent, stage)
{
}

template <class PARENT>
IRockMaterialServerTempl<PARENT>::~IRockMaterialServerTempl()
{
}

template <class PARENT>
bool IRockMaterialServerTempl<PARENT>::ConnectItem(const CGraphNode &item)
{
  if(!CanConnectItem(item))
  return false;

  const CValueType *pValueType = dynamic_cast<const CValueType*>(&item);

  TBase::DefaultStatusMessage();

  if(pValueType)
  {
  assert(TBase::LibraryMaterial());
  QString strMatLibXID;
  const CMatParamsTensor *pMPTensor = dynamic_cast<const CMatParamsTensor*>(pValueType);
  if(pMPTensor)
  {
      for(unsigned int i = 0; i < pMPTensor->ComponentSize(); ++i)
      {
    VERIFY(CMaterialParameter::MatLibXIDFromValueTypeID(pMPTensor->ComponentTypeID(i), strMatLibXID));
      }
  }
  else
  {
      VERIFY(CMaterialParameter::MatLibXIDFromValueTypeID(pValueType->TypeId(), strMatLibXID));
  }
  bool bIsFixed = false;

  if(!TBase::Loading())
  {
      int i;
      for(i = 0; !FixedParameters[i].param.isEmpty(); ++i)
      {
    if((FixedParameters[i].model == -1 ||
          FixedParameters[i].model ==
      TBase::LibraryMaterial()->MaterialModel()) &&
          FixedParameters[i].param == strMatLibXID)
    {
          bIsFixed = true;
          break;
    }
      }
  }

  if(bIsFixed)
  {
      // link to all servers
      CModelBase& model = static_cast<CModelBase&>(TBase::Model());
      CDepletionStage* pStage = &model.InitialDepletionStage();
      while(pStage)
      {
    IRockMaterialServerTempl* pServer =
          TBase::Parent().ConnectedMaterial(*pStage);
    if(pServer && pServer->LibraryMaterial())
          pServer->LinkTo(const_cast<CValueType&>(*pValueType));

    if(pStage->Last())
          pStage = 0;
    else
          pStage = &pStage->Next();
      }
  }
  else
  {
      // simply link to this server
      TBase::LinkTo(const_cast<CValueType&>(*pValueType));
  }

  return true;
  }

  return TBase::ConnectItem(item);
}

template <class PARENT>
bool IRockMaterialServerTempl<PARENT>::CanConnectItem(const CGraphNode &item) const
{
  if(!TBase::ReadOnly())
  {
  // Accepting property entries
    const CValueType *pType = dynamic_cast<const CValueType*> (&item);
    if(pType && TBase::LibraryMaterial())
    {
      if(!TBase::IsLinkedTo(item) && IsValueTypeValid(*pType))
      {
    if(!TBase::Stage().Initial())
    {
          if(IsInitialOnlyParameter(pType->TypeId()))
          {
      TBase::StatusMessage("This material parameter can only be attached to the initial depletion stage's material");
      return false;
          }
    }

    const IMaterialRock* pMatRock = TBase::LibraryMaterial();
    if(pMatRock)
    {
          const CLibraryMaterial* pMat = dynamic_cast<const CLibraryMaterial*>(&pMatRock->LibraryMaterial());
          if(pMat && pMat->GroupSize() > 1)
          {
      size_t i;
      for(i = 0; i < pMat->ParameterSize(); ++i)
      {
              const ml::CMatParam& param = pMat->Parameter(i);
              unsigned int uiValueTypeID;
              VERIFY(CMaterialParameter::ValueTypeIDFromMatLibXID(param.Name(), uiValueTypeID));
              if(uiValueTypeID == pType->TypeId())
              {
        const ml::CMaterial::CGroup* pGroup = pMat->ParameterGroup(param);
        if(pGroup)
        {
                  // this parameter is in a specific group
                  // it can only be attached if there are no parameters from one of the
                  // other groups attached

                  // get all linked valuetypes
                  std::set<CValueType*> stVT = (const_cast<IRockMaterialServerTempl<PARENT>*>(this))->template Links<CValueType>();
                  std::set<CValueType*>::iterator it;
                  for(it = stVT.begin(); it != stVT.end(); ++it)
                  {
          QString strParamName;
          if(CMaterialParameter::MatLibXIDFromValueTypeID((*it)->TypeId(), strParamName))
          {
                      const ml::CMatParam* pParam = pMat->Parameter(strParamName);
                      if(pParam)
                      {
            const ml::CMaterial::CGroup* parameterGroup = pMat->ParameterGroup(*pParam);
            if((parameterGroup != 0) && (parameterGroup != pGroup))
            {
                          TBase::StatusMessage("Material parameters of different parameter groups are not allowed simultaneously");
                          return false;
            }
                      }
          }
                  }
        }
              }
      }
          }
    }

    // make sure it's not a fixed parameter and the initial material is read-only
    const IRockMaterialServerTempl* pIniServer =
          TBase::Parent().ConnectedMaterial(TBase::Stage().InitialStage());
    assert(pIniServer != 0);
    if(!TBase::Loading() && pIniServer->ReadOnly())
    {
          const CMatParamsTensor *pMPTensor = dynamic_cast<const CMatParamsTensor*>(pType);
          if(pMPTensor)
          {
      for(unsigned int i = 0; i < pMPTensor->ComponentSize(); ++i) 
      {
              if(IsFixedParameter(pMPTensor->ComponentTypeID(i)))
              {
        TBase::StatusMessage("This material parameter is fixed");
        return false;
              }
      }
          }
          else 
          {
      if(IsFixedParameter(pType->TypeId()))
      {
              TBase::StatusMessage("This material parameter is fixed");
              return false;
      }
          }
    }

    TBase::DefaultStatusMessage();
        return true;
      }
    }
  }

  return TBase::CanConnectItem(item);
}

template <class PARENT>
bool IRockMaterialServerTempl<PARENT>::IsFixedParameter(unsigned int typeID) const
{
  QString strMatLibXID;
  VERIFY(CMaterialParameter::MatLibXIDFromValueTypeID(typeID, strMatLibXID));
  int i;
  for(i = 0; !FixedParameters[i].param.isEmpty(); ++i)
  {
  if((FixedParameters[i].model == -1 ||
      FixedParameters[i].model == TBase::LibraryMaterial()->MaterialModel()) &&
      FixedParameters[i].param == strMatLibXID)
  {
      
      return true;
  }
  }
  return false;
}

template <class PARENT>
bool IRockMaterialServerTempl<PARENT>::IsValueTypeValid(const CValueType& value_type) const
{
  // Must be a material parameter's value type ID
  QString strMatLibXID;

  if(!TBase::IsValueTypeValid(value_type))
  {
  const CMatParamsTensor *pMPTensor = dynamic_cast<const CMatParamsTensor*>(&value_type);
  if(pMPTensor)
  {
      for(unsigned int i = 0; i < pMPTensor->ComponentSize(); ++i)
      {
    if(!CMaterialParameter::MatLibXIDFromValueTypeID(pMPTensor->ComponentTypeID(i), strMatLibXID))
          return false;
    if(!TBase::LibraryMaterial()->IsParameter(
          pMPTensor->ComponentTypeID(i)))
          return false;
      }
      return true;
  }

  return false;
  }

  if(!CMaterialParameter::MatLibXIDFromValueTypeID(value_type.TypeId(), strMatLibXID))
    return false;

  return true;
}

template <class PARENT>
bool IRockMaterialServerTempl<PARENT>::MatchingParameterValues(const IMaterialRock& mat1, const IMaterialRock& mat2, const QString& sParamName) const
{
  return (fabs(mat1.ParameterValue(sParamName) - mat2.ParameterValue(sParamName)) < 1e-8);
}

template <class PARENT>
bool IRockMaterialServerTempl<PARENT>::MatchingMaterials(const IMaterialRock& mat1, const IMaterialRock& mat2) const
{
  if(mat1.MaterialModel() != mat2.MaterialModel())
  return false;

  int i;
  for(i = 0; !FixedParameters[i].param.isEmpty(); ++i)
  {
  if(FixedParameters[i].model == -1 || FixedParameters[i].model == mat1.MaterialModel())
  {
      if(!MatchingParameterValues(mat1, mat2, FixedParameters[i].param))
    return false;
  }
  }

  return true;
}

template <class PARENT>
void IRockMaterialServerTempl<PARENT>::OnNeighbourDeleted(const CGraphNode &node)
{
  for(size_t i = 0; i < TBase::ValueTypePairVec().size(); i++)
  {
  if(TBase::ValueTypePairVec()[i].first == &node)
  {
      const CMatParamsTensor *pMPTensor = dynamic_cast<const CMatParamsTensor*>(
    TBase::ValueTypePairVec()[i].first);
      if(pMPTensor)
      {
    for(unsigned int j = 0; j < pMPTensor->ComponentSize(); ++j)
    {
          if(UnlinkFixedParameterFromStages(pMPTensor->ComponentTypeID(j), *pMPTensor))
      break;
    }
      }
      else
      {
    //wjrx mantis 2583
    unsigned int typeId= TBase::ValueTypePairVec()[i].second;
    UnlinkFixedParameterFromStages(typeId,
          *TBase::ValueTypePairVec()[i].first);
      }
  }
  }

  TBase::OnNeighbourDeleted(node);
}

template <class PARENT>
bool IRockMaterialServerTempl<PARENT>::UnlinkFixedParameterFromStages(unsigned int typeId, const CValueType &valueType)
{
  QString strMatLibXID;
  VERIFY(CMaterialParameter::MatLibXIDFromValueTypeID(typeId, strMatLibXID));
  int j;
  for(j = 0; !FixedParameters[j].param.isEmpty(); ++j)
  {
  if(  TBase::LibraryMaterial() && (FixedParameters[j].model == -1
      || FixedParameters[j].model == TBase::LibraryMaterial()->MaterialModel())
      && FixedParameters[j].param == strMatLibXID )
  {
      // it's a fixed parameter, remove it from all stages
      const CDepletionStage* pStage = &TBase::Stage().InitialStage();
      while(pStage)
      {
    IRockMaterialServerTempl* pServer =
          TBase::Parent().ConnectedMaterial(*pStage);
    if(pServer && pServer != this && pServer->IsLinkedTo(valueType))
          pServer->UnLink(const_cast<CValueType&>(valueType));

    if(pStage->Last())
          pStage = 0;
    else
          pStage = &pStage->Next();
      }

      return true;
  }
  }
  return false;
}

template <class PARENT>
bool IRockMaterialServerTempl<PARENT>::IsInitialOnlyParameter(unsigned int typeID) const
{
  QString strMatLibXID;
  VERIFY(CMaterialParameter::MatLibXIDFromValueTypeID(typeID, strMatLibXID));
  int i;
  for(i = 0; !InitialOnlyParameters[i].param.isEmpty(); ++i)
  {
  if((InitialOnlyParameters[i].model == -1 ||
      InitialOnlyParameters[i].model ==
    TBase::LibraryMaterial()->MaterialModel()) &&
      InitialOnlyParameters[i].param == strMatLibXID)
      return true;
  }
  return false;
}

template <class PARENT>
void IRockMaterialServerTempl<PARENT>::OnNewNeighbour(const CGraphNode &node)
{
  TBase::OnNewNeighbour(node);

  const IMaterialRock* pMaterial = dynamic_cast<const IMaterialRock*>(&node);

  if(pMaterial && !TBase::Stage().Initial() && !TBase::Loading())
  {
  // copy fixed valuetypes (distributed parameters) from the initial stage
  IRockMaterialServerTempl* pIniServer =
      TBase::Parent().ConnectedMaterial(TBase::Stage().InitialStage());
  if(pIniServer)
  {
      const typename TBase::TValueTypePairVec& vcValueTypes =
    pIniServer->ValueTypePairVec();
      for(size_t i = 0; i < vcValueTypes.size(); ++i)
      {
    QString strMatLibXID;
    const CValueType* pValueType = vcValueTypes[i].first;
    if(CMaterialParameter::MatLibXIDFromValueTypeID(pValueType->TypeId(), strMatLibXID))
    {
          int j;
          for(j = 0; !FixedParameters[j].param.isEmpty(); ++j)
          {
      if( (  FixedParameters[j].model == -1
        || FixedParameters[j].model ==
                  TBase::LibraryMaterial()->MaterialModel()
        ) && FixedParameters[j].param == strMatLibXID)
      {
              TBase::LinkTo(const_cast<CValueType&>(*pValueType));
              break;
      }
          }
    }
      }
  }
  }
}

template <class PARENT>
const CFFMaterial &IRockMaterialServerTempl<PARENT>::Material(const geo::IElement& element, const CFFMaterial::TValueMap& mpValueInit) const
{
  assert(TBase::LibraryMaterial() != 0);

  for(size_t i = 0; i < TBase::ValueTypePairVec().size(); i++)
  {
    const CValueType *pValueType =
      TBase::ValueTypePairVec()[i].first;
    assert(pValueType);
    // this is a distributed value linked to this material, so
    // the value from the material library will be replaced by it
  // wedx mantis #2628: if required
  const CMatParamsTensor *pMPTensor = dynamic_cast<const CMatParamsTensor*>(pValueType);
  if(pMPTensor)
  {
    	CFFMaterial::TValueMap mpValue(mpValueInit);

      for(unsigned int j = 0; j < pMPTensor->ComponentSize(); ++j) 
      {
    if(TBase::IsExtrapolatingValueType(*pMPTensor) ||
          pMPTensor->PointSet().PointInConvexHull(element.MidPoint()))
    {
          IValueDomainScalar::TValueVec vcValues = pMPTensor->Component(j).ScalarData().ValueElement(element);
          assert(vcValues.size() != 0);
          for(size_t k = 0; k < vcValues.size(); ++k)
          {
      if(vcValues[k].Valid())
              mpValue.insert(CFFMaterial::TValueMap::value_type(pMPTensor->ComponentTypeID(j), vcValues[k].Value()));
          }
    }
      }

      return TBase::FetchFFMaterial(mpValue);
  }
  }

  return TBase::Material(element, mpValueInit);
}

template <class PARENT>
const CFFMaterial &IRockMaterialServerTempl<PARENT>::Material(const geo::IPoint& point, const CFFMaterial::TValueMap& mpValueInit) const
{
  assert(TBase::LibraryMaterial() != 0);

  for(size_t i = 0; i < TBase::ValueTypePairVec().size(); i++)
  {
    const CValueType *pValueType =
      TBase::ValueTypePairVec()[i].first;
    assert(pValueType);
    // this is a distributed value linked to this material, so
    // the value from the material library will be replaced by it

  const CMatParamsTensor *pMPTensor = dynamic_cast<const CMatParamsTensor*>(pValueType);
  if(pMPTensor)
  {
    	CFFMaterial::TValueMap mpValue(mpValueInit);

      for(unsigned int j = 0; j < pMPTensor->ComponentSize(); ++j) 
      {
    geo::CValue value = pMPTensor->Component(j).ScalarData().ValuePoint(point);
      if(value.Valid() &&
           (TBase::IsExtrapolatingValueType(*pMPTensor) ||
      pMPTensor->PointSet().PointInConvexHull(point)))
    {
        mpValue.insert(CFFMaterial::TValueMap::value_type(pMPTensor->ComponentTypeID(j), value.Value()));
    }
      }

      return TBase::FetchFFMaterial(mpValue);
  }
  }

  return TBase::Material(point, mpValueInit);
}

template <class PARENT>
IMaterialRock* IRockMaterialServerTempl<PARENT>::FindIndexInEntry(int nIndex) const
{
  CMaterialEntry& material_entry = (CMaterialEntry&)*((CModelBase&)
    TBase::Model()).GraphEntry(MD_ROCK_MATERIAL);
  if(material_entry.FindIndex(nIndex))
  {
  assert(dynamic_cast<IMaterialRock*>(material_entry.FindIndex(nIndex)));
    return static_cast<IMaterialRock*>(material_entry.FindIndex(nIndex));
  }

  return 0;
}

template <class PARENT>
int IRockMaterialServerTempl<PARENT>::MaterialModel() const
{
  return IMaterialServerTempl <IMaterialRock, PARENT> ::LibraryMaterial()->MaterialModel();
}

template <class PARENT>
bool IRockMaterialServerTempl<PARENT>::FixedMaterialModel() const
{
  const CModelBase& model = static_cast<const CModelBase&>(IMaterialServerTempl <IMaterialRock, PARENT> ::Model());

//  const CDepletionStage& inistage = model.InitialDepletionStage();
//  const CDepletionStage* pStage = inistage.Last() ? 0 : &inistage.Next();
  const CDepletionStage* pStage = &model.InitialDepletionStage();

  while(pStage)
  {
  const IRockMaterialServerTempl* pServer = IMaterialServerTempl <IMaterialRock, PARENT> ::Parent().ConnectedMaterial(*pStage);
  if(pServer && pServer != this && pServer->LibraryMaterial() != 0 && pServer->LibraryMaterial() != IMaterialServerTempl <IMaterialRock, PARENT> ::LibraryMaterial())
      return true;

  if(pStage->Last())
      pStage = 0;
  else
      pStage = &pStage->Next();
  }

  return false;
}


#endif // _IROCKMATERIALSERVERTEMPL_H_
