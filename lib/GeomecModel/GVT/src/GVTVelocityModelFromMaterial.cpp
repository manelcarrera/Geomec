#include <fstream>
#include <stdexcept>

#include "GVTVelocityModelFromMaterial.h"
#include "FormationBase.h"
#include "BoundaryBase.h"
#include "GeomecUtils.h"

namespace GVT
{

CGVTVelocityModelFromMaterial::CGVTVelocityModelFromMaterial(
  CModelBase& /*modelBase*/)
: CGVTVelocityModelSource()
, m_velocityModelFile(GetGeomecTempPathExt(CTempPath::TEMP_GENERAL) + tmpnam(NULL))
{
}

CGVTVelocityModelFromMaterial::~CGVTVelocityModelFromMaterial()
{
  if (!m_velocityModelFile.isEmpty() && FileExists(m_velocityModelFile))
  {
    remove(m_velocityModelFile.toStdString().c_str());
  }
}


void CGVTVelocityModelFromMaterial::SetContents(const std::stringstream& buffer)
{
  std::fstream velocityModelFile(m_velocityModelFile.toStdString().c_str(),
    std::ios_base::out);

  velocityModelFile << buffer.str();

  velocityModelFile.close();
}


const QString& CGVTVelocityModelFromMaterial::velocityModelFile() const
{
  return m_velocityModelFile;
}

void CGVTVelocityModelFromMaterial::velocityModelFile(
  const QString& /*velocityModelFile*/)
{
  // the member 'm_velocityModelFile' is truly constant and initialized at
  // construction time
}

bool CGVTVelocityModelFromMaterial::isVelocityModelFileReady() const
{
  return true;
}

int CGVTVelocityModelFromMaterial::velocityModelSourceNumerator() const
{
  return VELOCITY_FROM_MATERIAL;
}

void CGVTVelocityModelFromMaterial::LoadStream(TSTREAM& /*stream*/,
  CStreamVersion& /*version*/, TPROGRESS& /*progress*/)
{
}

void CGVTVelocityModelFromMaterial::SaveStream(TSTREAM& stream,
  TPROGRESS& /*progress*/)
{
  stream << VELOCITY_FROM_MATERIAL;
}

#if 0
// private

namespace
{

typedef std::pair <double, CFormationBase*> TTopFormation;

bool compareTopFormations(const TTopFormation& lhs, const TTopFormation& rhs)
{
  return (lhs.first < rhs.first);
}

typedef std::pair <double, double> TTopBottom;

bool compareTopBottoms(const TTopBottom& lhs, const TTopBottom& rhs)
{
  return (lhs.first < rhs.first);
}

CFormationBase* getTopFormation(const CModelBase& modelBase)
{
  const TFormationBaseEntry* formationBaseEntry =
    dynamic_cast <const TFormationBaseEntry*> (
      modelBase.GraphEntry(MD_BASE_FORMATION));

  std::vector <TTopFormation> topFormations;

  for (TFormationBaseEntry::TNodeSet::const_iterator formation =
    formationBaseEntry->EntryNodes().begin();
    formation != formationBaseEntry->EntryNodes().end(); ++formation)
  {
    topFormations.push_back(
      std::make_pair((*formation)->Min().Z(), (*formation)));
  }

  std::sort(topFormations.begin(), topFormations.end(), compareTopFormations);

  return (topFormations.begin())->second;
}

bool areFormationsConsecutive(CModelBase& modelBase)
{
  const TFormationBaseEntry* formationBaseEntry =
    dynamic_cast <const TFormationBaseEntry*> (
      modelBase.GraphEntry(MD_BASE_FORMATION));

  std::vector <TTopBottom> topBottoms;

  for (TFormationBaseEntry::TNodeSet::const_iterator formation =
    formationBaseEntry->EntryNodes().begin();
    formation != formationBaseEntry->EntryNodes().end(); ++formation)
  {
    topBottoms.push_back(
      std::make_pair((*formation)->Min().Z(), (*formation)->Max().Z()));
  }

  std::sort(topBottoms.begin(), topBottoms.end(), compareTopBottoms);

  for (std::vector <TTopBottom> ::const_iterator topBottom = topBottoms.begin();
    topBottom != topBottoms.end(); ++topBottom)
  {
    std::vector <TTopBottom> ::const_iterator next = (topBottom + 1);

    if (next != topBottoms.end())
    {
      if ((*topBottom).second != (*next).first)
      {
        return false;
      }
    }
  }

  return true;
}

typedef std::pair <double, double> TDistanceTime;
typedef std::vector <TDistanceTime> TDistanceTimeVector;

const double ZERO = 0.0;
const double VELOCITY_IN_WATER = 1500.0;  // 1500 m/s

const size_t SECONDS_2_MILLI_SECONDS_FACTOR = 1000;
const size_t TWO_WAY = 2;

// originally 3.280839895013123 (1/0.3048, from GVT)
// incremented the constant to prevent rounding and range errors

const double METERS_2_FEET_FACTOR = 3.280839895013124;

void addCalculatedDistanceTimeInstance(TDistanceTimeVector& distanceTime,
  double distance, double velocity)
{
  double time =
      (distance / velocity) * SECONDS_2_MILLI_SECONDS_FACTOR * TWO_WAY;  // ms

  distanceTime.push_back(std::make_pair(distance * METERS_2_FEET_FACTOR, time));
}

void addInitialDistanceTimeInstances(TDistanceTimeVector& distanceTime,
  const CModelBase& modelBase)
{
  // add the first TDistanceTime for the sea-level

  distanceTime.push_back(std::make_pair(ZERO, ZERO));

  CFormationBase* topFormation = getTopFormation(modelBase);

  if (topFormation->Min().Z() > ZERO)
  {
    // the assumption is (as stated in TFS 98644) that when the top formation
    // has a positive depth the subsidence consists of (sea-) water

    addCalculatedDistanceTimeInstance(distanceTime, topFormation->Min().Z(),
      VELOCITY_IN_WATER);
  }
}

void addFormationDistanceTimeInstances(TDistanceTimeVector& distanceTime,
  const CModelBase& modelBase)
{
  const TFormationBaseEntry* formationBaseEntry =
    dynamic_cast <const TFormationBaseEntry*> (
      modelBase.GraphEntry(MD_BASE_FORMATION));

  for (TFormationBaseEntry::TNodeSet::const_iterator formation =
    formationBaseEntry->EntryNodes().begin();
    formation != formationBaseEntry->EntryNodes().end(); ++formation)
  {
    // CGVTSettingsDlg::allMaterialModelsHaveVelocityP() returned true, implying
    // ((*formation)->ConnectedMaterial(modelBase.InitialDepletionStage()) != 0)

    CMaterialServer& materialServer =
      (*formation)->Material(modelBase.InitialDepletionStage());
    IMaterialRock* materialRock = materialServer.LibraryMaterial();

    // CGVTSettingsDlg::allMaterialModelsHaveVelocityP() returned true, implying
    // (materialRock != 0)

    CLibraryMaterial& libraryMaterial = materialRock->LibraryMaterial();
    const CLibraryMaterialParameter* libraryMaterialParameter =
      libraryMaterial.ParameterByValueTypeID(IDT_VALUETYPE_VELOCITYP);

    addCalculatedDistanceTimeInstance(distanceTime,
      (*formation)->Max().Z() - (*formation)->Min().Z(),
      libraryMaterialParameter->Value());
  }
}

void convertDistanceTime2DepthTime(TDistanceTimeVector& distanceTime)
{
  TDistanceTimeVector depthTime;

  for (TDistanceTimeVector::const_iterator distanceTimePair =
    distanceTime.begin(); distanceTimePair != distanceTime.end();
    ++distanceTimePair)
  {
    if (distanceTimePair == distanceTime.begin())
    {
      depthTime.push_back(*distanceTimePair);
    }
    else
    {
      TDistanceTime depthTimePair = std::make_pair(
        depthTime.back().first + (*distanceTimePair).first,
        depthTime.back().second + (*distanceTimePair).second);

      depthTime.push_back(depthTimePair);
    }
  }

  depthTime.swap(distanceTime);
}

const char TAB = '\t';

QString writeDistanceTime2VelocityModelFile(TDistanceTimeVector& distanceTime)
{
  QString velocityModelFileName = GetGeomecTempPath() + tmpnam(NULL);
  std::fstream velocityModelFile(velocityModelFileName.toStdString().c_str(),
    std::ios_base::out);

  for (TDistanceTimeVector::const_iterator distanceTimePair =
    distanceTime.begin(); distanceTimePair != distanceTime.end();
    ++distanceTimePair)
  {
    velocityModelFile << (*distanceTimePair).first << TAB <<
      (*distanceTimePair).second << std::endl;
  }

  velocityModelFile.close();

  return velocityModelFileName;
}

const QString FORMATIONS_ARE_NOT_CONSECUTIVE =
  "the formations are not consecutive";

} // anonymous namespace

// static

QString CGVTVelocityModelFromMaterial::createVelocityModelFile(
  CModelBase& modelBase)
{
  if (!areFormationsConsecutive(modelBase))
  {
    if (!modelBase.Loading())
      _m()->msg("Taking initial velocities from models with vertically non-consecutive formations is not supported.");
    throw std::runtime_error(FORMATIONS_ARE_NOT_CONSECUTIVE.toStdString());
  }

  std::vector <TDistanceTime> distanceTime;

  addInitialDistanceTimeInstances(distanceTime, modelBase);
  addFormationDistanceTimeInstances(distanceTime, modelBase);
  convertDistanceTime2DepthTime(distanceTime);

  return writeDistanceTime2VelocityModelFile(distanceTime);
}
#endif

} // namespace GVT
