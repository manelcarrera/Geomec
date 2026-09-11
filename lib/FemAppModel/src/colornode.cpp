// ColorNode.cpp: implementation of the CColorNode class.
//
//////////////////////////////////////////////////////////////////////

#include "StorageNode.h"
#include "OpenGLNodeBase.h"
#include "openglnode.h"
#include "colornode.h"
#include "ColorEntry.h"
#include "FemAppModel.h"
#include "FemAppEntryTypes.h"
#include "IProgressBase.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CColorNode::CColorNode(CFemAppModel& model)
: COpenGLNode(model), m_nColor(0)
{
}

CColorNode::CColorNode(const QString &strName, CFemAppModel& model, COLORREF nColor)
: COpenGLNode(strName, model), m_nColor(nColor)
{
	// Link To the factory
	assert(Model().GraphEntry(MD_BASE_COLOR_NODE));
	LinkTo(*Model().GraphEntry(MD_BASE_COLOR_NODE));
}

CColorNode::CColorNode(const unsigned int uName, CFemAppModel& model, COLORREF nColor)
: COpenGLNode(uName, model), m_nColor(nColor)
{
	// Link To the factory
	assert(Model().GraphEntry(MD_BASE_COLOR_NODE));
	LinkTo(*Model().GraphEntry(MD_BASE_COLOR_NODE));
}


CColorNode::CColorNode(const QString &strName, CFemAppModel& model)
: COpenGLNode(strName, model)
{
	CColorEntry *pEntry = (CColorEntry*)(Model().GraphEntry(MD_BASE_COLOR_NODE));
	m_nColor = pEntry->GetColor();
	LinkTo(*pEntry);
}

CColorNode::CColorNode(const unsigned int uName, CFemAppModel& model)
: COpenGLNode(uName, model)
{
	CColorEntry *pEntry = (CColorEntry*)(Model().GraphEntry(MD_BASE_COLOR_NODE));
	m_nColor = pEntry->GetColor();
	LinkTo(*pEntry);
}

CColorNode::CColorNode(const CColorNode &rhs)
: COpenGLNode(rhs), m_nColor(rhs.m_nColor)
{
}


CColorNode::~CColorNode()
{
	// TODO : Release color factory if any ...
}
	
CColorNode::TColor CColorNode::Color() const
{
	return m_nColor;
}

void CColorNode::Color(TColor nColor)
{
	if(m_nColor != nColor)
	{
		m_nColor = nColor;
		Modified(COLOR_CHANGED);
	}

	// TODO : Release color factory if any ...
}

CColorNode& CColorNode::operator=(const CColorNode &rhs)
{
	COpenGLNode::operator=(rhs);

	m_nColor = rhs.m_nColor;
	
	return *this;
}

bool CColorNode::operator==(const CColorNode &rhs) const
{
	if(!COpenGLNode::operator==(rhs))
		return false;

	return m_nColor == rhs.m_nColor;
}

bool CColorNode::Empty() const
{
	CColorEntry *pEntry = (CColorEntry*)(((CFemAppModel&)Model()).GraphEntry(MD_BASE_COLOR_NODE));
	return !IsLinkedTo(*pEntry);
}

void CColorNode::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
	int iRed, iGreen, iBlue;

	// Load the three color components
	stream >> iRed;		// Red
	stream >> iGreen;	// Green 
	stream >> iBlue;	// Blue

	// Update progress
	progress.Step();

	// Set the color
	m_nColor = qRgb(iRed, iGreen, iBlue);

	// Load base
	COpenGLNode::LoadStream(stream, version, progress);

	// LinkTo 
	CColorEntry *pEntry = (CColorEntry*)(Model().GraphEntry(MD_BASE_COLOR_NODE));
	assert(!IsLinkedTo(*pEntry));
	LinkTo(*pEntry);
}

void CColorNode::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
	int iRed = qRed(m_nColor);
	int iGreen = qGreen(m_nColor);
	int iBlue = qBlue(m_nColor);

	// Save the three color components
	stream << iRed;
	stream << iGreen;
	stream << iBlue;

	// Update progress
	progress.Step();

	// Save the base shit
	COpenGLNode::SaveStream(stream, progress);
}

long CColorNode::SavedItems() const
{
	// This function should return the number of progress.Step() in the above Save() function....
	return COpenGLNode::SavedItems() + 1;
}

