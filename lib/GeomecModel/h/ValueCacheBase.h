#ifndef _ValueCacheBase_h_
#define _ValueCacheBase_h_

class CValueCacheBase {
public:
  virtual void clearCache() = 0;

protected:
  CValueCacheBase();
  virtual ~CValueCacheBase() = 0;
};

#endif // _ValueCacheBase_h_
