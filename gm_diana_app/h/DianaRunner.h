#pragma once

//own
#include "IListener.h"

class IListenerIpc;

class DR : public IListener
{
  IListenerIpc* m_gi;

  bool m_done;
  void done(bool val){m_done=val;}

protected:
  void handle(Cmd cmd);

public:
  std::thread* m_th; // run diana takes time

public:

  DR( IListenerIpc* gi );
  ~DR();
  
  void on_run_diana(Cmd& cmd);

  void run_( const QStringList& params ); // thread funtion

  bool done(){return m_done;}
};