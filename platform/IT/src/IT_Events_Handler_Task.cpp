#include "IT_Events_Handler_Task.h"

#include "IT_Events_Handler.h" //FIXME: 'lib' including 'app'

tbb::task* IT_Events_Handler_Task::execute() 
{
  IT_Events_Handler handler( gm_iface );
  handler.run();
  return NULL;
}
IT_Events_Handler_Task::IT_Events_Handler_Task( GeomecPythonInterface* gm_iface_, HWND hWnd_ ) : gm_iface( gm_iface_ ), hWnd( hWnd_ ) {}

void Launch_IT_Events_Handler_Task( GeomecPythonInterface* gm_iface, HWND hWnd ) 
{
  IT_Events_Handler_Task* t = new( tbb::task::allocate_root() ) IT_Events_Handler_Task( gm_iface, hWnd );
  tbb::task::enqueue( *t );
}