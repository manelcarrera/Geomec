// DrillingDirectionTable.h: interface for the CDrillingDirectionTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRILLINGDIRECTIONTABLE_H__88CFF566_FFEC_455F_B5C6_C73C0DC58B38__INCLUDED_)
#define AFX_DRILLINGDIRECTIONTABLE_H__88CFF566_FFEC_455F_B5C6_C73C0DC58B38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSTable.h"

#define DRILLING_DIR_TABLE				"Drilling Direction"
#define DRILLING_DIR_ID					"Drilling Direction ID"
#define DRILLING_DIR_BOTTOM_TMD			"Bottom TMD"
#define DRILLING_DIR_WELL_INCLINATION	"Well Inclination"
#define DRILLING_DIR_AZIMUTH_WITH_NORTH	"Azimuth with North"

class CDSModelDatabase;

class CDrillingDirectionTable  : public CDSTable
{
public:
	CDrillingDirectionTable(CDaoDatabase& Database);
	virtual ~CDrillingDirectionTable();
	virtual void Open();
	virtual void Create();

};

#endif // !defined(AFX_DRILLINGDIRECTIONTABLE_H__88CFF566_FFEC_455F_B5C6_C73C0DC58B38__INCLUDED_)
