#include <QCoreApplication>
#include <QThread>
#include <QTimer>

#include "worker.h"

/////////////////////////////////////////////
//
//					main
//
/////////////////////////////////////////////

//
// Worker thread stops processing events during infinite loop in Qt
// https://stackoverflow.com/questions/51470464/worker-thread-stops-processing-events-during-infinite-loop-in-qt
//

#ifdef KK
int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  QThread *thread = new QThread;

  Worker *worker = new Worker();
  worker->moveToThread(thread);

  // connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
  QObject::connect(thread, SIGNAL(started()), worker, SLOT(process()));

  QObject::connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
  // [slot]void QThread::quit()
  // Tells the thread's event loop to exit with return code 0 (success). Equivalent to calling QThread::exit(0).
  // This function does nothing if the thread does not have an event loop.

  QObject::connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

  qDebug("start");
  thread->start();

  // protected
  // thread->exec();

  // TRY
  int aa = 0;
  qDebug("wait : before");
  thread->wait();
  qDebug("wait : after");
  int b = 0;

  return a.exec();
}
#endif

//
// mcr 2020-02-28
//
// Conclusion:
//
// Next needs to be done in the thread loop
// m_thread.eventDispatcher()->processEvents( QEventLoop::AllEvents );
// otherwise events are never processed: so signal 'finished' never processed and 'quit' slot never executed so stays
// forever in 'wait'
//
// don't use qt for events guided programming, no need to be verified again and again
//
#ifdef KK
int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  Worker *worker = new Worker();
  worker->moveToThread(&worker->m_thread);

  // connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
  QObject::connect(&worker->m_thread, SIGNAL(started()), worker, SLOT(process()));

  QObject::connect(worker, SIGNAL(finished()), &worker->m_thread, SLOT(quit()));
  // [slot]void QThread::quit()
  // Tells the thread's event loop to exit with return code 0 (success). Equivalent to calling QThread::exit(0).
  // This function does nothing if the thread does not have an event loop.

  // QObject::connect( worker, SIGNAL( finished() ), worker, SLOT(deleteLater() ) );
  // QObject::connect( &worker->m_thread, SIGNAL( finished() ), &worker->m_thread, SLOT(deleteLater() ) );

  qDebug("start");
  worker->m_thread.start();

  // protected
  // thread->exec();

  // TRY
  /*
  int aa=0;
  qDebug("wait : before");
  thread->wait();
  qDebug("wait : after");
  int b=0;
  */

  QTimer::singleShot(5000, worker, &Worker::stop);

  return a.exec();
}
#endif

/////////////////////////////////////////////////////////////////
//
//			C++11 concurrency
//
/////////////////////////////////////////////////////////////////
#include <chrono>
#include <iostream>
#include <thread>

class background_task {
public:
  void operator()() const {
    // do_something();
    // do_something_else();
  }
};

void hello() { std::cout << "Hello Concurrent World\n"; }
class X {
public:
  bool m_quit;
  std::mutex m_mut;
  std::condition_variable m_cond;

public:
  void do_lengthy_work() {
    int x = 1 * 1000;

    // wait to launch condition
    std::unique_lock<std::mutex> lk(m_mut);
    m_cond.wait(lk);
    lk.unlock();

    while (!m_quit) {
      std::this_thread::sleep_for(std::chrono::milliseconds(x));
      std::cout << "  thread : alive" << std::endl;
    }
    std::cout << "  thread : quit" << std::endl;
  };
};

//
// this works Ok
//
int main_(int argc, char *argv[]) {
  // 1)
  // std::thread t( hello );
  // t.join();

  // 2)
  // std::thread my_thread( background_task() );

  // 3)
  X my_x;
  my_x.m_quit = false;
  std::thread t(&X::do_lengthy_work, &my_x);

  // endless loop
  int x = 5 * 1000;
  int count = 0;
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(x));
    std::cout << "main" << std::endl;
    if (count == 2) {
      // launch thread
      my_x.m_cond.notify_one();
    }
    if (count == 4) {
      // quit thread
      my_x.m_quit = true;
    }
    count++;
  }
}