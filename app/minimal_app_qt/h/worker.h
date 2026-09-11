#pragma once

#include <QThread>

/////////////////////////////////////////////
//
//					Worker
//
/////////////////////////////////////////////

class Worker : public QObject {
    Q_OBJECT

	bool m_quit;

public:
	QThread m_thread;
 
public:
    Worker();
    ~Worker();

	void stop();
 
public slots:
    void process();
 
signals:
    void finished();
    void error(QString err);
 
private:
    // add your variables here
};
