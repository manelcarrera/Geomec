// ResultRegister.h: interface for the ResultRegister class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESULTREGISTER_H__98AB36A2_B9BE_4A57_AA36_C7B0DA00952D__INCLUDED_)
#define AFX_RESULTREGISTER_H__98AB36A2_B9BE_4A57_AA36_C7B0DA00952D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAnalysisType;
class CDepletionStage;

namespace geo
{

class CValue;
class IElement;
class IPoint;
class CPoint;

} // namespace geo

#include "mfStressTensor.h"
#include "mfStrainTensor.h"

// Forward declarations
class CDepletionStageEntry;
class CMeshBase;
class CResultCache;
class IProgressBase;
class CVectorValueSet;
class CScalarValueSet; 

// column indices for MeshDataCacher
// displacements
const int CI_DISPLA_X = 0;
const int CI_DISPLA_Y = 1;
const int CI_DISPLA_Z = 2;

// pore pressure
const int CI_POREPRES = 3;

// stresses
const int CI_STRESS_XX = 4;
const int CI_STRESS_YY = 5;
const int CI_STRESS_ZZ = 6;
const int CI_STRESS_XY = 7;
const int CI_STRESS_YZ = 8;
const int CI_STRESS_XZ = 9;

// strains
const int CI_STRAIN_XX = 10;
const int CI_STRAIN_YY = 11;
const int CI_STRAIN_ZZ = 12;
const int CI_STRAIN_XY = 13;
const int CI_STRAIN_YZ = 14;
const int CI_STRAIN_XZ = 15;

// plastic strains
const int CI_PSTRAIN_XX = 16;
const int CI_PSTRAIN_YY = 17;
const int CI_PSTRAIN_ZZ = 18;
const int CI_PSTRAIN_XY = 19;
const int CI_PSTRAIN_YZ = 20;
const int CI_PSTRAIN_XZ = 21;

// creep strains
const int CI_CSTRAIN_XX = 22;
const int CI_CSTRAIN_YY = 23;
const int CI_CSTRAIN_ZZ = 24;
const int CI_CSTRAIN_XY = 25;
const int CI_CSTRAIN_YZ = 26;
const int CI_CSTRAIN_XZ = 27;

// temperature
const int CI_TEMPERATURE = 28;

// preconsolidation stress
const int CI_PRECON = 29;

// total number of columns
const int NUM_COLUMNS = 30;
// displacements from import
// when imported the displacements are 'as is'. They do not need to be
// calculated from the difference in position of a node as is the case
// for results from a normal calculation.
/*const int CI_IDISPLA_X = 28;
const int CI_IDISPLA_Y = 29;
const int CI_IDISPLA_Z = 30;
*/
// total number of columns
//const int NUM_COLUMNS = 31;

template<class TENSOR_TYPE>
class CTensorValueSet;

typedef CTensorValueSet<CStressTensor> CStressTensorValueSet;
typedef CTensorValueSet<CStrainTensor> CStrainTensorValueSet; 

#include "StorageNode.h"
#include "GeomecModelVisitor.h"

// The result register collects the basic Diana results from the filos file. The
// result register can also collect the basic results from other kernels. The result register
// stores and load the data in the Geomec project file
class CResultRegister : public CStorageNode
{
	template <typename T> friend class CTensorValueSet;
	friend class CVectorValueSet;
	friend class CScalarValueSet;
  friend class CConsistencyGuard;

  CResultCache* m_pCache;

	int TimeStepSize() const;
	void Clear(const CAnalysisType& antype);

	long ProcessMemorySize() const;

	// Reference to the mesh
	CDepletionStageEntry& m_dp_entry;

	typedef std::set<int> TAvailableResultsSet; // stage index
	TAvailableResultsSet m_stAvailableLinearResults;
	TAvailableResultsSet m_stAvailableNonlinResults;
  TAvailableResultsSet m_stAvailableHeatResults;
  TAvailableResultsSet m_stAvailableMixtureResults;
  TAvailableResultsSet m_stAvailableMixtureContainmentResults;
	TAvailableResultsSet& AvailableResultsSet(const CAnalysisType& antype);
	const TAvailableResultsSet& AvailableResultsSet(const CAnalysisType& antype) const;

  QString localStringTable(int resultType) const;
  QString localStringTableSet(std::set<int> resultTypes) const;
  void showChecksumErrors(std::set<int> stages, std::set<int> resultTypes) const;

	// the column number in the mesh data cacher
public:
	int ColumnNumber(const CAnalysisType& antype, int nTimeStep, int ResultType) const;
private:
	const double *ValuesFromCache(int nElement, int nColumn) const;
	void TensorFromCache(ITensor &target, const CDepletionStage &stage, const CAnalysisType& antype, int nElementIndex, int nNodeIndex, int nStart) const;
	void VectorFromCache(geo::IVector &target, const CDepletionStage &stage, const CAnalysisType& antype, int nElementIndex, int nNodeIndex, int nStart) const;

