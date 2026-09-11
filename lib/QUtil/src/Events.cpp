#include "Events.h"
#include "Printer.h"


namespace
{
	//
	// FIXME: create a new module
	//
	Printer* printer = Printer::instance(Printer::Evt);
}

 const char* CEvents::m_eh_cp[] = { "GuiApp_EH", "NonGuiApp_EH", "RA_EH", "DI_EH", "GI_EH", "DR_EH", "HB_EH", "HB2_EH", "Question_EH", "Controller_EH", "Tests_EH", "TI_EH", "Undefined_EH" };

CEvents* CEvents::instance()
{
	static CEvents* _instance = new CEvents();
	return _instance;
}

//
// overrides the existing one, if any
//
void CEvents::register_( const EventsHandler& eh )
{
	m_eh_map[eh.id]=eh;

	printer->debug("register : %s", m_eh_cp[eh.id]);
	auto print_ = [=]()
	{
		for (auto const& item : m_eh_map) // (eh id, eh struct)
		{
			const eEventsHandler& eh_id = item.first;
			const EventsHandler& eh = item.second;

			std::string s;
			for (auto const& evt_id : eh.events_v) // cmd_v
				s += g_cmd_s[evt_id] + ", ";
			printer->debug("** %s: %s", m_eh_cp[eh_id],s.c_str());
		}
	};
	//print_();
}

void CEvents::set( eEventsHandler id, std::vector<eCmd> events_v )
{
	if ( m_eh_map.find(id) == m_eh_map.end() ) 
		printer->error("set : %s not found", m_eh_cp[id]);
	else 
	{
		m_eh_map[id].events_v = events_v;
	}
}

void CEvents::add( eEventsHandler id, eCmd cmd_id )
{
	if ( m_eh_map.find(id) == m_eh_map.end() ) 
		printer->error("add %s: %s not found",  g_cmd_s[cmd_id].c_str(), m_eh_cp[id]);
	else 
	{
		std::vector<eCmd>& v = m_eh_map[id].events_v;
		if( std::find(v.begin(),v.end(),cmd_id) == v.end() ) //not found so Ok to be added
		{
			printer->debug("%s added to %s",  g_cmd_s[cmd_id].c_str(), m_eh_cp[id]);
			m_eh_map[id].events_v.push_back( cmd_id );
		}
		else
		{
			printer->error("%s was already at %s",  g_cmd_s[cmd_id].c_str(), m_eh_cp[id]);
		}
	}
}

void CEvents::remove( eEventsHandler id, eCmd cmd_id )
{
	if ( m_eh_map.find(id) == m_eh_map.end() ) 
		printer->error("remove %s from %s: eh not found",  g_cmd_s[cmd_id].c_str(), m_eh_cp[id]);
	else 
	{
		std::vector<eCmd>& v = m_eh_map[id].events_v;

		auto it = std::find(v.begin(), v.end(), cmd_id);
		if(it != v.end()) // found so Ok to be removed
		{
			printer->debug("%s removed from %s",  g_cmd_s[cmd_id].c_str(), m_eh_cp[id]);
			v.erase(it);
		}
		else
		{
			printer->error("remove %s from %s : cmd not found at this eh",  g_cmd_s[cmd_id].c_str(), m_eh_cp[id]);
		}
	}
}

void CEvents::remove( eEventsHandler id, std::vector<eCmd> cmd_v )
{
	for( const auto cmd : cmd_v )
		remove(id,cmd);
}

void CEvents::unregister( eEventsHandler id )
{
	m_eh_map.erase(id);
	printer->debug("un-register : %s", m_eh_cp[id]);
}

void CEvents::enable( eEventsHandler id, bool val )
{
	if ( m_eh_map.find(id) == m_eh_map.end() ) 
		printer->error("%s %s : not found", val ? "enable":"disable", m_eh_cp[id]);
	else 
	{
		printer->debug("%s: %s", m_eh_cp[id], val ? "enabled":"disabled");
		m_eh_map[id].enabled = val;
	}
}


void CEvents::notify( Cmd cmd, eEventsHandler id )
{
	if ( m_eh_map.find(id) == m_eh_map.end() ) 
		printer->error("forward %s: unknown event handler : %s", g_cmd_s[cmd.first].c_str(), m_eh_cp[id]);
	else 
	{
		printer->debug("push_f: %s -> %s", g_cmd_s[cmd.first].c_str(), m_eh_cp[id]);
		m_eh_map[id].push_cb(cmd);
	}
}

void CEvents::notify( eCmd cmd, eEventsHandler id )
{
	notify( make_cmd(cmd), id );
}

void CEvents::notify(std::vector<eCmd> cmd_v, eEventsHandler id)
{
	for( const auto cmd : cmd_v )
		notify(cmd,id);
}

//
// not efficient, maybe reimplement but not needed
//
void CEvents::broadcast( Cmd cmd )
{
	auto print_ = [=]()
	{
		for (auto const& item : m_eh_map) // (eh id, eh struct)
		{
			const eEventsHandler& eh_id = item.first;
			const EventsHandler& eh = item.second;

			std::string s;
			for (auto const& evt_id : eh.events_v) // cmd_v
				s += g_cmd_s[evt_id] + ", ";
			printer->error("**%s: %s: %s", eh.enabled?"E":"D",m_eh_cp[eh_id],s.c_str());
		}
	};


	bool found = false;
	bool has_params = true;
	int cmd_id = cmd.first;
	for (auto const& item : m_eh_map) // (eh id, eh struct)
	{
		eEventsHandler eh_id = item.first;
		const EventsHandler& eh = item.second;

		if(!eh.enabled)
			continue;

		for (auto const& evt_id : eh.events_v) // cmd_v
		{
			if(evt_id == cmd_id) // is the event registered in this event handler ?
			{
				if(	cmd_id!=Progress && 
					cmd_id!=Log && 
					cmd_id!=DianaMsg)

					printer->debug("push: %s -> %s", g_cmd_s[evt_id].c_str(), m_eh_cp[eh_id]);

				has_params = cmd.second != nullptr;
				eh.push_cb(cmd);

				found=true;
				break;	// go to next event handler
						// event can be needed by multiples classes (eith their event handler)
			}
		}
		//
		// FIXME: patch to avoid same evevnt treated twice as second time data has already been deleted
		//
		if(found && has_params)
			break;
	}
	if(!found)
	{
		//
		// print all the eh with all the registered events
		//
		printer->error("event not registerd in any event handler : %s (%d)", g_cmd_s[cmd_id].c_str(), cmd_id);
		print_();
	}
}

bool CEvents::registered( eCmd cmd_id )
{
	bool found = false;
	for (auto const& item : m_eh_map) // (eh id, eh struct)
	{
		eEventsHandler eh_id = item.first;
		const EventsHandler& eh = item.second;

		if(!eh.enabled)
			continue;

		for (auto const& evt_id : eh.events_v) // cmd_v
		{
			if(evt_id == cmd_id) // is the event registered in this event handler ?
			{
				found=true;
				break;
			}
		}
		if(found)
			break;
	}
	return found;
}


void CEvents::broadcast(std::vector<eCmd> cmd_v)
{
	for (const auto cmd : cmd_v)
		broadcast(cmd);
}

void CEvents::broadcast(eCmd cmd_id)
{
	broadcast(make_cmd(cmd_id));
}