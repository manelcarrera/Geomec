// CasSignalReciever.cpp: implementation of the CCasSignalReciever class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CasSignalReciever.h"
#include <iostream>
#include "..\DCasing\DCasing.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CCasingSignalReciever::CCasingSignalReciever(CDCasing * obj) :m_obj(obj)
{

}

CCasingSignalReciever::~CCasingSignalReciever()
{

}

void CCasingSignalReciever::OnMessage(QString str)
{
	if(!str.isEmpty())
		std::cout << (const char*)str << "\n";
}

void CCasingSignalReciever::OnError(QString str)
{
	if(!str.isEmpty())
	std::cout << (const char*)str << "\n";
}

void CCasingSignalReciever::OnWarning(QString str)
{
	if(!str.isEmpty())
	std::cout << (const char*)str;
}

void CCasingSignalReciever::OnFinished()
{
	std::cout << "finished" "\n";

	if (m_obj)
		m_obj->ReadOutputFile("casing.cao");
}
