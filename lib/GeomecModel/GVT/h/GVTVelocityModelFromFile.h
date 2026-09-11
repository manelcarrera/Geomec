#ifndef _GVTVelocityModelFromFile_h_
#define _GVTVelocityModelFromFile_h_

#include "GVTVelocityModelSource.h"

namespace GVT
{

class CGVTVelocityModelFromFile : public CGVTVelocityModelSource
{
  public:
  CGVTVelocityModelFromFile(const QString& velocityModelFile = QString());
  virtual ~CGVTVelocityModelFromFile();

  virtual const QString& velocityModelFile() const;
  virtual void velocityModelFile(const QString& velocityModelFile);

  virtual bool isVelocityModelFileReady() const;
  virtual int velocityModelSourceNumerator() const;

  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version,
      TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  private:
  CGVTVelocityModelFromFile(const CGVTVelocityModelFromFile& rhs);
  CGVTVelocityModelFromFile& operator = (
      const CGVTVelocityModelFromFile& rhs);

  QString m_velocityModelFile;
};

} // namespace GVT

#endif  // _GVTVelocityModelFromFile_h_
