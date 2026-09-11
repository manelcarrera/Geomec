#ifndef _cora_FailureTypeParameterBase_h_
#define _cora_FailureTypeParameterBase_h_

#include "safeQSharedPointer.h"


namespace cora
{

class CMapSingleValue2ElementValues;
class CMapInterfaceElement2ElementValues;

} // namespace cora

#include "Parameter.h"
#include "ParameterModifier.h"
#include "Object.h"
#include "FemAppEntryTypes.h"
#include "ModelBase.h"
#include "ValueTypeFactory.h"
#include "ElementValueSet.h"
#include "SummaryResultFile.h"

namespace cora
{

class CFailureTypeParameterBase
{
  public:
  CFailureTypeParameterBase(CSummaryResultFile& summaryResultFile,
      const std::string& object, const std::string& option,
      const std::string& parameter, std::vector <double>& value,
      const TObjects& objects, const QString& objectType);
  virtual ~CFailureTypeParameterBase() = 0;

  virtual void modify(CModelBase* modelBase);

  const TParameter getActualParameter() const;

  static TObject findActualObject(CSummaryResultFile& summaryResultFile,
      const std::string& object, const TObjects& objects,
      const QString& objectType);
  static TParameter findActualParameter(CSummaryResultFile& summaryResultFile,
      const std::string& object, const std::string& parameter,
      const TParameters& parameters);

  protected:
  template <typename V>
      void removeValueType(CModelBase* modelBase, const V& valueNode,
    unsigned int valueTypeID) const;
  template <typename V>
      void createAndLinkValueType(CModelBase* modelBase,
    CElementValueSet& elementValueSet, V& valueNode, IPointSet& pointSet,
    unsigned int valueTypeID) const;

  friend class CMapSingleValue2ElementValues;
  friend class CMapInterfaceElement2ElementValues;

  TObject m_actualObject;
  TParameter m_actualParameter;
  QString m_object;
  QString m_parameter;
  TParameterModifier m_parameterModifier;
  CSummaryResultFile& m_summaryResultFile;
  const QString& m_objectType;

  private:
  CFailureTypeParameterBase(const CFailureTypeParameterBase& rhs);
  CFailureTypeParameterBase& operator = (
      const CFailureTypeParameterBase& rhs);
};

typedef QSharedPointer <CFailureTypeParameterBase> TFailureTypeParameterBase;

template <typename V>
  void CFailureTypeParameterBase::removeValueType(CModelBase* modelBase,
  const V& valueNode, unsigned int valueTypeID) const
{
  TValueCompositeEntry* valueCompositeEntry =
  dynamic_cast <TValueCompositeEntry*> (
      modelBase->GraphEntry(MD_BASE_VALUE_COMPOSITE));

  assert(valueCompositeEntry);

  // TODO investigate dependencies (parents) to resolve ordering.
  // TSortedNodeSet cannot be used due to a Less(...) operator that results in
  // conflicting values ((A < B) && (B < A)).

  TValueCompositeEntry::TNodeSet
  entryNodes = valueCompositeEntry->EntryNodes();
  TValueCompositeEntry::TNodeSet::iterator node = entryNodes.begin();

  while (node != entryNodes.end())
  {
  IValueComposite* valueComposite = *(node++);

  if (valueComposite->TypeId() == valueTypeID)
  {
      if (valueNode.IsLinkedTo(*valueComposite) &&
    valueComposite->IsLinkedTo(valueNode))
      {
    entryNodes.erase(--node);
    delete valueComposite;
    node = entryNodes.begin();
      }
  }
  }
}

namespace
{

const QString READ_ONLY_MATERIAL = "The material is read-only";

} // anonymous namespace

template <typename V>
  void CFailureTypeParameterBase::createAndLinkValueType(CModelBase* modelBase,
  CElementValueSet& elementValueSet, V& valueNode, IPointSet& pointSet,
  unsigned int valueTypeID) const
{
  const CValueTypeFactory *valueTypeFactory = CValueTypeFactory::instance();
 
  if (valueTypeFactory->ValueTypeAvailable(valueTypeID))
  {
  unsigned int valueTypeName = valueTypeFactory->NameIndex(valueTypeID);
  CValueType* valueType = valueTypeFactory->BuildValueType(
      pointSet, valueTypeID, valueTypeName);

  assert(valueType);

  removeValueType(modelBase, valueNode, valueTypeID);

  valueType->MapType(CValueType::MT_NONE);
  elementValueSet.LinkTo(valueType->Component());

  if (valueNode.CanConnectItem(*valueType))
  {
      valueNode.ConnectItem(*valueType);
  }
  else
  {
      m_summaryResultFile.
    setResultValue(CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
      m_summaryResultFile.addAdditionalInformation(READ_ONLY_MATERIAL);
  }
  }
}

} // namespace cora

#endif  // _cora_FailureTypeParameterBase_h_
