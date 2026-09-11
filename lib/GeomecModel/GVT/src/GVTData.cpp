#include "GVTData.h"
#include "GVTSeismicGridDefinition.h"
#include "ModelBase.h"
#include "ResultRegister.h"
#include "ResultTree.h"
#include "MaterialResultTree.h"
#include "BaseEntryTypes.h"
#include "PointSet.h"
#include "MeshSampler.h"
#include "FieldFactor.h"
#include "ValueTypeFactory.h"
#include "NodalValueSet.h"
#include "MPKernel.h"
#include "ValueMapper.h"
#include "GlobalMessage.h"
#include "GlobalInitialStressNode.h"

#include <cassert>
#include <GVTVelocityModel.h>


namespace
{

CPointSet *FindPointSet(const CModelBase& model, const QString& name)
{
  TPointSetEntry::TNodeSet stPsNode = ((TPointSetEntry*)model.GraphEntry(MD_BASE_POINTSET))->EntryNodes();
  for (TPointSetEntry::TNodeSet::iterator it = stPsNode.begin(); it != stPsNode.end(); ++it)
  {
    CPointSet* pPointSet = dynamic_cast<CPointSet*>(*it);
    if (pPointSet && (pPointSet->Name() == name))
      return pPointSet;
  }

  return 0;
}

template<class VECTOR>
void SetupVectors(const CPillarMap& pillarMap, VECTOR& vector)
{
  vector.resize(pillarMap.size());

  size_t i = 0;
  for (CPillarMap::const_iterator it = pillarMap.begin(); it != pillarMap.end(); ++i, ++it)
  {
    vector[i].resize(it->second.size());
  }
}

}


CGVTDataTypes::TPillarValueVector *CGVTDataTypes::FindValueVector(TStageMap& map, const QString& name)
{
  std::string t = name.toStdString();
  QStringList nameParts = name.split('_');

  if (nameParts.size() < 1)
    return 0;

 
  QString depStage;

  if (nameParts[0] == "Vp")
  {
    if (nameParts.size() != 2)
      return 0;
    depStage = nameParts.last();
  }
  else
  {
    if (nameParts.size() < 3)
      return 0;
    depStage = nameParts[nameParts.size() - 2];
  }

  depStage.remove('D');

  int depIndex = depStage.toInt();

  
  CGVTDataTypes::TStageMap::iterator itMap = map.find(depIndex);

  if (itMap != map.end())
  {
    for (TValueMap::iterator itVM = itMap->second.begin(); itVM != itMap->second.end(); ++itVM)
    {
      if (itVM->second.first->ExportLabel() == name)
        return &itVM->second.second;
    }
  }

  return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CGVTDataInput
/////////////////////////////////////////////////////////////////////////////

class CTaskGatherInput
{
  std::vector<const CPillar *>& m_pillars;
  CGVTDataTypes::TStageMap& m_input;

  mp::CKernelDispatcher *m_dispatcher;

  bool m_bSequential;

public:
  CTaskGatherInput(std::vector<const CPillar *>& pillars, CGVTDataTypes::TStageMap& input)
    : m_pillars(pillars)
    , m_input(input)
    , m_dispatcher(0)
    , m_bSequential(false)
  {
  }

  MP_DUMMY_DATA

    size_t size() const { return m_pillars.size(); }

  void setDispatcher(mp::CKernelDispatcher *d) { m_dispatcher = d; }

  void setSequential() { m_bSequential = true; }

  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    geo::CValue v;

    for (CGVTDataTypes::TStageMap::const_iterator stageIt = m_input.begin(); stageIt != m_input.end(); ++stageIt)
    {
      for (CGVTDataTypes::TValueMap::const_iterator valuesIt = stageIt->second.begin(); valuesIt != stageIt->second.end(); ++valuesIt)
      {

        const IValueComponentBase *pValueComponent = valuesIt->second.first;

        geo::IParallelInitializationCallback *callback = 0;

        if (!m_bSequential && pValueComponent->NeedParallelInitializationCallback())
        {
          callback = const_cast<IValueComponentBase *>(pValueComponent)->GetParallelInitializationCallback();
          if (!callback)
            callback = const_cast<IValueComposite&>(pValueComponent->Parent()).GetParallelInitializationCallback();
          assert(callback);
        }

        for (size_t i = r.begin(); i != r.end(); ++i)
        {
          const CPillar *pillar = m_pillars[i];

          CGVTDataTypes::TPillarPointValueVector& values = const_cast<CGVTDataTypes::TPillarPointValueVector&>(valuesIt->second.second[i]);

          size_t j = 0;
          for (CPillar::const_iterator it = pillar->begin(); it != pillar->end(); ++j, ++it)
          {
            values[j] = pValueComponent->ScalarData().ValuePoint((*it).first, CDoubleQuantity::SI_UNIT, callback);
          }

        }

        delete callback;

      }

    }

    if (m_dispatcher)
      m_dispatcher->Step(r.size());
  }
};

