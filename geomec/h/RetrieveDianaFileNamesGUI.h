#ifndef _RetrieveDianaFileNamesGUI_h_
#define _RetrieveDianaFileNamesGUI_h_

class CModelBase;

#include "IRetrieveDianaFileNames.h"
#include "AnalysisType.h"

class CRetrieveDianaFileNamesGUI : public IRetrieveDianaFileNames
{
public:
  CRetrieveDianaFileNamesGUI(CAnalysisType::TAnalysisType analysisType,
    bool writeInputFiles, bool writeQuadDat, const CModelBase& model);
  virtual ~CRetrieveDianaFileNamesGUI();

  virtual bool retrieveDianaFileNames(QString& newTitle,
    const std::string& title) const;

private:
  CRetrieveDianaFileNamesGUI(const CRetrieveDianaFileNamesGUI& rhs);
  CRetrieveDianaFileNamesGUI& operator = (
    const CRetrieveDianaFileNamesGUI& rhs);

  QString stripExtension(const QString& string, const QString& extension) const;

  CAnalysisType::TAnalysisType m_analysisType;
  bool m_writeInputFiles;
  bool m_writeQuadDat;
  const CModelBase& m_model;
};

#endif  // _RetrieveDianaFileNamesGUI_h_
