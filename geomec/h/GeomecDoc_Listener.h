#pragma once

#include "IListener.h"

class CGeomecDoc_Listener : public IListener
{
protected:
	virtual void handle(Cmd cmd);

public:
	CGeomecDoc_Listener();
	~CGeomecDoc_Listener();
};