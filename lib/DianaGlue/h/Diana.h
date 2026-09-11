// Diana.h: interface for the CDiana class.

#pragma once

class IDianaX;

typedef int BOOL;

#include <QString>

class QWidget;

class IDianaX;

class CDiana  
{
public:
  CDiana( IDianaX* diana_x );
  virtual ~CDiana();

  BOOL Calculate();
  bool GetCalculationResult();
  BOOL SetEnv();

protected:
  BOOL InitializeControl();
  void UnInitializeControl();
  BOOL SetEnvironment();
  BOOL StartDiana();

public:
  QString m_strDiaPath;
  QString m_strTempPath;
  QString m_strDiaExeName;
  QString m_strDataFile;
  QString m_strBaseName;
  //FIXEM: MCR
  QWidget* m_pParentWnd;
  //CWnd *m_pParentWnd;
  QString m_strDiaShare;

protected:
  IDianaX *m_pDianaX;
  BOOL m_bCalcResult;
};