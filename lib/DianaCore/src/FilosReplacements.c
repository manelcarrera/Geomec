#include "dimple.h"
#include "lbcx.h"
#include "lbfl.h"


//extern void FTN_CALL_TYPE diabrt_() {}

//extern void SaveTimes(ftn_int_t i) { ; }

static int    DsPrintf2Buffer;
static char **ppPrintBuffer;

extern void DsPrintfBuffer(int on, char **dspbfpp)
{
	DsPrintf2Buffer = on;
	if ( on )
	{
		ppPrintBuffer = dspbfpp;
		*ppPrintBuffer = DiStrsave( "" );
	}
	else if ( ppPrintBuffer ) {
		DiFree( *ppPrintBuffer, "DsPrintBuffer" );
		*ppPrintBuffer = NULL;
		ppPrintBuffer  = NULL;
	}
}

extern void AppendToPrintBuffer(const char *fmt, va_list args)
{
	char cuf[1024];

	vsnprintf( cuf, 1024, fmt, args );
	if ( DsPrintf2Buffer ) {
		char *p = vDiStrsave( *ppPrintBuffer, cuf, NULL );
		if ( p ) {
			DiFree( *ppPrintBuffer, "ftn_vprintf" );
			*ppPrintBuffer = p;
		}
	}
}

extern void InitializeFilos()
{
	ftn_int_t idx = 0;
	ftn_int_t level;

	int svfi = SetFunctionInfoFlags(1, DS_FUNC_WRITE);
	// Create SGMINF
	if(!f_exist("SGMINF")) F_Create("SGMINF", 0, 0, 'w', 0);

	if(!f_exist("DIASW "))
	{
		// Create "DIASW "
		ftn_int_t diasw[] = {0, 0, 0, 0, 0, 0, 6};
		ftn_int_t jref[1];
		ftn_int_t lout;

		idx = F_Karray("DIASW", jref, "IW", 7, &lout) - 1;
		memcpy(&jref[idx], diasw, 7 * sizeof (ftn_int_t));
		F_Release("DIASW");
	}

	if(!f_exist("DIASWI"))
	{
		ftn_int_t jref[1];
		ftn_int_t lout;
		ftn_int_t diaswi[] = {0, 0, 0, 0, 0, 0};

		idx = F_Karray("DIASWI", jref, "IW", 6, &lout) - 1;
		memcpy(&jref[idx], diaswi, 6 * sizeof (ftn_int_t));
		F_Release("DIASWI");
	}

	PushDir();

	ChangeDir("/JOBINF");

	level = 4;
	PutItem("LOGLVL", &level);
	PutItem("MONLVL", &level);

	PutCharItem("ENVIRO", "TERMIN");

	PopDir();

	SetFunctionInfoFlags(svfi, DS_FUNC_WRITE);
}