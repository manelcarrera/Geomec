#pragma once

#include "Events.h"
//
// Interface Diana Executer, for diana_app in dsa
//
class IGI
{

public:

	IGI(){};
	~IGI(){};

	virtual void push(Cmd cmd)=0;
};