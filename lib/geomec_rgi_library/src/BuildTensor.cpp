
#include <vector>

#include "BuildTensor.h"
#include "ModelBase.h"
#include "RGInterface.h"
#include "ElementValueSet.h"
#include "StrainLoad.h"
#include "RockMechProcessor.h"
#include "RGUtils.h"


namespace
{

const int NULL_TENSOR_COMPONENT = -1;
const size_t TENSOR_COMPONENTS_COUNT = 6;
std::vector <int> g_residualStrainSet(TENSOR_COMPONENTS_COUNT);
TStrainTensor* g_tensor = 0;

CElementValueSet& getResidualStrainSet(int& residualStrainSet,
  CModelBase& modelBase)
{
  if (residualStrainSet == NULL_TENSOR_COMPONENT)
  {
    residualStrainSet = modelBase.Mesh().AddElementValueSet();
  }

  return modelBase.Mesh().ElementValueSet(residualStrainSet);
}

void loadValues(CElementValueSet& elementValueSet, RGInterface& rgi,
  const RGProperty& rgProperty, CModelBase& modelBase,
  int /*residualStrainSet*/)
{
  std::vector <double> values;

  rgi.loadProperty(rgProperty, values);

  for(int i = 0; i < values.size(); ++i)
  {
    const geo::IElement& element = modelBase.Mesh().Mesh().Element(i);
    int numberOfNodes = element.NrOfNodes();
    geo::CValue v;
    if (!RGUtils::isNull(values[i]))
      v = values[i];
    std::vector <geo::CValue> nodalValues(numberOfNodes, v);

    elementValueSet.PushBack(nodalValues);
  }
}

const QString TENSOR_ALREADY_CONNECTED_2_STRAIN_LOAD =
  "A tensor is already connected to the strain load, "
  "not attempting to connect again";

void connectStrainLoad2Tensor(CModelBase& modelBase, RGInterface& rgi,
  GeomecRGI::CRockMechProcessor& rmp)
{
  CDepletionStage& depletionStage = modelBase.DepletionStageEntry().
    StageByIndex(rgi.getCurrentDepletionStage().getDepletionStage());
	TFormationBaseEntry* formationBaseEntry =
    dynamic_cast <TFormationBaseEntry*> (
      modelBase.GraphEntry(MD_BASE_FORMATION));

	assert(formationBaseEntry != 0);

  for (TFormationBaseEntry::TNodeSet::iterator formation =
    formationBaseEntry->EntryNodes().begin();
    formation != formationBaseEntry->EntryNodes().end(); ++formation)
  {
    CStrainLoad& strainLoad = (**formation).Strain(depletionStage);

    if (strainLoad.Links <TStrainTensor> ().size() == 0)
    {
      strainLoad.ConnectItem(*g_tensor);
    }
    else
    {
      QString message = TENSOR_ALREADY_CONNECTED_2_STRAIN_LOAD;

      rmp.AddLogLine(message, &rgi, false, false);
    }
  }
}

} // anonymous namespace

namespace GeomecRGI
{

// CBuildTensorImpl

class CBuildTensor::CBuildTensorImpl
{
  public:
    CBuildTensorImpl();
    ~CBuildTensorImpl();

    bool loadProperty(RGInterface& rgi, CModelBase& modelBase,
      CRockMechProcessor& rmp, const RGProperty& rgProperty,
      const ITensorGroup::CComponentComposite::TENSOR_COMPONENT&
        tensorComponent);

    bool isTensorComplete() const;

  private:
    CBuildTensorImpl(const CBuildTensorImpl& rhs);
    CBuildTensorImpl& operator = (const CBuildTensorImpl& rhs);
};

CBuildTensor::CBuildTensorImpl::CBuildTensorImpl()
{
  std::fill(g_residualStrainSet.begin(), g_residualStrainSet.end(), NULL_TENSOR_COMPONENT);
}

CBuildTensor::CBuildTensorImpl::~CBuildTensorImpl()
{
  // should the Tensor be deleted?

  g_tensor = 0;
}

bool CBuildTensor::CBuildTensorImpl::loadProperty(RGInterface& rgi,
  CModelBase& modelBase, CRockMechProcessor& rmp, const RGProperty& rgProperty,
  const ITensorGroup::CComponentComposite::TENSOR_COMPONENT& tensorComponent)
{
  CElementValueSet& elementValueSet =
    getResidualStrainSet(g_residualStrainSet[tensorComponent], modelBase);

  loadValues(elementValueSet, rgi, rgProperty, modelBase,
    g_residualStrainSet[tensorComponent]);

  QStringList l = QString(rgProperty.toString().c_str()).split("_");

  assert(l.size() == 2);

  if (g_tensor == 0)
  {
    g_tensor = new TStrainTensor(elementValueSet.PointSet(), l[0]);
  }

  IValueComponentBase& valueComponentBase =
    g_tensor->Component(tensorComponent);

  elementValueSet.Name(l[1]);

  if (!elementValueSet.IsLinkedTo(valueComponentBase))
  {
    elementValueSet.LinkTo(valueComponentBase);
  }

  if (isTensorComplete())
  {
    connectStrainLoad2Tensor(modelBase, rgi, rmp);
  }

  return true;
}

bool CBuildTensor::CBuildTensorImpl::isTensorComplete() const
{
  return (std::find(g_residualStrainSet.begin(), g_residualStrainSet.end(),
    NULL_TENSOR_COMPONENT) == g_residualStrainSet.end());
}

// CBuildTensor

namespace
{

QSharedPointer <CBuildTensor::CBuildTensorImpl> g_tensorImpl;
size_t g_tensorImplCount = 0;

QSharedPointer <CBuildTensor::CBuildTensorImpl>
  constructTensor()
{
  if (g_tensorImpl.isNull())
  {
    g_tensorImpl =
      QSharedPointer <CBuildTensor::CBuildTensorImpl> (
        new CBuildTensor::CBuildTensorImpl());
  }

  ++g_tensorImplCount;

  return g_tensorImpl;
}

void destructTensor()
{
  if (--g_tensorImplCount == 0)
  {
    g_tensorImpl.clear();
  }
}

} // anonymous namespace

CBuildTensor::CBuildTensor()
: m_Tensor(constructTensor())
{
}

CBuildTensor::~CBuildTensor()
{
  destructTensor();
}

bool CBuildTensor::loadProperty(RGInterface& rgi, CModelBase& modelBase,
  CRockMechProcessor& rmp, const RGProperty& rgProperty,
  const ITensorGroup::CComponentComposite::TENSOR_COMPONENT& tensorComponent)
{
  return m_Tensor->loadProperty(rgi, modelBase, rmp, rgProperty,
    tensorComponent);
}

bool CBuildTensor::isTensorComplete() const
{
  return m_Tensor->isTensorComplete();
}

} // namespace GeomecRGI