void CGVTDataInput::Build(IProgressBase &progress, const CModelBase& model, const CPillarMap& pillarMap, const CGVTDataTypes::TFormationVelocityModels& formVelocityModels, const CGVTDataTypes::TTimeLapses& timelapses, bool bRebuild, bool bReinterpolate, CPointSet *input, bool& requireNewInput)
{
  progress.StatusMessage("Gathering input");

  if (bRebuild || bReinterpolate)
  {
    m_stageMap.clear();
  }

  if (m_stageMap.empty())
  {
    progress.AddSteps(pillarMap.size());

    for (CGVTDataTypes::TTimeLapses::const_iterator it = timelapses.begin(); it != timelapses.end(); ++it)
    {
      m_stageMap.insert(std::make_pair(it->first->Index(), CGVTDataTypes::TValueMap()));
      m_stageMap.insert(std::make_pair(it->second->Index(), CGVTDataTypes::TValueMap()));
    }

    Setup(model, pillarMap, formVelocityModels);

    bool bInitDone = false;

    if (input)
    {
      // sanity checks

      int size = 0;
      for (CPillarMap::const_iterator mapIt = pillarMap.begin(); mapIt != pillarMap.end(); ++mapIt)
      {
        size += mapIt->second.size();
      }

      bInitDone = size == input->PointSize();

      size = 0;
      if (bInitDone) // let's first check that we only have the properties we need
      {
        for (size_t v = 3; v < input->NodalValueSetSize(); ++v)
        {
          std::string t = input->NodalValueSet(v).Component()->Parent().Name().toStdString();
          CGVTDataTypes::TPillarValueVector *pvv = CGVTDataTypes::FindValueVector(m_stageMap, input->NodalValueSet(v).Component()->Parent().Name());

          if (!pvv)
          {
            bInitDone = false;
            break;
          }

          ++size;
        }
      }

      if (bInitDone) // and that we have all the properties (technically, we may not need to, as time-lapse set could be subset of input, but we don't check for that yet
      {
        for (CGVTDataTypes::TStageMap::iterator mapIt = m_stageMap.begin(); mapIt != m_stageMap.end(); ++mapIt)
        {
          size -= mapIt->second.size();
        }

        bInitDone = size == 0;
      }

      if (bInitDone)
      {
        for (size_t v = 3; v < input->NodalValueSetSize(); ++v)
        {
          std::string t = input->NodalValueSet(v).Component()->Parent().Name().toStdString();
          CGVTDataTypes::TPillarValueVector *pvv = CGVTDataTypes::FindValueVector(m_stageMap, input->NodalValueSet(v).Component()->Parent().Name());

          if (!pvv)
          {
            bInitDone = false;
            break;
          }

          size_t i = 0, j = 0;

          for (size_t p = 0; p < input->PointSize(); ++p)
          {
            if (input->NodalValueSet(v).Value(p).Valid())
              (*pvv)[i][j] = input->NodalValueSet(v).Value(p);
            ++j;

            if (j == (*pvv)[i].size())
            {
              ++i;
              j = 0;
            }
          }
        }
      }
    }

    if (bInitDone)
    {
      requireNewInput = false;
      return;
    }

    requireNewInput = true;

    std::vector<const CPillar *> pillars;
    pillars.reserve(pillarMap.size());

    for (CPillarMap::const_iterator it = pillarMap.begin(); it != pillarMap.end(); ++it)
    {
      pillars.push_back(&it->second);
    }

    typedef CTaskGatherInput CTaskCollect;

    CTaskCollect taskCollect(pillars, m_stageMap);

    mp::CKernelDispatcher kernelDispatcher;

    CValueMapper *vm = CValueMapper::instance();

    bool bAllowParallel = true;
    for (CGVTDataTypes::TStageMap::iterator stageIt = m_stageMap.begin(); stageIt != m_stageMap.end(); ++stageIt)
    {
      for (CGVTDataTypes::TValueMap::iterator it = stageIt->second.begin(); it != stageIt->second.end(); ++it)
        if (!vm->PrepareMapping(&model.Mesh().Mesh(), it->second.first))
          bAllowParallel = false;
    }

    if (bAllowParallel)
    {
      mp::CKernelParallel<CTaskCollect> parKernelCollect;
      mp::IDispatchedTask *disTaskCollect = NEW_DISPATCH_TASK(mp::CKernelParallel, CTaskCollect)(parKernelCollect, taskCollect);

      kernelDispatcher.launch(progress, disTaskCollect);
    }
    else
    {
      taskCollect.setSequential();

      mp::CKernelSequential<CTaskCollect> seqKernelCollect;
      mp::IDispatchedTask *disTaskCollect = NEW_DISPATCH_TASK(mp::CKernelSequential, CTaskCollect)(seqKernelCollect, taskCollect);

      kernelDispatcher.launch(progress, disTaskCollect);
    }

    vm->FinishMapping();

    if (kernelDispatcher.Canceled())
    {
      m_stageMap.clear();
    }
  }
}

int CGVTDataInput::ValueSize() const
{
  int nValueSize = 0;

  for (CGVTDataTypes::TStageMap::const_iterator stageIt = m_stageMap.begin(); stageIt != m_stageMap.end(); ++stageIt)
    nValueSize += stageIt->second.size();

  return nValueSize;
}

