// DoubleEdit.cpp : implementation file
//


#include "stdafx.h"
#include "dsealect.h"
#include "DoubleFormat.h"
#include "DoubleEdit.h"
#include <math.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif





/////////////////////////////////////////////////////////////////////////////
// CDoubleEdit

//##ModelId=3C0DC15D035C
CDoubleEdit::CDoubleEdit()
{

}

//##ModelId=3C0DC15D037A
CDoubleEdit::~CDoubleEdit()
{

}


BEGIN_MESSAGE_MAP(CDoubleEdit, CEdit)
	//{{AFX_MSG_MAP(CDoubleEdit)
	ON_WM_CHAR()
	ON_WM_KEYUP()
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDoubleEdit message handlers


//##ModelId=3C0DC15D038A
void CDoubleEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  int              i, num_typed, start_char, end_char;
  int              nELoc        = -1;
  BOOL             nPeriodLoc   = -1;
  BOOL             bSignTyped   = FALSE;
  BOOL             bESignTyped  = FALSE;
  CString          temp_str;

    // this is the set of printing characters that we will NOT allow for
    // float edit fields. The reason we do it this way instead of specifying
    // what we DO want is because we need to allow all nonprinting characters
    // such as shift, control, arrows, etc., and we cannot mask all of those.
  CString Holder = "`~!@#$%^&*()_=qwrtyuiop[]\\asdfghjkl;\'zxcvbnm,/QWRTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?/";

    // 1st pass on unwanted characters
  if (Holder.Find(nChar) != -1)
    return;

    // length, selection info
  num_typed = CEdit::GetWindowTextLength();
  GetSel(start_char, end_char);

    // current CEdit text
  GetWindowText(temp_str);
  
  temp_str.TrimLeft();
  temp_str.TrimRight();

	//no 'E' or 'e' at beginning
  if( (temp_str.GetLength()==0) && (nChar == 'e' || nChar == 'E'))
	  return;
  
    // most of the work done in here..
  for (i = 0; i < temp_str.GetLength(); i++) {
        // selected chars don't count..
    if ((i >= start_char) && (i < end_char))
      continue;

    switch (temp_str[i]) {

      case '+':
      case '-':
        if (nELoc >= 0)
          bESignTyped = TRUE;
        else
          bSignTyped = TRUE;
        break;

      case '.':
        nPeriodLoc = i;
        break;

      case 'e':
      case 'E':
        nELoc = i;
        break;
    }
  }

    // determine if new character is valid
  switch (nChar) {

      // allow sign only once, and only on the first char
    case '+':
    case '-':
      if (start_char == 0) {
        if (bSignTyped)
          return;
      }
      else if ((start_char-1) == nELoc) {
        if (bESignTyped)
          return;
      }
      else {
        return;
      }
      break;

      // allow period only once
    case '.':
      if (nPeriodLoc != -1)
        return;
      break;

    case 'e':
    case 'E':
        // only allow one 'e'
      if (nELoc != -1)
        return;
        // don't allow 'e' before the period
      if (start_char <= nPeriodLoc)
        return;
      break;
  }

	SetModifiedFlag();

    // if we get here, the character is ok
	CEdit::OnChar(nChar, nRepCnt, nFlags);

}

//##ModelId=3C60D57903D9
void CDoubleEdit::SetModifiedFlag()
{
	SetModify(TRUE);
	try
	{
		CView* view = 0;

		if ((view = dynamic_cast <CView*> (this->GetParent())) != 0)
		{
			view->GetDocument()->SetModifiedFlag(TRUE);
		}
	}
	catch(...)
	{
	}
}

//##ModelId=3C0DC15D036F
double CDoubleEdit::GetValue()
{
	ASSERT(!IsEmpty());
    char *stopstring=0;
	CString str;
	GetWindowText(str);
	str.TrimLeft();
	str.TrimRight();
	double dTmp = strtod(str,&stopstring);
	ASSERT(*stopstring ==0);
	return dTmp;
}


//##ModelId=3C0DC15D036E
BOOL CDoubleEdit::IsEmpty()
{
	CString str;
	GetWindowText(str);
	str.TrimLeft();
	str.TrimRight();
	if(!IsNumericValue(str))
	{
		MakeEmpty();
		return TRUE;
	}
	TRACE("l= %i", str.GetLength());
	return (str.GetLength()==0);
}


//##ModelId=3C0DC15D036C
void CDoubleEdit::ShowValue(const double &NewValue)
{
	SetWindowText(Format(NewValue));
}

//##ModelId=3C0DC15D036B
void CDoubleEdit::MakeEmpty()
{
	SetWindowText("");
}


//##ModelId=3C0DC15D039A
BOOL CDoubleEdit::IsNumericValue(CString str)
{
	if(str.GetLength() == 0)
		return FALSE;
	char *stopstring=0;
	double dTmp = strtod(str,&stopstring);
	return (*stopstring == 0);
}


//##ModelId=3C60D579038B
CString CDoubleEdit::Format(double dblVal)
{
	CString str;
	int     decimal, sign;
	int     precision = 10;

	 _ecvt( dblVal, precision, &decimal, &sign );
	if(dblVal == 0.0)
	{
		str="0.0";
	}
	else if(fabs(dblVal)>= 1E6)
	{
		str.Format("%.3e",dblVal);
	}
	else if( fabs(dblVal) < 1E-6 )
	{
		str.Format("%.3e",dblVal);
	}
	else if( fabs(dblVal) < 1E-4 )
	{
		str.Format("%.8f",dblVal);
	}
	else
	{
		
		str.Format("%.8g",dblVal);
		/*
		double frac_part,int_part;
		frac_part=modf(dblVal,&int_part);
		if(fabs(frac_part) == 0.0 )
		{
			str=str + ".0";
		}
		*/
		if(str.Find(".") == -1 )
		{
			str=str + ".0";
		}
	}
	
	

	return str;
	
}

//##ModelId=3C0DC15D038F
void CDoubleEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
		
	CEdit::OnKeyUp(nChar, nRepCnt, nFlags);

	if(nChar==VK_RETURN) 
	{
		//validate on enter key
		DoValidation();
	}
	else if(nChar==VK_DELETE)
	{
		SetModifiedFlag();
	}
}

//##ModelId=3C2044040000
CString CDoubleEdit::GetString()
{
	CString str;
	GetWindowText(str);
	return str;	
}

//##ModelId=3C8D9E1D0292
void CDoubleEdit::OnKillfocus() 
{
	DoValidation();
}


//##ModelId=3C8D9E1D02A2
void CDoubleEdit::DoValidation()
{
	if(GetModify())
	{
		SetModify(FALSE);
		int nIDC=GetDlgCtrlID();
		CWnd *pParent=GetParent();
		
		if(pParent && nIDC)
		{
			pParent->SendMessage(WM_VALIDATE_DOUBLE_EDIT, nIDC,0);
		}
	}
}
