#include "IListenerIpc.h"
//std
#include <thread>
//boost
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
//own
#include "Printer.h"
#include "Events.h"
#include "Global.h"

#include "QUtil.h" // namespace util::concurrency
using namespace util::concurrency;

#include "doc_anonymous_condition_shared_data_MOD.h"

using namespace boost::interprocess;
using namespace boost::posix_time; //ptime


IListenerIpc::IListenerIpc(eType type, const std::string& token_) : 
	m_type(type),
	//
	// FIXME: pass them to the thread as a parameters, instead of make them members
	//
	m_b_quit_listen( false )
{
	m_printer_ipc = Printer::instance( 
		type == Server ?	Printer::DiaI : 
							Printer::DiaE );
	if( type==Server )
	{
		std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(
			std::chrono::system_clock::now().time_since_epoch()	);

		token( std::to_string( ms.count() ) );
	}
	else // Client
	{
		token( token_ );
	}


	m_thread_listen = new std::thread(&IListenerIpc::listen, this );
	m_thread_send = new std::thread(&IListenerIpc::send, this );
}

void IListenerIpc::quit_threads()
{
	auto quit_thread_send = [=]()
	{
		m_printer_ipc->debug("ipc: quit thread : send >>");

		//
		// release the wait
		//
		if(m_data_send)
			m_data_send->cond_full.notify_one();

		IListenerIpc::m_queue.clear(); 
		push_ipc( Quit_Thread );

		threads::quit(m_thread_send,m_printer_ipc);

		m_printer_ipc->debug("<< ipc: quit thread : send");
	};

	auto quit_thread_listen = [=]()
	{
		m_printer_ipc->debug("ipc: quit thread : listen >>");

		m_b_quit_listen = true;

		//
		// release the wait
		//
		if(m_data_listen)
			m_data_listen->cond_empty.notify_one();

		threads::quit(m_thread_listen,m_printer_ipc);

		m_printer_ipc->debug("<< ipc: quit thread : listen");
	};

	quit_thread_listen();
	quit_thread_send();
}

IListenerIpc::~IListenerIpc()
{

	m_printer_ipc->debug("ipc: delete listener >>");

	quit_threads();

	m_printer_ipc->info("<< ipc: delete listener");
}

//
// shm: create_only / open_only / open_or_create
//
void IListenerIpc::listen()
{
	m_printer_ipc->debug("listen_ipc >>");

	std::string shm_name = "shm-"+token();
	if(m_type==Server)
		shm_name+="-rec";
	m_printer_ipc->debug("listen_ipc shm name:%s", shm_name.c_str());

	try
	{
		shared_memory_object shm( open_or_create, shm_name.c_str(), read_write );

		if(m_type==Server)
			shm.truncate(sizeof(trace_queue));
		
		mapped_region region( shm, read_write );
		
		void* addr = region.get_address();
		
		trace_queue* data = m_type==Server ? 
			new (addr) trace_queue : 
			static_cast< trace_queue* >( addr ); 
		
		m_data_listen = data;
		
		m_printer_ipc->debug("listen_ipc : shm : Ok");

		auto timeout = [=]( int delay )
		{
			return second_clock::universal_time() + seconds( delay );
		};

		for(;;)
		{
			scoped_lock<interprocess_mutex> lock(data->mutex);
			//
			// first implementation: regular wait
			//
			if( !data->message_in )
				data->cond_empty.wait(lock); // FIXME: timed_wait and while
			//
			// timed_wait
			//
			//while( !data->message_in )
			//	bool res =  data->cond_empty.timed_wait( lock, timeout( 1 ) ); // FIXME: timed_wait and while

			//
			// quit thread !!!
			//
			if( m_b_quit_listen )
			{
				//
				// why this? mcr 2020-09-22
				//
				data->message_in = false;
				data->cond_full.notify_one();
				//
				break;
			}

			//m_printer_ipc->debug("listen_ipc : %s <<", _cmd_s(data->cmd_id) );

			received( Cmd( data->cmd_id, new std::string( data->param ) ) );

			data->message_in = false;
			data->cond_full.notify_one();
		}
	}
	catch( interprocess_exception &ex )
	{
		m_printer_ipc->error("listen_ipc : exception : %s", ex.what() );
	}

	if(m_type==Server)
		shared_memory_object::remove(shm_name.c_str());

	m_data_listen = nullptr;

	m_printer_ipc->debug("<< listen_ipc");
}