int CGVTDataInput::VpIndex(const CModelBase& model) const
{
  static int _index = -1;

  if (_index < 0)
  {
    for (int i = 0; i < model.MaterialResultTree().ChildSize(); ++i)
    {
      const CGraphNode *node = &model.MaterialResultTree().Child(i);

      if (dynamic_cast<const CMaterialResult *>(node))
      {
        if (static_cast<const CMaterialResult *>(node)->IconId() == IDI_VALUETYPE_VELOCITYP)
          _index = i;
      }
    }

    assert(_index >= 0);
  }

  return _index;
}

void CGVTDataInput::Setup(const CModelBase& model, const CPillarMap& pillarMap, const CGVTDataTypes::TFormationVelocityModels& formVelocityModels)
{
  // Setup velocity models
  SetupVectors(pillarMap, m_velocityVector);
  size_t i = 0;
  for (CPillarMap::const_iterator mapIt = pillarMap.begin(); mapIt != pillarMap.end(); ++mapIt, ++i)
  {
    size_t j = 0;
    for (CPillar::const_iterator pillarIt = mapIt->second.begin(); pillarIt != mapIt->second.end(); ++pillarIt, ++j)
    {
      const geo::IBody *body = (*pillarIt).second;
      CFormationBase *formationBase = const_cast<CFormationBase *>(model.Mesh().Formation(*body));

      m_velocityVector[i][j] = const_cast<CGVTDataTypes::TFormationVelocityModels&>(formVelocityModels)[formationBase];
    }
  }


  // Walk over timesteps
  const CDepletionStage* pStage = &model.InitialDepletionStage();

  CAnalysisType::TAnalysisType anType = model.ResultRegister().ResultsAvailable(CAnalysisType::AT_NONLIN) ? CAnalysisType::AT_NONLIN : CAnalysisType::AT_LINEAR;

  while (pStage)
  {
    CGVTDataTypes::TStageMap::iterator stageIt = m_stageMap.find(pStage->Index());

    if (stageIt != m_stageMap.end())
    {

      CGVTDataTypes::TValueMap& valueMap = stageIt->second;

      // we want the mean stress, vertical stress and vertical strain
      if (model.ResultTree().EffectiveStress().InvariantComposite()->Available(anType, *pStage))
      {
        std::pair<CGVTDataTypes::TValueMap::iterator, bool> retval = valueMap.insert(std::make_pair(CGVTDataTypes::MeanStress, std::make_pair(model.ResultTree().EffectiveStress().InvariantComposite()->ResultComponent(*pStage, anType, 0, IStressTensorGroup::CInvariantComposite::INV_I1), CGVTDataTypes::TPillarValueVector())));
        SetupVectors(pillarMap, retval.first->second.second);
        //m_TypeVec.push_back(IDT_VALUETYPE_PRESSURE);
        //m_DataVec.push_back(Model().ResultTree().EffectiveStress().InvariantComposite()->ResultComponent(*pStage, anType, 0, IStressTensorGroup::CInvariantComposite::INV_I1));
      }

      if (model.ResultTree().EffectiveStress().Components().Available(anType, *pStage))
      {
        std::pair<CGVTDataTypes::TValueMap::iterator, bool> retval = valueMap.insert(std::make_pair(CGVTDataTypes::StressZZ, std::make_pair(model.ResultTree().EffectiveStress().Components().ResultComponent(*pStage, anType, 0, ITensorGroup::CComponentComposite::TC_ZZ), CGVTDataTypes::TPillarValueVector())));
        SetupVectors(pillarMap, retval.first->second.second);
        //m_TypeVec.push_back(IDT_VALUETYPE_PRESSURE);
        //m_DataVec.push_back(Model().ResultTree().EffectiveStress().Components().ResultComponent(*pStage, anType, 0, ITensorGroup::CComponentComposite::TC_ZZ));
      }

      if (model.ResultTree().TotalStrain().Components().Available(anType, *pStage))
      {
        std::pair<CGVTDataTypes::TValueMap::iterator, bool> retval = valueMap.insert(std::make_pair(CGVTDataTypes::StrainZZ, std::make_pair(model.ResultTree().TotalStrain().Components().ResultComponent(*pStage, anType, 0, ITensorGroup::CComponentComposite::TC_ZZ), CGVTDataTypes::TPillarValueVector())));
        SetupVectors(pillarMap, retval.first->second.second);
        //m_TypeVec.push_back(IDT_VALUETYPE_STRAIN);
        //m_DataVec.push_back(Model().ResultTree().TotalStrain().Components().ResultComponent(*pStage, anType, 0, ITensorGroup::CComponentComposite::TC_ZZ));
      }

      {
        std::pair<CGVTDataTypes::TValueMap::iterator, bool> retval = valueMap.insert(std::make_pair(CGVTDataTypes::Velocity, std::make_pair(static_cast<const CMaterialResult *>(&model.MaterialResultTree().Child(VpIndex(model)))->ResultComponent(*pStage, anType), CGVTDataTypes::TPillarValueVector())));
        SetupVectors(pillarMap, retval.first->second.second);
      }

      if (model.ResultTree().Displacement().Available(anType, *pStage))
      {
        std::pair<CGVTDataTypes::TValueMap::iterator, bool> retval = valueMap.insert(std::make_pair(CGVTDataTypes::Displacement, std::make_pair(model.ResultTree().Displacement().ResultComponent(*pStage, anType, 0, IVectorResult::VC_Z), CGVTDataTypes::TPillarValueVector())));
        SetupVectors(pillarMap, retval.first->second.second);
        //m_TypeVec.push_back(IDT_VALUETYPE_STRAIN);
        //m_DataVec.push_back(Model().ResultTree().TotalStrain().Components().ResultComponent(*pStage, anType, 0, ITensorGroup::CComponentComposite::TC_ZZ));
      }
      //m_TypeVec.push_back(IDT_VALUETYPE_VELOCITYP);
      //m_DataVec.push_back(static_cast<CMaterialResult *>(&Model().MaterialResultTree().Child(63))->ResultComponent(*pStage, anType));
    }

    if (pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }
}

void CGVTDataInput::FillPointSet(IProgressBase& progress, const CPillarMap& pillarMap, CPointSet *pointSet)
{
  const CValueTypeFactory *factory = CValueTypeFactory::instance();

  int i = 0;
  for (CGVTDataTypes::TStageMap::const_iterator stageIt = m_stageMap.begin(); stageIt != m_stageMap.end(); ++stageIt)
  {
    for (CGVTDataTypes::TValueMap::const_iterator valuesIt = stageIt->second.begin(); valuesIt != stageIt->second.end(); ++valuesIt)
    {
      int type = -1;
      switch (valuesIt->first)
      {
      case CGVTDataTypes::MeanStress:
        type = IDT_VALUETYPE_MEANSTRESS;
        break;
      case CGVTDataTypes::StressZZ:
        type = IDT_VALUETYPE_NRMSTRESS;
        break;
      case CGVTDataTypes::StrainZZ:
        type = IDT_VALUETYPE_NRMSTRAIN;
        break;
      case CGVTDataTypes::Velocity:
        type = IDT_VALUETYPE_VELOCITYP;
        break;
      case CGVTDataTypes::Displacement:
        type = IDT_VALUETYPE_DISPLACEMENTZ;
        break;
      }
      if (type >= 0)
        pointSet->NodalValueSet(3 + i++).LinkTo(factory->BuildValueType(*pointSet, type, valuesIt->second.first->ExportLabel())->Component());
    }
  }


  std::vector<double> values(3 + ValueSize(), 0);

  i = 0;
  for (CPillarMap::const_iterator mapIt = pillarMap.begin(); mapIt != pillarMap.end(); ++mapIt, ++i)
  {
    int j = 0;
    for (CPillar::const_iterator pillarIt = mapIt->second.begin(); pillarIt != mapIt->second.end(); ++pillarIt, ++j)
    {
      values[0] = (*pillarIt).first.X();
      values[1] = (*pillarIt).first.Y();
      values[2] = (*pillarIt).first.Z();

      int k = 0;
      for (CGVTDataTypes::TStageMap::const_iterator stageIt = m_stageMap.begin(); stageIt != m_stageMap.end(); ++stageIt)
      {
        for (CGVTDataTypes::TValueMap::const_iterator valuesIt = stageIt->second.begin(); valuesIt != stageIt->second.end(); ++valuesIt)
        {
          values[3 + k++] = valuesIt->second.second[i][j].Value();
        }
      }

      pointSet->PushBack(values);
    }

    progress.Step();
  }
}

int CGVTDataInput::CalculationSize() const
{
  return m_stageMap.begin()->second.begin()->second.second.size();
}

CGVTDataTypes::TValueMap& CGVTDataInput::ValueMap(unsigned int key)
{
  return m_stageMap[key];
}

const CGVTDataTypes::TPillarVelocityModelVector& CGVTDataInput::VelocityModels() const
{
  return m_velocityVector;
}


/////////////////////////////////////////////////////////////////////////////
// CGVTDataOutput
/////////////////////////////////////////////////////////////////////////////

void CGVTDataOutput::Build(const CPillarMap& pillarMap, const CGVTDataTypes::TTimeLapses& timelapses, bool deltaVp, bool deltaT)
{
  m_stageMap.clear();

  for (CGVTDataTypes::TTimeLapses::const_iterator it = timelapses.begin(); it != timelapses.end(); ++it)
  {
    unsigned int lapse = (it->first->Index() << 16) + it->second->Index();
    m_stageMap.insert(std::make_pair(lapse, CGVTDataTypes::TValueMap()));
  }

  Setup(pillarMap, deltaVp, deltaT);
}

void CGVTDataOutput::Setup(const CPillarMap& pillarMap, bool /*deltaVP*/, bool /*deltaT*/)
{
  for (CGVTDataTypes::TStageMap::iterator stageIt = m_stageMap.begin(); stageIt != m_stageMap.end(); ++stageIt)
  {
    CGVTDataTypes::TValueMap& valueMap = stageIt->second;

    std::pair<CGVTDataTypes::TValueMap::iterator, bool> retval = valueMap.insert(std::make_pair(CGVTDataTypes::LENGTH, std::make_pair((const IResultComponent *)0, CGVTDataTypes::TPillarValueVector())));
    SetupVectors(pillarMap, retval.first->second.second);

    int i = 0;
    for (CPillarMap::const_iterator pillarMapIt = pillarMap.begin(); pillarMapIt != pillarMap.end(); ++pillarMapIt, ++i)
    {
      int j = 0;
      for (CPillar::const_iterator pillarIt = pillarMapIt->second.begin(); pillarIt != pillarMapIt->second.end(); ++pillarIt, ++j)
      {
        retval.first->second.second[i][j].Value((*pillarIt).first.Z());
      }
    }

    retval = valueMap.insert(std::make_pair(CGVTDataTypes::VSTRAIN, std::make_pair((const IResultComponent *)0, CGVTDataTypes::TPillarValueVector())));
    SetupVectors(pillarMap, retval.first->second.second);

    //retval = valueMap.insert(std::make_pair(CGVTDataTypes::TSTRAIN, std::make_pair((const IResultComponent *)0, CGVTDataTypes::TPillarValueVector())));
    //SetupVectors(pillarMap, retval.first->second.second);

#if 0
    if (deltaVp)
#else
    if (1)
#endif
    {
      retval = valueMap.insert(std::make_pair(CGVTDataTypes::TIMESHIFT, std::make_pair((const IResultComponent *)0, CGVTDataTypes::TPillarValueVector())));
      SetupVectors(pillarMap, retval.first->second.second);
    }
#if 0
    if (deltaT)
    {
#endif
      retval = valueMap.insert(std::make_pair(CGVTDataTypes::DELTAV, std::make_pair((const IResultComponent *)0, CGVTDataTypes::TPillarValueVector())));
      SetupVectors(pillarMap, retval.first->second.second);
#if 0
      retval = valueMap.insert(std::make_pair(5, std::make_pair((const IResultComponent *)0, TPillarVector())));
      SetupVectors(retval.first->second.second);
    }
#endif
  }
}

int CGVTDataOutput::ValueSize() const
{
  int nValueSize = 0;
  for (CGVTDataTypes::TStageMap::const_iterator stageIt = m_stageMap.begin(); stageIt != m_stageMap.end(); ++stageIt)
    nValueSize += stageIt->second.size() - 1;

  return nValueSize;
}

void CGVTDataOutput::FillPointSet(IProgressBase &progress, const CPillarMap& pillarMap, CPointSet *pointSet)
{
  const CValueTypeFactory *factory = CValueTypeFactory::instance();

  int i = 0;
  for (CGVTDataTypes::TStageMap::const_iterator stageIt = m_stageMap.begin(); stageIt != m_stageMap.end(); ++stageIt)
  {
    unsigned int lapse = stageIt->first;

    QString d = QString("_D%1_D%2").arg(lapse & 0xffff).arg(lapse >> 16);

    for (CGVTDataTypes::TValueMap::const_iterator valuesIt = stageIt->second.begin(); valuesIt != stageIt->second.end(); ++valuesIt)
    {
      int type = 0;
      QString s;
      switch (valuesIt->first) // if this is changed, also change the test in the next loop
      {
      case CGVTDataTypes::LENGTH:
        type = -1;
        break;
      case CGVTDataTypes::VSTRAIN:
        type = -1;
        s = "VStrain";
        break;
      case CGVTDataTypes::TSTRAIN:
        type = -1;
        s = "TStrain";
        break;
      case CGVTDataTypes::TIMESHIFT:
        type = IDT_VALUETYPE_DELTAT;
        s = "dt";
        break;
      case CGVTDataTypes::DELTAV:
        type = IDT_VALUETYPE_DELTAV;
        s = "dv";
        break;
      }
      if (type >= 0)
        pointSet->NodalValueSet(3 + i++).LinkTo(factory->BuildValueType(*pointSet, type, s + d)->Component());
    }
  }

  std::vector<double> values(3 + ValueSize(), 0);

  i = 0;
  for (CPillarMap::const_iterator mapIt = pillarMap.begin(); mapIt != pillarMap.end(); ++mapIt, ++i)
  {
    int j = 0;
    for (CPillar::const_iterator pillarIt = mapIt->second.begin(); pillarIt != mapIt->second.end(); ++pillarIt, ++j)
    {
      values[0] = (*pillarIt).first.X();
      values[1] = (*pillarIt).first.Y();
      values[2] = (*pillarIt).first.Z();

      int k = 0;
      for (CGVTDataTypes::TStageMap::const_iterator stageIt = m_stageMap.begin(); stageIt != m_stageMap.end(); ++stageIt)
      {
        for (CGVTDataTypes::TValueMap::const_iterator valuesIt = stageIt->second.begin(); valuesIt != stageIt->second.end(); ++valuesIt)
        {
          if (valuesIt->first == CGVTDataTypes::TIMESHIFT || valuesIt->first == CGVTDataTypes::DELTAV)
            values[3 + k++] = valuesIt->second.second[i][j].Value();
        }
      }

      pointSet->PushBack(values);
    }

    progress.Step();
  }
}

bool CGVTDataOutput::Empty() const
{
  return m_stageMap.empty() || m_stageMap.begin()->second.empty();
}

int CGVTDataOutput::CalculationSize() const
{
  return m_stageMap.begin()->second.begin()->second.second.size();
}

void CGVTDataOutput::GetKeys(std::vector<unsigned int>& keys)
{
  for (CGVTDataTypes::TStageMap::const_iterator it = m_stageMap.begin(); it != m_stageMap.end(); ++it)
  {
    keys.push_back(it->first);
  }
}

CGVTDataTypes::TValueMap& CGVTDataOutput::ValueMap(unsigned int key)
{
  return m_stageMap[key];
}



/////////////////////////////////////////////////////////////////////////////
// CGVTData
/////////////////////////////////////////////////////////////////////////////

CGVTData::CGVTData(CModelBase& model)
  : m_model(&model)
  , m_SamplePointSet(0)
  , m_AnalysisPointSet(0)
  , m_PillarMap(0)
  , m_PillarMapPointSet(0)
  , m_InputPointSet(0)
{
}

CModelBase& CGVTData::Model()
{
  return *m_model;
}


void CGVTData::Build(IProgressBase& progress, CGVTSeismicGridDefinition& gridDef, const CGVTDataTypes::TFormationVelocityModels& formVelocityModels, const CGVTDataTypes::TTimeLapses& timelapses, bool deltaVp, bool deltaT, bool bRebuild, bool bReinterpolate, bool bBuildInputPointSet)
{
  BuildSamplePointSet(progress, gridDef, bRebuild || bReinterpolate);
  /*
  BuildAnalysisPointSet(progress, gridDef, bRebuild, bReinterpolate);
  BuildVelocityPointSet(progress, gridDef);
  */
  BuildPillarMap(progress, gridDef, bRebuild || bReinterpolate);

  CPointSet *input = FindPointSet(Model(), gridDef.Name() + "_input");

  bool requireNewInput;
  m_Input.Build(progress, Model(), *m_PillarMap, formVelocityModels, timelapses, bRebuild, bReinterpolate, input, requireNewInput);
  m_Output.Build(*m_PillarMap, timelapses, deltaVp, deltaT);

  if (bBuildInputPointSet || !input || requireNewInput)
    BuildInputPointSet(progress, gridDef, bRebuild || bReinterpolate || !input || requireNewInput);
}


void CGVTData::BuildPillarMap(IProgressBase &progress, CGVTSeismicGridDefinition &gridDef, bool bRebuild)
{
  assert(m_SamplePointSet);

  progress.StatusMessage("Building pillar map");

  if (m_PillarMap && bRebuild)
  {
    delete m_PillarMap;
    m_PillarMap = 0;
  }
  // TODO: else convert input pointset (if we have it) to pillar map

  if (!m_PillarMap)
  {
    CPointsetSource source(*m_SamplePointSet, gridDef.Depth());
    CMeshSampler sampler(source, Model().Mesh());
    m_PillarMap = sampler.GeneratePillarMap("Pillar Map", Model(), &progress, true);
  }
}



void CGVTData::BuildInputPointSet(IProgressBase &progress, CGVTSeismicGridDefinition &gridDef, bool bRebuild)
{
  CPointSet *pPointSet = FindPointSet(Model(), gridDef.Name() + "_input");

  if (pPointSet)
  {
    if (!bRebuild)
    {
      return;
    }
    else
    {
      pPointSet->Destroy();
    }
  }

  progress.StatusMessage("Building input pointset");

  progress.AddSteps(m_PillarMap->size());

  int nValueSize = m_Input.ValueSize();

  m_InputPointSet = new CPointSet(gridDef.Name() + "_input", Model(), nValueSize, IPointSet::DIM_3D);

  m_Input.FillPointSet(progress, *m_PillarMap, m_InputPointSet);

  TPointSetEntry& psentry = (TPointSetEntry&)(*Model().GraphEntry(MD_BASE_POINTSET));
  psentry.ConnectItem(*m_InputPointSet);

  m_InputPointSet = 0;
}

void CGVTData::BuildOutputPointSet(IProgressBase &progress, CGVTSeismicGridDefinition &gridDef)
{
  if (m_Output.Empty())
    return;

  progress.StatusMessage("Building output pointset");
  progress.AddSteps(m_PillarMap->size());

  int nValueSize = m_Output.ValueSize();

  CPointSet *pointSet = new CPointSet(gridDef.Name() + "_output", Model(), nValueSize, IPointSet::DIM_3D);
  m_Output.FillPointSet(progress, *m_PillarMap, pointSet);

  TPointSetEntry& psentry = (TPointSetEntry&)(*Model().GraphEntry(MD_BASE_POINTSET));
  psentry.ConnectItem(*pointSet);
}






void CGVTData::BuildSamplePointSet(IProgressBase& progress, CGVTSeismicGridDefinition& gridDef, bool bRebuild)
{
  progress.StatusMessage("Building sample pointset");

  assert(gridDef.GridIsValid());
  
  // maybe the pointset already exists, use that
  CPointSet *pPointSet = FindPointSet(Model(), gridDef.Name());

  if (pPointSet)
  {
    if (!bRebuild)
    {
      m_SamplePointSet = pPointSet;
      return;
    }
    else
    {
      pPointSet->Destroy();
    }
  }

  m_SamplePointSet = new CPointSet(gridDef.Name().toStdString().c_str(), Model(), 0, CPointSet::DIM_3D);
  gridDef.GenerateGrid(*m_SamplePointSet);
}

void CGVTData::BuildAnalysisPointSet(IProgressBase &progress, CGVTSeismicGridDefinition& gridDef, bool bRebuild, bool bReinterpolate)
{
  assert(m_SamplePointSet);

  progress.StatusMessage("Building analysis pointset");

  //FillResultVector();
  QString pointSetName;
  pointSetName = QString("%1_analysis").arg(gridDef.Name());

  m_AnalysisPointSet = 0;
 
  if(!bRebuild)
  {
    // maybe the pointset already exists, use that
    TPointSetEntry::TNodeSet stPsNode = ((TPointSetEntry*)Model().GraphEntry(MD_BASE_POINTSET))->EntryNodes();
    TPointSetEntry::TNodeSet::iterator it;
    for(it = stPsNode.begin(); it != stPsNode.end(); ++it)
    {
      CPointSet* pPointSet = dynamic_cast<CPointSet*>(*it);
      if(pPointSet && (pPointSet->Name() == pointSetName))
      {
        if(bReinterpolate)
        {
          CopyPointSet(pointSetName, *pPointSet, progress);
          delete pPointSet;
        }
        else
        {// just use this pointset, it should still be good.
          m_AnalysisPointSet = pPointSet;
        }
        return;
      }
    }
  }
  ConstructNewPointSet(pointSetName, gridDef, progress);
}


void CGVTData::BuildVelocityPointSet(IProgressBase &progress, CGVTSeismicGridDefinition& gridDef)
{
  assert(m_SamplePointSet);

  progress.StatusMessage("Building velocity pointset");

  CPointsetSource source(*m_SamplePointSet, gridDef.Depth());
  CMeshSampler sampler(source, Model().Mesh());
  CPillarMap* pPillarMap = sampler.GeneratePillarMap("3D Velocity Pointset", Model(), &progress, true);

  const CDepletionStage& initialStage = Model().DepletionStageEntry()./*MarkedAs*/InitialStage();


  // retrieve velocities for the formations once
  std::map<const CFormationBase *, double> mpFormationVelocity;
  const TFormationBaseEntry* formationBaseEntry = static_cast<const TFormationBaseEntry*>(Model().GraphEntry(MD_BASE_FORMATION));

  for (TFormationBaseEntry::TNodeSet::const_iterator formationIt = formationBaseEntry->EntryNodes().begin(); formationIt != formationBaseEntry->EntryNodes().end(); ++formationIt)
  {
    const CMaterialServer& materialServer = (*formationIt)->Material(initialStage);
    const IMaterialRock* materialRock = materialServer.LibraryMaterial();

    if (materialRock)
    {
      const CLibraryMaterial& libraryMaterial = materialRock->LibraryMaterial();
      const CLibraryMaterialParameter* libraryMaterialParameter = libraryMaterial.ParameterByValueTypeID(IDT_VALUETYPE_VELOCITYP);

      mpFormationVelocity[(*formationIt)] = libraryMaterialParameter->Value();
    }
  }


  const int nValueSize = 1;

  // iterate over the pillar map, same way as grid was defined, so we have the bin/track number immediately
  CPointSet *pPointSet = new CPointSet(gridDef.Name() + "_velocities", Model(), nValueSize, IPointSet::DIM_3D);
  const CValueTypeFactory *factory = CValueTypeFactory::instance();
  pPointSet->NodalValueSet(nValueSize + 3 - 1).LinkTo(factory->BuildValueType(*pPointSet, IDT_VALUETYPE_VELOCITYP, "Vp_D0")->Component());

  std::vector<geo::CValue> vcRow(nValueSize + 3);

  std::stringstream buffer;

  for (CPillarMap::const_iterator mapIt = pPillarMap->begin(); mapIt != pPillarMap->end(); ++mapIt)
  {
    CPillarPos& pos = mapIt->first;
    CPillar& pillar = mapIt->second;

    const CFormationBase *prevFormation = 0;

    double extraTimeSingle = 0;

    int nBin, nTrack;
    if (!gridDef.FindBinTrack(pos.X(), pos.Y(), nBin, nTrack))
      continue;

    //if (CalculatorType() == EXTERNAL)
    //  AddVelocityFileCardHeader(buffer, pairPos, nBin, nTrack);

    for (CPillar::const_iterator pillarIt = pillar.begin(); pillarIt != pillar.end(); ++pillarIt)
    {
      CPillar::CPillarPoint pillarPt = (*pillarIt).first;
      const geo::IBody *body = (*pillarIt).second;

      if (!body)
        continue;

      const CFormationBase *formation = Model().Mesh().Formation(*body);

      double Vp = mpFormationVelocity[formation];
      double depth = pillarPt.Z();

      if (formation != prevFormation)
      {
        if (!prevFormation)
        {
          if (depth > 0)
          {
            extraTimeSingle = depth / 1500;
          }
        }

        prevFormation = formation;
      }

      //if (CalculatorType() == EXTERNAL)
      //{
      //  AddVelocityFileCardPair(buffer, pairPos, Vp, totalTimeSingle);
      //}
      //else
      //{
        vcRow[0] = pillarPt.X();
        vcRow[1] = pillarPt.Y();
        vcRow[2] = pillarPt.Z();
        vcRow[3] = Vp;
        pPointSet->PushBack(vcRow);
      //}
    }
  }

  //if (CalculatorType() == EXTERNAL)
  //{
  //  m_GVTVelocityModelFile.SetContents(buffer);
  //}
  //else
  //{
    TPointSetEntry& psentry = (TPointSetEntry&)(*Model().GraphEntry(MD_BASE_POINTSET));
    psentry.ConnectItem(*pPointSet);
  //}

}


#if 0
void CGVTData::AddVelocityFileCardHeader(std::stringstream& buffer, int& number, int nBin, int nTrack)
{
  // format accessible via the geomec help
  char str[49];
  memset(str, ' ', 48);
  str[36] = '0'; // ms
  str[42] = '1'; // ft
  str[48] = 0;

  strncpy(str, "VEL3D", 5);

  QString tmp = QString::number(nBin);
  strncpy(str + 6, tmp.toStdString().c_str(), tmp.size());
  tmp = QString::number(nTrack);
  strncpy(str + 12, tmp.toStdString().c_str(), tmp.size());

  if (number > 0) // previous card wasn't full
    buffer << std::endl;

  buffer << str << std::endl;

  number = 0;
}

void CGVTData::AddVelocityFileCardPair(std::stringstream& buffer, int& number, double velocity, double time)
{
  ++number;

  buffer << " " << velocity * FF_FACTOR_LENGTH * 1000 /* ms */ << " " << time * 1000 /* ms */ * 2 /* two-way */;

  if (number == 6)
  {
    buffer << std::endl;
    number = 0;
  }
}
#endif


void CGVTData::AddPoint(const geo::IPoint &point)
{
  assert(m_AnalysisPointSet);
  std::vector<geo::CValue> vcPointData(3);
  vcPointData[0] = point.X();
  vcPointData[1] = point.Y();
  vcPointData[2] = point.Z();
  m_AnalysisPointSet->PushBack(vcPointData);
}

void CGVTData::ConstructNewPointSet(const QString &name, const CGVTSeismicGridDefinition &gridDef, IProgressBase &progress)
{
  assert(m_SamplePointSet);
  
  CPointsetSource pointsetsource(*m_SamplePointSet, gridDef.Depth());
  CMeshSampler sampler(pointsetsource, Model().Mesh());
  progress.StatusMessage("Constructing analysis pointset (sampling elements)");
  m_AnalysisPointSet = sampler.GeneratePointset(name.toStdString().c_str(), Model(), &progress, true);
/*
  m_AnalysisPointSet = new CPointSet(name, Model(), 0, CPointSet::DIM_3D);

  double maxDepth = gridDef.Depth();
  if(maxDepth < max.Z() && maxDepth != 0.0)
    max.Z(maxDepth);
  geo::CCoordinateSet<geo::CPoint> intersects;
  for(int i = 0; i < m_SamplePointSet->PointSize(); ++i)
  {
    const geo::IPoint &samplePoint = m_SamplePointSet->PointAt(i);
    (const_cast<geo::IPoint&>(samplePoint)).Z(dZmin);
    if(samplePoint.X() > mesh.Max().X() ||
       samplePoint.Y() > mesh.Max().Y() ||
       samplePoint.X() < mesh.Min().X() ||
       samplePoint.Y() < mesh.Min().Y())
    {
      intersects.insert(samplePoint);
    }
    else
    {
      max.X(samplePoint.X());
      max.Y(samplePoint.Y());
      geo::CLine line(samplePoint, max);
      std::set<int> elements = mesh.ElementSet().Candidates(samplePoint, max);
     
      std::set<int>::iterator it;
   
      for(it = elements.begin(); it != elements.end(); ++it)
      {
     
        const geo::IBody *el = dynamic_cast<const geo::IBody*>(&mesh.ElementSet().Element(*it));
        if(el)
        {
          std::set<geo::CPoint> temp;
          el->Intersection(line, temp);
          std::set<geo::CPoint>::iterator iter;
          for(iter = temp.begin(); iter != temp.end(); ++iter)
          {
            intersects.insert(*iter);
          }
        }
      }
    }

    
    progress.Step();
  }
  
  geo::CCoordinateSet<geo::CPoint>::iterator pointIt;
  for(pointIt = intersects.begin(); pointIt != intersects.end(); ++pointIt)
  {
    AddPoint(*pointIt);
  }
*/
}

void CGVTData::CopyPointSet(const QString &name, const CPointSet &ptSet, IProgressBase &progress)
{
  m_AnalysisPointSet = new CPointSet(name.toStdString().c_str(), Model(), 0, CPointSet::DIM_3D);
  progress.AddSteps(ptSet.PointSize());
  QString status = QString("Using pointset ")+ ptSet.Name() + QString(", interpolating values");;
  progress.StatusMessage(status);
  for(int i = 0; i < ptSet.PointSize(); ++i)
  {
    AddPoint(ptSet.PointAt(i));
    progress.Step();
  }
}




CGVTDataInput& CGVTData::Input()
{
  return m_Input;
}

CGVTDataOutput& CGVTData::Output()
{
  return m_Output;
}


double CGVTData::WaterSurfaceDepth() const
{
  const CLengthQuantity& depth = m_model->GlobalInitialStress().WaterSurfaceDepth();

  if (depth.Undefined())
    return 0;

  return depth.Value();
}