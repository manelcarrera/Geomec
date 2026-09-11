// DCasintScene.h: interface for the CDCasintScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCASINTSCENE_H__AC69CE99_E13E_4538_923A_9176FDBFF8AF__INCLUDED_)
#define AFX_DCASINTSCENE_H__AC69CE99_E13E_4538_923A_9176FDBFF8AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDCasintScene  
{
	void Update();
	void Init();
	friend class CHistoryGraphParent;
public:
	CDCasintScene(CFemAppModel &model);
	CDCasintScene(UINT uName, CFemAppModel &model); 
	virtual ~CDCasintScene();

	void ClearResult();

	virtual void OnNeighbourDeleted(const CGraphNode &node);
	virtual void OnNewNeighbour(const CGraphNode &node);
	virtual void LoadStream(TSTREAM& stream, CVersion& version, TPROGRESS& progress);

// Connection ...
	virtual	bool ConnectItem(const CGraphNode &node);
	virtual bool CanConnectItem(const CGraphNode &node) const;

	// save and load
	virtual CString GetTypeString() const;
	virtual bool Properties();

	const CHistoryGraphParent& GraphParent() const;
	CHistoryGraphParent& GraphParent();

	int ResultComponentSize() const;
	const IResultComponent& ResultComponent(int nIndex) const;
};

#endif // !defined(AFX_DCASINTSCENE_H__AC69CE99_E13E_4538_923A_9176FDBFF8AF__INCLUDED_)
