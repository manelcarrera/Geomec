#ifndef _MP_KERNEL_H_
#define _MP_KERNEL_H_

#ifndef _WIN64
#include <unistd.h>
#endif

#include <tbb/tbb.h>
#include <iostream>
#include <algorithm>

#ifdef DEBUG_MPKERNEL
#include <fstream>
#include <sstream>
#endif

#include "IProgressBase.h"	

#include "Progress_.h" //CProgressCancel


#if __INTEL_COMPILER
#define __TBB_LAMBDAS_PRESENT ( _TBB_CPP0X && __INTEL_COMPILER > 1100 )
#elif __GNUC__
//#define __TBB_LAMBDAS_PRESENT ( _TBB_CPP0X && __TBB_GCC_VERSION >= 40500 )
//can't find _TBB_CPP0X in the headers, but just to be sure, we don't use lambdas on linux:
#define __TBB_LAMBDAS_PRESENT 0
#elif _MSC_VER
#define __TBB_LAMBDAS_PRESENT ( _MSC_VER>=1600 )
#endif

namespace mp
{


#ifdef DEBUG_MPKERNEL
// an output class for debugging purposes
class MPOutput
{
  tbb::mutex m_mutex;

public:
  static MPOutput *instance()
  {
    static MPOutput *_instance = new MPOutput();
    return _instance;
  }

  void write(std::string s)
  {
    tbb::mutex::scoped_lock scope(m_mutex);

    std::ofstream f;
    f.open("gm_mp_debug.txt", std::ios::out | std::ios::app);
    if (!f.is_open())
      f.open("gm_mp_debug.txt", std::ios::out | std::ios::trunc);

    if (f.is_open())
    {
      f << s;
      f.close();
    }
  }

private:
};
#endif




// Unfortunately, the tbb templates splice our TASK functor somewhere, so we can't use abstract base classes where it counts
// (the execute and sort methods). Instead, we also pass the TASK as a template variable.
//
// For now, we define both a Parallel and Sequential kernel, mainly for comparisons and to play around with different kinds
// of kernels. In the future, the Sequential kernel should probably go; and a GPUKernel might be added.

class IKernel
{
protected:
  IKernel() {}

  mutable tbb::tick_count m_start;
  mutable tbb::tick_count m_end;

  void start() const { m_start = tbb::tick_count::now(); }
  void stop() const { m_end = tbb::tick_count::now(); }

public:
  virtual ~IKernel() {}

  double seconds() const { return (m_end - m_start).seconds(); }
};


// For convenience:
#define MP_DUMMY_DATA std::vector<size_t>& data() const { std::vector<size_t> *dummy = 0; return *dummy; }


template<class TASK>
class CKernelParallel : public IKernel
{
  size_t grainSize;
public:
  CKernelParallel(size_t grainSize = 384) : grainSize(grainSize) {}
  virtual ~CKernelParallel() {}

  virtual void execute(const TASK& task) const
  {
    start();
    tbb::parallel_for(tbb::blocked_range<size_t>(0, task.size(), grainSize), task);
    stop();
  }

  virtual void sort(const TASK& task) const
  {
    start();
    tbb::parallel_sort(task.data().begin(), task.data().end());
    stop();
  }
};



template<class TASK>
class CKernelSequential : public IKernel
{
public:
  CKernelSequential() {}
  virtual ~CKernelSequential() {}

  virtual void execute(const TASK& task) const
  {
    start();
    task(tbb::blocked_range<size_t>(0, task.size()));
    stop();
  }

  virtual void sort(const TASK& task) const
  {
    start();
    std::sort(task.data().begin(), task.data().end());
    stop();
  }
};




// Ideally, we would change the ProgressDialog so that it dispatches a tbb::task in the background
// with the task posting (!) messages to update the counter.
// But this would require either to add tbb code to all progress bars, or to add a specific tbb progress bar.
//
// So for now we just define a CKernelDispatcher class, that can be added anywhere. It will run in the
// GUI thread, dispatch a task, and call the progress bar to update. The dispatched task calls the actual
// task, with a pointer to the dispatcher, and the task will atomically update a counter.


class CKernelDispatcher;

class IDispatchedTask : public tbb::task
{
public:
  virtual void setDispatcher(CKernelDispatcher *) {}
  virtual int  size() { return 0; }
  virtual tbb::task *execute() { return 0; }

};


template<class KERNEL, class TASK>
class CDispatchedTask : public IDispatchedTask
{
  KERNEL& m_kernel;
  TASK&   m_task;

public:
  CDispatchedTask(KERNEL& kernel, TASK& task) : m_kernel(kernel), m_task(task) {}

  CDispatchedTask(const CDispatchedTask& rhs) : m_kernel(rhs.m_kernel), m_task(rhs.m_task) {}

  virtual void setDispatcher(CKernelDispatcher *dispatcher) { m_task.setDispatcher(dispatcher); }

  virtual int size() { return (int)m_task.size(); }

  virtual tbb::task *execute()
  {
    m_kernel.execute(m_task);
    return 0;
  }

};


// shortcut; KERNEL and TASK are types, obviously, and we still need to pass in parameters after the macro call
// the pointer is auto-released, no delete is necessary


#define NEW_DISPATCH_TASK(KERNEL, TASK) new(tbb::task::allocate_root()) mp::CDispatchedTask<KERNEL<TASK>, TASK>



class CKernelDispatcher
{
  tbb::task_group *m_group;
  tbb::atomic<int> m_counter;
  int              m_total;
  int              m_last;

  IProgressBase   *m_progress;

  bool             m_canceled;


  tbb::tick_count  m_start;
  tbb::tick_count  m_end;

#if !__TBB_LAMBDAS_PRESENT
  class Run
  {
    IDispatchedTask *m_task;
  public:
    Run(IDispatchedTask *task) : m_task(task) {}
    void operator()() const { m_task->execute(); }
  };
#endif

public:

  void Init(IProgressBase& progress)
  {
    m_start   = tbb::tick_count::now();
    m_end     = m_start;

    m_group   = new tbb::task_group;
    m_counter = 0;
    m_total   = 0;
    m_last    = 0;

    m_progress = &progress;

    m_canceled = false;
  }

  void AddTask(IDispatchedTask *task)
  {
    if (!m_group->is_canceling())
    {
      task->setDispatcher(this);

      m_total += task->size();

#if __TBB_LAMBDAS_PRESENT
      m_group->run([=]{ task->execute(); });
#else
      m_group->run(Run(task));
#endif
    }
  }

  void Step(int n = 1)
  {
    m_counter.fetch_and_add(n);
  }

  void ProgressStep()
  {
    int step = m_counter - m_last;

    if (step > 0)
    {
      m_last += step;

	  //
	  // FIXME: mcr 2020-07-31
	  //
      try
      {
        m_progress->Step(step);
      }
      catch (CProgressCancel *e)
      {
        m_canceled = true;
        m_group->cancel();
        delete e;
      }

    }
  }

  void Wait()
  {
    while (m_counter < m_total && !m_group->is_canceling())
      ProgressStep();

    m_group->wait();
    delete m_group;

    m_end = tbb::tick_count::now();
  }

  bool Processed() const
  {
    return m_counter != 0;
  }

  bool Canceled() const
  {
    return m_canceled;
  }

  double seconds() const
  {
    return (m_end - m_start).seconds();
  }

  void launch(IProgressBase& progress, IDispatchedTask *task)
  {
    Init(progress);
    AddTask(task);
    Wait();
  }

};




}


#endif