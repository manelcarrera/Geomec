// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CDATAMODEL_3B65771E006F_INCLUDED
#define _INC_CDATAMODEL_3B65771E006F_INCLUDED

// only for testing purposes, cannot distribute (error when trying to build in release mode)
#define USE_SE40 0

#ifdef NDEBUG
#if USE_SE40
#error Can't use old Diana executable (SE40) in release mode (unable to distribute)
#endif
#endif


#include "IComponent.h"
#include "CDrillingScheme.h"
#include "CDependenciesManager.h"
#include "Control.h"
#include "VersionInfo.h"

#include "FormationMaterial.h"
#include "CasingMaterial.h"
#include "CementMaterial.h"
#include "MaterialListTemplate.h"

#if USE_SE40
#include "FgvRunCtr.h"
#endif

#include "DianaX.h"
#include "DianaXEventWnd.h"
#include "mfTensor.h"
#include "CTestLoads.h"
#include "CBatchTable.h"
#include "ValueProfile.h"

#define XCOOR 0
#define YCOOR 1
#define ZCOOR 2

#define ACCELERATION_OF_GRAVITY  9.81
#define WATER_DENSITY            10.0e-3  //Mkg/m3

#define TOOL_TIP_MARGE 20

#define  FGV_STR       "fgvstr.str"   


#define  IDC_FGVRUNCTR	2

#define  DIANA_KERNEL_VERSION	1
#define  DIANA_KERNEL_REVISION	1


class CDSModelDatabase;
class CBatchTable;
class CBatchTableRow;
class CAnalysis;
class CFormationTopography;
class CMeshParam;

//##ModelId=3DDA0C460311
class CStartAnalysisData
{
public:
	//##ModelId=3DDA0C460320
	CStartAnalysisData() {Basename="DSEALECT";}
	//##ModelId=3DDA0C460321
	CString Workingdir;
	//##ModelId=3DDA0C460330
	CString DianaPath;
  CString DianaSharePath;
	//##ModelId=3DDA0C460331
	CString Basename;
	//##ModelId=3DDA0C460332
	CString DatabaseName;

	//##ModelId=3DDA0C460333
	CString TempDir;

};

class CFgvRunner
{
public:
  CFgvRunner();

  void SetFgvPath(const CString& strFgvPath);
  void SetExeName(const CString& strExeName);
  void SetWorkingDir(const CString& strWorkingDir);
  void SetMonitorLogfile(bool bLog);
  void SetMonitorCaption(const CString& strCaption);
  void SetAutoCloseDialog(bool bAuto);

  const CString& GetWorkingDir() const;

  bool RunFemView();

private:
  CString m_strFgvPath;
  CString m_strExeName;
  CString m_strWorkingDir;
  bool m_bLog;
  CString m_strCaption;
  bool m_bAutoClose;
};


////!!!! DESCRIPTION OF DEPENDENCIE AT BOTTOM OF THIS HEADER FILE !!!!////

