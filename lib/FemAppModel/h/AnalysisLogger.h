#ifndef _ANALYSISLOGGER_H_
#define _ANALYSISLOGGER_H_

#include "IDCAnalysisLogger.h"
#include "StorageNode.h"

class CAnalysisLogger : public dia::IAnalysisLogger
{
public:
  CAnalysisLogger();
  ~CAnalysisLogger();

  // the number of progress steps that will be performed
  long SavedItems() const;

  // load and save the log to a streamfile
  void LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress);
  void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress) const;

private:
  bool m_bFilterBadShapeWarnings;
};

#endif // _ANALYSISLOGGER_H_