// GeomecDoc.h : interface of the CGeomecDoc class
// 
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOMECDOC_H__E321AF99_DA35_4988_AC51_3F4AAC08E45C__INCLUDED_)
#define AFX_GEOMECDOC_H__E321AF99_DA35_4988_AC51_3F4AAC08E45C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include < Utility>

#include "StreamVersion.h"
#include "GeomecDianaRunner.h"
#include "IListener.h"

#define ON_REGISTER_FONT				1045
#define ON_UNREGISTER_FONT				1046
#define ON_NODAL_SUPPORT_MOUSELISTENER	1047
#define ON_FACE_SUPPORT_MOUSELISTENER	1048

#define ON_UPDATE_CURRENT_SCENE			2000
#define SWITCH_TO_OPENINVENTOR      2001

#define ID_ON_CALCULATE_MUD_WEIGHT		3000 //TODO Place in recource

#define GEOMEC_DEFAULT_EXTENSION  "gm5"


#define ID_LAST 0xDFFF
//#define ID_INERSION_TOOL ID_LAST-1
//#define ID_ON_RUN_MODEL  3999 //FIXME
#define ID_ON_RUN_MODEL  ID_LAST-2
#define ID_ON_RUN_STEP  ID_LAST-3
#define ID_ON_WRONG_PYTHONROOT  ID_LAST-4

//##ModelId=3BC55D630054
class CProgressDlg_MFC;
class CModelBase;
class CAnalysisType;
class CModelTreeView;
class CDataTreeView;
class CModelView;
class COpenGLSplitView;
class CGUIBase;
class CTreeNode;

class ChangelogDlg;

//class CDianaStartUp;


#include "VersionManager.h"
#include "AnalysisLogger.h"
#include "FemAppDoc.h"
#include "GeomecDocDianaSignals.h"

#include "IDCAnalysisStatus.h" //MCR

#include "Events.h"
#include "Interprocess_OLD.h"

#include "RBox.h"

#include "IListener.h"


//synchro
#include <mutex>
#include <condition_variable>
#include <thread>

class CGeomecDoc_EventsHandler;

struct DSA_Context;

namespace
{
  static const IListenerDefinition RunDianaGui_ListenerDefinition{ 
    CEvents::GuiApp_EH,
    {	
      Progress, 
      DianaMsg,
      //
      HeartBeat_Ack,
      //
      Log,
      Status,
      //
      UpdateViews, 
      DisableViews, 
      //	
      Save_Done,			// the update views can be performed
      Quit_RA,			// neatly ends run analysis process
      ChecksError, 
      RunError, 
      //
      Question,
      //
      CancelRun,
      //
      QuitDiana_Done
    } 
  };
};

namespace gm{
namespace doc_{
  namespace gui{
    void popup(
      const QString& title, 
      const QString& msg, 
      int timeout=0);
  }//namespace gui
  CString executable();
  QString version(const QString& path_);

  enum eVersion{ App, Folder };
  QString version( eVersion type_ = Folder );
  QString version_APP_OLD();
}
}


class CGeomecDoc : public CFemAppDoc, public IListener
{
  CGeomecDoc_EventsHandler* m_eh;

  ChangelogDlg* m_changelog_dlg;

  //
  // it looks like not needed
  //
  friend class CTest;

public:
  DSA_Icon m_dsa_icon_id;

  //
  // Functonal test
  //
  std::map<UINT,AFX_PMSG> m_func_map;


public:
  void set_dsa_icon( DSA_Icon icon_id );
  bool test(); // test running?

  ChangelogDlg* changelog_dlg(){ return m_changelog_dlg; }
  void changelog_dlg( ChangelogDlg* dlg ){ m_changelog_dlg = dlg; }

private:

  bool m_bStartUp;
  CAnalysisLogger m_Logger;
  QString m_app_version;
  CVersionManager m_versionManager;
  CGeomecDocDianaSignals m_geomecDocDianaSignals;

protected:
  void handle(Cmd cmd);

public:
  //
  // used during RA 
  //
  void update_views();
  void disable_views();


public:
  CAnalysisLogger& Logger() { return m_Logger; }
  const CAnalysisLogger& Logger() const { return m_Logger; }

protected: // create from serialization only
  //##ModelId=3BC55D630092
  CGeomecDoc();

