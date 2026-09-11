#include "stdafx.h"

#include "AnalysisLogDlg.h"
#include "AttriHexaHorizon.h"
#include "BoundaryBase.h"
#include "BranchState.h"
#include "ColorScaleEntry.h"
#include "CrossSection.h"
#include "CrossSection_Delegate.h"
#include "DataTreeView.h"
#include "EclipseModel.h"
#include "ExportSvsDlg.h"
#include "FormationPlane.h"
#include "FvGocadFile.h"
#include "GVTAnalysisDlg.h"
#include "GVTSettings.h"
#include "GVTStartDlg.h"
#include "Geomec.h"
#include "GeomecDianaRunner.h"
#include "GeomecDoc.h"
#include "GeomecUtils.h"
#include "GocadModel.h"
#include "HexaEntryTypes.h"
#include "HexaFormation.h"
#include "HexaSurface.h"
#include "ISceneWrapper.h"
#include "KeyFile.h"
#include "LinstaParamsDlg.h"
#include "MainFrm.h"
#include "MeshBase.h"
#include "ModelTreeView.h"
#include "ModelValidationDlg.h"
#include "NewModelDlg.h"
#include "NewWellPath.h" // wjrx mantis 3185
#include "NodalValueSet.h"
#include "NonlinParams.h"
#include "OIV2DSceneWrapper.h"
#include "OIV2DWellPathGraphScene.h"
#include "OpenGLSplitView.h"
#include "PointSet.h"
#include "PointSetCreateDlg.h"
#include "Pressure.h"
#include "ProgressDlg_MFC.h"
#include "PropertyExcel.h"
#include "PropertyFile.h"
#include "QuickBlocksDlg.h"
#include "Result.h"
#include "ResultRegister.h"
#include "RichhInputSummaryDlg.h"
#include "TNOFileDialog.h"
#include "TetraEntryTypes.h"
#include "TetraFormation.h"
#include "TetraMesh.h"
#include "TetraModel.h"
#include "TetraSurface.h"
#include "Utils.h"
#include "VersionInfo.h"
#include "WellFormation.h"
#include "WellModelEntryTypes.h"
#include "calculationproperties.h"
#include "exportgocaddlg.h"
#include "geoprogress.h"
#include "hexameshregion.h"
#include "meshtolerancedlg.h"
#include "resultexportdlg.h"

#if LICENSE_BORROWING
#include "LicenseBorrowDlg.h"
#include <afxdtctl.h>
#endif

#include "ArchiveQDataStream.h"
#include "ConsistencyGuard.h"
#include "ConsistencyGuardErrorPg.h"
#include "ConsistencyGuardSettingsPg.h"
#include "ConsistencyGuardValidationPg.h"
#include "DianaLicenseCheck.h"
#include "DianaStartUp.h"
#include "DianaXWrapper.h"
#include "EditFaultSettings.h"
#include "FaultPressure.h"
#include "FemAppGUI.h"
#include "FemAppMainWindow.h"
#include "GM3Model.h"
#include "GUIBase.h"
#include "GUIFactory.h"
#include "GVTVtFile.h"
#include "Global.h"
#include "GlobalInitialStressNode.h"
#include "GlobalPressure.h"
#include "GoCadModel_Delegate.h"
#include "GocadImport.h"
#include "HexaModel.h"
#include "IDCAnalysisStatus.h"
#include "IProgressBase.h"
#include "IProgressFactory.h"
#include "IWellModel.h"
#include "ModelLifetimeFacade.h"
#include "ModelView.h"
#include "NewWellPath_Delegate.h" // wjrx mantis 3185
#include "OIV2DHistoryScene.h"
#include "OIV2DHistorySceneWrapper.h"
#include "OIV2DWellPathGraphSceneWrapper.h"
#include "OpenInventorSceneWrapper.h"
#include "OptimizationSettings.h"
#include "RetrieveDianaFileNames.h"
#include "RetrieveDianaFileNamesGUI.h"
#include "SaveModel.h"
#include "SaveModelGUI.h"
#include "TimeInterval.h"
#include "VersionNumbers.h"
#include "geoSurface_Delegate.h"
// IT
#include "GeomecPythonInterface.h" //FIXME
#include "IT_Events_Handler.h"     //FIXME
#include "IT_Events_Handler_Task.h"
#include "PyInterface.h"
#include "RunGm.h"
// Plugins
#include "GmInterfaceGui.h"
#include "Item.h"
#include "Plugins.h"
#include "PyTask.h"
// FIXME: next 3x shoulnd't be here
#include "OIDIMeshNodeManager.h"
#include "OpenInventorEventsHandler.h"
#include "WellCasingModel.h"
//
#include "meshconverterdlg.h"
// qt
#include <QDir>
#include <QFileDialog>
#include <QTimer> //singleshot
#include <QtCore\QEvent>
#include <QtWidgets\QAbstractButton>
#include <QtWidgets\QHBoxLayout>
#include <QtWidgets\QLabel>
#include <QtWidgets\QMessageBox>
#include <QtWidgets\QPushButton>
#include <QtWidgets\QVBoxLayout>
//
// FIXME: this should be also encapsulated in RA or in EventHandler
//
#include "DSA_Context.h"
#include "Events.h" // FIXME: only to send 'DisableViews_Done' at disable_views
#include "RunAnalysis.h"
// mesh refinement
#include "MeshRefinementDlg.h"
#include "RBox.h"
// global stuff
#include "Global.h"
#include "GlobalMessage.h"
#include "ISettings.h"
#include "Printer.h"
// utils
#include "GeomecHelper.h"
#include "util_.h" //GetAppPath
//
#include "EmptySceneWrapper.h" // for debug purpose
// RA (RunAnalysis)
#include "GeomecDoc_EventsHandler.h"
// std
#include <direct.h>
#include <io.h>

#include "MesherDlg.h"

#include "ChangelogDlg.h"

namespace {
static const std::string PLUGIN_NAME_IT = "Inversion tool";
}

#ifdef _DEBUG
////#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeomecDoc

IMPLEMENT_DYNCREATE(CGeomecDoc, CDocument)

BEGIN_MESSAGE_MAP(CGeomecDoc, CDocument)
//{{AFX_MSG_MAP(CGeomecDoc)
ON_COMMAND(ID_FILE_MODEL_PROPERTIES, OnModelProperties)
ON_UPDATE_COMMAND_UI(ID_FILE_MODEL_PROPERTIES, OnUpdateModelProperties)
ON_COMMAND(ID_FILE_SAVE, OnFileSave)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
ON_COMMAND(ID_ANALYSIS_LINEAR_DEPLETION, OnAnalysisLinearDepletion)
ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
ON_COMMAND(ID_ANALYSIS_NONLINEAR_CALCULATION, OnAnalysisNonlinearCalculation)
ON_UPDATE_COMMAND_UI(ID_ANALYSIS_NONLINEAR_CALCULATION, OnUpdateAnalysisNonlinearCalculation)
ON_COMMAND(ID_ANALYSIS_NONLINEAR_CALC_PARAMS, OnAnalysisNonlinearCalcParams)
ON_UPDATE_COMMAND_UI(ID_ANALYSIS_NONLINEAR_CALC_PARAMS, OnUpdateAnalysisNonlinearCalcParams)
ON_UPDATE_COMMAND_UI(ID_ANALYSIS_PROPERTIES, OnUpdateAnalysisProperties)
ON_COMMAND(ID_SET_MESH_TOLERANCE, OnSetMeshTolerance)
ON_UPDATE_COMMAND_UI(ID_SET_MESH_TOLERANCE, OnUpdateSetMeshTolerance)
ON_COMMAND(ID_CREATE_MESH, OnCreateMesh)
ON_UPDATE_COMMAND_UI(ID_CREATE_MESH, OnUpdateCreateMesh)
ON_COMMAND(ID_INVALIDATE_MESH, OnInvalidateMesh)
ON_UPDATE_COMMAND_UI(ID_INVALIDATE_MESH, OnUpdateInvalidateMesh)
ON_COMMAND(ID_INSERT_DEVIATED_WELLPATH_BY_POINTSET, OnInsertDeviatedWellPathByPointSet) // wjrx mantis 3185
ON_COMMAND(ID_ANALYSIS_PROPERTIES, OnAnalysisProperties)
// 	ON_COMMAND(ID_FORMATIONFULL, OnFormationfull)
// 	ON_COMMAND(ID_FORMATIONSKIN, OnFormationskin)
// 	ON_COMMAND(ID_FORMATIONCENTER, OnFormationcenter)
// 	ON_UPDATE_COMMAND_UI(ID_FORMATIONFULL, OnUpdateFormationfull)
// 	ON_UPDATE_COMMAND_UI(ID_FORMATIONSKIN, OnUpdateFormationskin)
// 	ON_UPDATE_COMMAND_UI(ID_FORMATIONCENTER, OnUpdateFormationcenter)
ON_UPDATE_COMMAND_UI(ID_VIEW_LOCAL, OnUpdateColorScaleSelection)
ON_UPDATE_COMMAND_UI(ID_VIEW_GLOBAL, OnUpdateColorScaleSelection)
ON_UPDATE_COMMAND_UI(ID_VIEW_HOTSPOT, OnUpdateColorScaleSelection)
ON_UPDATE_COMMAND_UI(ID_VIEW_COLORSCALE, OnUpdateColorScaleSelection)
//}}AFX_MSG_MAP
ON_UPDATE_COMMAND_UI(ID_ANALYSIS_LINEAR_DEPLETION, OnUpdateAnalysisLinearDepletion)
ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
ON_UPDATE_COMMAND_UI(ID_ANALYSISLOG, OnUpdateAnalysisLog)
ON_COMMAND(ID_ANALYSISLOG, OnAnalysisLog)
ON_COMMAND(ID_INPUT_STATUS, OnInputStatus)
ON_UPDATE_COMMAND_UI(ID_INPUT_STATUS, OnUpdateInputStatus)
ON_UPDATE_COMMAND_UI(ID_EXPORT_LINEAR, OnUpdateAnalysisExportLinear)
ON_COMMAND(ID_EXPORT_LINEAR, OnAnalysisExportLinear)
ON_UPDATE_COMMAND_UI(ID_EXPORT_NONLIN, OnUpdateAnalysisExportNonlin)
ON_COMMAND(ID_EXPORT_NONLIN, OnAnalysisExportNonlin)
ON_COMMAND(ID_EXPORT_QUAD_DAT, OnAnalysisExportQuadDat)
ON_UPDATE_COMMAND_UI(ID_EXPORT_QUAD_DAT, OnUpdateAnalysisExportQuadDat)

// wjrx mantis 2975
ON_UPDATE_COMMAND_UI(ID_EXPORT_RESULTS, OnUpdateAnalysisExportResults)
ON_COMMAND(ID_EXPORT_RESULTS, OnAnalysisExportResults)
ON_UPDATE_COMMAND_UI(ID_EXPORT_RESULTS_GOCAD, OnUpdateAnalysisExportResultsGoCad)
ON_COMMAND(ID_EXPORT_RESULTS_GOCAD, OnAnalysisExportResultsGoCad)
ON_UPDATE_COMMAND_UI(ID_EXPORT_SURFACES, OnUpdateAnalysisExportSurfaces)
ON_COMMAND(ID_EXPORT_SURFACES, OnAnalysisExportResultsSurfaces)

// wjrx mantis 2976
ON_UPDATE_COMMAND_UI(ID_IMPORT_IMPORTMESH, OnUpdateImportMesh)
ON_COMMAND(ID_IMPORT_IMPORTMESH, OnImportMesh)
ON_UPDATE_COMMAND_UI(ID_IMPORT_IMPORTRESULTS, OnUpdateImportResults)
ON_COMMAND(ID_IMPORT_IMPORTRESULTS, OnImportResults)
// ID_FILE_IMPORT still used by data storage context menus.
ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT, OnUpdateImport)
ON_COMMAND(ID_FILE_IMPORT, OnImport)

