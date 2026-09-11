#pragma once

#include <tbb/tbb.h>

class IT_Task : public tbb::task 
{
  HWND hWnd;
  tbb::task* execute();
public:
  IT_Task( HWND hWnd_ );
};

void Launch_IT_Task( HWND hWnd );

