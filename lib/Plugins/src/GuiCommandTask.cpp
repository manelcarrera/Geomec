#include "GuiCommandTask.h"

#include "GmInterfaceGui.h"

#include "SafeQueue.h"

GuiCommandTask::GuiCommandTask( GmInterfaceGui* gm_iface_, Command _cmd, HWND hWnd_  ) 
	: gm_iface( gm_iface_ ), cmd( _cmd ), hWnd( hWnd_ )
{}


tbb::task* GuiCommandTask::execute() 
{
	// process CMD

	// return response in a CMD
	gm_iface->results_queue().enqueue( cmd );
	return NULL;
}

void Launch_GuiCommandTask( GmInterfaceGui* gm_iface, Command _cmd, HWND _hWnd ) 
{
	GuiCommandTask* t = new( tbb::task::allocate_root() ) GuiCommandTask( gm_iface, _cmd, _hWnd );
	tbb::task::enqueue( *t );
}
