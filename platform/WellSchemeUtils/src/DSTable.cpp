// DSTable.cpp: implementation of the CDSTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <memory>

#ifndef _WIN64

#include "DSTable.h"




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

CDSTable::CDSTable(CDaoDatabase& database) : m_Database(database), m_TableRS((CDaoDatabase*)&Database())
{
}

CDSTable::~CDSTable()
{
  try {
    if(RS().IsOpen()){
      Close();
    }
  } catch (CDaoException* e){
    e->Delete();
  }
}

void CDSTable::Close()
{
  if(RS().IsOpen()){
    RS().Close();
  }
}

CDaoDatabase& CDSTable::Database() const
{
  return ((CDaoDatabase&)m_Database);
}

void CDSTable::SetField(const CString &sFieldName, const long iVal)
{
  RS().SetFieldValue(sFieldName,iVal);

}

void CDSTable::SetField(const CString &sFieldName, const double dVal)
{
  

  RS().SetFieldValue(sFieldName,dVal);

}

void CDSTable::SetField(const CString &sFieldName, const CString& sVal)
{
  RS().SetFieldValue(sFieldName,((CString&)sVal).GetBuffer(sVal.GetLength()));
}

void CDSTable::SetField(const CString &sFieldName, const CByteArray& Bytes)
{
  RS().SetFieldValue(sFieldName,Bytes);
}

void CDSTable::SetField(const CString &sFieldName, const BOOL& bVal)
{
  COleVariant TempVariant;
  TempVariant.ChangeType(VT_BOOL);
  TempVariant.bVal = bVal;
  RS().SetFieldValue(sFieldName,TempVariant);
}

void CDSTable::SetField(const CString& sFieldName, const CDoubleQuantity& qVal)
{
  if(qVal.Undefined())
  {
    RS().SetFieldValueNull(sFieldName);

  }
  else
  {
    RS().SetFieldValue(sFieldName,qVal.Value());
  }
  
}


void CDSTable::GetField(const CString &sFieldName, long& iVal)
{
  iVal = RS().GetFieldValue(sFieldName).iVal;
}

void CDSTable::GetField(const CString &sFieldName, double& dVal)
{
  dVal = RS().GetFieldValue(sFieldName).dblVal;
}

void CDSTable::GetField(const CString &sFieldName, CString& sVal)
{
  sVal = RS().GetFieldValue(sFieldName).pbVal;
}

void CDSTable::GetField(const CString& sFieldName, CByteArray& Bytes)
{
  COleVariant Variant(Bytes);

  RS().GetFieldValue(sFieldName,Variant);
  long iSize = Variant.parray->rgsabound[0].cElements;
  if(iSize > 0){
    Bytes.SetSize(iSize);
    LPVOID pTarget = (LPVOID)Bytes.GetData();
    LPVOID  pSource;
    if(SafeArrayAccessData(Variant.parray,&pSource) == S_OK){
      if(pSource){
        memcpy(pTarget, pSource, iSize);
      } else {
        Bytes.SetSize(0);
      }
      SafeArrayUnaccessData(Variant.parray);      
    } else {
      Bytes.SetSize(0);
    }
  } else {
    Bytes.SetSize(0);
  }
}

void CDSTable::GetField(const CString &sFieldName, BOOL& bVal)
{
  bVal =  RS().GetFieldValue(sFieldName).boolVal ==0 ? FALSE:TRUE;
}

void CDSTable::GetField(const CString& sFieldName, CDoubleQuantity& qVal)
{
  COleVariant TempVariant;
  TempVariant.Clear();

  TempVariant = RS().GetFieldValue(sFieldName);
  if(TempVariant.vt ==VT_NULL || TempVariant.vt ==VT_EMPTY)
  {
    qVal.Invalidate();
  }
  else
  {
    assert(TempVariant.vt ==VT_R8); // double
    qVal.Value(TempVariant.dblVal, CDoubleQuantity::SI_UNIT);
  }
}

BOOL CDSTable::FieldExists(const CString& fieldname) const
{
  if(!TableExists())
    return FALSE;

  CDaoTableDef table(&m_Database);
  table.Open(TableName());
  short count = table.GetFieldCount();
  short i;

  CDaoFieldInfo info;
  for(i=0 ; i<count ;i++)
  {
    table.GetFieldInfo(i,info);
    if(info.m_strName == fieldname)
    {
      return TRUE;
    }
  }
  return FALSE;
}


BOOL CDSTable::TableExists() const
{
  assert(!TableName().IsEmpty());

  short count = m_Database.GetTableDefCount();
  short i;
  CDaoTableDefInfo info;

  for(i=0 ; i<count ;i++)
  {
    m_Database.GetTableDefInfo(i,info);
    if(info.m_strName == TableName())
    {
      return TRUE;
    }

  }
  return FALSE;
}


CString CDSTable::TableName() const
{
  assert(FALSE);
  return CString();
}

#endif // _WIN64
