 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ObjectDispatcher.h: interface for the CObjectDispatcher class.
//

#include "GeometryExports.h"
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTDISPATCHER_H__F63B6F5E_EAA2_40A4_8E80_E47167313FFA__INCLUDED_)
#define AFX_OBJECTDISPATCHER_H__F63B6F5E_EAA2_40A4_8E80_E47167313FFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace geo {

class GEOMETRY_EXPORT  CObjectDispatcher  
{
public:
  CObjectDispatcher();
  virtual ~CObjectDispatcher();

  // dispatches the object and delegates to handler to handle the dispatched object
  bool Dispatch(const IObject &object, CDispatchVisitorBase &handler);
};

} // namespace geo

#endif // !defined(AFX_OBJECTDISPATCHER_H__F63B6F5E_EAA2_40A4_8E80_E47167313FFA__INCLUDED_)
