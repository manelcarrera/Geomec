#if !defined(_C_CACHE_MANAGER_H_)
#define _C_CACHE_MANAGER_H_

#include <map>

namespace geo {
class IElementSet;
}

class CCacheInterface;

class CCacheManager
{
	typedef std::map<const geo::IElementSet *, CCacheInterface *> TInterfaces;
	TInterfaces m_interfaces;

public:
	CCacheManager() {}
	virtual ~CCacheManager() {}

  virtual void DestroyCaches();

	virtual void RegisterElementSet(const geo::IElementSet *elt_set);
	virtual void UnregisterElementSet(const geo::IElementSet *elt_set);

protected:
	virtual void InvalidateMappings(const geo::IElementSet *elt_set);
};


#endif  // _C_CACHE_MANAGER_H_
