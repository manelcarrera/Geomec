#include "GlobalMessage_GUI.h"
#include "GlobalMessage.h" // also helper with static functions

#include "QuestionThreadWrapper.h"

//
// main functions
//
int GlobalMessage_GUI::msg(const char *message, unsigned int style, unsigned int contextID) {
  GlobalMessage_GUI::GMData *data = new GlobalMessage_GUI::GMData{message, style, contextID};

  CQuestionThreadWrapper th(CQuestionThreadWrapper::GlobalMessage,
                            data); // event send to doc / wait for the resposne / blocking
  th.join();
  return th.response();
}

int GlobalMessage_GUI::msg(unsigned int message, unsigned int style, unsigned int contextID) {
  return msg(std::to_string(message).c_str(), style, contextID);
}

void GlobalMessage_GUI::status(const QString &message) {
  _e->broadcast(make_cmd(eCmd::Status, new std::string(message.toStdString())));
}
