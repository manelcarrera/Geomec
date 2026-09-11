#ifndef _GVTVelocityModelFromMaterial_h_
#define _GVTVelocityModelFromMaterial_h_

class CModelBase;

#include "GVTVelocityModelSource.h"

namespace GVT {

class CGVTVelocityModelFromMaterial : public CGVTVelocityModelSource {
public:
  CGVTVelocityModelFromMaterial(CModelBase &modelBase);
  virtual ~CGVTVelocityModelFromMaterial();

  virtual void SetContents(const std::stringstream &buffer);

  virtual const QString &velocityModelFile() const;
  virtual void velocityModelFile(const QString &velocityModelFile);

  virtual bool isVelocityModelFileReady() const;
  virtual int velocityModelSourceNumerator() const;

  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);

private:
  CGVTVelocityModelFromMaterial(const CGVTVelocityModelFromMaterial &rhs);
  CGVTVelocityModelFromMaterial &operator=(const CGVTVelocityModelFromMaterial &rhs);

  // static QString createVelocityModelFile(CModelBase& modelBase);

  const QString m_velocityModelFile;
};

} // namespace GVT

#endif // _GVTVelocityModelFromMaterial_h_
