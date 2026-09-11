// CrossSectionObserver.h: interface for the CCrossSection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CROSSSECTIONOBSERVER_H__546C7678_8805_4507_9733_43F2EDD43CF8__INCLUDED_)
#define AFX_CROSSSECTIONOBSERVER_H__546C7678_8805_4507_9733_43F2EDD43CF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef COpenGLNodeObserver_Delegate<CCrossSection, CCrossSection_Delegate, CDummyNode, CDummyObserver, FALSE,
                                     FIXED_ITEM>
    TCrossSectionObserver;

#endif // !defined(AFX_CROSSSECTIONOBSERVER_H__546C7678_8805_4507_9733_43F2EDD43CF8__INCLUDED_)