// import surfaces and point/elementsets does the same!
ON_UPDATE_COMMAND_UI(ID_IMPORT_IMPORTPOINTELEMENTSETS, OnUpdateImport)
ON_COMMAND(ID_IMPORT_IMPORTPOINTELEMENTSETS, OnImport)
ON_UPDATE_COMMAND_UI(ID_IMPORT_INSERTDEVIATEDWELLPATH, OnUpdateImportInsertDeviatedWellPath)
ON_COMMAND(ID_IMPORT_INSERTDEVIATEDWELLPATH, OnImportInsertDeviatedWellPath)

ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
ON_COMMAND(ID_ANALYSIS_LINEAR_SETTINGS, OnAnalysisLinearSettings)
ON_UPDATE_COMMAND_UI(ID_ANALYSIS_LINEAR_SETTINGS, OnUpdateAnalysisLinearSettings)
ON_UPDATE_COMMAND_UI(ID_ANALYSIS_CLEARHISTORY, OnUpdateAnalysisClearHistory)
ON_COMMAND(ID_ANALYSIS_CLEARHISTORY, OnAnalysisClearHistory)
ON_COMMAND(ID_VIEW_SMOOTH_RESULTS, OnViewSmoothResults)
ON_UPDATE_COMMAND_UI(ID_VIEW_SMOOTH_RESULTS, OnUpdateViewSmoothResults)
ON_COMMAND(ID_ANALYSIS_HEAT_FLOW, OnAnalysisHeatFlow)
ON_UPDATE_COMMAND_UI(ID_ANALYSIS_HEAT_FLOW, OnUpdateAnalysisHeatFlow)
ON_COMMAND(ID_EXPORT_HEAT_FLOW, OnAnalysisExportHeatFlow)
ON_UPDATE_COMMAND_UI(ID_EXPORT_HEAT_FLOW, OnUpdateAnalysisExportHeatFlow)
ON_COMMAND(ID_ANALYSIS_QUICKBLOCKS, OnStartQB)
ON_UPDATE_COMMAND_UI(ID_ANALYSIS_QUICKBLOCKS, OnUpdateQBAnalysis)
ON_COMMAND(ID_START_GVT, OnStartGVT)
ON_UPDATE_COMMAND_UI(ID_START_GVT, OnUpdateGVTAnalysis)
ON_COMMAND(ID_ANALYSIS_MIXTURE, OnAnalysisMixture)
ON_UPDATE_COMMAND_UI(ID_ANALYSIS_MIXTURE, OnUpdateAnalysisMixture)
ON_COMMAND(ID_ANALYSIS_MIXTURE_CONTAINMENT, OnAnalysisMixtureContainment)
ON_UPDATE_COMMAND_UI(ID_ANALYSIS_MIXTURE_CONTAINMENT, OnUpdateAnalysisMixture)
ON_COMMAND(ID_CONSISTENCY_GUARD_BUTTON, OnConsistencyGuard)
ON_UPDATE_COMMAND_UI(ID_DIANA_RUNNING, OnUpdateDianaRunning)
ON_UPDATE_COMMAND_UI(ID_CONSISTENCY_GUARD_BUTTON, OnUpdateConsistencyGuard)
ON_COMMAND(ID_EXPORT_MIXTURE, OnAnalysisExportMixture)
ON_UPDATE_COMMAND_UI(ID_EXPORT_MIXTURE, OnUpdateAnalysisExportMixture)
ON_COMMAND(ID_EXPORT_MIXTURE_CONTAINMENT, OnAnalysisExportMixtureContainment)
ON_UPDATE_COMMAND_UI(ID_EXPORT_MIXTURE_CONTAINMENT, OnUpdateAnalysisExportMixture)
ON_COMMAND(ID_SETTINGS_GVTSETTINGS, OnGVTSettings)
ON_UPDATE_COMMAND_UI(ID_SETTINGS_GVTSETTINGS, OnUpdateGVTSettings)
ON_COMMAND(ID_SETTINGS_BORROWLICENSE, OnBorrowLicense)
ON_UPDATE_COMMAND_UI(ID_SETTINGS_BORROWLICENSE, OnUpdateBorrowLicense)
ON_COMMAND(ID_EXPORT_SVS, OnExportSVS)
ON_UPDATE_COMMAND_UI(ID_EXPORT_SVS, OnUpdateExportSVS)
ON_COMMAND(ID_INVALIDATERESULTS_ALL, OnInvalidateResultsAll)
ON_UPDATE_COMMAND_UI(ID_INVALIDATERESULTS_ALL, OnUpdateInvalidateResultsAll)
ON_COMMAND(ID_INVALIDATERESULTS_LINEAR, OnInvalidateResultsLinear)
ON_UPDATE_COMMAND_UI(ID_INVALIDATERESULTS_LINEAR, OnUpdateInvalidateResultsLinear)
ON_COMMAND(ID_INVALIDATERESULTS_NONLINEAR, OnInvalidateResultsNonlinear)
ON_UPDATE_COMMAND_UI(ID_INVALIDATERESULTS_NONLINEAR, OnUpdateInvalidateResultsNonlinear)
ON_COMMAND(ID_INVALIDATERESULTS_HEATFLOW, OnInvalidateResultsHeatFlow)
ON_UPDATE_COMMAND_UI(ID_INVALIDATERESULTS_HEATFLOW, OnUpdateInvalidateResultsHeatFlow)
ON_COMMAND(ID_INVALIDATERESULTS_MIXTURE, OnInvalidateResultsMixture)
ON_UPDATE_COMMAND_UI(ID_INVALIDATERESULTS_MIXTURE_CONTAINMENT, OnUpdateInvalidateResultsMixtureContainment)
ON_COMMAND(ID_INVALIDATERESULTS_MIXTURE_CONTAINMENT, OnInvalidateResultsMixtureContainment)
ON_UPDATE_COMMAND_UI(ID_INVALIDATERESULTS_MIXTURE, OnUpdateInvalidateResultsMixture)
ON_COMMAND(ID_SETTINGS_FAULTINITIALIZATIONSETTINGS, OnFaultInitializationSettings)
ON_UPDATE_COMMAND_UI(ID_SETTINGS_FAULTINITIALIZATIONSETTINGS, OnUpdateFaultInitializationSettings)
ON_COMMAND(ID_SETTINGS_OPTIMIZATIONSETTINGS, OnOptimizationSettings)
ON_UPDATE_COMMAND_UI(ID_SETTINGS_OPTIMIZATIONSETTINGS, OnUpdateOptimizationSettings)
ON_COMMAND(ID_NOVALUEDRAW, OnViewAutomaticallyDisableDrawing)
ON_UPDATE_COMMAND_UI(ID_NOVALUEDRAW, OnUpdateViewAutomaticallyDisableDrawing)
ON_COMMAND(ID_DRAWVALUE, OnViewRefreshView)
ON_UPDATE_COMMAND_UI(ID_DRAWVALUE, OnUpdateViewRefreshView)
ON_COMMAND(ID_FILE_MODELSUMMARY, &CGeomecDoc::OnFileModelsummary)
ON_UPDATE_COMMAND_UI(ID_FILE_MODELSUMMARY, &CGeomecDoc::OnUpdateFileModelsummary)
ON_COMMAND(ID_FILE_SAVEMODELWITHAVERAGEDRESULTSONLY, OnFileSaveModelWithAveragedResultsOnly)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVEMODELWITHAVERAGEDRESULTSONLY, OnUpdateFileSaveModelWithAveragedResultsOnly)
ON_COMMAND(ID_VIEW_STICKTOVIEW, OnViewStickToView)
ON_UPDATE_COMMAND_UI(ID_VIEW_STICKTOVIEW, OnUpdateViewStickToView)
ON_COMMAND(ID_EXPORTCOMMANDFILESWITHDEFAULTS, OnExportCommandFilesWithDefaults)
ON_UPDATE_COMMAND_UI(ID_EXPORTCOMMANDFILESWITHDEFAULTS, OnUpdateExportCommandFilesWithDefaults)
ON_COMMAND(ID_EXECUTE_SVS, OnExecuteSVS)
ON_UPDATE_COMMAND_UI(ID_EXECUTE_SVS, OnUpdateExecuteSVS)
ON_COMMAND(ID_EXPORT_LINE_DATA, OnExportLineData)
ON_UPDATE_COMMAND_UI(ID_EXPORT_LINE_DATA, OnUpdateExportLineData)
ON_COMMAND(ID_VIEW_CONVEX_HULL, OnViewConvexHull)
ON_UPDATE_COMMAND_UI(ID_VIEW_CONVEX_HULL, OnUpdateViewConvexHull)
ON_COMMAND(ID_PLUGINS_ADD, &CGeomecDoc::OnPlugins_Add)
ON_COMMAND(WM_USER + 1000, &CGeomecDoc::OnSetDefault)

ON_COMMAND_RANGE(WM_USER + CItem::Launch, WM_USER + CItem::Manual - 1, &CGeomecDoc::OnPlugins_Launch)
ON_COMMAND_RANGE(WM_USER + CItem::Manual, WM_USER + CItem::Remove - 1, &CGeomecDoc::OnPlugins_Manual)
ON_COMMAND_RANGE(WM_USER + CItem::Remove, WM_USER + CItem::Max - 1, &CGeomecDoc::OnPlugins_Remove)

ON_UPDATE_COMMAND_UI_RANGE(WM_USER + CItem::Remove, WM_USER + CItem::Max - 1, &CGeomecDoc::OnPlugins_Remove_Update)
ON_UPDATE_COMMAND_UI_RANGE(WM_USER + CItem::Manual, WM_USER + CItem::Remove - 1, &CGeomecDoc::OnPlugins_Manual_Update)

ON_COMMAND(ID_ON_RUN_MODEL, &OnRunModel)
ON_COMMAND(ID_ON_RUN_STEP, &OnRunStep)
ON_COMMAND(ID_ON_WRONG_PYTHONROOT, &OnWrongPythonroot)

//
// even worse
//
// ON_NOTIFY(ID_ON_EVENT, &on_event)

//
// this is not Ok
//
// ON_COMMAND(ID_ON_EVENT, &on_event)

// TODO: needs to be defined in MainFrame (CWnd*), not in the doc
// ON_MESSAGE(ID_ON_EVENT, &on_event)

END_MESSAGE_MAP()

namespace {
Printer *printer = Printer::instance(Printer::Doc);
}

namespace {
/*
//_m()->msg("The heat flow analysis will invalidate the results from other analysis types. Are you sure you want to
continue?", MB_YESNO) == IDNO) void globalMessageW_special( char const * msg, int code, char const * caller_name, int
line )
{
  printer->info( "'_m()->msg' was called from '%s' (L%d)", caller_name, line );
  _m()->msg( msg, code );
}

void globalMessageW_special( char const * msg, char const * caller_name, int line )
{
  printer->info( "'_m()->msg' was called from '%s' (L%d)", caller_name, line );
  _m()->msg( msg );
}

void globalMessageW_special( const QString msg, char const * caller_name, int line )
{
  printer->info( "'_m()->msg' was called from '%s' L%d", caller_name, line );
  _m()->msg( msg );
}

#define _m()->msg( msg ) globalMessageW_special( msg, __func__, __LINE__ )
//#define _m()->msg( msg, code ) globalMessageW_special( msg, code, __func__ )
*/

static const QString TEXT_OLD_MESHER_DLG_TITLE = QString("Models using old FGV meshers found");

static const QString TEXT_OLD_MESHER_DLG_TEXT =
    QString("Upgrading to the new CM2 mesher can be performed now.<br><br> \
                                If needed, the model can be also converted from hexa to tetra.<br><br> \
                                This can be handled now or <font color='red'>later on</font> \
                                <p style='text-align:center'><b>Handle this now?</b><a href='http://google.com/'>(Help)</a></p>");

static const QString TEXT_OLD_MESHER_DLG_BTN_YES = QString("Handle now");
static const QString TEXT_OLD_MESHER_DLG_BTN_NO = QString("Skip");

void show_my_tree_view_simple() {
  QMessageBox::StandardButton res = QMessageBox::question(nullptr, TEXT_OLD_MESHER_DLG_TITLE, TEXT_OLD_MESHER_DLG_TEXT);
  if (res == QMessageBox::Yes)
    MeshConverterDlg::instance()->show();
}

/*void show_my_tree_view_dlg()
{
  MyDlg dlg;
  dlg.exec();
}

// MCR: Don't use!
void show_my_tree_view()
{
  show_my_tree_view_dlg();
  return;

  QMessageBox box;

  box.setIcon( QMessageBox::Question );

  box.setWindowTitle( TEXT_OLD_MESHER_DLG_TITLE );
  box.setText( TEXT_OLD_MESHER_DLG_TEXT );

  box.setStandardButtons( QMessageBox::Yes | QMessageBox::No | QMessageBox::Help );

  QAbstractButton* btn_y = box.button( QMessageBox::Yes );
  btn_y->setText( TEXT_OLD_MESHER_DLG_BTN_YES );

  QAbstractButton* btn_n = box.button( QMessageBox::No );
  btn_n->setText( TEXT_OLD_MESHER_DLG_BTN_NO );

  QAbstractButton* btn_h = box.button( QMessageBox::Help );
  //btn_h->blockSignals( true );
  //btn_h->installEventFilter( MeshConverterDlg::instance() ); // doesn't work

  //btn_h->disconnect();
  //connect( btn_h, &QAbstractButton::clicked, MeshConverterTreeView::instance(), [=](){
AfxGetApp()->GetMainWnd()->PostMessage( WM_COMMAND, ID_HELP_FINDER, 0 ); } );

  QMessageBox::StandardButton res = (QMessageBox::StandardButton)box.exec();

  if( res == QMessageBox::Yes )
    MeshConverterDlg::instance()->show();
  else if( res == QMessageBox::Help )
  {
    AfxGetApp()->GetMainWnd()->PostMessage( WM_COMMAND, ID_HELP_FINDER, 0 );
    show_my_tree_view(); // MCR: ?????
  }
}*/
} // namespace

namespace gm {
namespace doc_ {

namespace gui {
//
// comments:
//
// QMessageBox::NoButton : doesn't hide all btns
//
// dlg->setStandardButtons(0); // this avoids dlg to be closed so don't do it
void popup(const QString &title, const QString &msg,
           int timeout) // 2s.
{
  QMessageBox *dlg = new QMessageBox(QMessageBox::Information, title, msg, QMessageBox::NoButton);
  dlg->setAttribute(Qt::WA_DeleteOnClose, true);
  dlg->show();
  if (timeout)
    QTimer::singleShot(timeout, dlg, SLOT(close()));
};
} // namespace gui

//
// m_pszExeName:
//	CGeomecApp constructor: still not availble, so hardcoded to 'Geomec'
//	CGeomecApp::InitInstance:  available
//
CString executable() {
  CString executable = GetGeomecApp()->m_pszExeName;
  if (executable.IsEmpty())
    executable = "Geomec";
  executable += ".exe";
  return executable;
}

//
// path_: binary name: 'Geomec.exe' for example
//
QString version(const QString &path_) {
  CString executable(path_.toStdString().c_str());

  LPTSTR path = new TCHAR[executable.GetLength() + 1];
  _tcscpy(path, executable);

  CVersionInfo version;
  version.GetVersionInfo(path);
  QStringList v_l = QString((LPCTSTR)version.m_strFullVersion).split(" ");
  QString v_ = v_l.first() + "_build_" + v_l.last();
  delete path;
  return v_;
}

//
// not app version but the name of the folder containing Geomec.exe
// used in multiple instances Geomec version
//
// Ex. C:\Program Files\Geomec - SVS\5.7.0.11_build_20041.1
//
// -> 5.7.0.11_build_20041.1
//
QString version_FOLDER() {
  QString p = QString::fromStdString(util::GetAppPath());
  p.replace("\\", "/");
  QString folder_name = p.right(p.length() - p.lastIndexOf("/") - 1);
  return folder_name;
}

//
// app version
//
// -> 5.7.0.11_build_20041.1
//
QString version_APP() { return version(QString::fromStdString(util::GetAppFileName())); }

QString version_APP_OLD() { return version((LPCTSTR)executable()); }

QString version(eVersion type_) { return type_ == Folder ? version_FOLDER() : version_APP(); }

} // namespace doc_
} // namespace gm

/////////////////////////////////////////////////////////////////////////////
// CGeomecDoc construction/destruction

// ##ModelId=3BC55D630092
CGeomecDoc::CGeomecDoc()
    : IListener(RunDianaGui_ListenerDefinition)
      //
      ,
      m_bStartUp(true), m_app_version(), m_versionManager(m_app_version)
      //
      ,
      m_geomecDocDianaSignals(CDianaStartUp::instance(), this)
      //
      ,
      m_pModelTreeView(0), m_pDataTreeView(0), m_pModelView(0), m_pLegendModelView(0), m_bChangeMode(false),
      m_nLastCGStatus(-1), m_bRunning(false), m_dsa_icon_id(DSA_Icon::Empty), m_changelog_dlg(nullptr) {
  m_eh = new CGeomecDoc_EventsHandler;

  //
  // for functional test purpose
  //
  auto save_msg_map = [=]() {
    // struct Accessor : public CWnd
    struct Accessor : public CDocument {
      // overwrite protected
      // virtual const AFX_MSGMAP* GetMessageMap() const { return CDocument::GetMessageMap(); }
      virtual const AFX_MSGMAP *GetMessageMap() const { return GetMessageMap(); }
    };

    Accessor *msg_map_access = (Accessor *)this;
    const AFX_MSGMAP *msg_map = msg_map_access->GetMessageMap();

    AFX_MSGMAP_ENTRY *entry = (AFX_MSGMAP_ENTRY *)msg_map->lpEntries;
    while (entry->nSig != AfxSig_end) {
      m_func_map[entry->nID] = entry->pfn;
      entry++;
    }
  };
  save_msg_map();
}

// ##ModelId=3BC55D6300B1
CGeomecDoc::~CGeomecDoc() {
  delete m_eh;

  if (m_changelog_dlg)
    delete m_changelog_dlg;
}

bool CGeomecDoc::test() { return m_eh->test(); };

/////////////////////////////////////////////////////////////////////////////
// CGeomecDoc diagnostics

#ifdef _DEBUG
// ##ModelId=3BC55D6300B3
void CGeomecDoc::AssertValid() const { CDocument::AssertValid(); }

// ##ModelId=3BC55D6300B5
void CGeomecDoc::Dump(CDumpContext &dc) const { CDocument::Dump(dc); }
#endif //_DEBUG

bool CGeomecDoc::ExperimentalUnlocked() const {
  CGeomecApp *pApp = (CGeomecApp *)AfxGetApp();
  return pApp->KeyFile().Unlocked();
}

bool CGeomecDoc::Model2DUnlocked() const {
  return false;
  CGeomecApp *pApp = (CGeomecApp *)AfxGetApp();
  return pApp->KeyFile().Model2DUnlocked();
}

/////////////////////////////////////////////////////////////////////////////
// CGeomecDoc construction/destruction

