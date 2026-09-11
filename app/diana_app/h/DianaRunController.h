#pragma once

class CDianaRunner;

#include "IDianaRunController.h"


class CDianaRunController : public IDianaRunController
{
	CDianaRunner* m_pRunner;

public:
	CDianaRunController(	CModelBase& model,
							const CAnalysisType& antype, 
							bool bWriteInputFiles, 
							bool bWriteQuadDat,
							bool bWriteOutputStreamFile,
							const QString& strTempPath );

	bool RunFirst(	const QString& getPathName, 
					IDianaXWrapper* dianaXWrapper,
					const std::string &title, 
					ISaveModel& saveModel,
					IRetrieveDianaFileNames& retrieveDianaFileNames );

	bool RunNext( const QString& getPathName, 
				IDianaXWrapper* dianaXWrapper,
				const std::string &title, 
				ISaveModel& saveModel,
				IRetrieveDianaFileNames& retrieveDianaFileNames );

	void CleanUpAfterRun(	const QString& getPathName, 
						IDianaXWrapper* dianaXWrapper,
						const std::string &title, 
						ISaveModel& saveModel,
						IRetrieveDianaFileNames& retrieveDianaFileNames );
    

	bool Run(	const QString& getPathName, 
				IDianaXWrapper* dianaXWrapper,
				const std::string &title, 
				ISaveModel& saveModel,
				IRetrieveDianaFileNames& retrieveDianaFileNames, 
				dia::IAnalysisStatusContainer *pAnalysisStatusContainer = 0, 
				bool bSilent = false);

	/*
	void SetLicenseRetry(int nLicenseRetry);
	QString LicenseError() const;
	bool HaveResults() const;
	bool GetCalculationResult() const;

	CModelBase& Model();
	const CModelBase& Model() const;
	const CAnalysisType& AnalysisType() const;
	bool WriteInputFiles() const;
	bool WriteQuadDat() const;
	bool WriteOutputStreamFile() const;
	bool CleanupOldResults(int iStartStage) const;
	const CDepletionStage& StartStage() const;
	const CDepletionStage& EndStage() const;

	bool OnWriteBranchFiles(const QString& getPathName, bool bCalcResult,
		ISaveModel& saveModel) const;
	bool WriteCommands(IProgressBase& progress) const;
	bool OnModifyInputFileNames(std::string& sComfileName, std::string& sDatfileName) const;
	bool OnQueryInputFileNames(const std::string& title,
	std::string& sComfileName, std::string& sDatfileName,
	IRetrieveDianaFileNames& retrieveDianaFileNames) const;
	void RemoveRedundantLoads() const;

private:
	bool CreateBranchFiles() const;
	QString CreateBranchFileBaseName(const QString& sBaseName, int nStageIndex) const;

private:
	CModelBase& m_model;
	CAnalysisType m_antype;

	bool m_bWriteInputFiles;
	bool m_bWriteQuadDat;
	bool m_bWriteOutputStreamFile;
	const CDepletionStage* m_pStartStage;
	const CDepletionStage* m_pEndStage;
	QString m_strTempPath;
	CGeomecDianaRunnerBase* m_pRunner;
	int m_nLicenseRetry;
	QString m_sLicenseError;
	bool m_bHaveResults;
	bool m_bCalculationResult;
	mutable std::string m_sComFile;
	mutable std::string m_sDatFile;
	*/
};
