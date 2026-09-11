#include "stdafx.h"

#include "CustomCmdLineInfo.h"

#include "Printer.h"
#include "DianaStartUp.h"

#include <QString>
#include <QStringList>

CCustomCmdLineInfo::CCustomCmdLineInfo()
  : m_version(false)
  , m_exit(false)
  , m_output(false)
  , m_test{ false, "", "" }
{
}

void CCustomCmdLineInfo::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
  bool bHandled = false;

	QString param = QString( pszParam );

	//"--version" (sent) -> "-version" (reeceived)
	if( param == "-version" || param ==  "v" )
	{
		m_version = true;
		bHandled = true;
	}
  else if (param == "x")
  {
    m_exit = true;
    bHandled = true;
  }
  else if (param.startsWith("o=") || param.startsWith("o:"))
  {
    m_output = true;
    m_outputFile = param.right(param.length() - 2).toStdString().c_str();
    bHandled = true;
  }
	//
	// GUI tests
	//
  else if (param.startsWith("test:"))
  {
	enum eTestParams{Url,Token};

	QStringList p = param.split(":");
	p.removeFirst();

	if(p.size()==2)
		m_test = { true, p[ Url ].toStdString(), p[ Token ].toStdString() };

	bHandled = true;
  }
	else
	{
		QStringList list = param.split("=");
		if( list.size() == 2 )
		{
			QString id = list.at( 0 );
			QString value = list.at( 1 );
			bHandled = true;

			/*if( id == "pid" )
			{
				//Printer::instance()->debug( "CGeomecApp : pid:%d", value.toInt() );
				CDianaStartUp* dsu = CDianaStartUp::instance();
				dsu->pid( value.toInt() );
			}*/
		}
	}

  // If the last parameter has no flag, it is treated as the file name to be
  //  opened and the string is stored in the m_strFileName member.
  if (!bHandled)
    CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
}