	void CreateCache();
	void RemoveCache(bool bForUpdate = false);

  bool SendModified(bool bSendModified) const;

	typedef const double& (ITensor::*TTensorGet)(void) const;
	typedef const double& (geo::CVector::*TVectorGet)(void) const;

	// Functions loading from filos
	void ReadFilosTimeSteps(const CAnalysisType& antype, const CDepletionStage& startStage, const CDepletionStage& endStage, IProgressBase& progress);
	void ReadFilosNodeResults(const geo::IElement &element, const CDepletionStage& stage, const CAnalysisType& antype);
	void DefaultElementNodeResult(const geo::IElement &element,
									int nNodeNr,
									const CDepletionStage &stage,
									const CAnalysisType& antype);
	void ReadFilosElementNodeResult(const geo::IElement& element,
									int nNodeNr,
									const CDepletionStage &stage,
									const CAnalysisType& antype);
	void ReadFilosElementResults(const CDepletionStage &Stage,
								 const CAnalysisType& antype,
								 IProgressBase& progress);

	int  NrOfSavedPoints(const geo::IElement& element) const;
	bool LoadDisplacementsToCache(TSTREAM &stream, TPROGRESS &progress, const CAnalysisType& antype);
	void LoadBodyTensorToCache(TSTREAM &stream, TPROGRESS &progress, const CAnalysisType& antype, int iBegin);
	void LoadBodyDoubleToCache(TSTREAM &stream, TPROGRESS &progress, const CAnalysisType& antype, int iBegin);
	void IncrementCacheValue(int iElement, const CAnalysisType& antype, int iStage, int iCacheEntry, int iNode, const double& dAdd);
	bool LoadOldLinearBodyResults(TSTREAM &stream, TPROGRESS &progress);
	bool LoadOldNonlinearBodyResults(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
	void LoadFaultDoubleToCache(TSTREAM &stream, TPROGRESS &progress, const CAnalysisType& antype, int iBegin);
	void LoadFaultVectorToCache(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress, const CAnalysisType& antype, int iBegin);
	void LoadOldFaultResults(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);

	void SaveCacheResults(TSTREAM &stream, TPROGRESS &progress, const CAnalysisType& antype);
	void LoadCacheResults(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress, const CAnalysisType& antype);

	// Functions for loading the 3.1 results
	void load_old_stream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	void load_old_result(TSTREAM& stream,const CStreamVersion& version, TPROGRESS& progress, const geo::IElement& element, bool bLinear);
	void load_old_geo_result(TSTREAM& stream, const CStreamVersion& version, const geo::IElement& element, int nNodeNr, int nTimeStep, bool bLinear);

	// Stream load helpers for tensor and vector
	void SaveTensor(TSTREAM& stream, const ITensor& tensor);
	CTensor LoadTensor(TSTREAM& stream);
	void SavePoint(TSTREAM& stream, const geo::IPoint& point);
	geo::CPoint LoadPoint(TSTREAM& stream);
	void SaveVector(TSTREAM& stream, const geo::IVector& point);
	geo::CVector LoadVector(TSTREAM& stream, const CStreamVersion& version);
	
	// Stream load vector array's, double array's point arrays
	void SaveStressTensorVec(TSTREAM& stream, TPROGRESS& progress, const std::vector<CStressTensor>& vcTensor);
	void SaveStrainTensorVec(TSTREAM& stream, TPROGRESS& progress, const std::vector<CStrainTensor>& vcTensor);
	void LoadStressTensorVec(TSTREAM& stream, TPROGRESS& progress, std::vector<CStressTensor>& vcTensor);
	void LoadStrainTensorVec(TSTREAM& stream, TPROGRESS& progress, std::vector<CStrainTensor>& vcTensor);
	void SaveDoubleVec(TSTREAM& stream, TPROGRESS& progress, const std::vector<double>& vcDouble);
	void LoadDoubleVec(TSTREAM& stream, const CStreamVersion& version, TPROGRESS& progress, std::vector<double>& vcDouble);
	void SavePointVec(TSTREAM& stream, TPROGRESS& progress, const std::vector<geo::CPoint>& vcPoint);
	void LoadPointVec(TSTREAM& stream, TPROGRESS& progress, std::vector<geo::CPoint>& vcPoint);
	void SaveVectorVec(TSTREAM& stream, TPROGRESS& progress, const std::vector<geo::CVector>& vcVector);
	void LoadVectorVec(TSTREAM& stream, const CStreamVersion& version, TPROGRESS& progress, std::vector<geo::CVector>& vcVector);

	bool IsPlainElement(const geo::IElement& element) const;
	bool IsFaultElement(const geo::IElement& element) const;
	bool ResultAvailable(const CDepletionStage &stage, const CAnalysisType& antype, int nElementIndex, int nResultType) const;
	bool ResultAvailable(int nTimeStep, const CAnalysisType& antype, int nElementIndex, int nResultType) const;
public:
	// Construction
	CResultRegister(CMeshBase& mesh,					// Mesh associated with result register
					CDepletionStageEntry& dp_entry);	// Depletion stage entry

	// Destruction
	virtual ~CResultRegister();

	bool SufficientFreeMemory() const;

	
	
	// Results for body- and interface like elements
	void Node(geo::IPoint &target, int nTimeStep, const CAnalysisType& antype, int nNodeIndex) const;
	void Node(geo::IPoint &target, const CDepletionStage& stage, const CAnalysisType& antype, int nNodeIndex) const;
	CVectorValueSet Displacement(const CDepletionStage& stage, const CAnalysisType& antype, bool bChange) const;

	CScalarValueSet PorePressure(const CDepletionStage& stage, const CAnalysisType& antype, bool bChange) const;
	CScalarValueSet Temperature(const CDepletionStage& stage, const CAnalysisType& antype, bool bChange) const;
  CScalarValueSet Consolidation(const CDepletionStage& stage, const CAnalysisType& antype, bool bChange) const;
	
	// Tensor results
	CStressTensorValueSet TotalStress(const CDepletionStage& stage, const CAnalysisType& antype, bool bChange) const;
	CStressTensorValueSet EffectiveStress(const CDepletionStage& stage, const CAnalysisType& antype, bool bChange) const;
	CStrainTensorValueSet TotalStrain(const CDepletionStage& stage, const CAnalysisType& antype, bool bChange) const;
	CStrainTensorValueSet PlasticStrain(const CDepletionStage& stage, const CAnalysisType& antype, bool bChange) const;

	CVectorValueSet FaultEffectiveStress(const CDepletionStage& stage, const CAnalysisType& antype, bool bChange) const;
	CVectorValueSet FaultShearStress(const CDepletionStage& stage, const CAnalysisType& antype, bool bChange) const;
	CVectorValueSet FaultDisplacement(const CDepletionStage& stage, const CAnalysisType& antype, bool bChange) const;
	CScalarValueSet FaultPorePressure(const CDepletionStage& stage, const CAnalysisType& antype, bool bChange) const;

	// In order to get material parameter values for an element, use this function
	double MaterialParameterValue(int nElementIndex, unsigned int ValueTypeID, const CDepletionStage& stage) const;
	
	int MaterialModel(int nElementIndex) const;

	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	virtual long SavedItems() const;

	// Availability
  bool ResultsAvailable() const; // any result
	bool Linear() const;
	bool NonLinear() const;
  bool Heat() const;
  bool Mixture() const;
  bool MixtureContainment() const;
  bool ResultsAvailable(const CAnalysisType& antype) const;

	// Clearing
	void ClearAll();
	void ClearLinear(bool bSendModified = true);
	void ClearNonLinear(bool bSendModified = true);
  void ClearHeat(bool bSendModified = true);
  void ClearMixture(bool bSendModified = true);
  void ClearMixtureContainment(bool bSendModified = true);

	// Mesh access 
	const CMeshBase& Mesh() const;
	CMeshBase& Mesh();

	// Depletion stage entry acces
	const CDepletionStageEntry& DepletionStageEntry() const;
	CDepletionStageEntry& DepletionStageEntry();

	bool ReadFilosResults(	const CDepletionStage& startStage, 
							const CDepletionStage& endStage, 
							const CAnalysisType& antype);

  bool ReadFilosResults(const QString& filosFile);
  
  void AddResultFromImport(int nResultType,
		  					 int nElementIndex,
							 int nNodeIndex,
							 int nDepletionStage,
							 const CAnalysisType& antype,
							 double value);

	virtual unsigned int IconId() const;
	virtual unsigned int TypeId() const;

	virtual bool Empty() const { return false; }

  bool ResultsAvailable(const CAnalysisType& antype, const CDepletionStage& stage) const;

  void TriggerSmoothedResults();
  bool HasSmoothedResults() const;

  bool cacheExists() const;
  CResultCache& Cache() const;

  ACCEPT_GEOMECMODELVISITORS(VisitResultRegister);
};



#endif // !defined(AFX_RESULTREGISTER_H__98AB36A2_B9BE_4A57_AA36_C7B0DA00952D__INCLUDED_)