// ##ModelId=3BC55D630097
BOOL CGeomecDoc::OnNewDocument() {
  MeshConverterDlg::instance()->hide();

  if (m_bStartUp) {
    // Don't show the new model dlg at startup
    m_bStartUp = false;

    if (!CFemAppDoc::OnNewDocument())
      return FALSE; // Initialisation failed
    assert(Model() == 0);
    return TRUE; // Initialisation of no model succeed
  }

  BOOL bRet = FALSE;
  // Create new models and initialize the new model dlg box
  std::vector<CNewModelDlg::ModelInfo> vcModel;

  CNewModelDlg::ModelInfo modelInfo;

  modelInfo.nType = HEXA_MODEL;
  modelInfo.sType = CHexaModel::staticDocumentType();
  modelInfo.nIcon = CHexaModel::staticModelIconId();
  vcModel.push_back(modelInfo);

#ifdef TETRA_KEYFILE
  if (ExperimentalUnlocked()) {
#endif
    modelInfo.nType = GOCAD_MODEL;
    modelInfo.sType = CTetraModel::staticDocumentType();
    modelInfo.nIcon = CTetraModel::staticModelIconId();
    vcModel.push_back(modelInfo);

#ifdef TETRA_KEYFILE
  }
#endif

#ifdef ECLIPSE_KEYFILE
  if (ExperimentalUnlocked()) {
#endif
    modelInfo.nType = ECLIPSE_MODEL;
    modelInfo.sType = CEclipseModel::staticDocumentType();
    modelInfo.nIcon = CEclipseModel::staticModelIconId();
    vcModel.push_back(modelInfo);
#ifdef ECLIPSE_KEYFILE
  }
#endif

  CNewModelDlg dlg(vcModel);

  if (dlg.DoModal() == IDOK) {
    // User has made a selection

    int nSelectedModel = dlg.SelectedModel();

    assert(nSelectedModel != -1);

#if 0
  // Nothing here, but it looks like this was supposed to do something, so I leave it behind the #if

    // In case of eclipse model start eclipse wizard
    CEclipseModel *pModel = dynamic_cast<CEclipseModel*> (dlg.SelectedModel());
    if(pModel)
    {
    
    }
#endif

    // Call the frame work
    bRet = CDocument::OnNewDocument();

    // If the frame work agrees attach the document
    if (bRet) {
      DeleteContents();

      CModelBase *pModel = 0;
      switch (nSelectedModel) {
      case HEXA_MODEL:
      case GOCAD_MODEL:
      case ECLIPSE_MODEL:
        pModel = IModelLifetimeFacade::NewMain(nSelectedModel, m_Logger, m_versionManager);
      }

      assert(pModel);

      CGUIBase *guiBase = CGUIFactory::createGUI(0, pModel);

      if (!guiBase->NewModel())
        return FALSE;
      guiBase->LinkModelToOpenInventor();
    }
  }

  return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// CGeomecDoc commands

BOOL CGeomecDoc::OnImportFile(const CString &strFileName, IProgressBase &dlg, CGraphNode::TNodeVec &created_objects) {

  // Try .txt, .dat and .xyz extension
  if ((strFileName.Right(4).CompareNoCase(".txt") == 0) || (strFileName.Right(4).CompareNoCase(".dat") == 0) ||
      (strFileName.Right(4).CompareNoCase(".xyz") == 0)) {
    enum FILE_STATUS status = OnImportASCII(strFileName, dlg, created_objects);
    switch (status) {
    case FORMAT_OK:
      return TRUE;
    case FORMAT_ERROR:
    case CANCEL: {
      for (size_t i = 0; i < created_objects.size(); i++)
        delete created_objects[i];
      created_objects.clear();
      return FALSE;
    }
    case FORMAT_UNKNOWN: {
      CString msg;
      msg.Format(IDP_FAILUREPARSING, strFileName, "ASCII file format");
      _m()->msg(msg);
      return FALSE;
    }
    default:
      assert(FALSE);
    }
  }

  if (strFileName.Right(3).CompareNoCase(".so") == 0 || strFileName.Right(3).CompareNoCase(".ts") == 0 ||
      strFileName.Right(3).CompareNoCase(".mx") == 0 ||
      strFileName.Right(3).CompareNoCase(".vs") == 0) // wjrx mantis 2896
  {
    CGocadImport gi(false);
    if (!gi.Import((LPCSTR)strFileName)) {
      if (!gi.Error().empty()) // could have been cancelled
        _m()->msg(gi.Error().c_str());
      return FALSE;
    }

    int NrOfJobs = 0;
    if (gi.SolidSize() > 0)
      ++NrOfJobs;
    if (gi.VsetSize() > 0)
      ++NrOfJobs;
    if (gi.SurfSize() > 0)
      ++NrOfJobs;

    if (NrOfJobs > 0) {
      std::auto_ptr<IProgressBase> progdlg(
          (_g->prog()->create(eProgress::Dual, "Processing Gocad import", false, NrOfJobs)));

      CModelBase &model = ((CModelBase &)*Model()).RootModel();

      if (gi.SolidSize() > 0) {
        progdlg->NextJob("Creating Element Sets");
        progdlg->AddSteps(gi.SolidSize());

        for (int i = 0; i < gi.SolidSize(); ++i) {
          gi.CreateElementSet(model, i);
          progdlg->Step();
        }
      }

      if (gi.VsetSize() > 0) {
        progdlg->NextJob("Creating Point Sets");
        progdlg->AddSteps(gi.VsetSize());

        // wjrx mantis 2896
        for (int i = 0; i < gi.VsetSize(); ++i) {
          gi.CreatePointSet(model, i);
          progdlg->Step();
        }
      }

      if (gi.SurfSize() > 0) {
        progdlg->NextJob("Creating Surfacess");
        progdlg->AddSteps(gi.SurfSize());

        for (int i = 0; i < gi.SurfSize(); ++i) {
          gi.CreateSurface(model, i);
          progdlg->Step();
        }
      }
    }

    return TRUE;
  }

  // Try .xls extension
  if (strFileName.Right(4).CompareNoCase(".xls") == 0 || strFileName.Right(5).Left(4).CompareNoCase(".xls") == 0) {
    enum FILE_STATUS status = OnImportExcel(strFileName, dlg, created_objects);
    switch (status) {
    case FORMAT_OK:
      return TRUE;
    case FORMAT_ERROR:
    case CANCEL: {
      for (size_t i = 0; i < created_objects.size(); i++)
        delete created_objects[i];
      created_objects.clear();
    }
      return FALSE;
    case FORMAT_UNKNOWN: {
      CString msg;
      msg.Format(IDP_FAILUREPARSING, strFileName, "Excel format");
      _m()->msg(msg);
      return FALSE;
    }
    default:
      assert(FALSE);
    }
  }

  if (strFileName.Right(3).CompareNoCase(".vt") == 0) {
    if (!CGVTVtFile::IsBinaryVtFile((const char *)strFileName)) {
      CString msg;
      msg.Format(IDP_FAILUREPARSING, strFileName, "Vt binary format");
      _m()->msg(msg);
      return FALSE;
    }

    CGVTVtFile VtFile(*static_cast<CModelBase *>(Model()));
    if (!VtFile.Import((const char *)strFileName, dlg)) {
      CString msg;
      msg.Format(IDP_FAILUREPARSING, strFileName, "Vt binary format");
      _m()->msg(msg);
      return FALSE;
    }

    return TRUE;
  }

  CString msg;
  msg.Format(IDP_UNKNOWNFILEEXTENSIONIMPORT, strFileName);
  _m()->msg(msg);

  return FALSE;
}

// ##ModelId=3BC55D6300A2
BOOL CGeomecDoc::OnOpenDocument(LPCTSTR lpszPathName) {
  if (!FileExists(lpszPathName)) {
    CString msg;
    msg.Format("File not found: '%s'", lpszPathName);
    _m()->msg(msg);
    return FALSE;
  }
  MeshConverterDlg::instance()->hide();

  if (CString(lpszPathName).Right(4).CompareNoCase(".gm3") == 0) {
#ifdef _WIN64
    _m()->msg(
        "The 64 bits version of GEOMEC cannot import .gm3 files anymore. Use the 32 bits version to open this file.");
    return FALSE;
#else
    CProgressDlg_MFC dlg(2, "Importing GM3 model", true, FemAppGetMainWnd());
    dlg.StartDialog();

    // Try to open a 3D model
    gm::C3DModel mod;
    std::auto_ptr<IProgressBase> prog;
    try {
      prog.reset(_g->prog()->create(eProgress::Geo, dlg, "Reading GM3 model..."));
      mod.LoadModel(lpszPathName, false, *prog);
    } catch (CDaoException *e) {
      _m()->msg(e->m_pErrorInfo->m_strDescription, MB_ICONEXCLAMATION);
      e->Delete();
      dlg.DestroyWindow();
      return CANCEL;
    } catch (CProgressCancel *e) {
      delete e;
      return false;
    }

    // Delete the contents of the document
    DeleteContents();

    // Initialise the model ...
    try {
      CGm3HexaModel *pModel = new CGm3HexaModel(m_Logger, m_versionManager);

      CGUIBase *guiBase = CGUIFactory::createGUI(0, pModel);

      guiBase->NewModel();
      assert(GetGeomecDoc()->Model());
      prog.reset(_g->prog()->create(eProgress::Geo, dlg, "Converting model..."));
      pModel->Build(lpszPathName, mod, *prog);

      // Reset modified flag
      SetModifiedFlag(FALSE);
    } catch (CProgressCancel *e) {
      delete e;
      DeleteContents();
      return FALSE;
    }

    // End dialog and set path
    dlg.DestroyWindow(); //(IDOK);

    return TRUE;
#endif
  }

  if (CString(lpszPathName).Right(4).CompareNoCase(".gm2") == 0) {
    _m()->msg("2D models are no longer supported");
    return FALSE;
  }

  if ((CString(lpszPathName).Right(5).CompareNoCase(".grid") == 0) ||
      (CString(lpszPathName).Right(4).CompareNoCase(".ecl") == 0)) {
    CProgressDlg_MFC dlg(1, "Importing eclipse model", true, FemAppGetMainWnd());
    dlg.StartDialog();
    BOOL bRet = (OnImportEclipse(lpszPathName, dlg) == FORMAT_OK);
    dlg.DestroyWindow();
    return bRet;
  }

  if (CString(lpszPathName).Right(4).CompareNoCase(".gmp") == 0 ||
      CString(lpszPathName).Right(4).CompareNoCase(".gm4") == 0 ||
      CString(lpszPathName).Right(4).CompareNoCase(".gm5") == 0) {
    CGUIBase::CMainModelLoadSave modelLoadSave;

    CModelBase *pModel = IModelLifetimeFacade::LoadMain(modelLoadSave, lpszPathName, m_Logger);

    if (pModel) {
      m_app_version = modelLoadSave.GetAppVersion();

      OnAttachModel(*pModel, *modelLoadSave.GetGUI());
      // load succeeded, set document's name if it was a .gm4/5 file, so Save will not behave as Save As
      CString newName = lpszPathName;
      if (newName.Right(4).CompareNoCase(".gm4") == 0 || newName.Right(4).CompareNoCase(".gm5") == 0) {
        SetPathName(newName);
      } else // import, use title only
      {
        QString sPath = lpszPathName;
        QString sFile;
        QString sDir;
        SplitPathAndFileName(sPath, sDir, sFile);
        SetTitle(RemoveExtension(sFile).toStdString().c_str());
        m_strPathName = "";
      }

#if 0
// disable the popup, as it's irritating; TODO: put in changelog dialog that this functionality exists
    CModelBase* model = dynamic_cast< CModelBase* >( GetGeomecDoc()->Model() );
    if( model && MeshConverterTreeView::any_old_mesher( model ) && model->m_show_cm2_dlg )
      QTimer::singleShot( 2*1000, show_my_tree_view );
#endif

      return TRUE;
    } else {
      // If we don't reset the modified flag and the onopenfile was called via the most recent used menuitem
      // the framework decides that the user has to create a new model.
      SetModifiedFlag(FALSE);
      return FALSE;
    }
  }

  if (CString(lpszPathName).Right(3).CompareNoCase(".so") == 0) {
    CGoCadModel *pModel =
        static_cast<CGoCadModel *>(IModelLifetimeFacade::NewMain(GOCAD_MODEL, m_Logger, m_versionManager));

    assert(pModel);

    CGUIBase *guiBase = CGUIFactory::createGUI(0, pModel);

    if (!guiBase->NewModel())
      return FALSE;
    guiBase->LinkModelToOpenInventor();

    m_app_version = CurrentAppVersion();

    QString sPath = lpszPathName;

    static_cast<CGoCadSubModelEntry_Delegate *>(pModel->SubModelEntry()->getDelegate())->ImportMesh(sPath);

    return TRUE;
  }

  return FALSE; // not opened
}

// ##ModelId=3B6905BF03BA
void CGeomecDoc::OnModelProperties() {
  assert(GUI());
  GUI()->ModelProperties();
}

// ##ModelId=3B6905BF03CA
void CGeomecDoc::OnUpdateModelProperties(CCmdUI *pCmdUI) {
  // Enable model properties if we have a model
  pCmdUI->Enable(Model() != 0);
}

// ##ModelId=3B6905BF03D9
void CGeomecDoc::OnUpdateFileSave(CCmdUI *pCmdUI) {
  // Enable saving if we have a model
  pCmdUI->Enable(Model() != 0);
}

// ##ModelId=3B6905C00002
void CGeomecDoc::OnUpdateFileSaveAs(CCmdUI *pCmdUI) {
  // Enable saving as if we have a model
  pCmdUI->Enable(Model() != 0);
}

void CGeomecDoc::ModelName(CString &modelName) const {
  CString strDocPath = GetPathName();

  if (strDocPath.IsEmpty())
    return;

  CString strDocBase;
  int iDot = strDocPath.ReverseFind('.');
  if (iDot >= 0)
    strDocBase = strDocPath.Left(iDot); // excluding dot
  else
    strDocBase = strDocPath; // no dot found

  int iSlash = strDocBase.ReverseFind('\\');
  if (iSlash >= 0)
    modelName = strDocBase.Right(strDocBase.GetLength() - (iSlash + 1));
  else
    modelName = strDocBase;
}

CGUIBase *CGeomecDoc::RootGUI() {
  CFemAppGUI *pGUI = GUI();
  if (!pGUI)
    return 0; // no GUI at all...

  while (pGUI->parentGUI())
    pGUI = pGUI->parentGUI();

  return static_cast<CGUIBase *>(pGUI);
}

void CGeomecDoc::CurrentScene(COpenInventorSceneNode *pScene) {
  if ((!CurrentScene() || CurrentScene()->OpenInventorScene() != pScene) && !StickToScene()) {
    if (GUI()->disabled())
      CurrentScene(new CEmptySceneWrapper());
    else
      CurrentScene(new COpenInventorSceneWrapper(*pScene));
    UpdateAllViews(0, SWITCH_TO_OPENINVENTOR, (CObject *)(CurrentScene()));
  }
}

BOOL CGeomecDoc::SaveModel(LPCTSTR lpszPathName, CGUIBase *pGui, bool bSwitch) {
  BOOL ret = FALSE;

  if (!pGui) {
    // get the root GUI
    pGui = RootGUI();
  }

  assert(pGui);

  CModelBase &model = *pGui->model();
  assert(!model.parentModel()); // must be a root model

  // We are going to write to a filos file.
  CString pathName = lpszPathName;

  if (pathName.GetLength() + 4 > MAX_PATH) {
    CString path;
    CString file;

    int pivot = pathName.ReverseFind('\\');
    if (pivot >= 0) {
      for (int i = 0; i <= pivot; i++)
        path += pathName[i];
      for (int i = pivot + 1; i < pathName.GetLength(); ++i)
        file += pathName[i];
    } else {
      file = pathName;
    }
    int shorten = pathName.GetLength() + 4 - MAX_PATH + 6; // 6 for prefix

    pathName = path;

    CString shortFile;

    for (int i = shorten; i < file.GetLength(); ++i)
      shortFile += file[i];

    int p = 0;
    QString f;
    do {
      f = QString("%1gm_%2%3").arg((const char *)pathName).arg(p++, 3, 10, QChar('0')).arg((const char *)shortFile);
    } while (FileExists(f));

    f = QString("gm_%1").arg(--p, 3, 10, QChar('0'));
    shortFile = f.toStdString().c_str() + shortFile;

    pathName += shortFile;

    QString msg = QString("File name is too long and has been shortened to '%1'.").arg((const char *)shortFile);
    _m()->msg(msg.toStdString().c_str());
  }

  IModelLifetimeFacade::SaveMain(CGUIBase::CMainModelLoadSave(m_app_version, pGui), model, (const char *)pathName,
                                 bSwitch);

  return TRUE; // always return TRUE so MFC does not try to delete the file if it failed
               // these things have been handled here
}

// ##ModelId=3BC55D6300A5
BOOL CGeomecDoc::OnSaveDocument(LPCTSTR lpszPathName) {
  assert(Model());
  m_versionManager.FileVersion(CStreamVersion(VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION));

  m_app_version = CurrentAppVersion();

  SetPathName(lpszPathName, FALSE);

  BOOL bRet = SaveModel(lpszPathName);
  if (bRet)
    SetModifiedFlag(FALSE);

  return bRet;
}

// ##ModelId=3B6905BF036D

// Pre	: There is model that accepts pointsets ...
// ##ModelId=3BC55D6300F2
/// \brief import file
void CGeomecDoc::_OnImport(unsigned int filterStringId ///< identifier for string containing file extensions .xyz
) {
  CString sFilter;
  sFilter = getStringTableEntry(filterStringId);

  CTnoFileDialog dlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, sFilter);

  if (dlg.DoModal() == IDOK) {
    // Collect path name
    std::vector<CString> vcPathName;
    POSITION pos = dlg.GetStartPosition();
    while (pos)
      vcPathName.push_back(dlg.GetNextPathName(pos));

    // Create stupid dialog
    try { // we catch cancel at some places in OnImportFile, but we also need to catch it here in case the user cancels
          // outside those try-catch blocks
      CGraphNode::TNodeVec created_objects;

      std::auto_ptr<IProgressBase> progress(_g->prog()->create(eProgress::Dual, "Import", true, vcPathName.size()));

      for (size_t i = 0; i < vcPathName.size(); i++) {
        progress->NextJob((LPCSTR)vcPathName[i]);
        if (!OnImportFile(vcPathName[i], *progress, created_objects))
          break;
      }
    } catch (CProgressCancel *e) {
      delete e;
    }

    FemAppGetMainWnd()->SetForegroundWindow();
  }
}

