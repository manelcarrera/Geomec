#pragma once

#include "PyTypes.h"

class Command;

template< class T >
class SafeQueue;


class IGmInterface
{
public:
	virtual const char* module()=0;
	virtual PyMethodDef* methods()=0;

	virtual SafeQueue< Command >& requests_queue()=0;
	virtual SafeQueue< Command >& results_queue()=0;

public:
	IGmInterface(){};
	~IGmInterface(){};
};




