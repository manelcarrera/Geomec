#ifndef _POREPRESSUREPROFILETABLE_H_
#define _POREPRESSUREPROFILETABLE_H_

#include "DSTable.h"

#define PPP_TABLE "Pore Pressure Profile"
#define PPP_DEPTH "Depth"
#define PPP_VALUE "Value"

class CPorePressureProfileTable : public CDSTable
{
public:
  CPorePressureProfileTable(CDaoDatabase& Database);
	virtual void Open();
	virtual void Create();
};

#endif // _POREPRESSUREPROFILETABLE_H_
