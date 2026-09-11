#if !defined(_C_CACHE_INTERFACE_DEFAULT_H_)
#define _C_CACHE_INTERFACE_DEFAULT_H_

#include "ICacheInterface.h"

#include "Point.h"
#include "ElementPoint.h"
#include "IElementSet.h"
#include "ElementCacheObject.h"

#include <vector>

namespace geo {


class GEOMETRY_EXPORT CCacheInterfaceDefault : public ICacheInterface
{
	CCacheInterfaceDefault() {}
	CCacheInterfaceDefault(const CCacheInterfaceDefault &/*rhs*/) {}
	CCacheInterfaceDefault& operator=(const CCacheInterfaceDefault &/*rhs*/) { return *this; }
public:
	static CCacheInterfaceDefault& Instance()
	{
		static CCacheInterfaceDefault singleton;
		return singleton;
	}

	virtual bool NeedsDelete() { return false; }
	virtual void Resize(std::size_t /*size*/) {}

	virtual CPoint Min(int /*nIndex*/) { return CPoint(); }
	virtual void Min(int /*nIndex*/, CPoint &/*Point*/) {}

	virtual CPoint Max(int /*nIndex*/) { return CPoint(); }
	virtual void Max(int /*nIndex*/, CPoint &/*Point*/) {}

	virtual CElementPoint Mid(int /*nIndex*/) { return CElementPoint(); }
	virtual void Mid(int /*nIndex*/, CElementPoint &/*ElementPoint*/) {}

	virtual bool PointMap(const IPoint &/*Point*/, std::vector<int> &/*Map*/) { return false; }
	virtual void SetPointMap(const IPoint &/*Point*/, std::vector<int> &/*Map*/) {}

	virtual bool ShapeMap(int /*nIndex*/, const IPoint &/*Point*/, std::vector<double> &/*Map*/) { return false; }
	virtual void SetShapeMap(int /*nIndex*/, const IPoint &/*Point*/, std::vector<double> &/*Map*/) {}

	virtual CElementCacheObject* ElementCacheObject(const IElementSet* /*elt_set*/, const IBody* /*body*/) { return 0; }
	virtual void ElementCacheObject(const IElementSet* /*elt_set*/, int /*nIndex*/, CElementCacheObject &/*ElementCacheObject*/) {}

	virtual void Invalidate() {}
  virtual bool Cached(const IElementSet* /*elt_set*/) { return false; }

};

}

#endif
