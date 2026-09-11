#ifndef _GVT_DATA_H_
#define _GVT_DATA_H_

#include <map>
#include <set>
#include <sstream>
#include <vector>

namespace geo {
class IPoint;
class CValue;
} // namespace geo

class CModelBase;
class CPointSet;
class CPillarMap;
class IValueComponentBase;
class IProgressBase;
class CGVTSeismicGridDefinition;
class CFormationBase;
class CDepletionStage;
class CGVTVelocityModel;
class IResultComponent;

class QString;

class CGVTDataTypes {
public:
  typedef enum { MeanStress = 0, StressZZ, StrainZZ, Velocity, Displacement } INPUT;

  typedef enum {
    LENGTH = 0,
    VSTRAIN,
    TSTRAIN,
    TIMESHIFT,
    DELTAV,
  } OUTPUT;

  typedef std::map<CFormationBase *, CGVTVelocityModel *> TFormationVelocityModels;
  typedef std::vector<std::pair<CDepletionStage *, CDepletionStage *>> TTimeLapses;

  typedef std::vector<geo::CValue> TPillarPointValueVector;
  typedef std::vector<TPillarPointValueVector> TPillarValueVector;
  // typedef std::vector<TPillarValueVector> TPillarMapValueVector;

  typedef std::vector<CGVTVelocityModel *> TPillarPointVelocityModelVector;
  typedef std::vector<TPillarPointVelocityModelVector> TPillarVelocityModelVector;
  // typedef std::vector<TPillarFormationVector> TPillarMapFormationVector;

  typedef std::pair<const IResultComponent *, TPillarValueVector> TValuePair;
  typedef std::map<unsigned int, TValuePair> TValueMap;
  typedef std::map<unsigned int, TValueMap> TStageMap;

  static TPillarValueVector *FindValueVector(TStageMap &map, const QString &name);
};

class CGVTDataInput {
  CGVTDataTypes::TStageMap m_stageMap;
  CGVTDataTypes::TPillarVelocityModelVector m_velocityVector;

public:
  void Build(IProgressBase &progress, const CModelBase &model, const CPillarMap &pillarMap,
             const CGVTDataTypes::TFormationVelocityModels &formVelocityModels,
             const CGVTDataTypes::TTimeLapses &timelapses, bool bRebuild, bool bReinterpolate, CPointSet *input,
             bool &requireNewInput);

  int ValueSize() const;
  void FillPointSet(IProgressBase &progress, const CPillarMap &pillarMap, CPointSet *pointSet);

  int CalculationSize() const;
  CGVTDataTypes::TValueMap &ValueMap(unsigned int key);
  const CGVTDataTypes::TPillarVelocityModelVector &VelocityModels() const;

private:
  int VpIndex(const CModelBase &model) const;

  void Setup(const CModelBase &model, const CPillarMap &pillarMap,
             const CGVTDataTypes::TFormationVelocityModels &formVelocityModels);
};

class CGVTDataOutput {
  CGVTDataTypes::TStageMap m_stageMap;

public:
  void Build(const CPillarMap &pillarMap, const CGVTDataTypes::TTimeLapses &timelapses, bool deltaVp, bool deltaT);

  int ValueSize() const;
  void FillPointSet(IProgressBase &progress, const CPillarMap &pillarMap, CPointSet *pointSet);

  bool Empty() const;
  int CalculationSize() const;
  CGVTDataTypes::TValueMap &ValueMap(unsigned int key);

  void GetKeys(std::vector<unsigned int> &keys);

private:
  void Setup(const CPillarMap &pillarMap, bool deltaVp, bool deltaT);
};

class CGVTData {
  CModelBase *m_model;

  CModelBase &Model();

  CPointSet *m_SamplePointSet;

  void BuildSamplePointSet(IProgressBase &progress, CGVTSeismicGridDefinition &gridDef, bool bRebuild);

  // OLD STUFF

  CPointSet *m_AnalysisPointSet;

  std::vector<unsigned int> m_TypeVec;
  std::vector<const IValueComponentBase *> m_DataVec;

  void BuildAnalysisPointSet(IProgressBase &progress, CGVTSeismicGridDefinition &gridDef, bool bRebuild,
                             bool bReinterpolate);
  void BuildVelocityPointSet(IProgressBase &progress, CGVTSeismicGridDefinition &gridDef);

  // void AddVelocityFileCardHeader(std::stringstream& buffer, int& number, int nBin, int nTrack);
  // void AddVelocityFileCardPair(std::stringstream& buffer, int& number, double velocity, double time);

  // void FillResultVector();
  void ConstructNewPointSet(const QString &name, const CGVTSeismicGridDefinition &gridDef, IProgressBase &progress);
  void CopyPointSet(const QString &name, const CPointSet &ptSet, IProgressBase &progress);
  void AddPoint(const geo::IPoint &point);

  // NEW STUFF
  CPillarMap *m_PillarMap;
  CPointSet *m_PillarMapPointSet;
  CPointSet *m_InputPointSet;

public:
private:
  CGVTDataInput m_Input;
  CGVTDataOutput m_Output;

  void BuildPillarMap(IProgressBase &progress, CGVTSeismicGridDefinition &gridDef, bool bRebuild);
  void BuildInputPointSet(IProgressBase &progress, CGVTSeismicGridDefinition &gridDef, bool bRebuild);

public:
  CGVTData(CModelBase &model);

  void Build(IProgressBase &progress, CGVTSeismicGridDefinition &gridDef,
             const CGVTDataTypes::TFormationVelocityModels &formVelocityModels,
             const CGVTDataTypes::TTimeLapses &timelapses, bool deltaVp, bool deltaT, bool bRebuild,
             bool bReinterpolate, bool bBuildInputPointSet = true);

  const std::vector<const IValueComponentBase *> &DataVec() const { return m_DataVec; }
  const CPointSet *AnalysisPointSet() const { return m_AnalysisPointSet; }

  CGVTDataInput &Input();
  CGVTDataOutput &Output();

  double WaterSurfaceDepth() const;

  void BuildOutputPointSet(IProgressBase &progress, CGVTSeismicGridDefinition &gridDef);
};

#endif