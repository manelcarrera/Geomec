#include "stdafx.h"
#include "ContextMenuInvoker.h"
#include "GeomecStringTable.h"

// The contextmenuinvoker class is a specialised class to execute commands on objects based on a popup menu.
CContextMenuInvoker::CContextMenuInvoker()
{
	m_menu.CreatePopupMenu();
}

// This causes a separator to appear in the menu
void CContextMenuInvoker::AddSeparator()
{
	appendSeparator();
}

void CContextMenuInvoker::appendSeparator()
{
	CString sepa;
	sepa.Format("---%d", m_mpCommands.size());
	AddCommand((LPCTSTR)sepa,*(new CSeparatorCommand()));
}

// This causes an invoker to be added to the invoker ie a submenu appears
void CContextMenuInvoker::AddInvoker(const CString &title, CContextMenuInvoker &invoker)
{
	AddCommand((LPCTSTR)title,*(new CInvokerCommand(invoker)));
}

void CContextMenuInvoker::AddInvoker(unsigned int uTitle , CContextMenuInvoker &invoker)
{
	CString sTitle;
	sTitle = getStringTableEntry(uTitle);
	AddCommand((LPCTSTR)sTitle,*(new CInvokerCommand(invoker)));
}

// The buildmenu routine builds the popmenu based on the current commands in the Invoker
int CContextMenuInvoker::BuildMenu(int iStartIdx)
{
	int ret = iStartIdx;
	CInvokerCommand *pInvoker=0;
	CSeparatorCommand *pSeparator=0;
	for (TCommandVec::iterator iterVec = m_vcCommands.begin(); iterVec != m_vcCommands.end(); iterVec++)
	{
		TCommandMap::iterator iter = m_mpCommands.find(*iterVec);
		if (iter != m_mpCommands.end())
		{
			pInvoker = dynamic_cast<CInvokerCommand*>(iter->second.first);
			pSeparator = dynamic_cast<CSeparatorCommand*>(iter->second.first);
			if (pSeparator)
			{
				m_menu.AppendMenu(MF_SEPARATOR);
			}
			else if (pInvoker)
			{
				// When a command is actually an invoker we need a reference to the submenu, so first build a menu
				ret = pInvoker->Invoker().BuildMenu(ret);
				m_menu.AppendMenu(MF_POPUP, (UINT_PTR)pInvoker->Invoker().m_menu.Detach(), iter->first.c_str());
			}
			else
			{
				m_menu.AppendMenu(MF_STRING,ret,iter->first.c_str());
				if (!iter->second.first->canExecute())
					m_menu.EnableMenuItem(ret,MF_BYCOMMAND | MF_GRAYED);
				if (iter->second.first->isChecked())
					m_menu.CheckMenuItem(ret,MF_BYCOMMAND | MF_CHECKED);
				ret++;
			}
		}
	}
	return ret;
}

bool CContextMenuInvoker::TrackMenu(int x, int y, CWnd &parent)
{
	BuildMenu(1);
	bool ret=false;
	BOOL res = m_menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_NONOTIFY | TPM_RETURNCMD, x, y, &parent);
	if (res>0)
	{
		CString command;
		m_menu.GetMenuString(res,command,MF_BYCOMMAND);
		ret = Invoke((LPCTSTR)command);
	}
	return ret;
}

// Invoke will first try to invoke the command by its base. When this fails (ie the command can not be found) it searches all 
// other invokers an tries them.
bool CContextMenuInvoker::Invoke(std::string strCommand)
{
	bool bRet=false;
	TCommandMap::iterator iter1 = m_mpCommands.find(strCommand);
	if (iter1 != m_mpCommands.end())
	{
		bRet = true;
		if (iter1->second.first->canExecute())
			iter1->second.first->execute();
	}

	if (!bRet)
	{
		CInvokerCommand *pInvoker=0;
		for (TCommandMap::iterator iter2 = m_mpCommands.begin(); iter2 != m_mpCommands.end(); iter2++)
		{
			pInvoker = dynamic_cast<CInvokerCommand*>(iter2->second.first);
			if (pInvoker)
			{
				if ((bRet = (pInvoker->Invoker().Invoke(strCommand))))
				{
					break;
				}
			}
		}
	}
	else
		bRet = true;
	return bRet;
}

CContextMenuInvoker::~CContextMenuInvoker()
{
	TCommandMap::iterator iter = m_mpCommands.begin();
	for (; iter != m_mpCommands.end(); iter++)
	{
		if (iter->second.second)
			delete iter->second.first;
	}
}

bool CContextMenuInvoker::AddCommand(std::string strCommand, IGuiCommand &command, bool bAutoDelete)
{
	bool ret=false;
	ret = m_mpCommands.insert(TCommandPair(strCommand,TCommand(&command,bAutoDelete))).second;
	if (ret)
		m_vcCommands.push_back(strCommand);
	return ret;
}

bool CContextMenuInvoker::AddCommand(unsigned int uCommand, IGuiCommand &command, bool bAutoDelete)
{
	CString sTemp;
	sTemp = getStringTableEntry(uCommand);
	std::string strCommand((LPCSTR)sTemp);
	bool ret=false;
	ret = m_mpCommands.insert(TCommandPair(strCommand,TCommand(&command,bAutoDelete))).second;
	if (ret)
		m_vcCommands.push_back(strCommand);
	return ret;
}	

void CContextMenuInvoker::append(IGuiCommand* cmd)
{
	AddCommand(cmd->caption().toStdString(), *cmd);
}