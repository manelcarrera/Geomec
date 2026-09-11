// DDXQuantity.cpp: implementation of the CDDXQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSealect.h"
#include "DSealectDoc.h"
#include "CDependenciesManager.h"
#include "DoubleEdit.h"
#include "DDXQuantity.h"
#include "DSealectGlobal.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

static CString GetInvalidMaxMinString(CString sQuantityName, CDoubleQuantity *pQ);

void AFXAPI DDV_Quantity(CDataExchange* pDX, int nIDC, CDoubleQuantity *pQ,CDependenciesManager* pDM,CString sQuantityName)
{
	HWND hWndCtrl    = pDX->PrepareEditCtrl(nIDC);
	TRACE("hWndCtrl = %i", hWndCtrl);
	CDoubleEdit *pDE = (CDoubleEdit*)pDX->m_pDlgWnd->GetDlgItem(nIDC);

	//get the current unit system.
	CDSealectDoc* pDoc = GetDoc();
	CDoubleQuantity::UNIT us = pDoc->UnitSystem();
	
	if(pDX->m_bSaveAndValidate)
	{
		//From controls to data
		
		bool bWasValid = !pQ->Undefined();
		double prevValue;
		if(bWasValid)
		{
			//restore previous value.
			prevValue=pQ->Value(us);
		}
	
		if(!pDE->IsEmpty())
		{
			//the edit box is not empty
			double si_val = pQ->Convert(pDE->GetValue(),QU::SI_UNIT,us);
			std::string reason;
			if(pQ->CheckValue(si_val,reason))
			{
				//the max/min boundarys are ok
				pQ->Value(si_val,QU::SI_UNIT);
				std::string strDependency;

				//check dependency if pointer to manager is given
				if(pDM)
				{
					if(pDM->CheckQuantity(pQ,strDependency))
					{
						//the dependencys are ok
						pQ->Value(si_val,QU::SI_UNIT);
						pDE->ShowValue(pDE->GetValue());
					}
					else
					{
						//there are not satisfying dependencys,show a message
						AfxMessageBox(strDependency.c_str(),0,0);
						if(bWasValid)
						{
							//the previous value was ok, so resore this value
							pQ->Value(prevValue,us);
							pDE->ShowValue(prevValue);
						}
						else
						{
							//the previous value NOT was ok, so make edit box blank
							pDE->MakeEmpty();
							pQ->Invalidate();
						}
						pDX->Fail();
					}
				}
				else
				{
					//No dependency check values are ok
					pQ->Value(si_val,QU::SI_UNIT);
					pDE->ShowValue(pDE->GetValue());
				}
			}
			else
			{
				//the value is not within the boundarys.
				//show boundary messagebox.
				AfxMessageBox(GetInvalidMaxMinString(sQuantityName,pQ),0,0);
				if(bWasValid)
				{
					//the previous value was ok, so resore this value
					pDE->ShowValue(pQ->Value(us));
				}
				else
				{
					//the previous value NOT was ok, so make edit box blank
					pDE->MakeEmpty();
					pQ->Invalidate();
				}
				pDX->Fail();
			}
		}
		else
		{
			//the edit box is empty
			//invalidate the data.
			pQ->Invalidate();
		}
	}
	else
	{
		//From data to controls.
		if(!pQ->Undefined())
		{
			//show the value quantity in user units.
			pDE->ShowValue(pQ->Value(us));
		}
		else
		{
			//undifined quantities are shown as a
			//blank edit box.
			pDE->MakeEmpty();
		}
	}

}

static CString GetInvalidMaxMinString(CString sQuantityName, CDoubleQuantity *pQ)
{
	//get the current unit system.
	CDSealectDoc* pDoc = GetDoc();
	CDoubleQuantity::UNIT us = pDoc->UnitSystem();

	CString ret;
	CString frmt;

	if(pQ->MinValue() <= DOUBLE_MIN_)
	{
		frmt=" -INF";
	}
	else
	{
		frmt=CDoubleEdit::Format(pQ->MinValue(us));
	}

	ret = ret + frmt;
	ret =ret + " ";
	ret = ret + pQ->UnitName(us).c_str(); 
	ret =ret + " ";

	ret = ret + " <";
	if(pQ->IncludeMin())
	{
		ret=ret + "=";	
	}
	ret =ret + " ";
	ret=ret + sQuantityName;
	ret =ret + " ";

	ret =ret + "<";
	if(pQ->IncludeMax())
	{
		ret=ret + "=";	
	}
	ret =ret + " ";
	if(pQ->MaxValue() >= DOUBLE_MAX_)
	{
		frmt=" INF";
	}
	else
	{
		frmt=CDoubleEdit::Format(pQ->MaxValue(us));
	}
	ret = ret + frmt;
	ret =ret + " ";
	ret = ret + pQ->UnitName(us).c_str(); 

	return ret;
}