//##ModelId=3C0DDDAE0139
class CDataModel 
: public IComponent
{
public:
	//##ModelId=3CA01193004E
	class CDep6: public CDependency
	{
	private:
		//##ModelId=3DDA0C4803DC
		mutable CString m_DepString;
	public:
		//##ModelId=3CA0119300CB
		CDep6(const CDoubleQuantity* pQuantity);
		//##ModelId=3CA0119300CD
		virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const;
		//##ModelId=3CA0119300D0
		std::string GetDependencyString() const;
		//##ModelId=3CA0119300DB
		void SetCasingOuter(CDoubleQuantity* pCasingOuterDiameter);
		//##ModelId=3CA0119300DD
		CDoubleQuantity* GetCasingOuter();

	};

	BOOL InitInstance();

private:
	//##ModelId=3DDA0C480042
	BOOL m_bPolyLineCreated;

	//##ModelId=3CA0118E037A
	BOOL m_bIsCalculated;
	//##ModelId=3CA0118F010A
	CDep6* m_pBoreHoleDep;
	
	//##ModelId=3DDA0C480071
	CString CreateSubWorkDir(CString sWorkDir);

	//##ModelId=3DDA0C4800A0
	void InitModel();

	//##ModelId=3C8D9E0A0205
	BOOL WriteMeshFile(LPCTSTR sFileName);
	//CMeshParam *MeshParam();

	//##ModelId=3C8D9E00032E
	void InitDianaX();
	//##ModelId=3C8D9E0003BA
	void InitFgvRunCtr();

	//##ModelId=3C8D9E01007F
	CDianaX *m_pDianaX;
	//##ModelId=3C8D9E0100ED
#if USE_SE40
  CFgvRunCtr* m_pFgvRunCtr;
#else
	CFgvRunner *m_pFgvRunCtr;
#endif
	
	//##ModelId=3DDA0C4800C0
	CDianaXEventWnd* m_pDianaXEventWnd;

	//##ModelId=3C8D9E010188
	CBatchTable *m_pBatchTable;

	//##ModelId=3C60D5690300
	CMaterialListTemplate<CFormationMaterial> m_FormationMatList;
	//##ModelId=3C60D569035E
	CMaterialListTemplate<CCasingMaterial> m_CasingMatList;
	//##ModelId=3C60D56903BC
	CMaterialListTemplate<CCementMaterial> m_CementMatList;

	//##ModelId=3C2043F3035C
	geo::CPolyLine m_PolyLine;
  CString m_strSurveyFileName;

	//##ModelId=3C60D56A0001
	CLengthQuantity m_qModelBottomTVD;
	//##ModelId=3C60D56A0021
	CLengthQuantity m_qModelBottomTMD;
	//##ModelId=3C60D56A0040
	CLengthQuantity m_qModelTopTVD;
	//##ModelId=3C60D56A005F
	CLengthQuantity m_qModelTopTMD;

	//##ModelId=3CAAA65C01B9
	CFormationTopography* m_pFormationTopography;
	//##ModelId=3CAAA65C01F8
	CDrillingScheme m_DrillingScheme;
	
	//##ModelId=3CAAA65C0236
	CAnalysis* m_Analysis;
	//##ModelId=3CAAA65C0294
	CControl m_Control;
  
	//##ModelId=3C8D9E010254
	CMeshParam *m_pMeshParam;

	//##ModelId=3C60D56A00CD
	std::vector<CDependency*> m_DependencyVec;
	//##ModelId=3C8D9E0102D0
	CString m_sDianaPath;
  CString m_sDianaSharePath;
	//##ModelId=3C8D9E0200CC
	CString m_sWorkingDir;	
	//##ModelId=3C8D9E020310
	CVersionInfo m_VersionInfo;

	//##ModelId=3DDA0C4800FD
	CString m_sDatabaseName;

	CTestLoads m_TestLoads;

  CBatchTableSelection m_BatchTableSelection;

  CValueProfile m_PorePressureProfile;
  CString m_strPorePressureProfileFileName;

  CValueProfile m_DensityProfile;
  CString m_strDensityProfileFileName;

  CValueProfile m_TemperatureProfile;
  CString m_strTemperatureProfileFileName;

public:
	//##ModelId=3DDA0C48012C
	void CalculateNEgMatrix(geo::CMatrix &NEgRot, double dTMD);
	//##ModelId=3DDA0C48014B
	double GetCohesionAtDepth(double dDepth);
	//##ModelId=3DDA0C48017A
	double GetFrictionAtDepth(double dDepth);
	//##ModelId=3DDA0C48019A
	double GetDilatationAtDepth(double dDepth);
	//##ModelId=3DDA0C4801B9

#if USE_SE40
	CFgvRunCtr& GetFgvRunControl();
#else
	CFgvRunner& GetFgvRunControl();
#endif

	//##ModelId=3DDA0C4801C8
	CString GetWorkingDir();

	//##ModelId=3DDA0C4801C9
	void InvalidatePolyLine();
	
	//##ModelId=3DDA0C4801D8
	BOOL IsPolyLineCreated();

	//##ModelId=3DDA0C4801E8
	CString GetDatabaseName(){return m_sDatabaseName;}

	//##ModelId=3CA0118F0196
	//BOOL IsCalculated() const; 

	//##ModelId=3CA0118F0242
	void InvalidateResults(); //  {m_bIsCalculated=FALSE;}

	//##ModelId=3C8D9E0802B1
	CMeshParam *MeshParam();

	//##ModelId=3C8D9E02038C
	CVersionInfo VersionInfo();
	//##ModelId=3C8D9E030040
	BOOL CheckAllMaterials();
	
	//##ModelId=3C2043F3038B
	CLengthQuantity& GetModelTopTMD();
	//##ModelId=3C2043F303D9
	CLengthQuantity& GetModelTopTVD();
	//##ModelId=3C2043F4003F
	CLengthQuantity& GetModelBottomTMD();
	//Bottom TVD of lowest formation
	//##ModelId=3C2043F4007D
	CLengthQuantity& GetModelBottomTVD();


	//##ModelId=3C8D9E0300CC
	double GetGeoThermalTemperatureAtDepth(double dDepth);
	//##ModelId=3C8D9E03031E
	double GetVertStressAtDepth(double dDepth);
	
	//##ModelId=3C8D9E040188
	double GetMaxHorizontalStressAtDepth(double dDepth);
	//##ModelId=3C8D9E050030
	double GetMinHorizontalStressAtDepth(double dDepth);
	//##ModelId=3C8D9E050272
	double GetPorepressureAtDepth(double dDepth);

	//##ModelId=3DDA0C480217
	double GetUndrainedPorepressureAtDepth(double dDepth);

	//##ModelId=3C8D9E0600FB
	double GetEffectiveVertStressAtDepth(double dDepth);
	//##ModelId=3C8D9E06035D
	double GetEffectiveMaxHorizontalStressAtDepth(double dDepth);
	//##ModelId=3C8D9E0701E6
	double GetEffectiveMinHorizontalStressAtDepth(double dDepth);

	//##ModelId=3CA0118F030D
	double WaitingOnCementFluidPressureAtDepth(double dDepth); 

	//##ModelId=3CA011900167
	double GetTauRatioAtDepth(double dDepth);

	//##ModelId=3C8D9E08004F
	CTensor GetLocalEffectiveStressesAtDepth(double dDepth);

	//##ModelId=3CA011900399
	double CementSlurryPressureAtTMD(double dTMD);

	//##ModelId=3CA0119101C5
	double DrillingMudPressureAtDepth(double dDepth);

	//##ModelId=3CA0119103D8
	bool IsInTail(double dDepth);

	//##ModelId=3C60D56A00FB
	CDependenciesManager DependenciesManager;

	//##ModelId=3C2043F400CC
	BOOL CreatePolyLine();
	//##ModelId=3C2043F4011A
	BOOL CanCreatePolyLine();
	//##ModelId=3C2043F40168
  // second is false (and first undefined) if length > wellpath length or length < 0
  std::pair<double, bool> TMDtoTVD(const double &length);

	//##ModelId=3C0DDDAE0189
	CDataModel();

	//##ModelId=3DDA0C480274
	CDataModel(CString sWorkingDir);

	//##ModelId=3C2043F402CF
	~CDataModel();
	
	//##ModelId=3C60D56A011A
	CMaterialListTemplate<CFormationMaterial>* FormationMatList();
  const CMaterialListTemplate<CFormationMaterial>* FormationMatList() const;
	//##ModelId=3C60D56A0158
	CMaterialListTemplate<CCasingMaterial>* CasingMatList();
	//##ModelId=3C60D56A01A6
	CMaterialListTemplate<CCementMaterial>* CementMatList();

	//##ModelId=3C0DDDAE018A
	CFormationTopography* FormationTopography();
	const CFormationTopography* FormationTopography() const;
	
	//##ModelId=3C0DDDAE0197
	CControl* Control();
	
	

	//##ModelId=3C0DDDAE0198
	CDrillingScheme* DrillingScheme();
	
	//##ModelId=3C0DDDAE0199
	CAnalysis* Analysis();
	
	//##ModelId=3C8D9E08034D
	CBatchTable* BatchTable();

	//##ModelId=3C0DDDAE019A
	bool CheckComplete() const;

	//##ModelId=3C0DDDAE019B
	CString GetInfo();
	
	//##ModelId=3DDA0C4802D2
	BOOL LoadFromDatabase(CString strDatabaseName);
	//##ModelId=3DDA0C4802F1
	BOOL SaveToDatabase(CString strDatabaseName);

	//##ModelId=3C60D56A03AA
	void RemoveUnusedMaterial();

	//##ModelId=3DDA0C480320
	BOOL CanStartAnalysis();

	//##ModelId=3DDA0C480321
	BOOL StartAnalysis(CStartAnalysisData& sud);

  bool IsDeviated() const;
  void SurveyFileName(const CString& name);
  const CString& SurveyFileName() const;

	//##ModelId=3CA011920222
	CDep6* GetBoreholeDep(){return m_pBoreHoleDep;}
	CTestLoads *TestLoads();

  const geo::CPolyLine& PolyLine() const;

  CBatchTableSelection& BatchTableSelection();

  const CValueProfile& PorePressureProfile() const;
  CValueProfile& PorePressureProfile();
  const CString& PorePressureProfileFileName() const;
  void PorePressureProfileFileName(const CString& strName);

  const CValueProfile& DensityProfile() const;
  CValueProfile& DensityProfile();
  const CString& DensityProfileFileName() const;
  void DensityProfileFileName(const CString& strName);

  const CValueProfile& TemperatureProfile() const;
  CValueProfile& TemperatureProfile();
  const CString& TemperatureProfileFileName() const;
  void TemperatureProfileFileName(const CString& strName);

private:

	//##ModelId=3C60D56B005E
	class CDep1_1: public CDependency
	{
	private:
		
		//##ModelId=3DDA0C490080
		mutable CString m_DepString;
		//##ModelId=3C60D56B008F
		CDataModel *m_pDM;
	public:
		//##ModelId=3C60D56B009D
		CDep1_1(CDataModel* dm);
		//##ModelId=3C60D56B009F
		virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const;
		//##ModelId=3C60D56B00AD
		std::string GetDependencyString() const;
	};
	
	//##ModelId=3C60D56B010A
	class CDep1_2: public CDependency
	{
	private:
		
		//##ModelId=3DDA0C49010D
		mutable CString m_DepString;
		//##ModelId=3C60D56B014A
		CDataModel* m_pDM;
	public:
		//##ModelId=3C60D56B0158
		CDep1_2(CDataModel* dm);
		//##ModelId=3C60D56B015A
		virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const;
		//##ModelId=3C60D56B0168
		std::string GetDependencyString() const;
	};

	//##ModelId=3C60D56B01C6
	class CDep1_3: public CDependency
	{
	private:
		
		//##ModelId=3DDA0C49019A
		mutable CString m_DepString;
		//##ModelId=3C60D56B01F6
		CDataModel* m_pDM;
	public:
		//##ModelId=3C60D56B0204
		CDep1_3(CDataModel* dm);
		//##ModelId=3C60D56B0206
		virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const;
		//##ModelId=3C60D56B0209
		std::string GetDependencyString() const;
	};

	//##ModelId=3C60D56B0271
	class CDep2_1: public CDependency
	{
	private:
		//##ModelId=3C60D56B02B1
		CDataModel* m_pDM;
		
		//##ModelId=3DDA0C490226
		mutable CString m_DepString;
	public:
		//##ModelId=3C60D56B02C0
		CDep2_1(CDataModel* dm);
		//##ModelId=3C60D56B02C2
		virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const;
		//##ModelId=3C60D56B02C5
		std::string GetDependencyString() const;
	};
	
	//##ModelId=3C60D56B032D
	class CDep2_2: public CDependency
	{
	private:
		//##ModelId=3C60D56B036C
		CDataModel* m_pDM;
		
		//##ModelId=3DDA0C4902B3
		mutable CString m_DepString;
	public:
		//##ModelId=3C60D56B0370
		CDep2_2(CDataModel* dm);
		//##ModelId=3C60D56B037C
		virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const;
		//##ModelId=3C60D56B037F
		std::string GetDependencyString() const;
	};

	//##ModelId=3C60D56C0020
	class CDep2_3: public CDependency
	{
	private:
		//##ModelId=3C60D56C005F
		CDataModel* m_pDM;
		
		//##ModelId=3DDA0C49035F
		mutable CString m_DepString;
	public:
		//##ModelId=3C60D56C0063
		CDep2_3(CDataModel* dm);
		//##ModelId=3C60D56C006F
		virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const;
		//##ModelId=3C60D56C0072
		std::string GetDependencyString() const;
	};

	//##ModelId=3C60D56C00DB
	class CDep3_1: public CDependency
	{
	private:
		
		//##ModelId=3DDA0C4A0003
		mutable CString m_DepString;
		//##ModelId=3C60D56C010C
		CDataModel* m_pDM;
		
	public:
		//##ModelId=3C60D56C011A
		CDep3_1(CDataModel* dm);
		//##ModelId=3C60D56C011C
		virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const;
		//##ModelId=3C60D56C0129
		std::string GetDependencyString() const;
	};

	// Dependency between OverburdenBottom, TopOfCementTMD, AnalysisPointTMD
	//##ModelId=3C60D56C01A6
	class CDep4_1: public CDependency 
	{
	private:
		//##ModelId=3C60D56C01D7
		CDataModel* m_pDM;
		//##ModelId=3C60D56C01E8
		mutable std::string m_DependencyString;
	public:
		//##ModelId=3C60D56C01F4
		CDep4_1(CDataModel* dm);
		//##ModelId=3C60D56C01F6
		virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const;
		//##ModelId=3C60D56C01F9
		std::string GetDependencyString() const;
		//##ModelId=3C60D56C01FB
		bool RemainingsUndefined(const CDoubleQuantity *pQuantity ) const;
	};

	//##ModelId=3C60D56C0281
	class CDep4_2: public CDependency
	{
	private:
		//##ModelId=3C60D56C02B2
		CDataModel* m_pDM;
		
		//##ModelId=3DDA0C4A015B
		mutable CString m_DepString;
	public:
		//##ModelId=3C60D56C02C0
		CDep4_2(CDataModel* dm);
		//##ModelId=3C60D56C02C2
		virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const;
		//##ModelId=3C60D56C02CF
		std::string GetDependencyString() const;
	};

	//##ModelId=3C60D56C033D
	class CDep1: public CDependency
	{
	private:
		
		//##ModelId=3DDA0C4A01E8
		mutable CString m_DepString;
		//##ModelId=3C60D56C036D
		CDataModel* m_pDM;
	public:
		//##ModelId=3C60D56C037B
		CDep1(CDataModel* dm);
		//##ModelId=3C60D56C037D
		virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const;
		//##ModelId=3C60D56C0380
		std::string GetDependencyString() const;
	};

	//##ModelId=3C60D56D0010
	class CDep2: public CDependency
	{
	private:
		
		//##ModelId=3DDA0C4A0293
		mutable CString m_DepString;
		//##ModelId=3C60D56D0050
		CDataModel* m_pDM;
	public:
		//##ModelId=3C60D56D005E
		CDep2(CDataModel* dm);
		//##ModelId=3C60D56D0060
		virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const;
		//##ModelId=3C60D56D0063
		std::string GetDependencyString() const;
	};

	//##ModelId=3C60D56D00DB
	class CDep3: public CDependency
	{
	private:
		//##ModelId=3DDA0C4A0330
		mutable CString m_DepString;
		//##ModelId=3C60D56D011B
		CDataModel* m_pDM;
	public:
		//##ModelId=3C60D56D011F
		CDep3(CDataModel* dm);
		//##ModelId=3C60D56D0129
		virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const;
		//##ModelId=3C60D56D012C
		std::string GetDependencyString() const;
	};

	//##ModelId=3C60D56D01A6
	class CDep4: public CDependency
		
	{
	private:
		//##ModelId=3C60D56D01D7
		CDataModel* m_pDM;
		
		//##ModelId=3DDA0C4A03CC
		mutable CString m_DepString;
	public:
		//##ModelId=3C60D56D01E5
		CDep4(CDataModel* dm);
		//##ModelId=3C60D56D01E7
		virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const;
		//##ModelId=3C60D56D01F4
		std::string GetDependencyString() const;
	};
	
	//##ModelId=3C60D56D0271
	class CDep5: public CDependency
		
		
	{
	private:
		//##ModelId=3C60D56D02A2
		CDataModel* m_pDM;
		//##ModelId=3DDA0C4B0090
		mutable CString m_DepString;
	public:
		//##ModelId=3C60D56D02B0
		CDep5(CDataModel* dm);
		//##ModelId=3C60D56D02B2
		virtual bool CheckQuantity(const CDoubleQuantity* pQuantity) const;
		//##ModelId=3C60D56D02C0
		std::string GetDependencyString() const;
	};
	
	

friend CDep1;		//Mean Sealevel < SeaBed
friend CDep1_1;		//Overburden Bottom TMD < WorkStringPackersTMD
friend CDep1_2;		//WorkStringPackersTMD < Guide Shoe depth
friend CDep1_3;		//Overburden Bottom TMD < Guide Shoe depth
friend CDep2;		//Mean sea/ground level < Overburden TVD
friend CDep2_1;		//Top of Cement TMD < Top of Tail Cement TMD
friend CDep2_2;		//Top of Tail Cement TMD < Guide Shoe depth
friend CDep2_3;		//Top of Cement TMD < Guide Shoe depth
friend CDep3;		//Sea bed TVD < Overburden TVD
friend CDep3_1;		//Overburden bottom TMD >= Model top TVD
friend CDep4;		//Overburden bottom TVD < Bottom of Top-Formation
friend CDep4_1;		//Overburden bottom TMD < Analysis point TMD OR Top of Cement TMD < Analysis point TMD
friend CDep4_2;		//Analysis point TMD < Guide Shoe depth
friend CDep5;		//Guide shoe depth <= bottom TMD
friend CDep6;		//Borehole diameter > Casing Outer Diameter

};

#endif /* _INC_CDATAMODEL_3B65771E006F_INCLUDED */
