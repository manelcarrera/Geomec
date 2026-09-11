// treeobject.h: interface for the ITreeObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREEOBJECT_H__B0597E40_F5EB_45CB_B5DF_A5C980A19E66__INCLUDED_)
#define AFX_TREEOBJECT_H__B0597E40_F5EB_45CB_B5DF_A5C980A19E66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CDSealectTreeView;
class CBaseFormView;
class CMultiViewSplitter;
class CTreeEntryLeaf;
class CTreeEntryBranch;

//##ModelId=3C0DC1560186
class ITreeObject 
{
private:
	//##ModelId=3C0DC15601A5
	HTREEITEM m_hTreeItemHandle;
	//##ModelId=3CAAA66200FE
	CDSealectTreeView* m_pTreeView;
protected:
	//##ModelId=3C0DC156032C
	~ITreeObject();
	//##ModelId=3C0DC156033C
	ITreeObject();


public:
	//##ModelId=3C0DC156034B
	CDSealectTreeView* TreeView();
	//##ModelId=3C0DC156034C
	CTreeCtrl* TreeCtr();
	//##ModelId=3C0DC156034D
	HTREEITEM TreeItemHandle();
	//##ModelId=3C0DC156034E
	void InsertInTree(CDSealectTreeView* pTreeView, HTREEITEM hParent=TVI_ROOT,HTREEITEM hInsertAfter=TVI_LAST);

	//virtuals
	//##ModelId=3C0DC156035B
	virtual CString Name() const=0;
	//##ModelId=3C0DC156035D
	virtual UINT IconID() const=0;
	//##ModelId=3C0DC156035F
	virtual BOOL Enable() const=0;
	//##ModelId=3C0DC156036C
	virtual void Enable(BOOL newval,BOOL bUpdateWindow=true)=0;

};


//##ModelId=3C0DC1570000
class ITreeEntryBase:public ITreeObject
{

friend CTreeEntryLeaf;
friend CTreeEntryBranch;
private:
	//##ModelId=3C0DC1570002
	ITreeEntryBase()
	{
		m_bEnable=TRUE;
		m_pNext=0;
		m_pPrevious=0;
		m_pFormView=0;
		m_pSplitter=0;
		m_ViewID=-1;
		m_IconID=-1;
	}


public:
	//##ModelId=3C0DC1570003
	void UpdateEnable();
	//##ModelId=3C0DC157000F
	ITreeEntryBase* GetFirstEntry();
	//##ModelId=3C0DC1570010
	CBaseFormView* FormView();
	//##ModelId=3C0DC1570011
	CMultiViewSplitter* Splitter();
	//##ModelId=3C0DC1570012
	virtual void Activate(ITreeEntryBase* prev=NULL);
	//##ModelId=3C0DC157001F
	virtual CString Name() const;
	//##ModelId=3C0DC1570021
	virtual UINT IconID() const;
	//##ModelId=3C0DC157002E
	virtual UINT ViewID() const;

	//##ModelId=3C0DC1570030
	virtual BOOL Enable() const;
	//##ModelId=3C0DC1570032
	virtual void Enable(BOOL newval,BOOL bUpdateWindow=true);

	//##ModelId=3C0DC1570040
	ITreeEntryBase* Next();
	//##ModelId=3C0DC1570041
	ITreeEntryBase* Previous();

protected:
	//##ModelId=3CAAA66201CB
	ITreeEntryBase* m_pNext;
	//##ModelId=3CAAA66201DB
	ITreeEntryBase* m_pPrevious;
	//##ModelId=3CAAA66201E8
	CBaseFormView*	m_pFormView;
	//##ModelId=3CAAA66201F8
	CMultiViewSplitter* m_pSplitter;
	//##ModelId=3C0DC157008C
	CString m_strName;
	//##ModelId=3C0DC157009C
	int m_ViewID;
	//##ModelId=3C0DC157009D
	UINT m_IconID;
private:
	//##ModelId=3C0DC15700AB
	BOOL m_bEnable;
};


//##ModelId=3C0DC15700DA
class CTreeEntryBranch :public ITreeEntryBase
{
friend ITreeEntryBase;

public:
	//##ModelId=3C0DC15700DC
	CTreeEntryBranch(CBaseFormView* FormView,
					 CMultiViewSplitter* SplitterView,
					 CString Name,
					 int ViewID,
					 CString Text,
					 UINT IconID,
					 ITreeEntryBase* pPrevious =NULL);
	
	//##ModelId=3C0DC15700ED
	CString Text();
private:
	//##ModelId=3C0DC15700EE
	CString m_strText;
};	


//##ModelId=3C0DC15700FA
class CTreeEntryLeaf:public ITreeEntryBase
{
friend ITreeEntryBase;
public:
	//##ModelId=3C0DC157010A
	CTreeEntryLeaf(CBaseFormView* FormView,
					 CMultiViewSplitter* SplitterView,
					 CString Name,
					 int ViewID,
					 UINT IconID,
					 ITreeEntryBase* pPrevious =NULL);
};




#endif // !defined(AFX_TREEOBJECT_H__B0597E40_F5EB_45CB_B5DF_A5C980A19E66__INCLUDED_)
