// Test program for MPKernel.h
//
// We define two tasks, one to initialize an array of vectors with random components, and one to calculate their length
// (stored in component 'l').
//
// These two tasks are then executed with:
//   a. Parallel kernels
//   b. Dispatcher that shows a progress bar
//   c. 100 times through the Dispatcher, where the progress bar stays alive during the execution of all 100
//
#include "MPKernel.h"
#include <cmath>
#include <ctime>
#include <iostream>
#include <vector>

using namespace mp;

struct Vec4f {
  float x;
  float y;
  float z;
  float l;

  Vec4f() : x(0), y(0), z(0), l(0) {}

  bool operator<(const Vec4f &rhs) const {
    if (x != rhs.x)
      return x < rhs.x;

    if (y != rhs.y)
      return y < rhs.y;

    return z < rhs.z;
  }
};

class CTaskInit {
  std::vector<Vec4f> &m_data;

  CKernelDispatcher *m_dispatcher;

public:
  CTaskInit(std::vector<Vec4f> &data) : m_data(data), m_dispatcher(0) {}

  std::vector<Vec4f> &data() const { return m_data; }
  size_t size() const { return m_data.size(); }

  void setDispatcher(CKernelDispatcher *d) { m_dispatcher = d; }

  void operator()(const tbb::blocked_range<size_t> &r) const {
    size_t i;
    for (i = r.begin(); i != r.end(); ++i) {
      m_data[i].x = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 100));
      m_data[i].y = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 100));
      m_data[i].z = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 100));
    }
    if (m_dispatcher)
      m_dispatcher->Step((int)r.size());
  }
};

class CTaskLength {
  std::vector<Vec4f> &m_data;

  CKernelDispatcher *m_dispatcher;

public:
  CTaskLength(std::vector<Vec4f> &data) : m_data(data), m_dispatcher(0) {}

  std::vector<Vec4f> &data() const { return m_data; }
  size_t size() const { return m_data.size(); }

  void setDispatcher(CKernelDispatcher *d) { m_dispatcher = d; }

  void operator()(const tbb::blocked_range<size_t> &r) const {
    size_t i;
    for (i = r.begin(); i != r.end(); ++i) {
      m_data[i].l = std::sqrt(m_data[i].x * m_data[i].x + m_data[i].y * m_data[i].y + m_data[i].z * m_data[i].z);
    }
    if (m_dispatcher)
      m_dispatcher->Step((int)r.size());
  }
};

class Progress : public IProgressBase {
public:
  virtual void Step(int n = 1) { std::cout << n << " "; }
};

int main(int argc, char *argv[]) {
  srand((unsigned int)time(0));

#ifdef _DEBUG
  // this is enough to get a decent idea of what is done
  std::vector<Vec4f> v(1000000);
#else
  // and this gives better timings
  std::vector<Vec4f> v(100 * 1000000);
#endif

  CTaskInit taskInit(v);
  CTaskLength taskLength(v);

#ifdef TEST_SEQUENTIAL
  // slow, and not interesting
  CKernelSequential<CTaskInit> seqKernelInit;
  CKernelSequential<CTaskLength> seqKernelLength;

  seqKernelInit.execute(taskInit);
  std::cout << "seq init executed in " << seqKernelInit.seconds() << " seconds" << std::endl;

  seqKernelInit.sort(taskInit);
  std::cout << "seq init sorted in " << seqKernelInit.seconds() << " seconds" << std::endl;

  seqKernelLength.execute(taskLength);
  std::cout << "seq length executed in " << seqKernelLength.seconds() << " seconds" << std::endl;
#endif

  CKernelParallel<CTaskInit> parKernelInit;
  CKernelParallel<CTaskLength> parKernelLength;

  parKernelInit.execute(taskInit);
  std::cout << "par init executed in " << parKernelInit.seconds() << " seconds" << std::endl;

  parKernelInit.sort(taskInit);
  std::cout << "par init sorted in " << parKernelInit.seconds() << " seconds" << std::endl;

  parKernelLength.execute(taskLength);
  std::cout << "par length executed in " << parKernelLength.seconds() << " seconds" << std::endl;

  CKernelDispatcher kernelDispatcher;
  Progress progress;

  IDispatchedTask *disTaskInit = NEW_DISPATCH_TASK(CKernelParallel, CTaskInit)(parKernelInit, taskInit);

  kernelDispatcher.launch(progress, disTaskInit);

  std::cout << std::endl
            << "dispatched par init executed in " << kernelDispatcher.seconds()
            << " seconds (kernel says: " << parKernelInit.seconds() << "s)" << std::endl;

  IDispatchedTask *disTaskLength = NEW_DISPATCH_TASK(CKernelParallel, CTaskLength)(parKernelLength, taskLength);

  kernelDispatcher.launch(progress, disTaskLength);

  std::cout << std::endl
            << "dispatched par length executed in " << kernelDispatcher.seconds()
            << " seconds (kernel says: " << parKernelLength.seconds() << "s)" << std::endl;

  kernelDispatcher.Init(progress);

  for (size_t i = 0; i < 100; ++i) {
    IDispatchedTask *disTaskLengthLocal = NEW_DISPATCH_TASK(CKernelParallel, CTaskLength)(parKernelLength, taskLength);
    kernelDispatcher.AddTask(disTaskLengthLocal);
    kernelDispatcher.ProgressStep();
  }

  kernelDispatcher.Wait();

  std::cout << std::endl
            << "100 * dispatched par length executed in " << kernelDispatcher.seconds() << " seconds" << std::endl;

  CKernelSequential<CTaskLength> seqKernelLength;

  kernelDispatcher.Init(progress);

  for (size_t i = 0; i < 100; ++i) {
    IDispatchedTask *disTaskLengthLocal =
        NEW_DISPATCH_TASK(CKernelSequential, CTaskLength)(seqKernelLength, taskLength);
    kernelDispatcher.AddTask(disTaskLengthLocal);
    kernelDispatcher.ProgressStep();
  }

  kernelDispatcher.Wait();

  std::cout << std::endl
            << "100 * dispatched seq length executed in " << kernelDispatcher.seconds() << " seconds" << std::endl;

  return 0;
}