// StorageNode.h: interface for the CStorageNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STORAGENODE_H__B7B2B7DB_DB8E_496B_970C_14AA979F1E85__INCLUDED_)
#define AFX_STORAGENODE_H__B7B2B7DB_DB8E_496B_970C_14AA979F1E85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDoubleQuantity;

#include "ArchiveInterface.h"
#include "GraphEntry.h"

#define RECALCULATE_ALL			0
#define RECALCULATE_LINEAR		1

#define IGNORE_ARCHIVE_BOOL_COMPAT

#if _MSC_VER > 1200
#ifndef IGNORE_ARCHIVE_BOOL_COMPAT
// wedx 26102007
// These two (non implemented) functions prevent usage of the CArchive operator<< and operator>> functions
// for bool variables.
// If a bool was written to a CArchive in VS6.0 it would implicitly be casted to an int. Reading a bool
// was impossible and an int was read in such cases. In VS7 (.NET 2003) and higher CArchive member methods
// were added for reading and writing bool values specifically. They would be written as a BYTE (in contrast
// with 4 bytes for an int). In old code this results in the situation where a bool is written as a BYTE and
// read as an int (4 bytes).
//
// If such code is encountered an error is raised by the (new) compiler that an ambiguous call is made.
//
// There are two options
// - Change the storage code such that an int is explicitly written
// - Change the reading code such that a bool is explicitly read and define IGNORE_ARCHIVE_BOOL_COMPAT to get
//    rid of the error message
CArchive& operator<<(CArchive& ar, bool b);
CArchive& operator>>(CArchive& ar, bool& b);
#endif
#endif

class CStreamVersion;
class CFemAppModel;

#include "IProgressBase.h"

class CStorageNode : public CGraphNode 
{
	int m_nIndex;
	CFemAppModel& m_model;
public:
// Type defs
	typedef IProgressBase		TPROGRESS;
	typedef CArchiveInterface	TSTREAM;
	typedef CStreamVersion			TSTREAMVERSION;

// Construction
	CStorageNode(CFemAppModel& model);
	CStorageNode(const QString &strName, CFemAppModel& model);
	CStorageNode(const unsigned int uName, CFemAppModel& model);
	CStorageNode(const CStorageNode &rhs);
	virtual ~CStorageNode();

// Assignment and equal
	CStorageNode& operator=(const CStorageNode &rhs);
	bool operator==(const CStorageNode &rhs) const;

// Save and load functions filos
	virtual bool Empty() const = 0;
	int Index() const;
	virtual long SavedItems() const = 0;

// Save and load stream
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	void Index(int nIndex);

// Save and load quantities
	virtual void Load(CDoubleQuantity &quantity, TSTREAM &stream) const;
	virtual void Save(const CDoubleQuantity &quantity, TSTREAM &stream) const;

	CFemAppModel& Model();
	const CFemAppModel& Model() const;

	// Model invalidation ...
	void InvalidateStorage();
	void InvalidateCalculation(int nType = RECALCULATE_ALL);

	// Modified invalidates the file
	virtual void Modified(enum ModifiedHint uHint = Default);

  virtual bool Accept(graphnode::IConstVisitor& visitor) const { return visitor.VisitStorageNode(*this); }
  virtual bool Accept(graphnode::IVisitor& visitor) { return visitor.VisitStorageNode(*this); }
};

template<class T>
class CStorageNodeEntry : public CGraphEntryTemp<T>
{
public:
	typedef CStorageNode::TPROGRESS		TPROGRESS;
	typedef CStorageNode::TSTREAM		TSTREAM;

	CStorageNodeEntry(const int nEntryId, const unsigned int uIconId, const QString& strName, CFemAppModel& model)
		: CGraphEntryTemp<T>(nEntryId, uIconId, strName, model)
	{
	}

	CStorageNodeEntry(const int nEntryId, const unsigned int uIconId, const unsigned int uNameId, CFemAppModel& model)
		: CGraphEntryTemp<T>(nEntryId, uIconId, uNameId, model)
	{
	}

	virtual long SavedItems() const
	{
		long lRet = 0;
		typename CGraphEntryTemp <T> ::TNodeSet stNode =
			CGraphEntryTemp <T> ::EntryNodes();
		for(typename CGraphEntryTemp <T> ::TNodeSet::iterator it =
			stNode.begin(); it != stNode.end(); it++)
		{
			lRet += (*it)->SavedItems();
		}
		return lRet;
	}

	virtual void LoadStream(CFemAppModel& model, TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
	{
		int nSize;
		stream >> nSize;
		for(int i = 0; i < nSize; i++)
		{
			T* pStorageNode = new T(model);
			pStorageNode->LoadStream(stream, version, progress);
			assert( pStorageNode->CGraphNode::parent() == this );
		}
	}
	
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress)
	{
		typename CGraphEntryTemp <T> ::TNodeSet stNode =
			CGraphEntryTemp <T> ::EntryNodes();
    int nSize = (int)stNode.size();
		stream << nSize;
		for(typename CGraphEntryTemp <T> ::TNodeSet::iterator it =
			stNode.begin(); it != stNode.end(); it++)
		{
			assert( (*it)->CGraphNode::parent() == this );
			(*it)->SaveStream(stream, progress);
		}		
	}
};


#endif // !defined(AFX_STORAGENODE_H__B7B2B7DB_DB8E_496B_970C_14AA979F1E85__INCLUDED_)
