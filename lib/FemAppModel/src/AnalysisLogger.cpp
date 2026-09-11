
#include "AnalysisLogger.h"
#include "StreamVersion.h"

CAnalysisLogger::CAnalysisLogger()
{
}

CAnalysisLogger::~CAnalysisLogger()
{
}

// the number of progress steps that will be performed
long CAnalysisLogger::SavedItems() const
{
  return LineSize();
}

// load and save the log to a streamfile
void CAnalysisLogger::LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress)
{
  Empty();
  int nLines;
  stream >> nLines;

  int i;
  for(i = 0; i < nLines; i++)
  {
    QString str;
    stream >> str;
  int nVerbose = 0;
  if(version >= CStreamVersion(3, 7, 21))
      stream >> nVerbose;
  AddLine(str.toStdString(), (nVerbose != 0));
    progress.Step();
  }
}

void CAnalysisLogger::SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress) const
{
  stream << LineSize();
  int i;
  for(i = 0; i < LineSize(); i++)
  {
    stream << QString(Line(i).c_str());
  stream << int(Verbose(i) ? 1 : 0);
    progress.Step();
  }
}
