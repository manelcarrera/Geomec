// IDexInterface.cpp: implementation of the IDexInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wellschemeutils.h"
#include "IDexInterface.h"

#ifdef USE_DEX
/*
#ifdef _DEBUG
  #pragma comment(linker, "/DELAYLOAD:dexuid.dll")
  #pragma comment(linker, "/DELAYLOAD:dexd.dll")
#else
  #pragma comment(linker, "/DELAYLOAD:dexui.dll")
  #pragma comment(linker, "/DELAYLOAD:dex.dll")
#endif
*/
#include "dexui.h"
#include "dexiface.h"

#endif // USE_DEX

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IDexInterface::IDexInterface()
{

}

IDexInterface::~IDexInterface()
{

}


BOOL IDexInterface::InitDEXForImport(QString path_name, QString &strError)
{
#ifdef USE_DEX

  char *error_msg;
  BOOL bState = TRUE;
  bState = DexAttach(&error_msg);
  
  CFileStatus status;
  if(bState)
    bState = (CFile::GetStatus(path_name.toAscii().constData(), status) && DexOpen(path_name.toAscii().constData(), &error_msg, FALSE));

  if(!bState)
  {
    strError = error_msg; // deep copy
    DexDetach();
    return bState;
  }

  return bState;

#else // USE_DEX

  return FALSE;

#endif // USE_DEX
}

BOOL IDexInterface::CloseDEX(QString &strError)
{
#ifdef USE_DEX

  if(!DexDetach())
  {
    strError = "Unable to detach DEX.";
    return FALSE;
  }

  return TRUE;

#else // USE_DEX

  return FALSE;

#endif // USE_DEX
}

void IDexInterface::ExportValue(const char *table, const char *field, const char *value) const
{
#ifdef USE_DEX
  VERIFY(DexPutValue(table, field, value)); 
#endif
}

void IDexInterface::ExportValue(const char *table, const char *field, int value) const
{
#ifdef USE_DEX
  VERIFY(DexPutValue(table, field, ConvertNumberToString(value).toAscii().constData())); 
#endif
}

void IDexInterface::ExportValue(const char *table, const char *field, const double &value) const 
{
#ifdef USE_DEX
  VERIFY(DexPutValue(table, field, ConvertNumberToString(value).toAscii().constData())); 
#endif
}

void IDexInterface::ExportValue(const char *table, const char *field, const QDate &value) const
{
#ifdef USE_DEX
  VERIFY(DexPutValue(table, field, ConvertDateToString(value))); 
#endif
}

QString IDexInterface::ConvertNumberToString(int value) const
{
  QString str = QString("%1").arg(value, 0, 10);
  return str;
}

QString IDexInterface::ConvertNumberToString(const double &value) const
{
  QString str = QString("%1").arg(value, 0, 'G'); 
  return str;
}

const char* IDexInterface::ConvertDateToString(const QDate &value) const
{
#ifdef USE_DEX

  char *date;
  DexDateToString(value.year(), value.month(), value.day(), 0, 0, &date);
  return date;

#else // USE_DEX

  assert(FALSE);
  return 0;

#endif // USE_DEX
}

bool IDexInterface::DexAvailable()
{
#ifdef USE_DEX
  char *error;
  char *dex_path;
  dex_path = DiGetenv("DEX_PATH");
  if(!dex_path)
    return false;
  
  CFileFind finder;
  
  char *dex_dll = strdup(dex_path);
  strcat(dex_dll, "\\dex.dll");
  if(!finder.FindFile(dex_dll))
    return false;

  dex_dll = strdup(dex_path);
  strcat(dex_dll, "\\dexui.dll");
  if(!finder.FindFile(dex_dll))
    return false;
  
  try
  {
    DexAttach(&error);
    DexDetach();
  }
  catch(...)
  {
    return false;	
  }

  return true;

#else // USE_DEX

  return false;

#endif // USE_DEX
}