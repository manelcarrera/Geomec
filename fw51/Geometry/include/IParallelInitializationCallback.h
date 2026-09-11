#ifndef _I_PARALLEL_INITIALIZATION_CALLBACK_
#define _I_PARALLEL_INITIALIZATION_CALLBACK_

// Base class for a cache wrapper. Allows caches to be moved from a container object (like composites and components) to
// the calling thread. See, eg, IVectorResult. Can be setup in the composite or component, whatever is convenient.

namespace geo {

class IParallelInitializationCallback {
public:
  IParallelInitializationCallback();
  virtual ~IParallelInitializationCallback();

  static IParallelInitializationCallback *Sequential;
};

} // namespace geo

#endif