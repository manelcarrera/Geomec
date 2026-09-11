#pragma once

//own
#include "IListener.h"
//boost
#include <boost/date_time/posix_time/posix_time.hpp> //hb

using namespace boost::posix_time; //ptime


/////////////////////////////////////////////////////////////////
//
//			diana interface (DI)
//
/////////////////////////////////////////////////////////////////

class HB : public IListener
{
  ptime m_time_sent;
  ptime m_time_rec;

  bool m_b_quit_hb;

  bool m_b_hb; // sent but not response received

  int m_hb_timeout;

  std::thread* m_th; // heartbeat

  CEvents::eEventsHandler m_notify_to;


protected:
  void handle(Cmd cmd);//=0

public:
  HB(	CEvents::eEventsHandler eh = CEvents::HB_EH, 
    CEvents::eEventsHandler notify_to = CEvents::Undefined_EH );
  ~HB();

  //
  // '0' disables timeout mechanism, for test purpose
  //
  void hb_timeout( int val ) { m_hb_timeout = val; };
  int hb_timeout(){ return m_hb_timeout; };

  void hb();
};

