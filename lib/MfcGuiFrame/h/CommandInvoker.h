#ifndef _COMMANDINVOKER_H_
#define _COMMANDINVOKER_H_

#include <map>
#include <string>
#include <vector>
#include "ICommand.h"


typedef std::pair<ICommand*,bool> TCommand;
typedef std::pair<std::string,TCommand> TCommandPair;
typedef std::map<std::string,TCommand> TCommandMap;
typedef std::vector<std::string> TCommandVec;

class CCommandInvoker
{
private:
	TCommandMap m_mpCommands;
	TCommandVec m_vcCommands;
public:
	~CCommandInvoker()
	{
		TCommandMap::iterator iter = m_mpCommands.begin();
		for (; iter != m_mpCommands.end(); iter++)
		{
			if (iter->second.second)
				delete iter->second.first;
		}
	}
	virtual bool Invoke(std::string strCommand)
	{
		bool bRet=false;
		TCommandMap::iterator iter = m_mpCommands.find(strCommand);
		if (iter != m_mpCommands.end())
		{
			bRet = true;
			if (iter->second.first->CanExecute())
				iter->second.first->Execute();
		}
		return bRet;
	}
	bool AddCommand(std::string strCommand, ICommand &command, bool bAutoDelete = true)
	{
		bool ret=false;
		ret = m_mpCommands.insert(TCommandPair(strCommand,TCommand(&command,bAutoDelete))).second;
		if (ret)
			m_vcCommands.push_back(strCommand);
		return ret;
	}
	bool AddCommand(UINT uCommand, ICommand &command, bool bAutoDelete = true)
	{
		CString sTemp;
		sTemp.LoadString(uCommand);
		std::string strCommand((LPCSTR)sTemp);
		bool ret=false;
		ret = m_mpCommands.insert(TCommandPair(strCommand,TCommand(&command,bAutoDelete))).second;
		if (ret)
			m_vcCommands.push_back(strCommand);
		return ret;
	}

	TCommandMap::iterator begin() { return m_mpCommands.begin();}
	TCommandMap::iterator end() { return m_mpCommands.end();}
	TCommandMap::iterator find(std::string strCommand) { return m_mpCommands.find(strCommand);}
	TCommandVec::iterator beginVec() { return m_vcCommands.begin();}
	TCommandVec::iterator endVec() { return m_vcCommands.end();}
	long size() { return m_mpCommands.size();}
};
#endif