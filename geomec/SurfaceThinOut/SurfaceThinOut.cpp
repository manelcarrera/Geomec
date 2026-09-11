// SurfaceThinOut.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SurfaceThinOut.h"
#include <afxdao.h>
#include <map>
#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		if(argc != 3)
		{
			cerr << _T("Must provide database and interval") << endl;
			return -1;
		}

		CString db = argv[1];
		int nInterval = atoi(argv[2]);

		CDaoDatabase database;
		database.Open(db, TRUE);

		CDaoRecordset seismicpoint(&database);
		seismicpoint.Open(dbOpenTable, "SeismicPoint", dbDenyWrite);
		seismicpoint.MoveFirst();
		std::map<int, int> mpLeft;
		for(int i = 0; !seismicpoint.IsEOF(); seismicpoint.MoveNext(), i++)
		{
			COleVariant var;
			seismicpoint.GetFieldValue("InSeismic", var);
			int iSeismic = var.lVal;
			std::map<int, int>::iterator itIns = mpLeft.insert(std::make_pair(iSeismic, 0)).first;

			if((i % nInterval == 0) || itIns->second < 2)
			{
				itIns->second++;
				continue;
			}

			seismicpoint.Delete();
		}

		CDaoRecordset seismic(&database);
		seismic.Open(dbOpenTable, "Seismic", dbDenyWrite);
		seismic.MoveFirst();

		while(!seismic.IsEOF())
		{
			COleVariant var;
			seismic.GetFieldValue("SeismicNr", var);
			int idx = var.lVal;
			std::map<int, int>::iterator it = mpLeft.find(idx);
			assert(it != mpLeft.end());

			seismic.Edit();
			seismic.SetFieldValue("PointCount", COleVariant((long) it->second));
			seismic.Update();

			seismic.MoveNext();
		}
	}

	return nRetCode;
}


