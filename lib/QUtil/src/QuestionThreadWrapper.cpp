#include "QuestionThreadWrapper.h"

// std
#include <thread>

const char *CQuestionThreadWrapper::m_question_cp[] = {"GlobalMessage", "DatFileDlg", "MesherDlg"};

CQuestionThreadWrapper::CQuestionThreadWrapper(eType type_, void *data)
    : IListener(CEvents::Question_EH, {Response}), m_data({type_, data}),
      m_response(DataRes{-1, std::string()}) // no response
{
  _e->broadcast(make_cmd(Question, new Data{type_, data}));
}

CQuestionThreadWrapper::~CQuestionThreadWrapper() {}

void CQuestionThreadWrapper::handle(Cmd cmd) {
  int cmd_id = cmd.first;
  if (cmd_id == eCmd::Response) {
    m_response = *reinterpret_cast<DataRes *>(cmd.second);
    delete cmd.second;

    push_(Quit_Thread);
  }
}

void CQuestionThreadWrapper::join() { m_thread->join(); }
