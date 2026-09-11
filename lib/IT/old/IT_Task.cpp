#include "IT_Task.h"

#include "Printer.h"

tbb::task* IT_Task::execute() 
{
	Printer::_print("---------------------------------------------------------------------------");
	//FIXME
	IT_Interface::launch();					// Do long computation
	Printer::_print("///////////////////////////////////////////////////////////////////////////");
	return NULL;
}

IT_Task::IT_Task( HWND hWnd_ ) : hWnd( hWnd_ ) {}

void Launch_IT_Task( HWND hWnd ) 
{
	IT_Task* t = new( tbb::task::allocate_root() ) IT_Task( hWnd );
	tbb::task::enqueue( *t );
}
