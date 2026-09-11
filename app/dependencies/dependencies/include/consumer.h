#if !defined(Consumer_h)
#define Consumer_h

#include <QtCore/QThread>
#include <QtCore/QObject>

#include "_thread.h"
#include "dependencies.h"

class CConsumer : public QThread
{
	CThread& m_thread;
	CDependencies& m_dependencies;

public:
    CConsumer( CThread& _thread, CDependencies& d, QObject *parent = NULL );
    void run() override;

private:
	void print();

};


#endif //!defined(Consumer_h)