// IDexInterface.h: interface for the IDexInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IDEXINTERFACE_H__67CC84B2_EC1E_4781_B519_F090EBF6DE16__INCLUDED_)
#define AFX_IDEXINTERFACE_H__67CC84B2_EC1E_4781_B519_F090EBF6DE16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include <qdatetime.h>


class AFX_EXT_CLASS IDexInterface  
{
  QString m_sBusinessObject;
  QString m_sAuthor;
  QString m_sVersion;
  QString m_sWellID;
  QString m_sDate;
  QString m_sTable;
  QString m_sField;
  QString m_sType;
  QString m_sUnit;
  QString m_sValue;
  QString m_sParent;
protected:
  IDexInterface();
  virtual ~IDexInterface();

  BOOL InitDEXForImport(QString path_name,QString &strError);
  BOOL CloseDEX(QString &strError);
  
  QString BusinessObj() { return m_sBusinessObject; }
  const QString BusinessObj() const { return m_sBusinessObject; }
  void BusinessObj(const char *bus_obj) { m_sBusinessObject = bus_obj; }

  QString Author() { return m_sAuthor; }
  const QString Author() const { return m_sAuthor; }
  void Author(const char *author) { m_sAuthor = author; }

  QString Version() { return m_sVersion; }
  const QString Version() const { return m_sVersion; }
  void Version(const char* version) { m_sVersion = version; }

  QString WellID() { return m_sWellID; }
  const QString WellID() const { return m_sWellID; }
  void WellID(const char *id) { m_sWellID = id; }

  QString Date() { return m_sDate; }
  const QString Date() const { return m_sDate; }
  void Date(const char *date) { m_sDate = date; }
  
  QString Table() { return m_sTable; }
  const QString Table() const { return m_sTable; }
  void Table(const char *table) { m_sTable = table; }

  QString Filed() { return m_sField; }
  const QString Field() const { return m_sField; }
  void Field(const char *field) { m_sField = field; }

  QString Type() { return m_sType; }
  const QString Type() const { return m_sType; }
  void Type(const char *type) { m_sType = type; }
  
  QString Unit() { return m_sUnit; }
  const QString Unit() const { return m_sUnit; }
  void Unit(const char *unit) { m_sUnit = unit;}

  QString Value() { return m_sValue; }
  const QString Value() const { return m_sValue; }
  void Value(const char *value) {m_sValue = value; }

  QString Parent() { return m_sParent; }
  const QString Parent() const { return m_sParent; }
  void Parent(const char *parent) { m_sParent = parent; }
  
  void ExportValue(const char *table, const char *field, const char *value) const;
  void ExportValue(const char *table, const char *field, int value) const;
  void ExportValue(const char *table, const char *field, const double &value) const;
  void ExportValue(const char *table, const char *field, const QDate &value) const;

  QString ConvertNumberToString(int value) const;
  QString ConvertNumberToString(const double &value) const;
  const char* ConvertDateToString(const QDate &value) const;

public:
  bool DexAvailable();



};

#endif // !defined(AFX_IDEXINTERFACE_H__67CC84B2_EC1E_4781_B519_F090EBF6DE16__INCLUDED_)
