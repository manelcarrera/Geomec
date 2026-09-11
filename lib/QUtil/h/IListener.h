#pragma once

#include "Events.h"
#include "SafeQueue.h"

namespace std {
class thread;
}
class Printer;

struct IListenerDefinition {
  CEvents::eEventsHandler eh_id;
  std::vector<eCmd> events_v;
};

class IListener {
  CEvents::eEventsHandler m_eh_id;

private:
  void run();
  Cmd pop();
  virtual void handle(Cmd cmd) = 0;

protected:
  std::thread *m_thread;  // join()
  SafeQueue<Cmd> m_queue; // clear()
  Printer *m_printer;

public:
  IListener(const IListenerDefinition &d);
  IListener(CEvents::eEventsHandler eh_id = CEvents::Undefined_EH, std::vector<eCmd> events_v_ = {});
  virtual ~IListener();
  //
  void push(Cmd cmd);
  void push_(eCmd cmd_id);
  void push_(std::vector<eCmd> cmd_v);
  //
  // not essential but maybe useful
  //
  void clear();
  int size();
};