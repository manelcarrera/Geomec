#ifndef _IT_Tasks
#define _IT_Tasks

#include "IT_Interface.h"
#include "IT_Events_Handler.h"
#include "Printer.h"



//------------------------------------------------------------------------------ IT_Task
class IT_Task : public tbb::task 
{
  HWND hWnd;
  tbb::task* execute() 
  {
    Printer::_print("---------------------------------------------------------------------------");
    IT_Interface::launch();					// Do long computation
    Printer::_print("///////////////////////////////////////////////////////////////////////////");
    return NULL;
  }
public:
  IT_Task( HWND hWnd_ ) : hWnd( hWnd_ ) {}
};

void Launch_IT_Task( HWND hWnd ) 
{
  IT_Task* t = new( tbb::task::allocate_root() ) IT_Task( hWnd );
  tbb::task::enqueue( *t );
}
//------------------------------------------------------------------------------ IT_Events_Handler_Task
class IT_Events_Handler_Task : public tbb::task 
{
  HWND hWnd;

  tbb::task* execute() 
  {
    IT_Events_Handler handler;
    handler.run();
    return NULL;
  }
public:
  IT_Events_Handler_Task( HWND hWnd_ ) : hWnd( hWnd_ ) {}
};

void Launch_IT_Events_Handler_Task( HWND hWnd ) 
{
  IT_Events_Handler_Task* t = new( tbb::task::allocate_root() ) IT_Events_Handler_Task( hWnd );
  tbb::task::enqueue( *t );
}

#endif

