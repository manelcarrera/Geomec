#ifndef _WriteLogASCIIStandardFile_h_
#define _WriteLogASCIIStandardFile_h_

class CNewWellPath;

#include "ExportFormat.h"

class CWriteLogASCIIStandardFile
{
  public:
  CWriteLogASCIIStandardFile(const CNewWellPath* newWellPath);

  void writeLogASCIIStandardFile(const QString& fileName,
      const IExportFormat::TDataVec& dataVector,
      const IExportFormat::CExportArg& arguments);

  private:
  CWriteLogASCIIStandardFile(const CWriteLogASCIIStandardFile& rhs);
  CWriteLogASCIIStandardFile& operator = (CWriteLogASCIIStandardFile rhs);

  const CNewWellPath* m_newWellPath;
};

#endif  // _WriteLogASCIIStandardFile_h_
