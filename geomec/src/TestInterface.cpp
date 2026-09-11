#include "stdafx.h"

#include "TestInterface.h"

TI::TI(const std::string& token)
  :	IListener(CEvents::TI_EH, {Test_Ack,Test_End}), 
    IListenerIpc(IListenerIpc::Client,token)
{
}

void TI::handle(Cmd cmd)
{
  eCmd cmd_id = (eCmd)cmd.first;
  switch(cmd_id)
  {
    case HeartBeat:
      push_ipc( HeartBeat_Ack ); 
      break;
    //
    // forward them to the unitest
    //
    case Test_Ack:
    case Test_End:
    case Test_Step: // Test_Step: has data
      push_ipc( cmd ); 
      break;
    default:
      break;
  }
}

void TI::received(Cmd cmd) //ipc
{
  push(cmd);
}