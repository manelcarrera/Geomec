#include "IListener.h"
//own
#include "Printer.h"
#include "Events.h"
#include "Global.h"

#include "QUtil.h" // namespace util::concurrency
using namespace util::concurrency;

//std
#include <thread>

IListener::IListener(const IListenerDefinition& d) : IListener(d.eh_id,d.events_v){}

IListener::IListener(
	CEvents::eEventsHandler eh_id,
	std::vector<eCmd> events_v_)

:	m_thread(nullptr), 
	m_eh_id( eh_id )
{
	Printer::eModule module = 
		eh_id == CEvents::GuiApp_EH ?		Printer::DocT :
		eh_id == CEvents::NonGuiApp_EH ?	Printer::RunWF :
		eh_id == CEvents::RA_EH ?			Printer::RunA :
		//
		eh_id == CEvents::DI_EH ?			Printer::DiaI :
		eh_id == CEvents::HB_EH ?			Printer::DiaH :	// Heartbeat
		//
		eh_id == CEvents::GI_EH ||
		eh_id == CEvents::DR_EH ?			Printer::DiaE :
		//
		eh_id == CEvents::Question_EH ?		Printer::Doc :		// FIXME
		eh_id == CEvents::Controller_EH ?	Printer::RunC : 
											Printer::Gen;
	m_printer = Printer::instance( module );
	//
	m_thread = new std::thread(&IListener::run, this);
	//
	CEvents::TFunction_push push_cb = std::bind( &IListener::push, this, std::placeholders::_1 );
	_e->register_( CEvents::EventsHandler{ eh_id, push_cb, events_v_ } );
}

IListener::~IListener()
{
	m_printer->debug("delete listener >>" );

	IListener::m_queue.clear(); 
	push_( Quit_Thread ); 
	threads::quit(m_thread,m_printer);

	_e->unregister( m_eh_id );
	m_printer->info("<< delete listener");
}

void IListener::push(Cmd cmd)
{
	if(	cmd.first!=Progress && 
		cmd.first!=Log && 
		cmd.first!=DianaMsg )
		m_printer->debug("push : %s", _cmd_s(cmd));

	m_queue.enqueue(cmd);
}

// for convinience
void IListener::push_(std::vector<eCmd> cmd_v)
{
	for( const auto cmd : cmd_v )
		push_(cmd);
}

// for convinience
void IListener::push_(eCmd cmd_id)
{
	push(make_cmd(cmd_id));
}

Cmd IListener::pop()
{
	Cmd cmd = m_queue.dequeue();

	if(	cmd.first!=Progress && 
		cmd.first!=Log && 
		cmd.first!=DianaMsg )
		m_printer->info("pop : %s", _cmd_s(cmd));

	return cmd;
}

void IListener::clear()
{
	m_queue.clear();
}

int IListener::size()
{
	return m_queue.size();
}

void IListener::run()
{
	m_printer->debug("run >>");

	Cmd cmd;
	for (;;)
	{
		cmd = pop();

		int cmd_id = cmd.first;
		if (cmd_id == eCmd::Quit_Thread)
			break;

		handle(cmd);
	} // forever

	m_printer->debug("<< run" );
}