/// \brief Import file with preference for text files
void CGeomecDoc::OnImport() { _OnImport(IDS_IMPORT_FILTER); }

/// \brief Import file with preference for GoCad files
void CGeomecDoc::OnImportSurface() { _OnImport(IDS_IMPORT_GOCAD_FILTER); }

// ##ModelId=3BC55D6300F4
void CGeomecDoc::OnUpdateImport(CCmdUI *pCmdUI) { pCmdUI->Enable(Model() != 0); }

// ##ModelId=3BC55D630110
void CGeomecDoc::OnItemAttributes() {
  CGraphNode_Delegate *graphNode_Delegate = new CGraphNode_Delegate(&ContextMenuNode());

  graphNode_Delegate->Attributes();
}

// ##ModelId=3BC55D630112
void CGeomecDoc::OnUpdateItemDelete(CCmdUI *pCmdUI) {
  assert(Model());
  pCmdUI->Enable(ContextMenuCtrlObject().CanDelete());
}

void CGeomecDoc::OnUpdateItemModify(CCmdUI *pCmdUI) {
  assert(Model());
  pCmdUI->Enable(ContextMenuCtrlObject().CanModify());
}

void CGeomecDoc::OnItemModify() {
  assert(Model());
  assert(ContextMenuCtrlObject().CanModify());
  ContextMenuCtrlObject().Modify();
}

// ##ModelId=3BC55D63011F
void CGeomecDoc::OnItemDelete() {
  assert(Model());
  assert(ContextMenuCtrlObject().CanDelete());
  ContextMenuCtrlObject().Delete();
}

bool CGeomecDoc::show_xsec_manipulator_when_needed() {
  bool res = false;
  COpenInventorSceneNode *scene_node =
      CurrentScene()->OpenInventorScene(); // -> Main scene : CStorageNode -> CGraphNode
  if (scene_node) {
    // CModelBase* model = dynamic_cast<CModelBase*>( &scene_node->Model() );
    // if( model )
    //{
    // CConsistencyGuard* guard = model->GetConsistencyGuard();
    // if( guard && guard->is_model_in_scope( CConsistencyGuard::XSecImprovement ) )
    const CWellCasingModel *model_casing = dynamic_cast<const CWellCasingModel *>(&scene_node->Model());
    if (model_casing) {
      printer->debug("doc : xsc : refresh");
      CCrossSection *xsec = dynamic_cast<CCrossSection *>(SelectedNode()); // -> CCrossSection : CStorageNode
      if (xsec) {
        OIDIMeshNodeManager *manager = CurrentScene()->OpenInventorScene()->OIV_node_manager();
        manager->OnShowCrossSection(*xsec);
        manager->resetTimeOut(true);
        res = true;
      }
    }
    //}
  }
  return res;
}

// all meshes bounding box if OIV scene, otherwise all zeros
TBox CGeomecDoc::bbox() {
  CModelBase *model = static_cast<CModelBase *>(Model());
  CBoundaryBase &b = model->Boundary();
  geo::CPoint M1 = b.Max();
  geo::CPoint m1 = b.Min();

  CMeshBase *mesh = &model->Mesh();

  T3da m1_;
  T3da M1_;

  T3da &t_ = RefinementBox::m_translation;
  if (t_ != T3da{0, 0, 0})
  // if( mesh && mesh->IsMesh() )
  {
    m1_ = RBox::rw_to_oiv(T3da{m1.X(), m1.Y(), m1.Z()}, RBox::Pos);
    M1_ = RBox::rw_to_oiv(T3da{M1.X(), M1.Y(), M1.Z()}, RBox::Pos);
  } else {
    // translation = center
    T3da t = T3da{(m1.X() + M1.X()) / 2, (m1.Y() + M1.Y()) / 2, (m1.Z() + M1.Z()) / 2};

    m1_ = T3da{m1.X() - t[0], m1.Y() - t[1], m1.Z() - t[2]};

    M1_ = T3da{M1.X() - t[0], M1.Y() - t[1], M1.Z() - t[2]};
  }

  return TBox(m1_, M1_);

  /*COpenInventorSceneNode* scene_node = CurrentScene()->OpenInventorScene(); // -> Main scene : CStorageNode ->
  CGraphNode if( scene_node )
  {
    OIDIMeshNodeManager* manager = scene_node->OIV_node_manager();
    SbBox3f bb_ = manager->GetAllMeshesBoundingBox();

    const SbVec3f& m = bb_.getMin();
    const SbVec3f& M = bb_.getMax();

    return TBox(
      T3da{ m[0], m[1], m[2] },
      T3da{ M[0], M[1], M[2] } );
  }
  else
    return TBox( T3da{0,0,0}, T3da{0,0,0} );*/
}

// ##ModelId=3BC55D630093
void CGeomecDoc::OnSelect(CGraphNode *pNode) {
  if (dynamic_cast<IResultComponent *>(pNode))
    return;

  if (IsDeleting())
    return;

  assert(Model());
  CModelBase &model = static_cast<CModelBase &>(*Model());

  assert(GUI());
  CGUIBase &gui = static_cast<CGUIBase &>(*GUI());

  // make sure to disable drawing when switching scenes before calling the base class'
  // implementation, which will force a redraw on the newly selected scene
  if (dynamic_cast<COpenGLSceneBase *>(pNode)) {
    if (model.AutomaticallyDisableDrawing())
      model.DisableRedrawing();
  }

  // Let's do the base it's work
  CFemAppDoc::OnSelect(pNode);

  if (SelectedNode() == 0)
    return;

  // Is the OpenInventor scene selected?
  COpenInventorSceneNode *_pOIVScene = dynamic_cast<COpenInventorSceneNode *>(SelectedNode());
  if (_pOIVScene) {
    // make sure we have nice (i.e. not garbled) colors again
    _pOIVScene->ResetRenderAction();

    // Inactivate original scene type
    CurrentScene((ISceneWrapper *)0);
    UpdateAllViews(0, NEW_CURRENT_SCENE, 0);

    // Set the OpenInventor scene active
    CurrentScene(_pOIVScene);
    UpdateAllViews(0, SWITCH_TO_OPENINVENTOR, (CObject *)CurrentScene());
  }

  if (dynamic_cast<OIV2DWellPathGraphScene *>(SelectedNode())) {
    CurrentScene(new OIV2DWellPathGraphSceneWrapper(*static_cast<OIV2DWellPathGraphScene *>(SelectedNode())));
    UpdateAllViews(0, SWITCH_TO_OPENINVENTOR, (CObject *)CurrentScene());
  }

  if (dynamic_cast<OIV2DHistoryScene *>(SelectedNode())) {
    CurrentScene(new OIV2DHistorySceneWrapper(*static_cast<OIV2DHistoryScene *>(SelectedNode())));
    UpdateAllViews(0, SWITCH_TO_OPENINVENTOR, (CObject *)CurrentScene());
  }

  // Can we view the current selection with the current scene
  if (CurrentScene()) {
    if (CurrentScene()->IsLinkedTo(*SelectedNode())) {
      show_xsec_manipulator_when_needed();
      return;
    }

    if (CurrentScene()->CanConnectItem(*SelectedNode())) {
      CurrentScene()->ConnectItem(*SelectedNode());
      return;
    }
  }
}

void CGeomecDoc::OnUpdateColorScaleSelection(CCmdUI *pCmdUI) {

  CColorScaleEntry *pEntry = 0;

  if (Model())
    pEntry = dynamic_cast<CColorScaleEntry *>(Model()->GraphEntry(MD_BASE_COLOR_SCALE));

  if (pEntry) {
    switch (pCmdUI->m_nID) {
    case ID_VIEW_LOCAL:
      pCmdUI->Enable(TRUE);
      pCmdUI->SetRadio(pEntry->ColorScaleType() == CColorScaleEntry::LOCAL);
      break;
    case ID_VIEW_GLOBAL:
      pCmdUI->Enable(pEntry->GlobalColorGradient() != 0);
      pCmdUI->SetRadio(pEntry->ColorScaleType() == CColorScaleEntry::GLOBAL);
      break;
    case ID_VIEW_HOTSPOT:
      pCmdUI->Enable(pEntry->HotSpot() != 0);
      pCmdUI->SetRadio(pEntry->ColorScaleType() == CColorScaleEntry::HOTSPOT);
      break;
    case ID_VIEW_COLORSCALE:
      break;
    default:
      assert(FALSE);
      break;
    }
  } else
    pCmdUI->Enable(FALSE);
}

enum CGeomecDoc::FILE_STATUS CGeomecDoc::OnImportEclipse(const CString &sPath, CProgressDlg_MFC &dlg) {
  return FORMAT_OK;
}

enum CGeomecDoc::FILE_STATUS CGeomecDoc::OnImportASCII(const CString &sPath, IProgressBase &dlg,
                                                       CGraphNode::TNodeVec &created_objects) {
  assert(Model());
  CModelBase &model = ((CModelBase &)*Model()).RootModel();
  CElementSetFile file(model, created_objects);

  try {
    if (file.Open((LPCSTR)sPath, dlg)) {
      if (created_objects.empty())
        _m()->msg("No valid objects for import found");
    } else {
      _m()->msg(file.ErrorMessage());
      return FORMAT_ERROR;
    }

  } catch (CProgressCancel *e) {
    delete e;
    // WARNING
    IPointSet *pPointSet = &file.PointSet();
    delete pPointSet;
    file.Close();
    return CANCEL;
  }

  //	IPointSet* pSet;
  //	if(file.IsElementSet()) pSet = &file.ElementSet();
  //	else pSet = &file.PointSet();

  //	if( file.AxisSystem() != CElementSetFile::INodalFormat::UNKNOWN )
  //		model.ConvertSet(pSet, (file.AxisSystem() == CElementSetFile::INodalFormat::AXIS3D));

  return FORMAT_OK;
}

enum CGeomecDoc::FILE_STATUS CGeomecDoc::OnImportExcel(const CString &sPath, IProgressBase &dlg,
                                                       CGraphNode::TNodeVec &created_objects) {
  assert(Model());
  CModelBase &model = ((CModelBase &)*Model()).RootModel();

  CElementSetExcelFile excelfile(model);
  if (!excelfile.ReadSheets((LPCSTR)sPath, dlg, created_objects))
    return FORMAT_ERROR;

  //	model.ConvertSet(&excelfile.PointSet(), (excelfile.AxisSystem() == CElementSetFile::INodalFormat::AXIS3D));

  return FORMAT_OK;
}

//////////////////////////////////////////// RunAnalysis [Ini]

/*
  dsa: diana stand alone
  icons id / status:
  IDI_DIANA_1P : available but one process (seleceted in settings)
  IDI_DIANA_2P : available and two process (Geomec and diana) (seleceted in settings)
  IDI_DIANA_RUNNING
  IDI_DIANA_CRASH : diana process crashed while running the model (in a separated process)
  IDI_DIANA_EMPTY : not available
*/
int convert_dsa_icon_id(DSA_Icon icon_id) {
  switch (icon_id) {
  case DSA_Icon::_1P:
    return IDI_DIANA_1P;
  case DSA_Icon::_2P:
    return IDI_DIANA_2P;
  case DSA_Icon::Running:
    return IDI_DIANA_RUNNING;
  case DSA_Icon::Crash:
    return IDI_DIANA_CRASH;
  case DSA_Icon::Empty:
    return IDI_DIANA_EMPTY;
  default:
    return IDI_DIANA_EMPTY; // ERROR
  }
}

/*
  ////
  //CToolBarCtrl& ctrl = bar.GetToolBarCtrl();
  //ctrl.EnableButton( dsa_icon_idx, FALSE );
  ////

  // If run 'RunAnalysis2' in a different thread Geomec crashes
  if( doc->m_dsa_icon_id == IDI_DIANA_RUNNING && icon_id == IDI_DIANA_2P ) // end run model Ok
  {
    printer->debug( "set_dsa_icon : Launch 'RunAnalysis2'" );

    // 0. Local var so bad scope
    //std::thread t1( &CGeomecDoc::RunAnalysis2, GetGeomecDoc() ); // Not working, review this

    // 1. Next also crashes
    //doc->RunAnalysis2();

    // 2. Next crashes
    //std::thread* t1 = new std::thread( &CGeomecDoc::RunAnalysis2, GetGeomecDoc() ); // Not working, review this

    // 3. Never executed
    // QTimer::singleShot( 1000, [=](){ doc->RunAnalysis2(); } );
    // QApplication::instance()->processEvents();
  }
*/
void CGeomecDoc::set_dsa_icon(DSA_Icon icon_id) {
  /*printer->debug( "DSA Icon: '%s'->'%s'",
    DSA_Icon_s_v[ m_dsa_icon_id ].c_str(),
    DSA_Icon_s_v[ icon_id ].c_str() );*/

  int dsa_icon_idx = 10;

  CMainFrame *pMainFrame = (CMainFrame *)(FemAppGetMainWnd());
  CToolBar &bar = pMainFrame->GetConsistencyToolbar();
  pMainFrame->SetIcon(bar, dsa_icon_idx, convert_dsa_icon_id(icon_id));

  m_dsa_icon_id = icon_id;
}

bool CGeomecDoc::RunAnalysis(const CAnalysisType::TAnalysisType antype, bool bWriteInputFiles, bool bWriteQuadDat) {
  m_eh->start(antype, bWriteInputFiles, bWriteQuadDat);
  return true; // This means nothing
}

//
// Run analysis
//

void CGeomecDoc::OnAnalysisLinearDepletion() { RunAnalysis(CAnalysisType::AT_LINEAR, false, false); }

void CGeomecDoc::OnAnalysisNonlinearCalculation() { RunAnalysis(CAnalysisType::AT_NONLIN, false, false); }

void CGeomecDoc::OnAnalysisHeatFlow() {
  CModelBase &model = static_cast<CModelBase &>(*Model());

  if (model.ResultRegister().Linear() || model.ResultRegister().NonLinear() || model.ResultRegister().Mixture()) {
    if (_m()->msg("The heat flow analysis will invalidate the results from other analysis types. Are you sure you want "
                  "to continue?",
                  MB_YESNO) == IDNO)
      return;
  }

  if (RunAnalysis(CAnalysisType::AT_HEAT, false, false)) {
    CModelBase &modelBase = static_cast<CModelBase &>(*Model());
    modelBase.ResultRegister().ClearLinear(false);
    modelBase.ResultRegister().ClearNonLinear(false);
    modelBase.ResultRegister().ClearMixture(true); // now send modified signal...
  }
}

void CGeomecDoc::OnAnalysisMixture() { RunAnalysis(CAnalysisType::AT_MIXTURE, false, false); }

void CGeomecDoc::OnAnalysisMixtureContainment() { RunAnalysis(CAnalysisType::AT_MIXTURE_CONTAINMENT, false, false); }

void CGeomecDoc::OnUpdateAnalysisLinearDepletion(CCmdUI *pCmdUI) {
  if (Model()) {
    CModelBase *pModel = dynamic_cast<CModelBase *>(Model());
    if (pModel)
      pCmdUI->Enable(pModel->GetConsistencyGuard()->CanCalculate(*pModel, CAnalysisType::AT_LINEAR));
  } else
    pCmdUI->Enable(FALSE);
}

