#ifndef _cora_ResponseParameterFile_h_
#define _cora_ResponseParameterFile_h_

#include <fstream>

#include <QString>

namespace cora
{

class CResponseParameterFile
{
  public:
    CResponseParameterFile(const QString& fileName);

    void addResponseParameter(const QString& responseParameter);

  private:
    CResponseParameterFile(const CResponseParameterFile& rhs);
    CResponseParameterFile& operator = (CResponseParameterFile rhs);

    std::ofstream m_responseParameterFile;
};

} // namespace cora

#endif  // _cora_ResponseParameterFile_h_
