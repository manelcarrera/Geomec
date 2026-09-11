// IDianaRunner.h: interface for the IDianaRunner class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IDIANARUNNER_H__226D0237_EAF6_412C_8567_AA32F7769695__INCLUDED_)
#define AFX_IDIANARUNNER_H__226D0237_EAF6_412C_8567_AA32F7769695__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace geo {
  class CMeshNodeGroup;
} // namespace geo

class IDianaXWrapper;
class ISaveModel;
class IRetrieveDianaFileNames;

#include "IDCElementProperty.h"
#include "Vector.h"
#include "CoordinateMap.h"
#include "DCLoadManager.h"

#include "Printer.h"

#include "FilosFile.h"

class IProgressBase;


namespace dia {

class ITemperatureTable;
class IConcentrationTable;
class ILoad;
class CFixedPotential;
class IAnalysisStatus;

class IAnalysisStatusContainer; // only needed by run controller, inclueded to use the struct in both controller and runner

class IDianaRunner  
{

	friend class ILoad;
	ITemperatureTable *m_pTempTable;
  IConcentrationTable* m_pConcenTable;
  CFixedPotential* m_pFixedPotential;

  bool m_bCalculationResultExpectOKMessage;
  bool m_bCanceled;
  int  m_nLicenseRetry;

  bool m_bHaveResults;
  bool m_bCalculationResult;

  
protected:

	enum eError { WriteDB, WriteRunInfo_, RunAnalysis, UserDefinedFF, CopyMaterials, Running, PreProcessWriteInputFiles, PostProcessWriteInputFiles, EndAnalysis, CreateSubDir };

	IProgressBase* m_progress;

public:
	void progress(IProgressBase* val) { m_progress = val; }
	IProgressBase* progress() { return m_progress; }

public:
	IDianaRunner(const geo::IMesh &mesh);
	virtual ~IDianaRunner();

	const geo::IMesh &Mesh() const;

	struct RunParams
	{
		QString path;
		std::string title;
		std::string rootdir;

		IDianaXWrapper* dxw;
		ISaveModel* save;
		IRetrieveDianaFileNames* retrieve;

		IAnalysisStatusContainer* ana_status_container; // onyl controller

		bool bSilent;

		bool dsa;
	};

protected:
	RunParams m_params;

public:
	RunParams& params(){ return m_params; };
	void params(const RunParams& p){ m_params = p; };

	bool start();


public:
	const std::string &FFDIR() const;

	// return material for the given element
	virtual const IMaterial &Material(const geo::IElement &element) const = 0;

	// return element geometry for the given element
	// return 0 if no geometry attached
	virtual const IElementGeometry *ElementGeometry(const geo::IElement &element) const;

	// return element data for the given element
	// return 0 if no data attached
	virtual const IElementData *ElementData(const geo::IElement &element) const;

	// returns the 1-based index of the material, for use in Filos
	// return 0 on error
	virtual int AddElementProperty(const IElementProperty &prop, int type, bool comparePointers = false);

	// returns the 1-based index of the direction, for use in Filos
	virtual int AddDirection(const geo::IVector &dir);

	// use this function to write application (non-standard) data to the filos file,
	// this function is the last function that is called in WriteDatabase()
	virtual bool OnWriteMiscelaneous() { return true; }

	// return the 1-based index of the material
	int FilosMaterialIndex(const IMaterial &material) const;

	//be aware that if you use the root dir the rootdir is cleanup after start
	virtual bool UseRootDirAsWorkDir() const {return false;} 

	virtual bool CanCleanupAfterRun() const {return true;} 
  virtual bool TranslateElementsToXYOrigin() const { return false; }

	virtual bool OpenFilosfileReadOnlyForResult() const {return true;}

	const std::string& FF() const;

  // first call creates the object, subsequent calls return that same object instance
  CFixedPotential& FixedPotentialNodeSet();

  bool CalculationResultExpectOKMessage() const;
  void CalculationResultExpectOKMessage(bool flag);