void CGeomecDoc::OnUpdateAnalysisExportLinear(CCmdUI *pCmdUI) { OnUpdateAnalysisLinearDepletion(pCmdUI); }

void CGeomecDoc::OnUpdateAnalysisHeatFlow(CCmdUI *pCmdUI) {
  CModelBase *pModel = static_cast<CModelBase *>(Model());
  pCmdUI->Enable(pModel && pModel->GetConsistencyGuard()->CanCalculate(*pModel, CAnalysisType::AT_HEAT));
}

void CGeomecDoc::OnUpdateAnalysisMixture(CCmdUI *pCmdUI) {
  CModelBase *pModel = static_cast<CModelBase *>(Model());
  pCmdUI->Enable(pModel && pModel->GetConsistencyGuard()->CanCalculate(*pModel, CAnalysisType::AT_MIXTURE));
}

//
// Export [Ini]
//

void CGeomecDoc::OnAnalysisExportLinear() { RunAnalysis(CAnalysisType::AT_LINEAR, true, false); }

void CGeomecDoc::OnUpdateAnalysisExportNonlin(CCmdUI *pCmdUI) { OnUpdateAnalysisNonlinearCalculation(pCmdUI); }

void CGeomecDoc::OnAnalysisExportQuadDat() { RunAnalysis(CAnalysisType::AT_LINEAR, false, true); }

void CGeomecDoc::OnUpdateAnalysisExportQuadDat(CCmdUI *pCmdUI) {
  if (Model()) {
    CModelBase *pModel = dynamic_cast<CModelBase *>(Model());
    if (pModel)
      pCmdUI->Enable(pModel->IsMesh());
  } else
    pCmdUI->Enable(FALSE);
}

void CGeomecDoc::OnAnalysisExportNonlin() { RunAnalysis(CAnalysisType::AT_NONLIN, true, false); }

void CGeomecDoc::OnAnalysisExportHeatFlow() { RunAnalysis(CAnalysisType::AT_HEAT, true, false); }

void CGeomecDoc::OnAnalysisExportMixture() { RunAnalysis(CAnalysisType::AT_MIXTURE, true, false); }

void CGeomecDoc::OnAnalysisExportMixtureContainment() {
  RunAnalysis(CAnalysisType::AT_MIXTURE_CONTAINMENT, true, false);
}

void CGeomecDoc::OnUpdateAnalysisExportHeatFlow(CCmdUI *pCmdUI) { OnUpdateAnalysisHeatFlow(pCmdUI); }

void CGeomecDoc::OnUpdateAnalysisExportMixture(CCmdUI *pCmdUI) { OnUpdateAnalysisMixture(pCmdUI); }

//
// Export [End]
//

void CGeomecDoc::OnConsistencyGuard() {
  CModelBase *pModel = static_cast<CModelBase *>(Model());
  CConsistencyGuard *guard = pModel->GetConsistencyGuard();

  guard->Validate(*pModel);

  OnInputStatus();
}

void CGeomecDoc::OnUpdateDianaRunning(CCmdUI *pCmdUI) { pCmdUI->Enable(_g->dsa_available()); }

void CGeomecDoc::OnUpdateConsistencyGuard(CCmdUI *pCmdUI) {
  if (Model()) {
    CModelBase *pModel = static_cast<CModelBase *>(Model());
    pCmdUI->Enable(TRUE);

    CConsistencyGuard *guard = pModel->GetConsistencyGuard();

    int CGStatus = guard->Status(*pModel);

    if (CGStatus != m_nLastCGStatus) {
      CMainFrame *pMainFrame = (CMainFrame *)(FemAppGetMainWnd());

      int img_id = IDI_CG_UNKNOWN;
      switch (CGStatus) {
      case CConsistencyGuard::OK:
        img_id = IDI_CG_OK;
        break;
      case CConsistencyGuard::UNKNOWN:
        img_id = IDI_CG_UNKNOWN;
        break;
      case CConsistencyGuard::NEED_CLEANUP:
        img_id = IDI_CG_MESSY;
        break;
      case CConsistencyGuard::INVALID:
        img_id = IDI_CG_INVALID;
        break;
      case CConsistencyGuard::BROKEN:
        img_id = IDI_CG_BROKEN;
        break;
      }
      pMainFrame->SetIcon(pMainFrame->GetConsistencyToolbar(), 9, img_id);

      m_nLastCGStatus = CGStatus;
    }
  } else
    pCmdUI->Enable(FALSE);
}

void CGeomecDoc::OnFileOpen() {
  CString sFilter;
  sFilter = getStringTableEntry(IDS_FILE_OPEN_FILTER);

  CTnoFileDialog dlg(TRUE, GEOMEC_DEFAULT_EXTENSION, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sFilter,
                     FemAppGetMainWnd());

  printer->info("on file open");

  if (dlg.DoModal() == IDOK) {
    CString newName = dlg.GetPathName();
    if (OnOpenDocument(newName)) {
      UpdateAllViews(NULL, NEW_MODEL);
    }
  }
}

void CGeomecDoc::OnFileSave() {
  CModelBase *pModel = static_cast<CModelBase *>(Model());
  QString database;

  if (pModel)
    database = pModel->GetConsistencyGuard()->HDF5FileName();

  if (database.isEmpty()) {
    CString strPath = GetPathName();
    if (!strPath.IsEmpty()) {
      int iDotPos = strPath.ReverseFind('.');
      CString strExtension;
      if (iDotPos != -1)
        strExtension = strPath.Mid(iDotPos);

      if (strExtension == ".gm4") {
        strPath = strPath.Left(iDotPos) + "." GEOMEC_DEFAULT_EXTENSION;
        QString file_name =
            QFileDialog::getSaveFileName(nullptr /*parent*/, "Save File", (LPCTSTR)strPath, "Geomec Files (*.gm5)");
        strPath = file_name.toStdString().c_str();
      }

      OnSaveDocument(strPath);
    } else
      CDocument::OnFileSave();
  } else
    OnSaveDocument(database.toStdString().c_str());
}

void CGeomecDoc::OnFileSaveAs() {
  CString strPath = GetPathName();
  if (!strPath.IsEmpty()) {
    int iDotPos = strPath.ReverseFind('.');
    CString strExtension;
    if (iDotPos != -1)
      strExtension = strPath.Mid(iDotPos);

    if (strExtension == ".gm4" || strExtension == ".gm5")
      strPath = strPath.Left(iDotPos);

    // if (strExtension != "." GEOMEC_DEFAULT_EXTENSION)
    // strPath += "." GEOMEC_DEFAULT_EXTENSION;
  }

  CFileDialog dlg(FALSE, GEOMEC_DEFAULT_EXTENSION, strPath, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                  getStringTableEntry(IDS_FILE_SAVE_FILTER));
  if (dlg.DoModal() == IDOK) {
    OnSaveDocument(dlg.GetPathName());
  }
}

void CGeomecDoc::OnEditPaste() {
  CMainFrame *pMainFrame = (CMainFrame *)(FemAppGetMainWnd());
  assert(pMainFrame->Paste());
  assert(SelectedNode());
  assert(SelectedNode()->CanConnectItem(*pMainFrame->Paste()));
  SelectedNode()->ConnectItem(*pMainFrame->Paste());
}

void CGeomecDoc::OnUpdateEditPaste(CCmdUI *pCmdUI) {
  CMainFrame *pMainFrame = (CMainFrame *)(FemAppGetMainWnd());
  if (pMainFrame->Paste() && SelectedNode()) {
    pCmdUI->Enable(SelectedNode()->CanConnectItem(*pMainFrame->Paste()));
    return;
  }

  pCmdUI->Enable(FALSE);
}

void CGeomecDoc::OnEditCopy() {
  assert(SelectedNode());
  CMainFrame *pMainFrame = (CMainFrame *)(FemAppGetMainWnd());
  pMainFrame->Copy(*SelectedNode());
}

void CGeomecDoc::OnUpdateEditCopy(CCmdUI *pCmdUI) { pCmdUI->Enable(SelectedNode() != 0); }

void CGeomecDoc::OnUpdateAnalysisNonlinearCalculation(CCmdUI *pCmdUI) {
  if (Model()) {
    CModelBase *pModel = dynamic_cast<CModelBase *>(Model());
    if (pModel)
      pCmdUI->Enable(pModel->GetConsistencyGuard()->CanCalculate(*pModel, CAnalysisType::AT_NONLIN));
    //			pCmdUI->Enable(pModel->ValidateForAnalysis());
  } else
    pCmdUI->Enable(FALSE);
}

void CGeomecDoc::OnAnalysisNonlinearCalcParams() {
  CNonlinParams dlg(*((CModelBase *)Model()));
  dlg.DoModal();
}

void CGeomecDoc::OnUpdateAnalysisNonlinearCalcParams(CCmdUI *pCmdUI) { pCmdUI->Enable(Model() != 0); }

void CGeomecDoc::OnAnalysisLinearSettings() {
  CLinstaParamsDlg dlg(*((CModelBase *)Model()));
  dlg.DoModal();
}

void CGeomecDoc::OnUpdateAnalysisLinearSettings(CCmdUI *pCmdUI) { pCmdUI->Enable(Model() != 0); }

void CGeomecDoc::OnAnalysisProperties() {
  CModelBase *ptr = dynamic_cast<CModelBase *>(Model());
  if (ptr) {
    CCalculationProperties props(*ptr, *GetGeomecApp()->TempPath());
    props.DoModal();
  }
}

void CGeomecDoc::OnUpdateAnalysisProperties(CCmdUI *pCmdUI) {
  if (Model()) {
    pCmdUI->Enable(TRUE);
  } else
    pCmdUI->Enable(FALSE);
}

// wjrx mantis 2975
void CGeomecDoc::OnUpdateAnalysisExportResults(CCmdUI *pCmdUI) {
  bool bEnable = Model() && static_cast<CModelBase *>(Model())->Mesh().IsMesh();
  pCmdUI->Enable(bEnable);
}

void CGeomecDoc::OnAnalysisExportResults() {
  CResultExportDlg dlg(*static_cast<CModelBase *>(Model()));
  dlg.DoModal();
}

void CGeomecDoc::OnUpdateAnalysisExportResultsGoCad(CCmdUI *pCmdUI) {
  bool bEnable = Model() && static_cast<CModelBase *>(Model())->Mesh().IsMesh();
  pCmdUI->Enable(bEnable);
}

void CGeomecDoc::OnAnalysisExportResultsGoCad() {
  CGocadExport ge(*static_cast<CModelBase *>(Model()));
  CExportGocadDlg dlg(ge, 0);
  dlg.DoModal();
}

void CGeomecDoc::OnUpdateAnalysisExportSurfaces(CCmdUI *pCmdUI) {
  bool bEnable = Model() && static_cast<CModelBase *>(Model())->GraphEntry(MD_BASE_SURFACE);
  pCmdUI->Enable(bEnable);
}

void CGeomecDoc::OnAnalysisExportResultsSurfaces() {
  CModelBase *pModel = static_cast<CModelBase *>(Model());
  assert(pModel);
  CSurfaceEntry *pSurfaceEntry = static_cast<CSurfaceEntry *>((pModel->GraphEntry(MD_BASE_SURFACE)));
  dynamic_cast<CSurfaceEntry_Delegate *>(pSurfaceEntry->getDelegate())->ExportSurfaces();
}

// wjrx mantis 2976
void CGeomecDoc::OnUpdateImportMesh(CCmdUI *pCmdUI) {
  const CGoCadModel *model = dynamic_cast<const CGoCadModel *>(Model());

  bool bEnable = model && model->CanImportMesh();
  pCmdUI->Enable(bEnable);
}

void CGeomecDoc::OnImportMesh() {
  CGoCadModel *model = dynamic_cast<CGoCadModel *>(Model());
  assert(model);
  static_cast<CGoCadSubModelEntry_Delegate *>(model->SubModelEntry()->getDelegate())->ImportMesh();
}

void CGeomecDoc::OnUpdateImportResults(CCmdUI *pCmdUI) {
  CModelBase *pModel = dynamic_cast<CModelBase *>(Model());
  bool bEnable = pModel && pModel->CanImportResults();
  pCmdUI->Enable(bEnable);
}

void CGeomecDoc::OnImportResults() {
  CGUIBase *pGUI = dynamic_cast<CGUIBase *>(GUI());
  assert(pGUI);
  pGUI->ImportResults();
}

void CGeomecDoc::OnUpdateImportInsertDeviatedWellPath(CCmdUI *pCmdUI) {
  bool bEnable = false;
  CModelBase *pModel = dynamic_cast<CModelBase *>(Model());
  bEnable = pModel && pModel->CanInsertNewWellPath();
  pCmdUI->Enable(bEnable);
}

void CGeomecDoc::OnImportInsertDeviatedWellPath() {
  CGUIBase *pGUI = dynamic_cast<CGUIBase *>(GUI());
  assert(pGUI);
  pGUI->InsertDeviatedWellPath();
}

void CGeomecDoc::OnGVTSettings() {
  CModelBase *pModel = static_cast<CModelBase *>(Model());
  CGVTSettingsDlg dlg(pModel->GVTSettings());
  dlg.DoModal();
}

void CGeomecDoc::OnUpdateGVTSettings(CCmdUI *pCmdUI) {
  if (!CGVTSettings::CanCalculate()) {
    pCmdUI->Enable(FALSE);
    return;
  }

  if (Model() == 0 || ((TFormationBaseEntry *)Model()->GraphEntry(MD_BASE_FORMATION))->EntryNodes().size() == 0) {
    pCmdUI->Enable(FALSE);
    return;
  }

  pCmdUI->Enable(TRUE);
}

void CGeomecDoc::OnBorrowLicense() {
#if LICENSE_BORROWING
  CLicenseBorrowDlg dlg;
  dlg.DoModal();
#endif
}

void CGeomecDoc::OnUpdateBorrowLicense(CCmdUI *pCmdUI) {
#if LICENSE_BORROWING
  pCmdUI->Enable(TRUE);
#else
  pCmdUI->Enable(FALSE);
#endif
}

void CGeomecDoc::OnExportSVS() {
  // const CModelBase* pModel = static_cast<CModelBase*>(Model());
  CExportSvsDlg dlg(*((CModelBase *)Model()));
  dlg.DoModal();
}
void CGeomecDoc::OnUpdateExportSVS(CCmdUI *pCmdUI) {
  CModelBase *pModel = static_cast<CModelBase *>(Model());
  pCmdUI->Enable(pModel && pModel->ResultRegister().ResultsAvailable() ? 1 : 0);
}

void CGeomecDoc::OnDianaXCloseDialog() {
  // this function is called just before the DianaX dialog is closed. A perfect moment to re-enable the MainFrame.
  FemAppGetMainWnd()->EnableWindow(true);
}

void CGeomecDoc::OnDianaXDiaMessage(LPCTSTR strMessage) {
  if (strMessage)
    m_Logger.AddLine(strMessage);
}

void CGeomecDoc::OnDianaXDiaWarning(LPCTSTR strMessage) {
  if (strMessage)
    m_Logger.AddLine(strMessage);
}

void CGeomecDoc::OnDianaXDiaError(LPCTSTR strMessage) {
  if (strMessage)
    m_Logger.AddLine(strMessage);
}

void CGeomecDoc::OnAnalysisLog() {
  CAnalysisLogDlg dlg(m_Logger, FemAppGetMainWnd());
  dlg.DoModal();
}

void CGeomecDoc::OnUpdateAnalysisLog(CCmdUI *pCmdUI) {
  // pCmdUI->Enable(!m_Logger.IsEmpty());
}

void CGeomecDoc::OnSetMeshTolerance() {
  CTetraModel *pModel = dynamic_cast<CTetraModel *>(Model());
  assert(pModel);

  geo::CTetMeshBase &tmb = dynamic_cast<geo::CTetMeshBase &>(pModel->Mesh().Mesh());
  CMeshToleranceDlg dlg(tmb);
  dlg.DoModal();
}

void CGeomecDoc::OnUpdateSetMeshTolerance(CCmdUI *pCmdUI) {
  const CTetraModel *pModel = dynamic_cast<const CTetraModel *>(Model());
  if (pModel)
    pCmdUI->Enable(TRUE);
  else
    pCmdUI->Enable(FALSE);
}

