#include "stdafx.h"

#include "GlobalMessage.h" // also helper with static functions
#include "GlobalMessage_GUI_MFC.h"

#include "QuestionThreadWrapper.h"

#include "GmHelper.h" // gm::send::status

//
// main functions
//
int GlobalMessage_GUI_MFC::msg(const char *message, unsigned int style, unsigned int contextID) {
  return AfxMessageBox(message, style, contextID);
}

int GlobalMessage_GUI_MFC::msg(unsigned int message, unsigned int style, unsigned int contextID) {
  return AfxMessageBox(message, style, contextID);
}

void GlobalMessage_GUI_MFC::status(const QString &message) { gm::send::status(message); }

//
// functions for convenience
//
int GlobalMessage_GUI_MFC::msg(const QString &message, unsigned int style, unsigned int contextID) {
  return msg(message.toStdString().c_str(), style, contextID);
}

int GlobalMessage_GUI_MFC::msg(const std::vector<std::string> &message, unsigned int style, unsigned int contextID) {
  std::string newMessage = constructMessage(message);

  return msg(newMessage.c_str(), style, contextID);
}