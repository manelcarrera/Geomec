#ifndef _GEOMECCOMPACTIONDIANARUNNER_H_
#define _GEOMECCOMPACTIONDIANARUNNER_H_

#include "GeomecDianaRunner.h"

class CGeomecCompactionDianaRunner : public CGeomecDianaRunnerBase
{
  friend class CDianaRunController;

public:
	CGeomecCompactionDianaRunner(CMeshBase &meshbase,
						                   CModelBase &model,
						                   CDianaRunController& controller);

protected:
	virtual bool OnApplyLoads(dia::CLoadManager &manager);
	virtual bool OnWriteTyings();
	virtual bool OnWriteSupports();
	virtual bool OnWriteCommands();

 	virtual int SupportProgressSize() const;
	virtual int CommandsProgressSize() const;
	virtual int TyingsProgressSize() const;

  virtual bool OnEndPostprocessWriteInputFiles(const std::string& sComfileName, const std::string& sDatfileName);

	virtual bool CreateTopLoad(const CHorizonBase &top, dia::CLoadCase &lcase) = 0;
	virtual bool FetchInitialPressures(const geo::IElement& element, const CDepletionStage& stage, IValueDomainScalar::TValueVec& vcInitialPressures) = 0;
	virtual bool CreateInitialLoads(const geo::IElement &element, const CDepletionStage &stage, dia::CLoadCase &lcase, const IValueDomainScalar::TValueVec &vcInitialPressures, const IValueDomainScalar::TValueVec& vcInitialTemperatures) = 0;
	virtual bool CreateStageLoads(const geo::IElement &element, const CDepletionStage &stage, dia::CLoadCase &lcase, const IValueDomainScalar::TValueVec &vcInitialPressures, const IValueDomainScalar::TValueVec& vcInitialTemperatures) = 0;
	virtual void CreateWeightLoad(dia::CLoadCase &lcase, const double &gravity) = 0;

	virtual bool FetchInitialTemperatures(const geo::IElement& element, const CDepletionStage& stage, IValueDomainScalar::TValueVec& vcInitialTemperatures);
	void FetchElementInitialTemperatures(const CDepletionStage& stage, const geo::IElement& element, IValueDomainScalar::TValueVec& vcInitialTemperatures);
	bool FetchElementInitialPressures(const CDepletionStage& stage, const geo::IElement& element, IValueDomainScalar::TValueVec& vcInitialPressures);
	void WriteElementInitialLoads(const CDepletionStage &stage, const geo::IElement &element, dia::CLoadCase &lcase, const IValueDomainScalar::TValueVec &vcInitialPressures, const IValueDomainScalar::TValueVec& vcInitialTemperatures);
	void WriteElementStageLoads(const CDepletionStage &stage, const geo::IElement &element, dia::CLoadCase &lcase, const IValueDomainScalar::TValueVec &vcInitialPressures, const IValueDomainScalar::TValueVec& vcInitialTemperatures);
  virtual void FetchStagePressures(const geo::IElement& element, const CDepletionStage& stage, IValueDomainScalar::TValueVec& vcPressures);
  virtual void FetchStageTemperatures(const geo::IElement& element, const CDepletionStage& stage, IValueDomainScalar::TValueVec& vcTemperatures);

	bool ProcessStages(dia::CLoadManager &manager, TStageMap &mpStages);

  virtual bool OnModifyInputFileNames(std::string& sComfileName, std::string& sDatfileName) const;
  virtual std::string InitialComFileText() const;

private:
  bool OnlyNewLoads(const CDepletionStage& startstage) const;
	void WriteNonlinCommands(const CDepletionStage& startstage, const CDepletionStage& endstage, int iStartS);
  bool DoWriteCommands(const CDepletionStage& startstage, const CDepletionStage& endstage, int iStartS);
  QString FaultInitCommand(CModelBase::TFaultInit nFaultInit);
};

#endif // _GEOMECCOMPACTIONDIANARUNNER_H_
