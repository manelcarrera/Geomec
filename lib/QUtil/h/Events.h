#pragma once

#include "Events_.h"

#include <map>
#include <vector>
#include <functional> //std::function

class CEvents
{
public:
	enum eEventsHandler{ GuiApp_EH, NonGuiApp_EH, RA_EH, DI_EH, GI_EH, DR_EH, HB_EH, HB2_EH, Question_EH, Controller_EH, Tests_EH, TI_EH, Undefined_EH };

	static const char* m_eh_cp[];

	typedef std::function< void( Cmd ) > TFunction_push;

	struct EventsHandler
	{
		EventsHandler( 
			eEventsHandler id_=Undefined_EH,
			TFunction_push push_cb_ = [](Cmd){},
			std::vector<eCmd> events_v_={},
			bool enabled_=true) : id(id_), push_cb(push_cb_), events_v(events_v_),enabled(enabled_){}

		eEventsHandler id;
		TFunction_push push_cb;
		std::vector<eCmd> events_v;
		bool enabled;
	};

	struct ErrorData
	{
		eEventsHandler sender; // sender
		int error_id;
	};

private:
	std::map<eEventsHandler,EventsHandler> m_eh_map;

private:
	CEvents(){}
	~CEvents(){}

public:
	static CEvents* instance();

	void broadcast( Cmd cmd );
	// for convenience
	void broadcast(std::vector<eCmd> cmd_v);
	void broadcast(eCmd cmd_id);

	void register_( const EventsHandler& eh );
	//
	// events handlers are not automatically unregistered so don't forget to unregister them !!!
	//
	void unregister( eEventsHandler id );

	void enable( eEventsHandler id, bool val =true );
	//for convenience
	void disable( eEventsHandler id ){ enable(id,false); };

	void add( eEventsHandler id, eCmd cmd );
	void remove( eEventsHandler id, eCmd cmd_id );
	void remove( eEventsHandler id, std::vector<eCmd> cmd_v );

	void set( eEventsHandler id, std::vector<eCmd> events_v );

	//
	// ra forwards msgs to doc in gui non-dsa scenario
	//
	void notify( Cmd cmd, eEventsHandler eh );
	void notify( eCmd cmd, eEventsHandler id );
	void notify(std::vector<eCmd> cmd_v, eEventsHandler id);

	//
	// is there any listener for a given event ?
	//
	bool registered( eCmd cmd_id );
};

namespace
{
	static CEvents* _e = CEvents::instance();
}

