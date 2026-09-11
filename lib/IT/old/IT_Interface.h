#ifndef _IT_Interface
#define _IT_Interface

#include "PyInterface.h"
#include "PyWrapper.h"

class IT_Interface : public PyInterface
{
	static const std::string PYTHON_MODULE;
	static const std::string FUNCTION;
	static const std::string PARAM;
	static PyTuple* PARAM_PY;

	static const TPyModule& PyModuleDefault;
	static const TCModule& CModuleDefault;
	static const TCModule& CModuleExtended;

public:

	IT_Interface();
	~IT_Interface();

	/*
	Use of 'Py_Finalize':
	---------------------
		I managed to work past this problem. It seems that some modules have problems when their initialization routines are called more than once, and numpyis one of those. 
		The solution is to call Py_Finalize() only once at the very end of the program. Py_Initialize() can be called as many times as you want, as if Python is already initialized, Py_Initialize() is a non-op ...
		And also, discovered that this solution turns the application faster since python doesn't need to restart every time there's a call to some of its function.
		More information about it here: http://stackoverflow.com/questions/7676314/py-initialize-py-finalize-not-working-twice-with-numpy
	*/
	/*static*/ int launch(	TPyModule py_module = IT_Interface::PyModuleDefault, 
						TCModuleV c_modules_v = {	IT_Interface::CModuleDefault, 
													IT_Interface::CModuleExtended } ); //FIXME: pass params from 'GeomecDoc'
	/*
		Needs to be call when Geomec closes.

		Otherwise python module (IT) can be call once; at second call it crashes
	*/
	/*static*/ void finalize();
};

#endif