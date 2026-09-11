#if !defined(_thread_h)
#define _thread_h

#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>

class CThread
{
public:
	QWaitCondition		m_cmds_available;
	QMutex				m_mutex;
	QList< QString >	m_cmds;
public:
	CThread(){};
};


#endif //!defined(_thread_h)