//void IListenerIpc::send( const std::string& shm_name )
void IListenerIpc::send()
{
	m_printer_ipc->debug("send_ipc >>");

	std::string shm_name = "shm-"+token();
	if(m_type==Client)
		shm_name+="-rec";

	//std::string shm_name = m_shm_name_send;
	m_printer_ipc->debug("send_ipc shm name:%s", shm_name.c_str());
	/*struct shm_remove{
		std::string name;
		shm_remove(const std::string& name_):name(name_){ shared_memory_object::remove(name.c_str());  }
		~shm_remove(){ shared_memory_object::remove(name.c_str());  }
	} remover(shm_name);*/


	try
	{
	   shared_memory_object shm( open_or_create, shm_name.c_str(), read_write );

		if(m_type==Server)
			shm.truncate(sizeof(trace_queue));

		mapped_region region( shm, read_write );

		void* addr = region.get_address();

		trace_queue* data = m_type==Server ? 
			new (addr) trace_queue : 
			static_cast< trace_queue* >( addr ); 
		m_printer_ipc->debug("send_ipc : shm : Ok");

		m_data_send = data;

		auto send_ = [=]( Cmd cmd )
		{
			//m_printer_ipc->debug("send_ipc: %s >>", _cmd_s( cmd ) );
			scoped_lock<interprocess_mutex> lock(data->mutex); 
			if( data->message_in )
				data->cond_full.wait(lock);
			//while( data->message_in )
			//	bool res = data->cond_full.timed_wait( lock, second_clock::universal_time() + seconds( 1 ) );

			data->cmd_id = cmd.first; // does this persist?

			std::string msg;
			void* data_ = cmd.second;
			if(data_)
			{
				msg = *reinterpret_cast< std::string* >(data_);
				delete data_;
			}
			std::snprintf( data->param, trace_queue::Length_, msg.c_str() );

			data->cond_empty.notify_one();	// Notify to the other process that there is a message
			data->message_in = true;		// Mark message buffer as full
		};

		for(;;)
		{
			Cmd cmd = pop(); // FIXME: It looks like if this is blocking things...

			if( cmd.first == eCmd::Quit_Thread )
				break;

			send_(cmd);
		} // forever
	}
	catch( interprocess_exception &ex )
	{
		m_printer_ipc->error("send_ipc : exception : %s", ex.what() );
	}

	if(m_type==Server)
		shared_memory_object::remove(shm_name.c_str());

	m_data_send = nullptr;

	m_printer_ipc->debug("<< send_ipc");
}

void IListenerIpc::push_ipc(Cmd cmd)
{
	if(	cmd.first!=Progress && 
		cmd.first!=Log && 
		cmd.first!=DianaMsg)
		m_printer_ipc->debug("push_ipc : %s", _cmd_s(cmd));

	m_queue.enqueue(cmd);
}

// for convinience
void IListenerIpc::push_ipc(std::vector<eCmd> cmd_v)
{
	for( const auto cmd : cmd_v )
		push_ipc(cmd);
}

// for convinience
void IListenerIpc::push_ipc(eCmd cmd_id)
{
	push_ipc(make_cmd(cmd_id));
}

Cmd IListenerIpc::pop()
{
	Cmd cmd = m_queue.dequeue();

	if(	cmd.first!=Progress && 
		cmd.first!=Log && 
		cmd.first!=DianaMsg)
		m_printer_ipc->info("pop_ipc : %s", _cmd_s(cmd));

	return cmd;
}