#include "Heartbeat.h"

//own
#include "Printer.h" 
#include "ISettings.h" 

#include "QUtil.h" // namespace util::concurrency
using namespace util::concurrency;

//std
#include <thread>

//qt
#include <QStringList> // split boost ptime


//
// 10 ms. : works fine
//
static const int HB_TIMEOUT = 1000;

HB::HB( CEvents::eEventsHandler eh, 
    CEvents::eEventsHandler notify_to )
:	IListener( eh, {} ),
  m_th( nullptr ),
  m_b_quit_hb( false ),
  m_b_hb( false ),
  m_hb_timeout(HB_TIMEOUT),
  m_notify_to( notify_to )
{
  m_printer->debug("hb : new : default timeout: %d", hb_timeout());
}

HB::~HB()
{
  m_printer->info("delete hb >>");

  m_b_quit_hb = true; // needed to force quit the thread loop
  threads::quit(m_th,m_printer);

  m_printer->info("delete hb <<");
}

//
// second_clock::universal_time():
//	don't use  it as resolution is seconds 
//	this was a problem when ack is not received for the first time
//	time sent and time rec were equal when assigned in the same second
//
void HB::hb() // thread
{
  m_time_rec = microsec_clock::universal_time();
  
  m_time_sent = m_time_rec;

  m_b_quit_hb = false;

  m_printer->info("hb : timeout: %d", m_hb_timeout);

  m_printer->info("hb >>");

  for(;;)
  {
    auto print_=[=]()
    {
      auto time_=[](const ptime& pt){ return QString::fromStdString( to_simple_string( pt ) ).split(" ")[ 1 ].toStdString(); };

      std::string t_send = time_( m_time_sent );
      std::string t_rec = time_( m_time_rec );

      ptime now = microsec_clock::universal_time();
      std::string t_now = time_( now );
      //
      // 'now' - 'sent' = around 100 ms. (not too usefull) ... more useful the diff between 'sent' and 'rec'
      //
      //time_duration diff = (now - m_time_sent)/1000; // microsencds to miliseconds

      m_printer->debug("hb : tic ... s -> %s, r <- %s, now:%s ... quit:%s ... res:%s", t_send.c_str(), t_rec.c_str(), t_now.c_str(), m_b_quit_hb ? "yes":"no", m_time_sent > m_time_rec ? "Error":"Ok" );
    };
    print_();

    //
    // a) force quit (after run is finished, no error)
    //
    if( m_b_quit_hb )
    {
      m_printer->info("hb quit requested" );
      break; // quit the loop
    }
    //
    // b) don't quit
    //
    else
    {
      bool ack_received = m_time_rec >= m_time_sent; // '=' for the first time
      //
      // b1) ack received -> send a new tic
      //
      if( ack_received ) 
      {
        if( !m_b_hb )					// set to false on ack
          push_( eCmd::HeartBeat );	// send only if ack received
      }
      //
      // b2) ack not received -> error
      //
      // ack received after sending hb and waiting for the timeout in the sleep below
      //
      else
      {
        m_printer->error("trigger hb error" );

        //
        // test-only, queue clear -> it works!
        //
        auto add_commands=[=](){
          for( int i=0; i < 10; i++ )
            push_( eCmd::DoNothing );
          m_printer->info("hb : size:%d", IListener::m_queue.size() );
        };
        //add_commands();

        IListener::m_queue.clear();			// this works fine
        push_( eCmd::HeartBeat_Error );
        break;
      }
      //
      // TODO: this must be here or inside the if?
      //
      std::this_thread::sleep_for(std::chrono::milliseconds(hb_timeout()));
    }
  }

  m_printer->info("hb <<");
}

void HB::handle(Cmd cmd)
{
  eCmd cmd_id = (eCmd)cmd.first;

  auto quit_hb=[=](){			
    m_b_quit_hb = true;
    threads::quit(m_th,m_printer);
  };

  auto notify=[=](Cmd cmd)
  {			
    if(m_notify_to==CEvents::Undefined_EH)
      _e->broadcast(cmd); // broadcast
    else
      _e->notify(cmd,m_notify_to); // notify_to
  };

  switch( cmd_id )
  {
    //
    // 1) from outside (DI)
    //
    case eCmd::Start_HB:
    {
      quit_hb(); // just in case
      m_th = new std::thread( &HB::hb, this );
      break;
    }
    case eCmd::Quit_HB:
    {
      quit_hb();
      break;
    }
    case eCmd::HeartBeat_Ack:
    {
      m_b_hb = false;
      m_time_rec = microsec_clock::universal_time();
      break;
    }
    //
    // 2) internal events: forward them to DI o broadcasted
    //
    case eCmd::HeartBeat:	
    {
      m_b_hb = true;
      m_time_sent = microsec_clock::universal_time();

      notify(cmd);
      break;
    }
    case eCmd::HeartBeat_Error:	
    {
      notify(cmd);
      break;
    }
    default:
    {
      m_printer->error("cmd not handled : %s", g_cmd_s[ cmd.first ].c_str());
      break;
    }
  }
}