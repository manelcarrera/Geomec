#pragma once

#include <string>

class IGmInterface;

class IPyInterface
{
public:

	IPyInterface(){};
	~IPyInterface(){}
	virtual int launch( std::string name, IGmInterface* gm_iface )=0;
	virtual void finalize()=0;
};
