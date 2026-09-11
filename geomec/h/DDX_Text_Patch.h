#pragma once
#include "stdafx.h"
#include <afxpriv.h>

// Copied from MFC source code
static void AFXAPI AfxTextFloatFormat(CDataExchange* pDX, int nIDC, void* pData, double value, int nSizeGcvt)
{
	ASSERT(pData != NULL);

	pDX->PrepareEditCtrl(nIDC);
	HWND hWndCtrl;
	pDX->m_pDlgWnd->GetDlgItem(nIDC, &hWndCtrl);

	const int TEXT_BUFFER_SIZE = 400;
	TCHAR szBuffer[TEXT_BUFFER_SIZE] = { 0 };          // added: initlialize whole buffer with 0

	if (pDX->m_bSaveAndValidate)
	{
	::GetWindowText(hWndCtrl, szBuffer, _countof(szBuffer));

	double d;
	if (_sntscanf_s(szBuffer, _countof(szBuffer), _T("%lf"), &d) != 1)
	{
		AfxMessageBox(AFX_IDP_PARSE_REAL);
		pDX->Fail();            // throws exception
	}
	if (nSizeGcvt == FLT_DIG)
		*((float*)pData) = (float)d;
	else
		*((double*)pData) = d;
	}
	else
	{
	ATL_CRT_ERRORCHECK_SPRINTF(_sntprintf_s(szBuffer, _countof(szBuffer), _countof(szBuffer) - 1, _T("%.*g"), nSizeGcvt, value));
	AfxSetWindowText(hWndCtrl, szBuffer);
	}
}

void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, float& value)
{
	AfxTextFloatFormat(pDX, nIDC, &value, value, FLT_DIG);
}

void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, double& value)
{
	AfxTextFloatFormat(pDX, nIDC, &value, value, DBL_DIG);
}
