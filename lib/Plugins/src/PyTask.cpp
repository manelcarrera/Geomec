#include "PyTask.h"
#include "PyInterface.h"
#include "IGmInterface.h"

tbb::task* PyTask::execute() 
{
	py_iface->launch( name, gm_iface );					// Do long computation

	delete gm_iface;
	delete py_iface;

	return NULL;
}

void Launch_Py_Task( std::string name, IGmInterface* gm_iface, IPyInterface* py_iface, HWND hWnd )  //FIXME: static, why multiple definition otherwise?
{
	PyTask* t = new( tbb::task::allocate_root() ) PyTask( name, gm_iface, py_iface, hWnd );
	tbb::task::enqueue( *t );
}

