#if !defined(AFX_RESULT_CACHE_TEMPLATE_H_INCLUDED_)
#define AFX_RESULT_CACHE_TEMPLATE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "result.h"

template <class T> class _ResultCache {
public:
  std::vector<T> m_value;
  const geo::CPoint m_point;
  const geo::IElement *m_element;
  bool m_mapped;
  typedef CValueType::MAP_TYPE TMapType;
  TMapType m_map_type;
  _ResultCache() : m_point(0), m_element(0), m_mapped(false), m_map_type(CValueType::MT_NONE) {}
};

template <class T> class IResultCacheTemplate : public IResult {
  typedef std::vector<_ResultCache<T>> TCache;
  typedef std::pair<TCache, TCache> TCachePair;
  typedef std::vector<TCachePair> TCacheVec;
  mutable TCacheVec m_cache;
  void BuildCache() const;

public:
  IResultCacheTemplate() {}
  typedef std::vector<T> TValueVec;
  typedef CValueType::MAP_TYPE TMapType;
  const T &ValuePoint(const geo::IPoint &point, const CResultRegister &result_register, const CDepletionStage &stage,
                      bool bLinear) const;
  const TValueVec &ValueElement(const geo::IElement &element, const CResultRegister &result_register,
                                const CDepletionStage &stage, bool bLinear) const;
  const TValueVec &MapValueElement(const geo::IElement &element, TMapType map_type,
                                   const CResultRegister &result_register, const CDepletionStage &stage,
                                   bool bLinear) const;
  virtual void ClearCache();

protected:
  virtual void onSetValuePoint(const geo::IPoint &point, T &value, const CResultRegister &result_register,
                               const CDepletionStage &stage, bool bLinear) const = 0;
  virtual void onSetValueElement(const geo::IElement &element, TValueVec &value, const CResultRegister &result_register,
                                 const CDepletionStage &stage, bool bLinear) const = 0;
  virtual void onSetMapValueElement(const geo::IElement &element, TValueVec &value, TMapType map_type,
                                    const CResultRegister &result_register, const CDepletionStage &stage,
                                    bool bLinear) const = 0;
};

#endif // !defined(AFX_RESULT_CACHE_TEMPLATE_H_INCLUDED_)