void CGeomecDoc::OnCreateMesh() {
  assert(Model());
  CModelBase &model = dynamic_cast<CModelBase &>(*Model());
  assert(!model.IsMesh() && model.CanCreateMesh());

  // save remesh boxes
  if (CurrentScene() && CurrentScene()->IsOpenInventorScene()) {
    COpenInventorSceneNode *node = CurrentScene()->OpenInventorScene();
    if (node) {
      MeshRefinementDlg *dlg = node->mesh_refinement_box_dlg();
      if (dlg) {
        if (dlg->save() != MeshRefinementDlg::NoError) // error
        {
          QMessageBox::warning(nullptr, QString("Meshing with mesh refinement boxes"),
                               QString("Error on refinement boxes.\nFix it before meshing"));
          return;
        }
      }
    }
  }

  CMesherDlg dlg;
  model.mesher_dlg(&dlg);
  //
  model.CreateMesh();
  //
  model.mesher_dlg(nullptr);

  UpdateAllViews(0, CURRENT_SCENE_MODIFIED, 0);

  if (CurrentScene() && CurrentScene()->IsOpenInventorScene())
    CurrentScene()->ZoomReset();
}

void CGeomecDoc::OnUpdateCreateMesh(CCmdUI *pCmdUI) {
  if (Model()) {
    CModelBase &model = dynamic_cast<CModelBase &>(*Model());
    pCmdUI->Enable(model.CanCreateMesh() && !model.IsMesh());
  } else
    pCmdUI->Enable(FALSE);
}

void CGeomecDoc::OnInvalidateMesh() {
  assert(Model());
  CModelBase &model = dynamic_cast<CModelBase &>(*Model());
  model.InvalidateMesh();
  UpdateAllViews(0, CURRENT_SCENE_MODIFIED, 0);
}

void CGeomecDoc::OnUpdateInvalidateMesh(CCmdUI *pCmdUI) {
  if (Model()) {
    CModelBase &model = dynamic_cast<CModelBase &>(*Model());
    pCmdUI->Enable(model.IsMesh() && model.Mesh().CanInvalidateMesh() && !model.BranchState().IsBranch());
  } else
    pCmdUI->Enable(FALSE);
}

void CGeomecDoc::OnInputStatus() {
  CModelBase *pModel = static_cast<CModelBase *>(Model());

  CPropertySheet sheet(IDS_CONSISTENCY_GUARD);

  sheet.AddPage(new CConsistencyGuardValidationPg(pModel));
  sheet.AddPage(new CConsistencyGuardSettingsPg(pModel));

  if (pModel->GetConsistencyGuard()->Status(*pModel) == CConsistencyGuard::BROKEN)
    sheet.AddPage(new CConsistencyGuardErrorPg(pModel));

  sheet.DoModal();
}

void CGeomecDoc::OnUpdateInputStatus(CCmdUI *pCmdUI) {
  if (Model()) {
    CModelBase *pModel = dynamic_cast<CModelBase *>(Model());
    if (pModel)
      pCmdUI->Enable(TRUE);
  } else
    pCmdUI->Enable(FALSE);
}

void CGeomecDoc::OnInsertDeviatedWellPathByPointSet() // wjrx mantis 3185
{
  CNewWellPathEntry &entry = dynamic_cast<CNewWellPathEntry &>(*Model()->GraphEntry(MD_NEW_WELLPATH));
  (dynamic_cast<CNewWellPathEntry_Delegate *>(entry.getDelegate()))->InsertDeviatedWellPathByPointSet();
}

void CGeomecDoc::OnFormationfull() {
  CTetraModel *pTetraModel = dynamic_cast<CTetraModel *>(Model());
  CHexaModel *pHexaModel = dynamic_cast<CHexaModel *>(Model());
  IWellModel *pWellModel = dynamic_cast<IWellModel *>(Model());

  if (pTetraModel != 0) {
    CTetraFormationEntry &entry = dynamic_cast<CTetraFormationEntry &>(*pTetraModel->GraphEntry(MD_TETRA_FORMATION));
    entry.OnShowFullMesh();
  } else if (pHexaModel != 0) {
    CHexaFormationEntry &entry = dynamic_cast<CHexaFormationEntry &>(*pHexaModel->GraphEntry(MD_HEXA_FORMATION));
    entry.OnShowFullMesh();
  } else if (pWellModel != 0) {
    CWellFormationEntry &entry = dynamic_cast<CWellFormationEntry &>(*pWellModel->GraphEntry(MD_WELLMODEL_FORMATION));
    entry.OnShowFullMesh();
  }
}

void CGeomecDoc::OnFormationskin() {
  CTetraModel *pTetraModel = dynamic_cast<CTetraModel *>(Model());
  CHexaModel *pHexaModel = dynamic_cast<CHexaModel *>(Model());
  IWellModel *pWellModel = dynamic_cast<IWellModel *>(Model());

  if (pTetraModel != 0) {
    CTetraFormationEntry &entry = dynamic_cast<CTetraFormationEntry &>(*pTetraModel->GraphEntry(MD_TETRA_FORMATION));
    entry.OnShowSkin();
  } else if (pHexaModel != 0) {
    CHexaFormationEntry &entry = dynamic_cast<CHexaFormationEntry &>(*pHexaModel->GraphEntry(MD_HEXA_FORMATION));
    entry.OnShowSkin();
  } else if (pWellModel != 0) {
    CWellFormationEntry &entry = dynamic_cast<CWellFormationEntry &>(*pWellModel->GraphEntry(MD_WELLMODEL_FORMATION));
    entry.OnShowSkin();
  }
}

void CGeomecDoc::OnFormationcenter() {
  CTetraModel *pTetraModel = dynamic_cast<CTetraModel *>(Model());
  CHexaModel *pHexaModel = dynamic_cast<CHexaModel *>(Model());
  IWellModel *pWellModel = dynamic_cast<IWellModel *>(Model());

  if (pTetraModel != 0) {
    CTetraFormationEntry &entry = dynamic_cast<CTetraFormationEntry &>(*pTetraModel->GraphEntry(MD_TETRA_FORMATION));
    entry.OnShowMidpoints();
  } else if (pHexaModel != 0) {
    CHexaFormationEntry &entry = dynamic_cast<CHexaFormationEntry &>(*pHexaModel->GraphEntry(MD_HEXA_FORMATION));
    entry.OnShowMidpoints();
  } else if (pWellModel != 0) {
    CWellFormationEntry &entry = dynamic_cast<CWellFormationEntry &>(*pWellModel->GraphEntry(MD_WELLMODEL_FORMATION));
    entry.OnShowMidpoints();
  }
}

void CGeomecDoc::OnUpdateFormationfull(CCmdUI *pCmdUI) {
  CTetraModel *pTetraModel = dynamic_cast<CTetraModel *>(Model());
  CHexaModel *pHexaModel = dynamic_cast<CHexaModel *>(Model());
  IWellModel *pWellModel = dynamic_cast<IWellModel *>(Model());

  if (pTetraModel) {
    pCmdUI->Enable(TRUE);
    CTetraFormationEntry &entry = dynamic_cast<CTetraFormationEntry &>(*pTetraModel->GraphEntry(MD_TETRA_FORMATION));
    pCmdUI->SetCheck(entry.ShowFullMesh());
  } else if (pHexaModel) {
    pCmdUI->Enable(TRUE);
    CHexaFormationEntry &entry = dynamic_cast<CHexaFormationEntry &>(*pHexaModel->GraphEntry(MD_HEXA_FORMATION));
    pCmdUI->SetCheck(entry.ShowFullMesh());
  } else if (pWellModel != 0) {
    pCmdUI->Enable(TRUE);
    CWellFormationEntry &entry = dynamic_cast<CWellFormationEntry &>(*pWellModel->GraphEntry(MD_WELLMODEL_FORMATION));
    pCmdUI->SetCheck(entry.ShowFullMesh());
  } else {
    pCmdUI->SetCheck(FALSE);
    pCmdUI->Enable(FALSE);
  }
}

void CGeomecDoc::OnUpdateFormationskin(CCmdUI *pCmdUI) {
  CTetraModel *pTetraModel = dynamic_cast<CTetraModel *>(Model());
  CHexaModel *pHexaModel = dynamic_cast<CHexaModel *>(Model());
  IWellModel *pWellModel = dynamic_cast<IWellModel *>(Model());

  if (pTetraModel) {
    pCmdUI->Enable(TRUE);
    CTetraFormationEntry &entry = dynamic_cast<CTetraFormationEntry &>(*pTetraModel->GraphEntry(MD_TETRA_FORMATION));
    pCmdUI->SetCheck(entry.ShowSkin());
  } else if (pHexaModel) {
    pCmdUI->Enable(TRUE);
    CHexaFormationEntry &entry = dynamic_cast<CHexaFormationEntry &>(*pHexaModel->GraphEntry(MD_HEXA_FORMATION));
    pCmdUI->SetCheck(entry.ShowSkin());
  } else if (pWellModel != 0) {
    pCmdUI->Enable(TRUE);
    CWellFormationEntry &entry = dynamic_cast<CWellFormationEntry &>(*pWellModel->GraphEntry(MD_WELLMODEL_FORMATION));
    pCmdUI->SetCheck(entry.ShowSkin());
  } else {
    pCmdUI->SetCheck(FALSE);
    pCmdUI->Enable(FALSE);
  }
}

void CGeomecDoc::OnUpdateFormationcenter(CCmdUI *pCmdUI) {
  CTetraModel *pTetraModel = dynamic_cast<CTetraModel *>(Model());
  CHexaModel *pHexaModel = dynamic_cast<CHexaModel *>(Model());
  IWellModel *pWellModel = dynamic_cast<IWellModel *>(Model());

  if (pTetraModel) {
    pCmdUI->Enable(TRUE);
    CTetraFormationEntry &entry = dynamic_cast<CTetraFormationEntry &>(*pTetraModel->GraphEntry(MD_TETRA_FORMATION));
    pCmdUI->SetCheck(entry.ShowMidpoints());
  } else if (pHexaModel) {
    pCmdUI->Enable(TRUE);
    CHexaFormationEntry &entry = dynamic_cast<CHexaFormationEntry &>(*pHexaModel->GraphEntry(MD_HEXA_FORMATION));
    pCmdUI->SetCheck(entry.ShowMidpoints());
  } else if (pWellModel != 0) {
    pCmdUI->Enable(TRUE);
    CWellFormationEntry &entry = dynamic_cast<CWellFormationEntry &>(*pWellModel->GraphEntry(MD_WELLMODEL_FORMATION));
    pCmdUI->SetCheck(entry.ShowMidpoints());
  } else {
    pCmdUI->SetCheck(FALSE);
    pCmdUI->Enable(FALSE);
  }
}

QString CGeomecDoc::CurrentAppVersion() {
  CVersionInfo vinfo;
  CString str = "geomec.exe";
  vinfo.GetVersionInfo((char *)LPCTSTR(str));
  QString strAppVersion = vinfo.m_strFullVersion;

  return QString("GEOMEC ") + strAppVersion;
}

void CGeomecDoc::OnAttachModel(CFemAppModel &model, CFemAppGUI &gui) {
  CFemAppDoc::OnAttachModel(model, gui);

  CGUIBase *pGuiBase = dynamic_cast<CGUIBase *>(&gui);
  assert(pGuiBase);
  CurrentScene(const_cast<COpenInventorSceneNode *>(pGuiBase->getDefaultOpenInventorSceneNode()));

  ((CMainFrame *)FemAppGetMainWnd())->OnModelChanged(&model);

  QString strMatLibPathName = ISettings::instance()->getProfileString("Settings", "XMLMatLib", "");
  if (!strMatLibPathName.isEmpty()) {
    CMaterialEntry *pMaterialEntry = (CMaterialEntry *)model.GraphEntry(MD_ROCK_MATERIAL);
    assert(pMaterialEntry);
    pMaterialEntry->SetExternalMatLibFileName(strMatLibPathName, true);
  }

  // check temp path
  while (!DirExists(GetGeomecApp()->TempPath()->Path(CTempPath::TEMP_GENERAL))) {
    if (_m()->msg("The path for temporary files is not valid. Please correct the settings", MB_OKCANCEL) == IDCANCEL) {
      // QString path = GetGeomecApp()->TempPath()->Path(CTempPath::TEMP_GENERAL);
      QString strTempPath = GetGeomecTempPathExt(CTempPath::TEMP_GENERAL);
      if (strTempPath.isEmpty()) {
        _m()->msg("GEOMEC will not function properly until the path for temporary files has been corrected");
      } else {
        QString msg;

        QString strTempPathCalculation = GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION);
        if (strTempPath == strTempPathCalculation)
          msg = QString("GEOMEC will use '%1' as path for temporary files for this model").arg(strTempPath);
        else
          msg = QString("GEOMEC will use '%1' as path for general temporary files for this model, and '%2' as path for "
                        "temporary calculation files")
                    .arg(strTempPath)
                    .arg(strTempPathCalculation);
        _m()->msg(msg);
      }

      return;
    }

    OnAnalysisProperties(); // dlg to set path in 'TempPath'
  }
}

// Switch between existing models (don't delete anything, just switch the current context)
void CGeomecDoc::OnSwitchModel(CFemAppModel &model, CFemAppGUI &gui) {
  if (&model == Model())
    return; // nothing to do

  CFemAppDoc::OnSwitchModel(model, gui);

  if (dynamic_cast<CModelBase *>(&model)) {
    CModelBase *pModel = static_cast<CModelBase *>(&model);
    pModel->GetConsistencyGuard()->SwitchToModel(*pModel);

    CDerivedResultGroup &results_tree = pModel->ResultTree().RegistryResults();
    results_tree.detach();
    results_tree.LoadResultFromRegistry();
  }

  CGUIBase *pGuiBase = dynamic_cast<CGUIBase *>(&gui);
  assert(pGuiBase);

  UpdateAllViews(0, NEW_MODEL, (CObject *)((CurrentScene())));

  if (CurrentScene())
    CurrentScene(const_cast<COpenInventorSceneNode *>(pGuiBase->getDefaultOpenInventorSceneNode()));

  static_cast<CGUIBase &>(gui).LinkModelToOpenInventor();

  ((CMainFrame *)FemAppGetMainWnd())->OnModelChanged(&model);
}

bool CGeomecDoc::CanSelectComposite(const IValueComposite &composite) const {
  if (CurrentScene()) {

    if (const_cast<CGeomecDoc *>(this)->CurrentScene()->ValueComponent() == &composite.Component() ||
        const_cast<CGeomecDoc *>(this)->CurrentScene()->CanConnectItem(composite.Component()))
      return true;
  }
  return false;
}

bool CGeomecDoc::SelectComposite(const IValueComposite &composite) const {
  if (CurrentScene()) {
    size_t count = const_cast<CGeomecDoc *>(this)->CurrentScene()->NumberOfValueComponents();
    for (auto i = 0; i < count; ++i) {
      if (const_cast<CGeomecDoc *>(this)->CurrentScene()->ValueComponent(i) == &composite.Component())
        return true;
    }
  }
  return false;
}

void CGeomecDoc::SelectComposite(const IValueComposite &composite, bool bEnable) {
  if (CurrentScene()) {
    if (bEnable) {
      if (CurrentScene()->CanConnectItem(composite.Component()))
        CurrentScene()->ConnectItem(composite.Component());
    } else {
      if (CurrentScene()->IsLinkedTo(composite.Component()))
        CurrentScene()->UnLink(composite.Component());
    }
  }
}

bool CGeomecDoc::MultipleValueComponentsAllowed() {
  if (CurrentScene())
    return CurrentScene()->MultipleValueComponentsAllowed();

  return false;
}

bool CGeomecDoc::ChangeMode() const { return m_bChangeMode; }

void CGeomecDoc::EnterChangeMode() {
  assert(!m_bChangeMode);
  m_bChangeMode = true;
  UpdateAllViews(0, CURRENT_SCENE_MODIFIED);
}

void CGeomecDoc::ExitChangeMode() {
  assert(m_bChangeMode);
  m_stChangeSelection.clear();
  m_bChangeMode = false;
  UpdateAllViews(0, CURRENT_SCENE_MODIFIED);
}