  DECLARE_DYNCREATE(CGeomecDoc)
public:
  enum FILE_STATUS {FORMAT_OK, FORMAT_ERROR, FORMAT_UNKNOWN, CANCEL };
  typedef std::pair<enum FILE_STATUS, CString> TFileStatus;
  // The filos relationship between nodes and elements
  typedef std::vector<int> TFilosNodeVec;
  typedef std::pair<const geo::IElement*, TFilosNodeVec> TFilosElement;
  typedef std::map<unsigned int, TFilosElement> TFilosElementMap;

private:
  enum FILE_STATUS OnImportASCII(const CString &sPath, IProgressBase &dlg, CGraphNode::TNodeVec& created_objects);
  enum FILE_STATUS OnImportExcel(const CString &sPath, IProgressBase &dlg, CGraphNode::TNodeVec& created_objects);
  enum FILE_STATUS OnImportEclipse(const CString& sPath, CProgressDlg_MFC& dlg);
  CModelTreeView *m_pModelTreeView;
  CDataTreeView  *m_pDataTreeView;
  CModelView* m_pModelView;
  COpenGLSplitView *m_pLegendModelView;
  bool m_bChangeMode;

  std::set<const CTreeNode*> m_stChangeSelection;

  int m_nLastCGStatus;

// Attributes
public:
   CModelTreeView *GetModelTreeView();
   CDataTreeView *GetDataTreeView();
   CModelView* GetModelView();
   COpenGLSplitView *GetLegendModelView();

  //##ModelId=3BC55D630093
  virtual void OnSelect(CGraphNode *pNode);
  BOOL OnImportFile(const CString& strFileName, IProgressBase& dlg, CGraphNode::TNodeVec& created_objects);

  bool ExperimentalUnlocked() const;
  bool Model2DUnlocked() const;

  // called by CSelectionBranchObserver_Delegate
  virtual void BranchToggle(bool bBranchToggle) {}

  // saves the model contained in the provided GUI
  // saves the root model if pGui is NULL
  BOOL SaveModel(LPCTSTR lpszPathName, CGUIBase* pGui = 0, bool bSwitch = true);

  // model name without the path and without the .gm4/.gm5
  void ModelName(CString &modelName) const;

  CGUIBase* RootGUI();

  using CFemAppDoc::CurrentScene;

  void CurrentScene(COpenInventorSceneNode *pScene);

// Operations
//protected:
public: //FIXME
  bool RunAnalysis(	const CAnalysisType::TAnalysisType antype, 
            bool bWriteInputFiles, 
            bool bWriteQuadDat);

  bool m_bRunning;
public:
  static QString CurrentAppVersion();
  afx_msg void OnAnalysisProperties();
  virtual void OnAttachModel(CFemAppModel& model, CFemAppGUI& gui);
  // Switch between existing models (don't delete anything, just switch the current context)
  virtual void OnSwitchModel(CFemAppModel& model, CFemAppGUI& gui);

  bool CanSelectComposite( const IValueComposite& composite ) const;
  bool SelectComposite( const IValueComposite& composite ) const;
  void SelectComposite( const IValueComposite& composite, bool bEnable );

  bool ChangeMode() const;
  void EnterChangeMode();
  void ExitChangeMode();
  void ToggleChangeState(const CTreeNode& node);
  bool IsChangeSelected(const CTreeNode& node) const;
  bool HasChangeSelection() const;
  void ChangeModeDeleteSelectedItems();

  TBox bbox();

  void ForceFileClose();

private:

  bool show_xsec_manipulator_when_needed();

private:
// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CGeomecDoc)
  public:
  virtual BOOL OnNewDocument();
  virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
  virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
  //}}AFX_VIRTUAL

// Implementation
private:
  void _OnImport(unsigned int);

