#ifndef _RegistryCleanupWrapper_h_
#define _RegistryCleanupWrapper_h_

class CRegistryCleanupWrapper {
public:
  CRegistryCleanupWrapper();
  ~CRegistryCleanupWrapper();

private:
  CRegistryCleanupWrapper(const CRegistryCleanupWrapper &rhs);
  CRegistryCleanupWrapper &operator=(const CRegistryCleanupWrapper &rhs);
};

#endif // _RegistryCleanupWrapper_h_
