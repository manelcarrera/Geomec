#ifndef _GVTVelocityModelFile_h_
#define _GVTVelocityModelFile_h_

#include <QSharedPointer>

class CModelBase;

#include "GVTVelocityModelSource.h"

namespace GVT
{

class CGVTVelocityModelFile
{
  public:
    static struct TVelocityFromFile {} velocityFromFile;
    static struct TVelocityFromMaterial {} velocityFromMaterial;

    CGVTVelocityModelFile(const TVelocityFromFile&, CModelBase &modelBase,
      const QString& velocityModelFile = QString());
    CGVTVelocityModelFile(const TVelocityFromMaterial&, CModelBase &modelBase);
    ~CGVTVelocityModelFile();

    CGVTVelocityModelFile(const CGVTVelocityModelFile& rhs);
    CGVTVelocityModelFile& operator = (const CGVTVelocityModelFile& rhs);

    void SetContents(const std::stringstream& buffer);

    const QString& velocityModelFile() const;
    void velocityModelFile(const QString& velocityModelFile);

    bool isVelocityModelFileReady() const;

    int velocityModelSourceNumerator() const;
    void velocityModelSourceNumerator(int velocityModelSourceNumerator);

    void LoadStream(TSTREAM& stream, CStreamVersion& version,
      TPROGRESS& progress);
    void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  private:
    void swap(CGVTVelocityModelFile& gvtVelocityModelFile);

    void tryVelocityFromMaterial();

    QSharedPointer <CGVTVelocityModelSource> m_GVTVelocityModelSource;
    CModelBase& m_modelBase;
};

} // namespace GVT

#endif  // _GVTVelocityModelFile_h_