void CGeomecDoc::ToggleChangeState(const CTreeNode &node) {
  if (m_stChangeSelection.find(&node) == m_stChangeSelection.end())
    m_stChangeSelection.insert(&node);
  else
    m_stChangeSelection.erase(&node);
  UpdateAllViews(0, CURRENT_SCENE_MODIFIED);
}

bool CGeomecDoc::IsChangeSelected(const CTreeNode &node) const {
  return m_stChangeSelection.find(&node) != m_stChangeSelection.end();
}

bool CGeomecDoc::HasChangeSelection() const { return !m_stChangeSelection.empty(); }

void CGeomecDoc::ChangeModeDeleteSelectedItems() {
  class CChangeSelectionNode : public CGraphNode {
  private:
    std::set<const CTreeNode *> &m_stSelection;
    typedef std::map<const CGraphNode *, const CTreeNode *> TNodeMap;
    TNodeMap m_mpNodes;

  public:
    CChangeSelectionNode(std::set<const CTreeNode *> &stSelection) : CGraphNode(""), m_stSelection(stSelection) {
      for (std::set<const CTreeNode *>::iterator it = m_stSelection.begin(); it != m_stSelection.end(); ++it) {
        LinkTo((*it)->Observer());
        m_mpNodes.insert(TNodeMap::value_type(&(*it)->Observer(), *it));
      }
    }

    virtual void OnNeighbourDeleted(const CGraphNode &node) {
      TNodeMap::iterator it = m_mpNodes.find(&node);
      if (it != m_mpNodes.end()) {
        const CTreeNode *pTreeNode = it->second;
        m_stSelection.erase(pTreeNode);
        m_mpNodes.erase(it);
      }
    }
  };

  CChangeSelectionNode csnode(m_stChangeSelection);

  while (!m_stChangeSelection.empty()) {
    const CTreeNode &treenode = **m_stChangeSelection.begin();
    m_stChangeSelection.erase(m_stChangeSelection.begin());
    (const_cast<CTreeNode &>(treenode)).Delete();
  }

  ExitChangeMode();
}

//
// this works, mcr 2020-10-28
//
void CGeomecDoc::ForceFileClose() {
  DeleteContents();
  SetTitle("Untitled");
  m_strPathName = "";
  m_app_version = "";
  ((CMainFrame *)FemAppGetMainWnd())->OnModelChanged(0);

  RefinementBox::m_translation = T3da{0, 0, 0};
}

void CGeomecDoc::OnFileClose() {
  assert(Model() != 0);

  if (SaveModified()) {
    DeleteContents();
    SetTitle("Untitled");
    m_strPathName = "";
    m_app_version = "";
    ((CMainFrame *)FemAppGetMainWnd())->OnModelChanged(0);
  }
  RefinementBox::m_translation = T3da{0, 0, 0};
}

void CGeomecDoc::OnUpdateFileClose(CCmdUI *pCmdUI) { pCmdUI->Enable(Model() != 0); }

void CGeomecDoc::OnUpdateAnalysisClearHistory(CCmdUI *pCmdUI) {
  CModelBase *pModel = static_cast<CModelBase *>(Model());
  pCmdUI->Enable(pModel != 0 && pModel->BranchState().IsBranch());
}

void CGeomecDoc::OnAnalysisClearHistory() {
  CModelBase *pModel = static_cast<CModelBase *>(Model());
  assert(pModel);
  assert(pModel->BranchState().IsBranch());

  if (_m()->msg(IDP_CONFIRMCLEARBRANCH, MB_YESNO) == IDYES) {
    pModel->ClearBranch();
  }
}

void CGeomecDoc::OnViewSmoothResults() {
  CModelBase *pModel = static_cast<CModelBase *>(Model());
  assert(pModel != 0);
  pModel->AverageResults(!pModel->AverageResults());

  if (CurrentScene()) {
    if (CurrentScene()->IsOpenInventorScene()) {
      COpenInventorSceneNode *oivScene = CurrentScene()->OpenInventorScene();
      if (oivScene && oivScene->ValueComponent())
        oivScene->ValueComponent()->Modified();
    } else if (CurrentScene()->IsOIV2DWellPathGraphScene()) {
      OIV2DScene *scene = CurrentScene()->getOIV2DScene();
      for (size_t i = 0; i < scene->NumberOfValueComponents(); ++i)
        scene->ValueComponent(i)->Modified();
    }
  }
}

void CGeomecDoc::OnUpdateViewSmoothResults(CCmdUI *pCmdUI) {
  if (Model() == 0) {
    pCmdUI->Enable(FALSE);
    return;
  }

  CModelBase *pModel = static_cast<CModelBase *>(Model());
  pCmdUI->SetCheck(pModel->AverageResults() ? 1 : 0);
}

void CGeomecDoc::OnStartGVT() {
  CModelBase *pModel = static_cast<CModelBase *>(Model());
  CGVTAnalysisDlg dlg(pModel->GVTSettings());
  dlg.DoModal();
}

void CGeomecDoc::OnUpdateGVTAnalysis(CCmdUI *pCmdUI) {
  if (!CGVTSettings::CanCalculate()) {
    pCmdUI->Enable(FALSE);
    return;
  }

  if (Model() == 0) {
    pCmdUI->Enable(FALSE);
    return;
  }

  CModelBase *pModel = static_cast<CModelBase *>(Model());

  pCmdUI->Enable(pModel->ResultRegister().ResultsAvailable() ? 1 : 0);
}

void CGeomecDoc::OnStartQB() {
  CModelBase *pModel = static_cast<CModelBase *>(Model());
  CQuickBlocksDlg dlg(pModel->QBSettings());
  dlg.DoModal();
}

void CGeomecDoc::OnUpdateQBAnalysis(CCmdUI *pCmdUI) {
  if (Model() == 0) {
    pCmdUI->Enable(FALSE);
    return;
  }

  CModelBase *pModel = static_cast<CModelBase *>(Model());
  pCmdUI->Enable(pModel->Mesh().IsMesh() ? 1 : 0);
}

// wjrx mantis 2975
CDataTreeView *CGeomecDoc::GetDataTreeView() {
  if (m_pDataTreeView)
    return m_pDataTreeView;

  POSITION pos = GetFirstViewPosition();
  while (pos != NULL && m_pDataTreeView == 0) {
    CView *pView = GetNextView(pos);
    m_pDataTreeView = dynamic_cast<CDataTreeView *>(pView);
  }

  return m_pDataTreeView;
}

// wjrx mantis 2975
CModelTreeView *CGeomecDoc::GetModelTreeView() {
  if (m_pModelTreeView)
    return m_pModelTreeView;

  POSITION pos = GetFirstViewPosition();
  while (pos != NULL && m_pModelTreeView == 0) {
    CView *pView = GetNextView(pos);
    m_pModelTreeView = dynamic_cast<CModelTreeView *>(pView);
  }

  return m_pModelTreeView;
}

CModelView *CGeomecDoc::GetModelView() {
  if (m_pModelView)
    return m_pModelView;

  POSITION pos = GetFirstViewPosition();
  while (pos != NULL && m_pModelView == 0) {
    CView *pView = GetNextView(pos);
    m_pModelView = dynamic_cast<CModelView *>(pView);
  }

  return m_pModelView;
}

COpenGLSplitView *CGeomecDoc::GetLegendModelView() {
  if (m_pLegendModelView)
    return m_pLegendModelView;

  POSITION pos = GetFirstViewPosition();
  while (pos != NULL && m_pLegendModelView == 0) {
    CView *pView = GetNextView(pos);
    m_pLegendModelView = dynamic_cast<COpenGLSplitView *>(pView);
  }

  return m_pLegendModelView;
}

void CGeomecDoc::OnInvalidateResultsAll() {
  if (_m()->msg("Are you sure you want to invalidate all results?", MB_ICONQUESTION | MB_YESNO) == IDYES) {
    CModelBase *pModel = static_cast<CModelBase *>(Model());
    if (pModel) {
      pModel->ResultRegister().ClearAll();
    }
  }
}

void CGeomecDoc::OnUpdateInvalidateResultsAll(CCmdUI *pCmdUI) {
  const CModelBase *pModel = static_cast<const CModelBase *>(Model());
  pCmdUI->Enable(pModel && pModel->ResultRegister().ResultsAvailable());
}

void CGeomecDoc::OnInvalidateResultsLinear() {
  if (_m()->msg("Are you sure you want to invalidate the linear results?", MB_ICONQUESTION | MB_YESNO) == IDYES) {
    CModelBase *pModel = static_cast<CModelBase *>(Model());
    if (pModel)
      pModel->ResultRegister().ClearLinear();
  }
}

void CGeomecDoc::OnUpdateInvalidateResultsLinear(CCmdUI *pCmdUI) {
  const CModelBase *pModel = static_cast<const CModelBase *>(Model());
  pCmdUI->Enable(pModel && pModel->ResultRegister().ResultsAvailable(CAnalysisType::AT_LINEAR));
}

void CGeomecDoc::OnInvalidateResultsNonlinear() {
  if (_m()->msg("Are you sure you want to invalidate the non-linear results?", MB_ICONQUESTION | MB_YESNO) == IDYES) {
    CModelBase *pModel = static_cast<CModelBase *>(Model());
    if (pModel)
      pModel->ResultRegister().ClearNonLinear();
  }
}

void CGeomecDoc::OnUpdateInvalidateResultsNonlinear(CCmdUI *pCmdUI) {
  const CModelBase *pModel = static_cast<const CModelBase *>(Model());
  pCmdUI->Enable(pModel && pModel->ResultRegister().ResultsAvailable(CAnalysisType::AT_NONLIN));
}

void CGeomecDoc::OnInvalidateResultsHeatFlow() {
  if (_m()->msg("Invalidating the heat flow results will also invalidate all dependent results.\nAre you sure you want "
                "to invalidate the heat flow results?",
                MB_ICONQUESTION | MB_YESNO) == IDYES) {
    CModelBase *pModel = static_cast<CModelBase *>(Model());
    if (pModel)
      pModel->ResultRegister().ClearHeat();
  }
}

void CGeomecDoc::OnUpdateInvalidateResultsHeatFlow(CCmdUI *pCmdUI) {
  const CModelBase *pModel = static_cast<const CModelBase *>(Model());
  pCmdUI->Enable(pModel && pModel->ResultRegister().ResultsAvailable(CAnalysisType::AT_HEAT));
}

void CGeomecDoc::OnInvalidateResultsMixture() {
  if (_m()->msg("Are you sure you want to invalidate the mixture results?", MB_ICONQUESTION | MB_YESNO) == IDYES) {
    CModelBase *pModel = static_cast<CModelBase *>(Model());
    if (pModel)
      pModel->ResultRegister().ClearMixture();
  }
}

void CGeomecDoc::OnUpdateInvalidateResultsMixture(CCmdUI *pCmdUI) {
  const CModelBase *pModel = static_cast<const CModelBase *>(Model());
  pCmdUI->Enable(pModel && pModel->ResultRegister().ResultsAvailable(CAnalysisType::AT_MIXTURE));
}

void CGeomecDoc::OnInvalidateResultsMixtureContainment() {
  if (_m()->msg("Are you sure you want to invalidate the mixture with containment results?",
                MB_ICONQUESTION | MB_YESNO) == IDYES) {
    CModelBase *pModel = static_cast<CModelBase *>(Model());
    if (pModel)
      pModel->ResultRegister().ClearMixtureContainment();
  }
}

void CGeomecDoc::OnUpdateInvalidateResultsMixtureContainment(CCmdUI *pCmdUI) {
  const CModelBase *pModel = static_cast<const CModelBase *>(Model());
  pCmdUI->Enable(pModel && pModel->ResultRegister().ResultsAvailable(CAnalysisType::AT_MIXTURE_CONTAINMENT));
}

void CGeomecDoc::OnFaultInitializationSettings() {
  CEditFaultSettings editFaultSettings(Model());

  editFaultSettings.DoModal();
}

void CGeomecDoc::OnUpdateFaultInitializationSettings(CCmdUI *pCmdUI) {
  pCmdUI->Enable((Model() != 0) && CEditFaultSettings::hasFaultFractures(Model()));
}

void CGeomecDoc::OnOptimizationSettings() {
  bool b_dsa_before = _g->dsa();
  COptimizationSettingsDlg dlg;

  dlg.DoModal();

  if (_g->dsa_available()) {
    _g->dsa(dlg.dsa());
    set_dsa_icon(_g->dsa() ? DSA_Icon::_2P : DSA_Icon::_1P);
  }
}

void CGeomecDoc::OnUpdateOptimizationSettings(CCmdUI *pCmdUI) { pCmdUI->Enable(Model() != 0); }

void CGeomecDoc::OnViewAutomaticallyDisableDrawing() {
  CModelBase *pModel = static_cast<CModelBase *>(Model());
  assert(pModel);
  pModel->AutomaticallyDisableDrawing(!pModel->AutomaticallyDisableDrawing());
  CGUIBase *pGUI = static_cast<CGUIBase *>(GUI());
  assert(pGUI);
  if (!pModel->AutomaticallyDisableDrawing()) {
    pGUI->AllowRedrawing();
    if (CurrentScene() && CurrentScene()->IsOpenInventorScene())
      CurrentScene()->SetDrawing();
  } else if (CurrentScene() && CurrentScene()->IsOpenInventorScene())
    CurrentScene()->SetNonDrawing();
}

void CGeomecDoc::OnUpdateViewAutomaticallyDisableDrawing(CCmdUI *pCmdUI) {
  const CModelBase *pModel = static_cast<const CModelBase *>(Model());
  pCmdUI->Enable(pModel != 0);

  if (pModel)
    pCmdUI->SetCheck(pModel->AutomaticallyDisableDrawing());
}

void CGeomecDoc::OnViewRefreshView() {
  CGUIBase *pModel = static_cast<CGUIBase *>(GUI());
  assert(pModel);
  pModel->AllowRedrawing();
}

void CGeomecDoc::OnUpdateViewRefreshView(CCmdUI *pCmdUI) {
  const CModelBase *pModel = static_cast<const CModelBase *>(Model());
  pCmdUI->Enable(pModel != 0); // && pModel->AutomaticallyDisableDrawing());
}

void CGeomecDoc::OnFileSaveModelWithAveragedResultsOnly() {
  CString strPath = GetPathName();
  if (!strPath.IsEmpty()) {
    int iDotPos = strPath.ReverseFind('.');
    CString strExtension;
    if (iDotPos != -1) {
      strExtension = strPath.Mid(iDotPos);
      strPath = strPath.Left(iDotPos);
    }

    strPath += "_smoothed" + strExtension;
  }

  CFileDialog dlg(FALSE, GEOMEC_DEFAULT_EXTENSION, strPath, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                  getStringTableEntry(IDS_FILE_SAVE_FILTER));
  if (dlg.DoModal() == IDOK) {
    CModelBase &model = static_cast<CModelBase &>(*Model());

    // trigger generation of smooth results first
    model.ResultRegister().TriggerSmoothedResults();

    if (model.ResultRegister().HasSmoothedResults()) // user may have cancelled smoothing operation
    {
      // save the model with smoothed results
      model.setSavingSmoothedResults(true);
      SaveModel(dlg.GetPathName());
      model.setSavingSmoothedResults(false);
    }
  }
}

void CGeomecDoc::OnUpdateFileSaveModelWithAveragedResultsOnly(CCmdUI *pCmdUI) { OnUpdateFileSaveAs(pCmdUI); }

void CGeomecDoc::OnUpdateFileModelsummary(CCmdUI *pCmdUI) {
  const CModelBase *pModel = static_cast<const CModelBase *>(Model());
  pCmdUI->Enable(pModel != 0);
}

void CGeomecDoc::OnFileModelsummary() {
  const CModelBase *pModel = static_cast<const CModelBase *>(Model());
  assert(pModel);
  CRichhInputSummaryDlg dlg(*pModel);
  dlg.DoModal();
}

void CGeomecDoc::OnViewStickToView() { StickToScene(!StickToScene()); }

void CGeomecDoc::OnUpdateViewStickToView(CCmdUI *pCmdUI) {
  pCmdUI->Enable(Model() != 0);
  if (Model())
    pCmdUI->SetCheck(StickToScene() ? 1 : 0);
}

