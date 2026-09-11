#include "Wait.h"

#include "Printer.h"

#include <chrono> // duration

CWait::CWait(CEvents::eEventsHandler eh_id) 
  :	m_eh_id(eh_id),
    m_waiting(false)
{
  Printer::eModule module = 
    eh_id == CEvents::NonGuiApp_EH ?	Printer::RunWF :
    eh_id == CEvents::GI_EH ?			Printer::DiaE :
    eh_id == CEvents::Question_EH ?		Printer::Doc :		// FIXME
    eh_id == CEvents::Controller_EH ?	Printer::RunC : 
                      Printer::Gen;

  m_printer = Printer::instance( module );
}

CWait::~CWait()
{
  //
  // just in case
  //
  signal();
}

//
// res:
//	- false: only when timeout
//  - true: if not needed or signal received
//
bool CWait::wait(int timeout)
{
  bool res=true;

  if(m_waiting)
  {
    m_printer->error("wait : already waiting");
    return res;
  }
  m_waiting = true;

  std::unique_lock<std::mutex> mlock(m_mutex);

  if(timeout)
  {
    m_printer->info("wait_for >> (%d ms.)",timeout);
    //
    // res:
    //	- false: timeout
    //	- etrue: awaken due to a signal
    //
    res = m_cond_var.wait_for(mlock, std::chrono::milliseconds( timeout ), [=]{return !m_waiting;} ); // predicate == false -> continue waiting
    m_printer->info("wait_for : res: %d",res);
    int a=0;
  }
  else
  {
    m_printer->info("wait >>");
    m_cond_var.wait(mlock);
  }

  m_printer->info("<< wait released");

  return res;
}

void CWait::signal()
{
  if(!m_waiting)
  {
    m_printer->error("wait : already released");
    return;
  }

  m_waiting = false;

  std::unique_lock<std::mutex> mlock(m_mutex);
  m_printer->info("release wait >>");
  m_cond_var.notify_one();
}