  void GetResultInfo(bool& bHaveResults, bool& bCalculationResult, bool bSave = true);
  static void GetResultInfo(const char *workingdir, const char *filosfile, bool& bHaveResults, bool& bCalculationResult, bool expectOkMessage = true);

  void Cancel();
  bool Canceled() const;

  void LicenseRetry(int nLicenseRetry);
  int LicenseRetry() const;

  bool clean(bool bForce=false);

protected:
	virtual bool OnInitialize(const std::string& /*title*/,
		IRetrieveDianaFileNames& /*retrieveDianaFileNames*/) { return true; }
	virtual bool OnWriteMesh();

  virtual bool OnWriteSupports();

  // writes a single support, nodeindex is 0-based, type is DIANA type name (TR, RO, PR, ...)
  // vecDirection can be empty for non-directional supports (e.g. PR)
  void WriteSupport(int nodeindex, const char* type, const geo::IVector& vecDirection);

	virtual bool OnWriteTyings();

  // writes a single tying
  // typename can be overridden, if bDirected == false, no direction info is written (e.g. TE tying)
  void WriteTying(const geo::CTying& tying, const char* type = 0, bool bDirected = true);

	virtual bool OnApplyLoads(CLoadManager &manager);
	virtual bool OnWriteLoads();

	// commands should start at index 10 in /CMNDS/SEGINF
	virtual bool OnWriteCommands();
	virtual bool OnEndWriteCommands(const std::string &title);
	virtual bool OnCopyMaterials();
	virtual bool OnRunAnalysis(IDianaXWrapper* dianaXWrapper, bool bSilent = false);
	virtual bool OnEndAnalysis(const QString& getPathName, ISaveModel& saveModel);
  virtual bool OnEndPreprocessWriteInputFiles();
  virtual bool OnEndPostprocessWriteInputFiles(const std::string& sComFileName, const std::string& sDatFileName);

	// for any error
	virtual void OnError(eError id, const std::string& msg2="");

	// group names
	virtual std::string GroupName(const geo::IElementSet &group, int nIndex);
	virtual std::string GroupName(const geo::CMeshNodeGroup &group);
/*
	class CDirectionCompare
	{
	public:
		bool operator()(const geo::IVector &v1, const geo::IVector &v2) const
		{
			geo::ICoordinate::
			const double eps = EPS * EPS;
			geo::CVector lv1(v1.UnitVector());
			geo::CVector lv2(v2.UnitVector());

			double diffX = lv1.X() - lv2.X();
			if(diffX < -eps) return true;
			if(diffX >  eps) return false;

			double diffY = lv1.Y() - lv2.Y();
			if(diffY < -eps) return true;
			if(diffY >  eps) return false;

			double diffZ = lv1.Z() - lv2.Z();
			if(diffZ < -eps) return true;
			return false;
		}
	};
*/
//	typedef std::map<geo::CVector, int /*, CDirectionCompare */> TDirectionMap;
public:

  class CDirectionEpsilon
  {
  public:
    double operator()()
    {
      return 1.0e-4; // Diana required a higher precision for a while (1E-10) but if all is well, this has been set back to 1E-4
    }
  };
  typedef geo::CCoordinateMap<geo::CVector, int, CDirectionEpsilon> TDirectionMap;

  TDirectionMap* DirectionMap();
  void DirectionMap(TDirectionMap* pMap);

  IAnalysisStatus *AnalysisStatus();
  void AnalysisStatus(IAnalysisStatus *pAnalysisStatus);

protected:
	TDirectionMap* m_pmpDirections;

	IElementGeometry::TElementPropertyMap m_mpElementProperty[3];
  IElementGeometry::TElementPropertyPointerMap m_mpElementPropertyPointer[3];

	// progress sizes
	virtual int MeshProgressSize() const;
	virtual int GroupsProgressSize() const;
	virtual int SupportProgressSize() const;
	virtual int TyingsProgressSize() const;
	virtual int LoadsProgressSize() const;
	virtual int CommandsProgressSize() const;

