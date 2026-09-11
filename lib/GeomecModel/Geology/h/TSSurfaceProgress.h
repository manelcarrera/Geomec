#ifndef _TSSURFACEPROGRESS_H_
#define _TSSURFACEPROGRESS_H_

#include <memory>

class IProgressBase;

#include "TSProgressBase.h"

class CTSSurfaceProgress : public ITSProgressBase
{
private:
	std::auto_ptr <IProgressBase> m_pWaitCursor;
	std::auto_ptr <IProgressBase> m_progress;
public:
	CTSSurfaceProgress();
	virtual ~CTSSurfaceProgress();

	virtual void Begin(int nSteps);
	virtual void End();
	virtual void Step();
};

#endif // _TSSURFACEPROGRESS_H_