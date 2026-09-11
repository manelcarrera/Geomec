#pragma once

#include "IListener.h"

//
// for further developments resposne can be of any type
//
class CQuestionThreadWrapper : public IListener {
public:
  enum eType { GlobalMessage, DatFileDlg, MesherDlg };

  static const char *m_question_cp[];

  // query
  struct Data {
    eType id;
    void *data;
  };

  // response
  struct DataRes {
    int res;
    std::string res_s;
  };

protected:
  virtual void handle(Cmd cmd);

private:
  DataRes m_response;
  Data m_data;

public:
  // response
  int response() { return m_response.res; }
  std::string response_s() { return m_response.res_s; }
  // query
  Data data() { return m_data; }

public:
  CQuestionThreadWrapper(eType type_, void *data = nullptr);
  ~CQuestionThreadWrapper();

  void join();
};