	// Return true if you want to export .com and .dat files
	// instead of actually running the analysis.
	// The analysis will not be performed, but input
	// will be run to generate the data file.
	virtual bool WriteInputFiles(std::string &comfilename, std::string &datfilename) const;

	// For a temper table, create an instance of the derived class of ITemperatureTable on the heap
	// The runner will delete this pointer
	virtual ITemperatureTable *OnCreateTemperatureTable() const;
  virtual void OnExtendTemperatureTable(ITemperatureTable* pTable) const;
  void OnDeleteTemperatureTable();
  ITemperatureTable *TemperatureTable();
  void TemperatureTable(ITemperatureTable *);

	// For a concen table, create an instance of the derived class of IConcentrationTable on the heap
	// The runner will delete this pointer
	virtual IConcentrationTable *OnCreateConcentrationTable() const;
  virtual void OnExtendConcentrationTable(IConcentrationTable* pTable) const;
  void OnDeleteConcentrationTable();
  IConcentrationTable *ConcentrationTable();
  void ConcentrationTable(IConcentrationTable *);

	// return an existing path if you want to use that instead of a generated one
	virtual std::string UserDefinedLocation() const;

	// allows user filos file (e.g. for restart)
	//  strFilosFile - set the name of the file
	//  bInitFilos   - set to false if filos file should not be initialized
	//  return false to stop analysis
	virtual bool UserDefinedFilosFile(const QString& /*getPathName*/,
		std::string& /*strFilosFile*/, bool& /*bInitFilos*/) const { return true; }

  // allows modification of the data and com filenames just before starting the analysis
  virtual bool OnModifyInputFileNames(std::string& /*sComfileName*/, std::string& /*sDatfileName*/) const { return true; }

  // initial text to be written to the command file, default is:
  // *FILOS\n INITIA\n*INPUT\n
  // should be written in one line, with \n characters. Don't forget the last \n !
  virtual std::string InitialComFileText() const;
  virtual bool ExportCommandFileWithDefaults() const;

  virtual void executeCommandInGeomec() const;

  virtual bool WriteElements() = 0;


private:
	///////////////////////////////////
	//
	//			CCurDirSaver
	//
	///////////////////////////////////
	class CCurDirSaver
	{
	public:
		CCurDirSaver();
		~CCurDirSaver();

	private:
		char *m_strOldCwd;
	};
	///////////////////////////////////
	//
	//			
	//
	///////////////////////////////////

	static char *GetCurrentDir();

	bool subdir(const std::string &rootdir);

	CFilosFile* m_FF_;

	CLoadManager m_LoadManager;
	std::string m_FFDIR;
	std::string m_FF;

	std::string m_sDatFileName;
	std::string m_sDatFileDir;

  IAnalysisStatus *m_pAnalysisStatus;

  void WriteTyingInfo(const geo::CTying& tying, const char* type, bool bDirected);
	bool WriteDatabase(	const std::string &title, 
						IRetrieveDianaFileNames& retrieveDianaFileNames);
	bool CreateSubdir(const std::string &rootdir);
	bool WriteNodes();
	bool WriteCommands();
	bool EndWriteCommands();
  bool WriteRunInfo();

protected:
  virtual void AnalysisSuccess(bool bAnalysisSuccess);
	void Cleanup(const std::string &subdir);
private:
	void InitializeFile();
  void InitDirectionsMapFromFilos();
	int ProgressSize() const;

	bool create_FF();

	bool run();

	bool PreprocessWriteInputFiles(std::string &sComfileName, std::string &sDatfileName);
  bool PostprocessWriteInputFiles(const std::string& sComfileName, const std::string &sDatFileName);

  public:
	bool end();
};

}

#endif // !defined(AFX_IDIANARUNNER_H__226D0237_EAF6_412C_8567_AA32F7769695__INCLUDED_)
