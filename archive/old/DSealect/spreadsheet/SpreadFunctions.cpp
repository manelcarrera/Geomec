

#include "stdafx.h"
#include "spreadsheet.h"
#include "SpreadFunctions.h"



//copy's spreaddata to clipboard
//this is a fix fix for the multiline bug when using spread.CopyToClipBoard() direct
void CopyToClipboard(CSpreadSheet& spread)
{
	
	CString sText;
	for(long row=0 ; row <= spread.GetMaxRows();row++)
	{
		spread.SetRow(row);
		for(long col=0 ; col <= spread.GetMaxCols();col++)
		{
			spread.SetCol(col);
			CString CellText = spread.GetText();
			CellText.Replace("\n",", ");
			sText = sText + CellText;
			sText = sText + "\t";
		}
		sText=sText + "\n";
	}

	LPTSTR pStr = sText.GetBufferSetLength(sText.GetLength());

	HGLOBAL hGlobalMemory = GlobalAlloc(GHND,sText.GetLength()+1);
	char* pGlobalMemory = (char*) GlobalLock(hGlobalMemory);

	for(int i=0;i<sText.GetLength();i++)
		*pGlobalMemory++=*pStr++;

	GlobalUnlock(hGlobalMemory);
	OpenClipboard(AfxGetMainWnd()->GetSafeHwnd());
	EmptyClipboard();
	SetClipboardData(CF_TEXT,hGlobalMemory);
	CloseClipboard();
}

