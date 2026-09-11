#include "dianaxqt.h"

#include <QString>


bool CDianaXQt::GetShowDialog()
{
	bool result = true;
	//GetProperty(0x1, VT_bool, (void*)&result);
	return result;
}

void CDianaXQt::SetShowDialog(bool propVal)
{
	//SetProperty(0x1, VT_bool, propVal);
}

QString CDianaXQt::GetSuccessMessageString()
{
	QString result;
	//GetProperty(0x2, VT_BSTR, (void*)&result);
	return result;
}

void CDianaXQt::SetSuccessMessageString(QString propVal)
{
	//SetProperty(0x2, VT_BSTR, propVal);
}

long CDianaXQt::GetNumberOfMessages()
{
	long result = 0;
	//GetProperty(0x3, VT_I4, (void*)&result);
	return result;
}

void CDianaXQt::SetNumberOfMessages(long propVal)
{
	//SetProperty(0x3, VT_I4, propVal);
}

bool CDianaXQt::GetDisplayStopMessage()
{
	bool result = true;
	//GetProperty(0x4, VT_bool, (void*)&result);
	return result;
}

void CDianaXQt::SetDisplayStopMessage(bool propVal)
{
	//SetProperty(0x4, VT_bool, propVal);
}

bool CDianaXQt::GetShowDefaultMessages()
{
	bool result = true;
	//GetProperty(0x5, VT_bool, (void*)&result);
	return result;
}

void CDianaXQt::SetShowDefaultMessages(bool propVal)
{
	//SetProperty(0x5, VT_bool, propVal);
}

QString CDianaXQt::GetDialogCaption()
{
	QString result;
	//GetProperty(0xa, VT_BSTR, (void*)&result);
	return result;
}

void CDianaXQt::SetDialogCaption(QString propVal)
{
	//SetProperty(0xa, VT_BSTR, propVal);
}

bool CDianaXQt::GetAutoCloseDialog()
{
	bool result = true;
	//GetProperty(0xb, VT_bool, (void*)&result);
	return result;
}

void CDianaXQt::SetAutoCloseDialog(bool propVal)
{
	//SetProperty(0xb, VT_bool, propVal);
}

bool CDianaXQt::GetAlwaysAutoCloseDialog()
{
	bool result = true;
	//GetProperty(0xe, VT_bool, (void*)&result);
	return result;
}

void CDianaXQt::SetAlwaysAutoCloseDialog(bool propVal)
{
	//SetProperty(0xe, VT_bool, propVal);
}


bool CDianaXQt::GetProgressIsLifesign()
{
	bool result = true;
	//GetProperty(0xc, VT_bool, (void*)&result);
	return result;
}

void CDianaXQt::SetProgressIsLifesign(bool propVal)
{
	//SetProperty(0xc, VT_bool, propVal);
}

long CDianaXQt::GetNumberOfAppSpcMessages()
{
	long result = 0;
	//GetProperty(0xd, VT_I4, (void*)&result);
	return result;
}

void CDianaXQt::SetNumberOfAppSpcMessages(long propVal)
{
	//SetProperty(0xd, VT_I4, propVal);
}

/////////////////////////////////////////////////////////////////////////////
// CDianaXQt operations

bool CDianaXQt::SetEnvironmentVar(QString VarName, QString Value)
{
	bool result = true;
	/*
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_bool, (void*)&result, parms,
		VarName, Value);
		*/
	return result;
}

short CDianaXQt::ExecuteDiana()
{
	short result = 0;
	/*
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_I2, (void*)&result, parms,
		ExeName, WorkingDir, ComFile, DatFile, FilosFile, BaseName);
		*/
	return result;
}

bool CDianaXQt::GetCalculationResult()
{
	bool result = true;
	//InvokeHelper(0x8, DISPATCH_METHOD, VT_bool, (void*)&result, NULL);
	return result;
}

bool CDianaXQt::SetUserMagic(QString Leader, QString Trailer)
{
	bool result = true;
	/*
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x9, DISPATCH_METHOD, VT_bool, (void*)&result, parms,
		Leader, Trailer);
		*/
	return result;
}

void CDianaXQt::AboutBox()
{
	//InvokeHelper(0xfffffdd8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}