  //CDianaStartUp* m_dsu;


public:
  virtual ~CGeomecDoc();
#ifdef _DEBUG
  //##ModelId=3BC55D6300B3
  virtual void AssertValid() const;
  //##ModelId=3BC55D6300B5
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
  //{{AFX_MSG(CGeomecDoc)
  afx_msg void OnModelProperties();
  afx_msg void OnUpdateModelProperties(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
  afx_msg void OnImport();
  afx_msg void OnUpdateImport(CCmdUI* pCmdUI);
  afx_msg void OnUpdateItemDelete(CCmdUI* pCmdUI);
  afx_msg void OnUpdateItemModify(CCmdUI* pCmdUI);
  afx_msg void OnItemAttributes();
  afx_msg void OnItemDelete();
  afx_msg void OnItemModify();
  afx_msg void OnUpdateColorScaleSelection(CCmdUI* pCmdUI);
  afx_msg void OnAnalysisLinearDepletion();
  afx_msg void OnEditPaste();
  afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
  afx_msg void OnEditCopy();
  afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
  afx_msg void OnMudWeightPoints();
  afx_msg void OnCalculateMudWeight();
  afx_msg void OnAnalysisNonlinearCalculation();
  afx_msg void OnUpdateAnalysisNonlinearCalculation(CCmdUI* pCmdUI);
  afx_msg void OnAnalysisNonlinearCalcParams();
  afx_msg void OnUpdateAnalysisNonlinearCalcParams(CCmdUI* pCmdUI);
  afx_msg void OnInsertXSection();
  afx_msg void OnUpdateInsertXSection(CCmdUI* pCmdUI);
  afx_msg void OnUpdateAnalysisProperties(CCmdUI* pCmdUI);
  afx_msg void OnCreateMesh();
  afx_msg void OnUpdateCreateMesh(CCmdUI* pCmdUI);
  afx_msg void OnSetMeshTolerance();
  afx_msg void OnUpdateSetMeshTolerance(CCmdUI* pCmdUI);
  afx_msg void OnInvalidateMesh();
  afx_msg void OnUpdateInvalidateMesh(CCmdUI* pCmdUI);
  afx_msg void OnInsertDeviatedWellPathByPointSet(); // wjrx mantis 3185
  afx_msg void OnFormationfull();
  afx_msg void OnFormationskin();
  afx_msg void OnFormationcenter();
  afx_msg void OnUpdateFormationfull(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFormationskin(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFormationcenter(CCmdUI* pCmdUI);
  //}}AFX_MSG
  afx_msg void OnUpdateAnalysisLinearDepletion(CCmdUI* pCmdUI);
  afx_msg void OnFileOpen();

  afx_msg void OnPlugins_Add();
  afx_msg void OnSetDefault();
  afx_msg void OnPlugins_Remove( UINT nID );
  afx_msg void OnPlugins_Launch( UINT nID );
  afx_msg void OnPlugins_Manual( UINT nID );
  afx_msg void OnPlugins_Remove_Update( CCmdUI* pCmdUI );
  afx_msg void OnPlugins_Manual_Update( CCmdUI* pCmdUI );


  afx_msg void OnFileSave();
  afx_msg void OnFileSaveAs();
  afx_msg void OnAnalysisLog();
  afx_msg void OnUpdateAnalysisLog(CCmdUI* pCmdUI);
  afx_msg void OnInputStatus();
  afx_msg void OnUpdateInputStatus(CCmdUI* pCmdUI);
  afx_msg void OnUpdateAnalysisExportLinear(CCmdUI* pCmdUI);
  afx_msg void OnAnalysisHeatFlow();
  afx_msg void OnUpdateAnalysisHeatFlow(CCmdUI* pCmdUI);
  afx_msg void OnAnalysisExportLinear();
  afx_msg void OnGVTSettings();
  afx_msg void OnUpdateGVTSettings(CCmdUI* pCmdUI);
  afx_msg void OnExportSVS();
  afx_msg void OnUpdateExportSVS(CCmdUI* pCmdUI);
  afx_msg void OnBorrowLicense();
  afx_msg void OnUpdateBorrowLicense(CCmdUI* pCmdUI);
  afx_msg void OnOptimizationSettings();
  afx_msg void OnUpdateOptimizationSettings(CCmdUI* pCmdUI);

  // wjrx mantis 2975
    afx_msg void OnUpdateAnalysisExportResults(CCmdUI* pCmdUI);
    afx_msg void OnAnalysisExportResults();
    afx_msg void OnUpdateAnalysisExportResultsGoCad(CCmdUI* pCmdUI);
    afx_msg void OnAnalysisExportResultsGoCad();
    afx_msg void OnUpdateAnalysisExportSurfaces(CCmdUI* pCmdUI);
    afx_msg void OnAnalysisExportResultsSurfaces();
    afx_msg void OnUpdateAnalysisExportDataPointsets(CCmdUI* pCmdUI);
    afx_msg void OnAnalysisExportDataPointsets();
    // wjrx mantis 2976
    afx_msg void OnUpdateImportMesh(CCmdUI* pCmdUI);
    afx_msg void OnImportMesh();
    afx_msg void OnImportJewelEarthMesh();
    afx_msg void OnUpdateImportResults(CCmdUI* pCmdUI);
    afx_msg void OnImportResults();
    afx_msg void OnUpdateImportInsertDeviatedWellPath(CCmdUI* pCmdUI);
    afx_msg void OnImportInsertDeviatedWellPath();

  afx_msg void OnUpdateAnalysisExportNonlin(CCmdUI* pCmdUI);
  afx_msg void OnAnalysisExportNonlin();
  afx_msg void OnAnalysisExportQuadDat();
  afx_msg void OnAnalysisExportHeatFlow();
  afx_msg void OnUpdateAnalysisExportHeatFlow(CCmdUI* pCmdUI);
  afx_msg void OnUpdateAnalysisExportQuadDat(CCmdUI* pCmdUI);
  afx_msg void OnFileClose();
  afx_msg void OnUpdateFileClose(CCmdUI* pCmdUI);
  afx_msg void OnAnalysisLinearSettings();
  afx_msg void OnUpdateAnalysisLinearSettings(CCmdUI* pCmdUI);
  afx_msg void OnUpdateAnalysisClearHistory(CCmdUI* pCmdUI);
  afx_msg void OnAnalysisClearHistory();
  afx_msg void OnViewSmoothResults();
  afx_msg void OnUpdateViewSmoothResults(CCmdUI* pCmdUI);
  afx_msg void OnStartGVT();
  afx_msg void OnUpdateQBAnalysis(CCmdUI* pCmdUI);
  afx_msg void OnStartQB();
  afx_msg void OnUpdateGVTAnalysis(CCmdUI* pCmdUI);
  afx_msg void OnAnalysisMixture();
  afx_msg void OnUpdateAnalysisMixture(CCmdUI* pCmdUI);
  afx_msg void OnAnalysisMixtureContainment();
  afx_msg void OnAnalysisExportMixture();
  afx_msg void OnUpdateAnalysisExportMixture(CCmdUI* pCmdUI);
  afx_msg void OnAnalysisExportMixtureContainment();
  afx_msg void OnConsistencyGuard();
  afx_msg void OnUpdateConsistencyGuard(CCmdUI* pCmdUI);
  afx_msg void OnUpdateDianaRunning(CCmdUI* pCmdUI);
  afx_msg void OnInvalidateResultsAll();
  afx_msg void OnUpdateInvalidateResultsAll(CCmdUI* pCmdUI);
  afx_msg void OnInvalidateResultsLinear();
  afx_msg void OnUpdateInvalidateResultsLinear(CCmdUI* pCmdUI);
  afx_msg void OnInvalidateResultsNonlinear();
  afx_msg void OnUpdateInvalidateResultsNonlinear(CCmdUI* pCmdUI);
  afx_msg void OnInvalidateResultsHeatFlow();
  afx_msg void OnUpdateInvalidateResultsHeatFlow(CCmdUI* pCmdUI);
  afx_msg void OnInvalidateResultsMixture();
  afx_msg void OnUpdateInvalidateResultsMixture(CCmdUI* pCmdUI);
  afx_msg void OnInvalidateResultsMixtureContainment();
  afx_msg void OnUpdateInvalidateResultsMixtureContainment(CCmdUI* pCmdUI);
  afx_msg void OnFaultInitializationSettings();
  afx_msg void OnUpdateFaultInitializationSettings(CCmdUI* pCmdUI);
  afx_msg void OnViewAutomaticallyDisableDrawing();
  afx_msg void OnUpdateViewAutomaticallyDisableDrawing(CCmdUI* pCmdUI);
  afx_msg void OnViewRefreshView();
  afx_msg void OnUpdateViewRefreshView(CCmdUI* pCmdUI);
  afx_msg void OnFileSaveModelWithAveragedResultsOnly();
  afx_msg void OnUpdateFileSaveModelWithAveragedResultsOnly(CCmdUI* pCmdUI);
  afx_msg void OnViewStickToView();
  afx_msg void OnUpdateViewStickToView(CCmdUI* pCmdUI);
  afx_msg void OnExportCommandFilesWithDefaults();
  afx_msg void OnUpdateExportCommandFilesWithDefaults(CCmdUI* pCmdUI);
  afx_msg void OnExecuteSVS();
  afx_msg void OnUpdateExecuteSVS(CCmdUI* pCmdUI);
  afx_msg void OnExportLineData();
  afx_msg void OnUpdateExportLineData(CCmdUI* pCmdUI);
  afx_msg void OnViewConvexHull();
  afx_msg void OnUpdateViewConvexHull(CCmdUI *pCmdUI);

public:
  //
  // doc can receive ON_COMMAND
  // but not ON_MESSAGE with params as it needs to derive from CWnd
  // this is the reason why event is received in MainFrame
  // and then forwarded to GeomecDoc
  //
afx_msg LRESULT on_event( WPARAM p1, LPARAM p2 );

  DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnImportSurface(); //waij TFS 92431
  afx_msg void OnDianaXCloseDialog();
  afx_msg void OnDianaXDiaMessage(LPCTSTR strMessage);
  afx_msg void OnDianaXDiaWarning(LPCTSTR strWarning);
  afx_msg void OnDianaXDiaError(LPCTSTR strError);
public:
  afx_msg void OnFileModelsummary();
  afx_msg void OnUpdateFileModelsummary(CCmdUI *pCmdUI);
  bool MultipleValueComponentsAllowed();
  afx_msg void OnRunModel();
  afx_msg void OnRunStep();
  afx_msg void OnWrongPythonroot();
};

// global function to get the active document
CGeomecDoc* GetGeomecDoc();

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GEOMECDOC_H__E321AF99_DA35_4988_AC51_3F4AAC08E45C__INCLUDED_)
