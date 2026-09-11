#pragma once

#include "IListener.h"

//
// non-gui also has thread but is different to gui one
//
// TODO: much more common functionality needs to be moved to here from gui and from non-gui derived classes
//
class IRunAnalysis : public IListener
{
public:
	IRunAnalysis(const IListenerDefinition& d);
	IRunAnalysis(
		CEvents::eEventsHandler eh_id = CEvents::Undefined_EH,
		std::vector<eCmd> events_v_={});
	virtual ~IRunAnalysis();
protected:
	virtual void handle(Cmd cmd)=0;
};