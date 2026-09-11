// FemAppModel.cpp: implementation of the CFemAppModel class.
//
//////////////////////////////////////////////////////////////////////

#include "FemAppEntryTypes.h" 
#include "FemAppModel.h"
#include "ModelProxyConsole.h"
#include "DocumentProxyConsole.h"
#include "GraphEntry.h"
#include "OpenGLNodeBase.h"
#include "ColorEntry.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFemAppModel::CFemAppModel(CAnalysisLogger& logger)
: CGraphNode("Model")
, m_logger(logger)
, m_modelProxy(0)
, m_documentProxy(0)
, m_deleting(false)
, m_savingSmoothedResults(false)
{
	CDocumentBase::create();
  setModelProxy(new CModelProxyConsole(*this));
  setDocumentProxy(new CDocumentProxyConsole());
}

CFemAppModel::~CFemAppModel()
{
	assert(!isOpen());

  if (m_modelProxy != 0)
  {
    delete m_modelProxy;
  }

  if (m_documentProxy != 0)
  {
    delete m_documentProxy;
  }
}

void CFemAppModel::OnExit()
{
}

void CFemAppModel::OnCloseModel()
{
  bool succeeded = true;

  // Not needed here:
  //if (getDocumentProxy()->currentScene() != 0)
  //  getDocumentProxy()->currentScene(0);

  if(GraphEntry(MD_BASE_SCENE_NODE))
    succeeded = DeleteEntry(MD_BASE_SCENE_NODE);

  succeeded = succeeded && DeleteEntry(MD_BASE_OPENGL_NODE);
  succeeded = succeeded && DeleteEntry(MD_BASE_COLOR_NODE);

  assert(succeeded);
}

void CFemAppModel::createContainers()
{
  new TOpenGLNodeBaseEntry(MD_BASE_OPENGL_NODE, 0, "OpenGLNodes", *this);
  new CColorEntry(*this);
  assert(m_modelProxy != 0);
  m_modelProxy->createContainers();
}

void CFemAppModel::createDefaults()
{
}

void CFemAppModel::OnInvalidateStorage()
{
	getDocumentProxy()->onInvalidateStorage();
}

void CFemAppModel::OnInvalidateCalculation(int /*nType*/)
{
	// Note : Document invalidates the calculation
}


//##ModelId=3BC55D39011F
CGraphEntry* CFemAppModel::GraphEntry(const int nEntryType)
{
	CEntryMap::iterator it = m_mpEntry.find(nEntryType);
	if(it != m_mpEntry.end())
	{
		// GraphEntry found ...
		return it->second;
	}
	
	return 0;
}

const CGraphEntry* CFemAppModel::GraphEntry(const int nEntryType) const
{
	if((m_mpEntry.find(nEntryType) != m_mpEntry.end()) ) {
		// GraphEntry found ...
		return m_mpEntry.find(nEntryType)->second;
	}
	
	return 0;
}

//##ModelId=3BF230B80263
bool CFemAppModel::DeleteEntry(int nEntryType)
{
	if(!GraphEntry(nEntryType))
		return false;

	delete GraphEntry(nEntryType);

	return true;
}

//##ModelId=3BC55D390111
bool CFemAppModel::NewModel(bool /*bAttachToDocument*/)
{
	// If the model is open close it
	if(isOpen()) {
		CloseModel();
	}

	newDocument();

	return true;
}

void CFemAppModel::DestroyCaches()
{
}

void CFemAppModel::CloseModel()
{
	// Call OnCloseModel to call the upper class set the close flag and if 
	// have document dettach from it
  assert(m_modelProxy != 0);
  m_modelProxy->onCloseModel();

  assert(m_modelProxy != 0);
  m_modelProxy->detachModel();

	closeDocument();
}

void CFemAppModel::setModelProxy(CModelProxy* modelProxy)
{
  /*
   * It could be possible to initialize m_closeModel while it still contains
   * a valid pointer. Since CFemAppModel takes ownership it has the duty to
   * destruct the already existing CCloseModel object.
   */

  if (m_modelProxy != 0)
  {
    delete m_modelProxy;
  }

  m_modelProxy = modelProxy;
}

CDocumentProxy* CFemAppModel::getDocumentProxy() const
{
  return m_documentProxy;
}

void CFemAppModel::setDocumentProxy(CDocumentProxy* documentProxy)
{
  /*
   * It could be possible to initialize m_documentProxy while it still contains
   * a valid pointer. Since CModelBase takes ownership it has the duty to
   * destruct the already existing CDocumentProxy object.
   */

  if (m_documentProxy != 0)
  {
    delete m_documentProxy;
  }

  m_documentProxy = documentProxy;
}

bool CFemAppModel::getDeleting() const
{
  return m_deleting;
}

void CFemAppModel::setDeleting(bool deleting)
{
  m_deleting = deleting;
}

bool CFemAppModel::getSavingSmoothedResults() const
{
  return m_savingSmoothedResults;
}

void CFemAppModel::setSavingSmoothedResults(bool savingSmoothedResults)
{
  m_savingSmoothedResults = savingSmoothedResults;
}
