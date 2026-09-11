#ifndef _ReadOnlyScene_h_
#define _ReadOnlyScene_h_

#include "OpenGLSceneBase.h"

class CReadOnlyScene : public COpenGLSceneBase
{
  public:
    CReadOnlyScene(const QString& type, CFemAppModel& femAppModel);
    virtual ~CReadOnlyScene();

    virtual void LoadStream(TSTREAM& stream, CStreamVersion &version,
      TPROGRESS& progress);
    virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  private:
    CReadOnlyScene(const CReadOnlyScene& rhs);
    CReadOnlyScene& operator = (const CReadOnlyScene& rhs);

    const QString m_type;
};

#endif  // _ReadOnlyScene_h_
