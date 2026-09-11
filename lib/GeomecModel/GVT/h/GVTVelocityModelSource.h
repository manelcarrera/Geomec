#ifndef _GVTVelocityModelSource_h_
#define _GVTVelocityModelSource_h_

#include "StorageNode.h"

#include <sstream>

namespace GVT
{

typedef CStorageNode::TSTREAM TSTREAM;
typedef CStorageNode::TPROGRESS TPROGRESS;

const int VELOCITY_FROM_FILE = 0;
const int VELOCITY_FROM_MATERIAL = 1;

class CGVTVelocityModelSource
{
  public:
    CGVTVelocityModelSource();
    virtual ~CGVTVelocityModelSource() = 0;

    virtual void SetContents(const std::stringstream&);

    virtual const QString& velocityModelFile() const = 0;
    virtual void velocityModelFile(const QString& velocityModelFile) = 0;

    virtual bool isVelocityModelFileReady() const = 0;
    virtual int velocityModelSourceNumerator() const = 0;

    virtual void LoadStream(TSTREAM& stream, CStreamVersion& version,
      TPROGRESS& progress) = 0;
    virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress) = 0;

  private:
    CGVTVelocityModelSource(const CGVTVelocityModelSource& rhs);
    CGVTVelocityModelSource& operator = (const CGVTVelocityModelSource& rhs);
};

} // namespace GVT

#endif  // _GVTVelocityModelSource_h_
