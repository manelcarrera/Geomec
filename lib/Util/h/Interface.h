#pragma once

#include <vector>

class IMesherDlg
{
public:
  virtual void DoModal_()=0;
  virtual void data( std::vector <std::string> msg_v )=0;
};
