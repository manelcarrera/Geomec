#pragma once

class IClass
{
public:
	virtual void start()=0;
};

//
// work-around OIV license
//
class Class_01 : public IClass
{
public:
	Class_01(){};
	void start();
};


//
// kill a process
//
class Class_02 : public IClass
{
public:
	Class_02(){};
	void start();
};


//
// boost consumer / producer
//
class Class_03 : public IClass
{
public:

	bool m_quit;

	enum eType
	{
		Producer,
		Consumer
	};

	Class_03() : m_quit(false) {};
	void start(){};
	void start( eType type_ );
};



//
// timed_wait
//
class Class_04 : public IClass
{
public:

	Class_04(){};
	void start();
};