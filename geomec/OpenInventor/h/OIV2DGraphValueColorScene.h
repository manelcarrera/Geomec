#ifndef _OIV2DGraphValueColorScene_h_
#define _OIV2DGraphValueColorScene_h_

#include "OIV2DScene.h"

class OIV2DGraphValueColorScene : public OIV2DScene
{
  public:
  OIV2DGraphValueColorScene(CFemAppModel& model);
  OIV2DGraphValueColorScene(const QString& name, CFemAppModel& model);
  OIV2DGraphValueColorScene(unsigned int name, CFemAppModel& model);

  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version,
      TPROGRESS& progress);

  private:
//    OIV2DGraphValueColorScene(                // rely on default
//      const OIV2DGraphValueColorScene& rhs);  // implementation
//    OIV2DGraphValueColorScene(OIV2DGraphValueColorScene&& rhs);
//    OIV2DGraphValueColorScene& operator = (   // rely on default
//      const OIV2DGraphValueColorScene& rhs);  // implementation
//    OIV2DGraphValueColorScene& operator = (OIV2DGraphValueColorScene&& rhs);
};

#endif  // _OIV2DGraphValueColorScene_h_