void CGeomecDoc::OnExportCommandFilesWithDefaults() {
  if (Model()) {
    CModelBase &model = static_cast<CModelBase &>(*Model());
    model.ExportCommandFilesWithDefaults(!model.ExportCommandFilesWithDefaults());
  }
}

void CGeomecDoc::OnUpdateExportCommandFilesWithDefaults(CCmdUI *pCmdUI) {
  if (Model()) {
    CModelBase &model = static_cast<CModelBase &>(*Model());
    pCmdUI->SetCheck(model.ExportCommandFilesWithDefaults());
  } else {
    pCmdUI->Enable(FALSE);
  }
}

namespace {

#ifdef WIN64
QString SVS_PATH = GetAppPath() + "/SVS/SVS64.exe";
#else  // WIN32
QString SVS_PATH = GetAppPath() + "/SVS/SVS.exe";
#endif // _WIN64

} // anonymous namespace

void CGeomecDoc::OnExecuteSVS() {
  /*
   * Although system(...) does the job, it has the side-effect of opening a
   * dummy terminal from which the executable is launched.
   * Hence the choice for the non-portable solution ShellExecute(...).
   */

  ShellExecute(GetDesktopWindow(), "open", QDir::toNativeSeparators(SVS_PATH).toStdString().c_str(), NULL, NULL,
               SW_SHOWNORMAL);
}

void CGeomecDoc::OnUpdateExecuteSVS(CCmdUI *pCmdUI) { pCmdUI->Enable(QFile::exists(SVS_PATH)); }

void CGeomecDoc::OnExportLineData() {
  OIV2DWellPathGraphScene *oiv2DWellPathGraphScene = dynamic_cast<OIV2DWellPathGraphScene *>(SelectedNode());
  QString filter = "ASCII Files (*.dat)|*.dat|";
  CFileDialog fileDialog(FALSE, "dat", oiv2DWellPathGraphScene->Name().toStdString().c_str(),
                         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter.toStdString().c_str());

  if (fileDialog.DoModal() == IDOK) {
    QFile file(QString((LPCSTR)(fileDialog.GetPathName())));

    file.open(QFile::WriteOnly);

    QTextStream textStream(&file);

    textStream << oiv2DWellPathGraphScene->copyDataToString();

    file.close();
  }
}

void CGeomecDoc::OnUpdateExportLineData(CCmdUI *pCmdUI) {
  OIV2DWellPathGraphScene *oiv2DWellPathGraphScene = dynamic_cast<OIV2DWellPathGraphScene *>(SelectedNode());

  pCmdUI->Enable((oiv2DWellPathGraphScene != 0) && oiv2DWellPathGraphScene->canCopyDataToClipboard());
}

void CGeomecDoc::OnViewConvexHull() {
  if (Model()) {
    assert(dynamic_cast<CModelBase *>(Model()));
    CModelBase *pModel = static_cast<CModelBase *>(Model());
    pModel->ShowConvexHulls(!pModel->ShowConvexHulls());
    // There is no automatic update, as we need to relink pointsets (display size changes)
    // We could use
    // OnViewRefreshView();
    // here, or a derivation thereof that only processes pointsets, but it is not clear whether this is desirable
    // (as it may trigger constructing CHs for all of them) so for now we don't do anything.
    // The user can always click Refresh to trigger the refresh.
  }
}

void CGeomecDoc::OnUpdateViewConvexHull(CCmdUI *pCmdUI) {
  if (Model()) {
    assert(dynamic_cast<CModelBase *>(Model()));
    pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck(static_cast<CModelBase *>(Model())->ShowConvexHulls());
  } else
    pCmdUI->Enable(FALSE);
}

/////////////////////////////////////////////////////////////////////
//
//				multiple instances [Start]
//
/////////////////////////////////////////////////////////////////////

void CGeomecDoc::OnSetDefault() {
  std::string c1, c2; // caption
  QString v = gm::doc_::version();
  if (v == gm::helper::version::read()) {
    gm::helper::version::unset();
    c1 = "Mark as NOT default version";
    c2 = "Mark as default version";
  } else {
    gm::helper::version::set(v);
    c1 = "Mark as default version";
    c2 = "Mark as NOT default version";
  }

  CItem item({"&Settings"}, c1);
  if (item.exist())
    item.rename(c2);
}

/////////////////////////////////////////////////////////////////////
//
//				multiple instances [End]
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//
//				plugins [Start]
//
/////////////////////////////////////////////////////////////////////

void CGeomecDoc::OnRunModel() {
  m_bRunning = true;
  RunAnalysis(CAnalysisType::AT_NONLIN, false, false);
  m_bRunning = false;
};
// OnRunStep works, except that it doesn't show the Diana window (we turned that off somewhere for the commandline
// version)
void CGeomecDoc::OnRunStep() { CRunGm::instance()->step(); /*OnRunModel();*/ };

void CGeomecDoc::OnWrongPythonroot() {
  bool res = CPlugins::instance()->provide_pythonroot(CPlugins::instance()->plugins_path());
  if (res)
    QMessageBox::warning(nullptr, QString("PYTHONROOT"),
                         QString("Now PYTHONROOT is correct.\n\nLaunch again the plugin please."));
};

void Launch_Py_Task_MFC(std::string name, IGmInterface *gm_iface, IPyInterface *py_iface) {
  class PyTask_MFC : public PyTask {

  public:
    PyTask_MFC(std::string name_, IGmInterface *gm_iface_, IPyInterface *py_iface_)
        : PyTask(name_, gm_iface_, py_iface_) {};

    virtual tbb::task *execute() {
      int res = py_iface->launch(name, gm_iface); // Do long computation

      delete gm_iface;
      delete py_iface;

      if (res) // error
        AfxGetApp()->GetMainWnd()->PostMessage(WM_COMMAND, ID_ON_WRONG_PYTHONROOT, 0);

      return NULL;
    }
  };

  PyTask_MFC *t = new (tbb::task::allocate_root()) PyTask_MFC(name, gm_iface, py_iface);
  tbb::task::enqueue(*t);
}

void CGeomecDoc::OnPlugins_Launch(UINT nID) {
  int plugin_idx = nID - (WM_USER + CItem::Launch);

  printer->info("plugin: launch: %s", CPlugins::instance()->get()[plugin_idx].c_str());

  if (plugin_idx < CPlugins::instance()->size()) {
    std::string name = CPlugins::instance()->name(plugin_idx);

    if (name.find(PLUGIN_NAME_IT) != std::string::npos) // FIXME: HC
    {
      printer->info("plugin: launch: ' IT SPECIFIC IMPLEMENTATION'");
      // if( Model() )
      {
        GeomecPythonInterface *gm_iface_it = new GeomecPythonInterface();
        PyInterface *py_iface = new PyInterface(); // IT_Interface();

        Launch_IT_Events_Handler_Task(gm_iface_it);
        // Launch_Py_Task( name, gm_iface_it, py_iface );
        Launch_Py_Task_MFC(name, gm_iface_it, py_iface);

        int a = 0;
      }
      /*else
      {
      AfxMessageBox("Model not loaded");
      }*/
    } else // TODO: To implement
    {
      printer->info("plugin: launch: 'GENERIC PLUGIN' - WORKING ON");

      /*std::string item_name = "Remove";
      //std::string item_name = "GUI";
      //std::string item_name = "Inversion tool";
      printer->debug("find item: %s", item_name.c_str());
      CItem item_ = CItem::item( item_name );
      bool exist = item_.exist();
      if( exist )
      {
      item_.rename("TEST");
      item_.disable_MENU_NOT_ITEM();
      }*/

      IGmInterface *gm_iface = new GmInterfaceGui(); // FIXME
      PyInterface *py_iface = new PyInterface();

      if (gm_iface && py_iface)
        Launch_Py_Task(name, gm_iface, py_iface);
    }
  } else {
    printer->error("-> %s | nID: %d", __FUNCTION__, nID);
  }
}

void CGeomecDoc::OnPlugins_Add() {
  static const CString PLUGIN_INSTALLER_EXT = "zip";
  static const CString PLUGIN_FILE_OPEN_FILTER = "*.zip|*.*";

  CFileDialog dlg(TRUE, PLUGIN_INSTALLER_EXT, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, PLUGIN_FILE_OPEN_FILTER,
                  FemAppGetMainWnd());

  if (dlg.DoModal() == IDOK) {
    std::string cfg_file = (LPCTSTR)dlg.GetPathName();
    bool res = CPlugins::instance()->install(cfg_file);
    if (res) {
      CItem::refresh(CItem::TPathV{CPlugins::DEF_PLUGINS_SUB_MENU_NAME}, CPlugins::instance()->get(), CItem::Launch);
      CItem::refresh(CItem::TPathV{"Plugins", "Remove"}, CPlugins::instance()->get(), CItem::Remove);
      CItem::refresh({"&Help"}, CPlugins::instance()->get(), CItem::Manual);
    } else {
      printer->error("-> %s", __FUNCTION__);
    }
  }
}

void CGeomecDoc::OnPlugins_Remove_Update(CCmdUI *pCmdUI) {
  // printer->debug("-> %s | nID: %d", __FUNCTION__, pCmdUI->m_nID );
}

void CGeomecDoc::OnPlugins_Manual_Update(CCmdUI *pCmdUI) {
  int idx = pCmdUI->m_nID - (WM_USER + CItem::Manual);

  bool enable = CPlugins::instance()->has_manual(idx);

  // printer->debug("-> %s | nID: %d | idx: %d (%s) | enable: %d ", __FUNCTION__, pCmdUI->m_nID, idx,
  // CPlugins::instance()->get()[ idx ].c_str(), enable );

  pCmdUI->Enable(enable);
}

void CGeomecDoc::OnPlugins_Remove(UINT nID) {
  int plugin_idx = nID - (WM_USER + CItem::Remove);

  printer->info("plugin: remove: '%s'", CPlugins::instance()->get()[plugin_idx].c_str());

  if (plugin_idx < CPlugins::instance()->size()) {
    bool res = CPlugins::instance()->uninstall(plugin_idx);
    if (res) {
      CItem::refresh(CItem::TPathV{CPlugins::DEF_PLUGINS_SUB_MENU_NAME}, CPlugins::instance()->get(), CItem::Launch);
      CItem::refresh(CItem::TPathV{"Plugins", "Remove"}, CPlugins::instance()->get(), CItem::Remove);
      CItem::refresh({"&Help"}, CPlugins::instance()->get(), CItem::Manual);
    }
  } else {
    printer->error("-> %s | nID: %d", __FUNCTION__, nID);
  }
}

void CGeomecDoc::OnPlugins_Manual(UINT nID) {
  int plugin_idx = nID - (WM_USER + CItem::Manual);

  printer->info("plugin: manual: '%s'", CPlugins::instance()->get()[plugin_idx].c_str());

  if (plugin_idx < CPlugins::instance()->size()) {
    if (CPlugins::instance()->has_manual(plugin_idx)) {
      CMainFrame *pMainFrame = (CMainFrame *)(FemAppGetMainWnd());

      std::string plugin_manual = CPlugins::instance()->manual(plugin_idx);

      pMainFrame->OpenPdfDocument(plugin_manual.c_str(), ExePath().c_str());
    } else {
      printer->error("plugin: %s -> HAS NO MANUAL", CPlugins::instance()->get()[plugin_idx]);
    }
  } else {
    printer->error("-> %s | nID: %d", __FUNCTION__, nID);
  }
}

/////////////////////////////////////////////////////////////////////
//
//				plugins [End]
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//
//				run analysis [Start]
//
/////////////////////////////////////////////////////////////////////

//
// pre: m_ra exists
//
//
// FIXME: ra
//
void CGeomecDoc::update_views() {
  printer->info("update views");

  //
  // crashing here: races
  //
  CRunAnalysis *ra = m_eh->ra();

  if (!ra || !ra->context()) {
    printer->error("update views");
    return;
  }

  DSA_Context *c = ra->context();

  COpenInventorSceneNode *pOIVScene = c->scene.main;
  OIV2DHistoryScene *pOIV2DHistoryScene = c->scene.history;
  OIV2DWellPathGraphScene *pOIV2DWellPathGraphScene = c->scene.wellpath;

  if (pOIVScene) {
    CurrentScene(pOIVScene);

    UpdateAllViews(0, SWITCH_TO_OPENINVENTOR, (CObject *)CurrentScene());
  } else if (pOIV2DWellPathGraphScene) {
    CurrentScene(new OIV2DWellPathGraphSceneWrapper(*pOIV2DWellPathGraphScene));
    UpdateAllViews(0, SWITCH_TO_OPENINVENTOR, (CObject *)CurrentScene());
  } else if (pOIV2DHistoryScene) {
    CurrentScene(new OIV2DHistorySceneWrapper(*pOIV2DHistoryScene));
    UpdateAllViews(0, SWITCH_TO_OPENINVENTOR, (CObject *)CurrentScene());
  }
}

//
// post: ra context relative to views updated
//
//
// FIXME: ra
//
void CGeomecDoc::disable_views() {
  printer->info("disable views");

  COpenInventorSceneNode *pOIVScene = nullptr;
  OIV2DWellPathGraphScene *pOIV2DWellPathGraphScene = nullptr;
  OIV2DHistoryScene *pOIV2DHistoryScene = nullptr;

  ISceneWrapper *pCurrentSceneWrapper = CurrentScene();

  if (pCurrentSceneWrapper) {
    bool bDeactivate = false;

    if (pCurrentSceneWrapper->IsOpenInventorScene()) {
      pOIVScene = pCurrentSceneWrapper->OpenInventorScene();

      if (pOIVScene) {
        pOIVScene->ResetRenderAction();
        bDeactivate = true;
      }
    } else if (pCurrentSceneWrapper->IsOIV2DWellPathGraphScene()) {
      pOIV2DWellPathGraphScene = dynamic_cast<OIV2DWellPathGraphScene *>(pCurrentSceneWrapper->getOIV2DScene());

      if (pOIV2DWellPathGraphScene) {
        bDeactivate = true;
      }
    } else if (pCurrentSceneWrapper->IsOIV2DHistoryScene()) {
      pOIV2DHistoryScene = dynamic_cast<OIV2DHistoryScene *>(pCurrentSceneWrapper->getOIV2DScene());

      if (pOIV2DHistoryScene) {
        bDeactivate = true;
      }
    }

    if (bDeactivate) {
      // Inactivate original scene type
      CurrentScene((ISceneWrapper *)0);
      UpdateAllViews(0, NEW_CURRENT_SCENE, 0);
    }
  }

  // save context
  CRunAnalysis *ra = m_eh->ra();
  if (!ra || !ra->context())
    return;

  _e->broadcast(DisableViews_Done);

  ra->context()->scene = {pOIVScene, pOIV2DHistoryScene, pOIV2DWellPathGraphScene};
}

void CGeomecDoc::handle(Cmd cmd) {
  eCmd cmd_id = (eCmd)cmd.first;
  // if(cmd_id==DianaMsg)
  //	printer->debug("handle: %s", _cmd_s(cmd_id));

  //
  // Filter progress messages when cancelling
  //
  // this is now doen differently, these two events are unregistered after CancelRun
  //
  if (_g->status() != CGlobal::Running && (cmd_id == DianaMsg || cmd_id == Progress)) {
    // m_printer->info("BLOQUED !!!");
    return;
  }

  AfxGetApp()->GetMainWnd()->PostMessage(ID_ON_EVENT, (WPARAM)cmd.first, (LPARAM)cmd.second);
}

LRESULT CGeomecDoc::on_event(WPARAM p1, LPARAM p2) {
  // eCmd cmd_id = (eCmd)p1;
  // if(cmd_id==DianaMsg)
  //	printer->debug("on event: %s", _cmd_s(cmd_id));

  m_eh->handle(make_cmd(p1, (void *)p2));

  return 0;
}

/////////////////////////////////////////////////////////////////////
//
//				run analysis [End]
//
/////////////////////////////////////////////////////////////////////

// global function to get the active document
CGeomecDoc *GetGeomecDoc() {
  POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
  CDocTemplate *pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
  POSITION docpos = pDocTemplate->GetFirstDocPosition();
  CGeomecDoc *pDoc = dynamic_cast<CGeomecDoc *>(pDocTemplate->GetNextDoc(docpos));

  return pDoc;
}