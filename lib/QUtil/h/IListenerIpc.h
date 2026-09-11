#pragma once

#include "SafeQueue.h"
#include "Events.h" // Cmd

namespace std{ class thread; }
class Printer;
struct trace_queue;

class IListenerIpc
{
protected:
  enum eType{Server,Client};

  Printer* m_printer_ipc;

private:
  std::thread* m_thread_listen;
  std::thread* m_thread_send;
  //
  SafeQueue< Cmd > m_queue;
  //
  trace_queue* m_data_listen;
  trace_queue* m_data_send;

  bool m_b_quit_listen;

  eType m_type;

  std::string m_token;

private:
  Cmd pop();
  void quit_threads();

  virtual void received(Cmd cmd)=0;

  void listen();
  void send();

protected:
  void token(const std::string& val){m_token=val;};

public:
  IListenerIpc(eType type, const std::string& token_=std::string());
  virtual ~IListenerIpc();
  //
  void push_ipc(Cmd cmd);
  void push_ipc(eCmd cmd_id);
  void push_ipc(std::vector<eCmd> cmd_v);
  //
  std::string token(){return m_token;};	
};