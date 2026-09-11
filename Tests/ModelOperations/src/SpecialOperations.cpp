#include "ModelOperations.h"
#include "ModelBase.h"
#include "HexaModel.h"
#include "GeomecUtils.h"
#include "ResultRegister.h"
#include "ResultCache.h"
#include "MeshDataCacher.h"
#include "ExportResultData.h"
#include "ExportResults.h"
#include "BaseEntryTypes.h"
#include "FormationBase.h"
#include "MaterialResultTree.h"
#include "GeomecModelVisitor.h"
#include "LocalResult.h"
#include "HexaFormation.h"
#include "ExportResultData.h"
#include "ExportResultDataEntry.h"
#include "InterfaceBoundary.h"

#include "HexaMesh.h"
#include "HexaEntryTypes.h"
#include "hexameshregion.h"
#include "TetraEntryTypes.h"
#include "TetraFormation.h"

#include "DianaStartUp.h"
#include "IDCDianaRunner.h"
//std
#include <fstream>
//qt
#include <QDir>

#include "StorageInterface.h"
#include "QUtil.h"
#include "RunAnalysis_CLI.h" // FIXME: includes DianaRunController
#include "Global.h"
#include "Environment.h"

#include "DianaExecuter.h" // for dxw_and_mc41 test

//
// ModelOperations.Listeners
//
#include "IListener.h"
#include "IListenerIpc.h"
#include "Heartbeat.h"
#include "Wait.h"
//
// ModelOperations.DianaAppFailure
//
#include "DianaInterface.h"
//
//
//
#include "GlobalMessage.h"
//
//
#include "ModelLifetimeFacade.h" // Rxport.Linear

//
// ModelOprationsExport
//
// disabled warning: 4701
#include <boost/crc.hpp>  // for boost::crc_32_type -> Export tests
#include <QByteArray> // md5
#include <QCryptographicHash> // checksum 


// Contains tests that are very specific (nuts & bolts) and unlikely to gain much from being scripted

#ifdef _WIN32
#define PATH_DELIM  "\\"
#else
#define PATH_DELIM  "/"
#endif

namespace
{
  Printer* printer = Printer::instance(Printer::Tests);
}

namespace test_model_operations
{


int LibraryLoad_SpecialModelOperations() { return 0; }


bool checkMDCLengths(CResultCache& resultCache, int expectedLen, int expectedMax, bool checkAveraged = false)
{
  mdc::CMeshDataCacherTestAccessor accessor(checkAveraged ? *resultCache.AveragedCacher() : resultCache.RawCacher());
  return accessor.Length() == expectedLen && accessor.MaxLength() == expectedMax;
}

TEST_F(ModelOperations, ResultCache) {
  QString sModelFile = m_ModelsPath + "TestResultCache.gm4";

  CModelBase* pModel = test_lib::TestLib::loadModel(sModelFile, &m_Logger);

  EXPECT_TRUE(pModel);

  //
  // RUN MODEL : 1
  //
  EXPECT_EQ(0, test_lib::TestLib::runModel(pModel, sModelFile));

  // This model has 240 results
  // Setting cache size to 1 allows 128 results to be in memory

  CResultCache *pCache = &pModel->ResultRegister().Cache();

  int numCols = pCache->NumColumns() / 5; // only interested in nonlin (until we have the others conveniently available)

  EXPECT_TRUE(checkMDCLengths(*pCache, 240, 0));

  pCache->switchRawCacher(true, 1, true);

  EXPECT_TRUE(checkMDCLengths(*pCache, 128, 128));

  pCache->switchRawCacher(true, 2, true);

  EXPECT_TRUE(checkMDCLengths(*pCache, 128, 256));

  pCache->switchRawCacher(false, 0);

  EXPECT_TRUE(checkMDCLengths(*pCache, 128, 0));

  for (size_t c = 0; c < numCols; ++c)
  pCache->RawCacher().Value(0, c);

  EXPECT_TRUE(checkMDCLengths(*pCache, 300, 0));

  pCache->switchRawCacher(true, 1, true);

  EXPECT_TRUE(checkMDCLengths(*pCache, 128, 128));

  //
  // RUN MODEL : 2
  //

  test_lib::TestLib::runModel(pModel, sModelFile);

  EXPECT_TRUE(checkMDCLengths(pModel->ResultRegister().Cache(), 128, 128)); // not sure where the 112 is coming from

  const int maxChecksumSize = 128;

  uchar *checksums_written = new uchar[numCols * maxChecksumSize]; // 300 columns, and we reserve 128 bytes for the checksum
  uchar *checksums_read = new uchar[maxChecksumSize];

  memset(checksums_written, 0, numCols * maxChecksumSize);
  memset(checksums_read, 0, maxChecksumSize);

  int checksumsOK = 0;
  int checksumsSame = 0;

  for (size_t c = 0; c < numCols; ++c)
  {
  if (pCache->RawCacher().ChecksumOK(c))
      ++checksumsOK;

  pCache->RawCacher().GetChecksum(c, &checksums_written[c * maxChecksumSize]);
  }

  EXPECT_EQ(numCols, checksumsOK);

  QString sNewModelPath = GetGeomecTempPathExt(CTempPath::TEMP_GENERAL) + PATH_DELIM + "temp.gm4";

  test_lib::TestLib::saveModel(*pModel, sNewModelPath);

  test_lib::TestLib::closeModel(&pModel);

  // checksums should remain the same across save/loads; let's try a few
  // keep in mind that we don't write all blocks in save model, only for the indices that are valid
#ifdef DEBUG_RESULTCACHE_TEST
  std::ofstream f;
  f.open("E:\\Temp\\test_log.txt");
#endif

  for (int i = 0; i < 4; ++i)
  {
#ifdef DEBUG_RESULTCACHE_TEST
  f << "Run " << i << std::endl;
#endif

  pModel = test_lib::TestLib::loadModel(sNewModelPath, &m_Logger);
  pCache = &pModel->ResultRegister().Cache();

#ifdef DEBUG_RESULTCACHE_TEST
  const char *d0 = pCache->RawCacher().StorageInterface()->Directory();

  f << "Directory (loaded) '" << (d0 ? d0 : "") << "'" << std::endl;
#endif

  if (i % 2)
      pCache->switchRawCacher(true, 1, true);

#ifdef DEBUG_RESULTCACHE_TEST
  const char *d1 = pCache->RawCacher().StorageInterface()->Directory();
  f << "Directory (current) '" << (d1 ? d1 : "") << "'" << std::endl;

  if (d1)
  {
      std::vector<QString> files;
      Glob(d1, "mdc", files);
      for (int fi = 0; fi < files.size(); ++fi)
    f << "  " << files[fi].toStdString() << std::endl;
  }
#endif

  EXPECT_TRUE(pModel);

  checksumsOK = 0;
  checksumsSame = 0;

  for (size_t c = 0; c < numCols; ++c)
  {
      int nStatus = -2;
      if (pCache->RawCacher().ChecksumOK(c, &nStatus) && nStatus != 15)
      {
    ++checksumsOK;

    memset(checksums_read, 0, maxChecksumSize);

    pCache->RawCacher().GetChecksum(c, checksums_read);

#ifdef DEBUG_RESULTCACHE_TEST
    int same = 0;
#endif

    if (!memcmp(&checksums_written[c * maxChecksumSize], checksums_read, maxChecksumSize) != 0)
    {
          ++checksumsSame;
#ifdef DEBUG_RESULTCACHE_TEST
          ++same;
#endif
    }

#ifdef DEBUG_RESULTCACHE_TEST
    f << "Checksum " << c << " 1 " << same << " " << nStatus << std::endl;
#endif
      }
#ifdef DEBUG_RESULTCACHE_TEST
      else f << "Checksum " << c << " 0 0 " << nStatus << std::endl;
#endif
  }

  EXPECT_EQ(240, checksumsOK);
  EXPECT_EQ(240, checksumsSame);

#ifdef DEBUG_RESULTCACHE_TEST
  f << "TOTAL " << checksumsOK << " " << checksumsOK << std::endl << std::endl;
#endif

  test_lib::TestLib::saveModel(*pModel, sNewModelPath);

  test_lib::TestLib::closeModel(&pModel);
  }

#ifdef DEBUG_RESULTCACHE_TEST
  f.close();
#endif

  // checksums should be different if we re-run the model
  pModel = test_lib::TestLib::loadModel(sNewModelPath, &m_Logger);
  pCache = &pModel->ResultRegister().Cache();

  EXPECT_TRUE(pModel);

  //
  // RUN MODEL : 3
  //

  EXPECT_EQ(0, test_lib::TestLib::runModel(pModel, sModelFile));

  checksumsOK = 0;
  checksumsSame = 0;

  for (size_t c = 0; c < numCols; ++c)
  {
  if (pCache->RawCacher().ChecksumOK(c))
  {
      ++checksumsOK;

      pCache->RawCacher().GetChecksum(c, checksums_read);

      if (!memcmp(&checksums_written[c * maxChecksumSize], checksums_read, maxChecksumSize))
    ++checksumsSame;
  }
  }

  EXPECT_EQ(240, checksumsOK);
  EXPECT_EQ(0, checksumsSame);

  delete[] checksums_written;
  delete[] checksums_read;

  test_lib::TestLib::closeModel(&pModel);

  QFile::remove(sNewModelPath);
}





class CollectMaterialResults : public IGeomecModelVisitor
{
  TResultComponentSet& results;

public:
  CollectMaterialResults(TResultComponentSet& results) : results(results) {}

  virtual bool VisitMaterialResult(CMaterialResult& matresult)
  {
  TResultComponent component(&matresult, 0);
  results.insert(component);

  return true;
  }

  virtual bool VisitResultGroup(CResultGroup& group)
  {
  for (size_t i = 0; i < group.ChildSize(); ++i)
      group.childAt(i).Accept(*this);
  
  return true;
  }

  virtual bool VisitDefault(CGraphNode& /*node*/)
  {
  return true;
  }
};


TEST_F(ModelOperations, ExportResults) {
  QString sBase = "TestLoadAndCalculate";

  QString sModelFile = m_ModelsPath + sBase + ".gm4";

  QString sExportFile = GetGeomecTempPathExt(CTempPath::TEMP_GENERAL) + PATH_DELIM + sBase + ".dat";

  QString sExportOrigin = m_ModelsPath + sBase + ".dat";

  CModelBase* pModel = test_lib::TestLib::loadModel(sModelFile, &m_Logger);

  EXPECT_TRUE(pModel);
  EXPECT_EQ(0, test_lib::TestLib::runModel(pModel, sModelFile));

  CExportResultData exportResultData(*pModel);

  const TFormationBaseEntry* pEntry = static_cast<const TFormationBaseEntry*>(pModel->GraphEntry(MD_BASE_FORMATION));
  const TFormationBaseEntry::TSortedNodeSet& stNodes = pEntry->SortedEntryNodes();

  for (TFormationBaseEntry::TSortedNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
  const CFormationBase& formation = **it;

  exportResultData.m_selection.insert(formation);
  }


  CollectMaterialResults collectMaterialResults(exportResultData.m_MatRGObserverData);

  pModel->MaterialResultTree().Accept(collectMaterialResults);

  exportResultData.m_bNonLinear = true;
  exportResultData.m_nLinear = 1;
  exportResultData.m_nCenterPoints = 1;
  exportResultData.m_nFormationNames = 1;

  exportResultData.m_pTimeStep = &pModel->LastDepletionStage();

  const CDepletionStage *stage = &pModel->InitialDepletionStage();
  while (stage)
  {
  exportResultData.m_stTimeStep.insert(stage);

  stage = stage->Last() ? 0 : &stage->Next();
  }

  CExportResults exportResults(exportResultData, *pModel);

  exportResults.onExport(sExportFile, false, false);

  EXPECT_TRUE(QFile::exists(sExportFile));
  
  EXPECT_TRUE(test_lib::FileCompare::IdenticalTokens(sExportFile.toStdString(), sExportOrigin.toStdString()));

  test_lib::TestLib::closeModel(&pModel);

  QFile::remove(sExportFile);
}


TEST_F(ModelOperations, ResultsBranched) {
  QString sBase = "ResultsBranched";

  // We need absolute path; in the tests we usually have --gm-model-path set to "../Models" and that won't work for saving .ff/.gm5
  QString sModelFile = QDir::cleanPath(QDir(".").absoluteFilePath(m_ModelsPath)) + PATH_DELIM + sBase + ".gm5";
  QString sExportMacroLib = m_ModelsPath + sBase + ".rmx";

  CModelBase* pModel = test_lib::TestLib::loadModel(sModelFile, &m_Logger);

  EXPECT_TRUE(pModel);
  EXPECT_EQ(0, test_lib::TestLib::runModelWithSave(pModel, sModelFile));

  EXPECT_TRUE(QFile::exists(m_ModelsPath + sBase + "_D3.ff"));
  EXPECT_TRUE(QFile::exists(m_ModelsPath + sBase + "_D3.gm5"));
  EXPECT_TRUE(QFile::exists(m_ModelsPath + sBase + "_D4.ff"));
  EXPECT_TRUE(QFile::exists(m_ModelsPath + sBase + "_D4.gm5"));

  test_lib::TestLib::TMacros macros;
  test_lib::TestLib::importResultMacroLibrary(macros, pModel, sExportMacroLib);

  for (test_lib::TestLib::TMacros::iterator macro = macros.begin(); macro != macros.end(); ++macro)
  {
  if ((*macro)->ReservoirMacro())
  {
      continue;
  }
  (*macro)->bind();

  CExportResults exportResults(*(*macro), *pModel);

  QString sExportOrigin = m_ModelsPath + (*macro)->Name() + ".dat";
  QString sExportFile = GetGeomecTempPathExt(CTempPath::TEMP_GENERAL) + PATH_DELIM + (*macro)->Name() + ".dat";

  exportResults.onExport(sExportFile, false, false);

  (*macro)->unbind(false);

  EXPECT_TRUE(QFile::exists(sExportFile));

  EXPECT_TRUE(test_lib::FileCompare::IdenticalTokens(sExportFile.toStdString(), sExportOrigin.toStdString()));

  QFile::remove(sExportFile);
  }

  test_lib::TestLib::closeModel(&pModel);

  QFile::remove(m_ModelsPath + sBase + "_D3.ff");
  QFile::remove(m_ModelsPath + sBase + "_D3.gm5");
  QFile::remove(m_ModelsPath + sBase + "_D4.ff");
  QFile::remove(m_ModelsPath + sBase + "_D4.gm5");
}


TEST_F(ModelOperations, ExportResultMacro)
{
  QString sBase = "ALotAndALittle";
  QString sModelFile = m_ModelsPath + sBase + ".gm4";
  QString sExportMacroLib = m_ModelsPath + sBase + ".rmx";

  CModelBase* pModel = test_lib::TestLib::loadModel(sModelFile, &m_Logger);

  EXPECT_TRUE(pModel);

  test_lib::TestLib::TMacros macros;
  test_lib::TestLib::importResultMacroLibrary(macros, pModel, sExportMacroLib);

  EXPECT_EQ(3, macros.size());

  int nSkipped = 0;

  for (test_lib::TestLib::TMacros::iterator macro = macros.begin(); macro != macros.end(); ++macro)
  {
  if ((*macro)->ReservoirMacro())
  {
      ++nSkipped;
      continue;
  }
  (*macro)->bind();

  CExportResults exportResults(*(*macro), *pModel);

  QString sExportOrigin = m_ModelsPath + (*macro)->Name() + ".dat";
  QString sExportFile = GetGeomecTempPathExt(CTempPath::TEMP_GENERAL) + PATH_DELIM + (*macro)->Name() + ".dat";

  exportResults.onExport(sExportFile, false, false);

  (*macro)->unbind(false);

  EXPECT_TRUE(QFile::exists(sExportFile));

  EXPECT_TRUE(test_lib::FileCompare::IdenticalTokens(sExportFile.toStdString(), sExportOrigin.toStdString()));

  QFile::remove(sExportFile);
  }

  EXPECT_EQ(1, nSkipped);
  
  test_lib::TestLib::closeModel(&pModel);
}

TEST_F(ModelOperations, ReservoirMacro)
{
  QString sBase = "ReservoirMacro";
  QString sModelFile = m_ModelsPath + sBase + ".gm5";
  CModelBase* pModel = test_lib::TestLib::loadModel(sModelFile, &m_Logger);

  EXPECT_TRUE(pModel);

  test_lib::TestLib::TMacros macros;
  test_lib::TestLib::importResultMacroLibrary(macros, pModel, QString());

  EXPECT_EQ(1, macros.size());

  int nSkipped = 0;
  int nFound = 0;

  std::set<const CFormationBase *> reservoirs;

  for (test_lib::TestLib::TMacros::iterator macro = macros.begin(); macro != macros.end(); ++macro)
  {
  if (!(*macro)->ReservoirMacro())
  {
      ++nSkipped;
      continue;
  }
  (*macro)->bind();
  for (COpenGLNodeSelection::iterator node = (*macro)->m_selection.begin(); node != (*macro)->m_selection.end(); ++node)
  {
      if (dynamic_cast<const CFormationBase *>(*node))
      {
    reservoirs.insert(static_cast<const CFormationBase *>(*node));
      }
  }
  (*macro)->unbind(false);

  for (int e = 0; e < static_cast<CHexaModel *>(pModel)->Mesh().Mesh().ElementSize(); ++e)
  {
      const geo::IElement& element = static_cast<CHexaModel *>(pModel)->Mesh().Mesh().Element(e);

      if (!element.IsInterfaceElement())
      {
    const CFormationBase* formationBase = static_cast<CHexaModel *>(pModel)->Mesh().Formation(element);
    if (reservoirs.find(formationBase) != reservoirs.end())
          ++nFound;
      }
  }

  }

  EXPECT_EQ(0, nSkipped);
  EXPECT_EQ(1, reservoirs.size());
  EXPECT_TRUE(reservoirs.size() > 0 && (*reservoirs.begin())->Name() == "Constant depth at 100 m");
  EXPECT_EQ(64, nFound);

  test_lib::TestLib::closeModel(&pModel);
}


TEST_F(ModelOperations, HDF5Test)
{
  QString sBase = "HDF5Test";
  QString sModelFile = m_ModelsPath + sBase + ".gm4";
  QString sModelFile4 = GetGeomecTempPathExt(CTempPath::TEMP_GENERAL) + PATH_DELIM + sBase + ".gm4";
  QString sModelFile5 = GetGeomecTempPathExt(CTempPath::TEMP_GENERAL) + PATH_DELIM + sBase + ".gm5";
  QString sExportMacroLib = m_ModelsPath + sBase + ".rmx";

  CModelBase* pModel = test_lib::TestLib::loadModel(sModelFile, &m_Logger);

  EXPECT_TRUE(pModel);

  EXPECT_EQ(0, test_lib::TestLib::runModel(pModel, sModelFile, CAnalysisType::AT_LINEAR));
  EXPECT_EQ(0, test_lib::TestLib::runModel(pModel, sModelFile)); // nonlin

  test_lib::TestLib::saveModel(*pModel, sModelFile4);
  test_lib::TestLib::saveModel(*pModel, sModelFile5);

  test_lib::TestLib::closeModel(&pModel);

  pModel = test_lib::TestLib::loadModel(sModelFile4, &m_Logger);

  test_lib::TestLib::TMacros macros;
  test_lib::TestLib::importResultMacroLibrary(macros, pModel, sExportMacroLib);

  EXPECT_EQ(2, macros.size());

  int nSkipped = 0;

  for (test_lib::TestLib::TMacros::iterator macro = macros.begin(); macro != macros.end(); ++macro)
  {
  if ((*macro)->ReservoirMacro())
  {
      ++nSkipped;
      continue;
  }
  (*macro)->bind();

  CExportResults exportResults(*(*macro), *pModel);

  QString sExportOrigin = m_ModelsPath + (*macro)->Name() + ".dat";
  QString sExportFile = GetGeomecTempPathExt(CTempPath::TEMP_GENERAL) + PATH_DELIM + (*macro)->Name() + "_4.dat";

  exportResults.onExport(sExportFile, false, false);

  (*macro)->unbind(false);

  EXPECT_TRUE(QFile::exists(sExportFile));

  EXPECT_TRUE(test_lib::FileCompare::IdenticalTokens(sExportFile.toStdString(), sExportOrigin.toStdString()));

  QFile::remove(sExportFile);
  }

  EXPECT_EQ(1, nSkipped);

  nSkipped = 0;
  macros.clear();

  test_lib::TestLib::closeModel(&pModel);


  pModel = test_lib::TestLib::loadModel(sModelFile5, &m_Logger);

  test_lib::TestLib::importResultMacroLibrary(macros, pModel, sExportMacroLib);

  EXPECT_EQ(2, macros.size());

  for (test_lib::TestLib::TMacros::iterator macro = macros.begin(); macro != macros.end(); ++macro)
  {
  if ((*macro)->ReservoirMacro())
  {
      ++nSkipped;
      continue;
  }
  (*macro)->bind();

  CExportResults exportResults(*(*macro), *pModel);

  QString sExportOrigin = m_ModelsPath + (*macro)->Name() + ".dat";
  QString sExportFile = GetGeomecTempPathExt(CTempPath::TEMP_GENERAL) + PATH_DELIM + (*macro)->Name() + "_5.dat";

  exportResults.onExport(sExportFile, false, false);

  (*macro)->unbind(false);

  EXPECT_TRUE(QFile::exists(sExportFile));

  EXPECT_TRUE(test_lib::FileCompare::IdenticalTokens(sExportFile.toStdString(), sExportOrigin.toStdString()));

  QFile::remove(sExportFile);
  }

  EXPECT_EQ(1, nSkipped);

  nSkipped = 0;
  macros.clear();

  test_lib::TestLib::closeModel(&pModel);

  QFile::remove(sModelFile4);
  QFile::remove(sModelFile5);
}


TEST_F(ModelOperations, HexaMeshRefinementToEdge)
{
  // Model contains a mesh with a meshregion up to the edge. Top and bottom edges have been saved as pointsets, and we're going to compare them with generated edges.

  QString sBase = "MeshRefinement";
  QString sModelFile = m_ModelsPath + sBase + ".gm5";

  CModelBase* pModel = test_lib::TestLib::loadModel(sModelFile, &m_Logger);

  EXPECT_TRUE(pModel);

  TPointSetEntry *pPSEntry = static_cast<TPointSetEntry *>(pModel->GraphEntry(MD_BASE_POINTSET));

  EXPECT_TRUE(pPSEntry);

  std::vector<geo::CPoint> vcPoints[2];

  TPointSetEntry::TNodeSet stNodes = pPSEntry->EntryNodes();

  for (TPointSetEntry::TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
  int i = -1;
  if ((*it)->Name() == "boundary_surface_0")
      i = 0;
  else if ((*it)->Name() == "boundary_surface_1")
      i = 1;

  if (i == -1)
      continue;

  for (int j = 0; j < (*it)->PointSize(); ++j)
      vcPoints[i].push_back((*it)->PointAt(j));
  }

  for (int i = 0; i < 2; ++i)
  std::sort(vcPoints[i].begin(), vcPoints[i].end());

  const CInterfaceBoundary& boundary = static_cast<const CInterfaceBoundary&>(pModel->Boundary());

  std::vector<const geo::ISurface *> vcSideSurfaces;
  const C3DHorizon *horizons[] = { &boundary.GetTopHorizon(), &boundary.GetBottomHorizon() };

  for (size_t i = 0; i < 2; ++i)
  {
  for (size_t j = 0; j < horizons[i]->MeshedSurfaceSize(); ++j)
  {
      vcSideSurfaces.push_back(&horizons[i]->MeshedSurface(j));
  }
  }

  EXPECT_TRUE(vcSideSurfaces.size() == 2);

  for (size_t i = 0; i < vcSideSurfaces.size(); ++i)
  {
  std::vector<std::vector<const geo::IPoint *> > vcSortedEdgePoints = vcSideSurfaces[i]->SortedEdgePoints();

  if (vcSortedEdgePoints.size() == 0)
  {
      double epsilon[] = { 0.0001, 0.001, 0.01 };

      for (int j = 0; j < 3 && vcSortedEdgePoints.size() != 1; ++j)
      {
    vcSortedEdgePoints = vcSideSurfaces[i]->SortedEdgePoints(true, epsilon[j]);
      }
  }

  EXPECT_TRUE(vcSortedEdgePoints.size() == 1);

  std::vector<const geo::IPoint *> vcpEdgePoints = vcSortedEdgePoints[0];

  std::vector<geo::CPoint> vcEdgePoints;
  vcEdgePoints.reserve(vcpEdgePoints.size());

  for (std::vector<const geo::IPoint *>::iterator it = vcpEdgePoints.begin(); it != vcpEdgePoints.end(); ++it)
      vcEdgePoints.push_back(**it);

  std::sort(vcEdgePoints.begin(), vcEdgePoints.end());

  EXPECT_TRUE(vcEdgePoints == vcPoints[i]);
  }

  test_lib::TestLib::closeModel(&pModel);
}


TEST_F(ModelOperations, HexaMeshRefinement)
{
  QString sBase = "HexaMeshRefinement";
  QString sModelFile = m_ModelsPath + sBase + ".gm5";
  QString sExportMacroLib = m_ModelsPath + sBase + ".rmx";

  CModelBase* pModel = test_lib::TestLib::loadModel(sModelFile, &m_Logger);

  EXPECT_TRUE(pModel);

  test_lib::TestLib::TMacros macros;
  test_lib::TestLib::importResultMacroLibrary(macros, pModel, sExportMacroLib);

  EXPECT_EQ(2, macros.size());

  CHexaMesh *pHexaMesh = dynamic_cast<CHexaMesh *>(&pModel->Mesh());

  EXPECT_TRUE(pHexaMesh);

  if (pHexaMesh->CurrentMesher() == "")
  pHexaMesh->SwitchMesher("NextGen");

  for (int i = 0; i < 2; ++i)
  {
  pModel->CreateMesh();

  const geo::IMesh& mesh = pHexaMesh->Mesh();

  QString sTyings = m_ModelsPath + sBase + "_tyings_" + QString::number(i);

  std::fstream fTyings(sTyings.toStdString() + ".txt", std::fstream::out | std::fstream::trunc);

  // check tyings
  for (int j = 0; j < mesh.TyingSize(); ++j)
  {
      const geo::CTying& tying = mesh.Tying(j);

      fTyings << tying.SlaveIndex() << " " << tying.SlaveTypeString() << " (" << tying.SlaveDirection().X() << " " << tying.SlaveDirection().Y() << " " << tying.SlaveDirection().Z() << ")" << std::endl;
      for (int k = 0; k < tying.MasterSize(); ++k)
    fTyings << "  " << tying.MasterIndex(k) << " " << tying.MasterTypeString(k)
        << " (" << tying.MasterDirection(k).X() << " " << tying.MasterDirection(k).Y() << " " << tying.MasterDirection(k).Z() << ") "
        << " " << tying.MasterFactor(k) << std::endl;
  }

  fTyings.close();

  EXPECT_TRUE(test_lib::FileCompare::IdenticalText(sTyings.toStdString() + ".txt", sTyings.toStdString() + "_ref.txt"));

  EXPECT_EQ(0, test_lib::TestLib::runModel(pModel, sModelFile));

  // check results
  for (test_lib::TestLib::TMacros::iterator macro = macros.begin(); macro != macros.end(); ++macro)
  {
      if ((*macro)->ReservoirMacro())
    continue;
      (*macro)->bind();

      CExportResults exportResults(*(*macro), *pModel);

      QString sExportOrigin = m_ModelsPath + "HexaMeshRefinement_" + (*macro)->Name() + "_" + QString::number(i) + ".dat";
      QString sExportFile = GetGeomecTempPathExt(CTempPath::TEMP_GENERAL) + PATH_DELIM + (*macro)->Name() + ".dat";

      exportResults.onExport(sExportFile, false, false);

      (*macro)->unbind(false);

      EXPECT_TRUE(QFile::exists(sExportFile));

      EXPECT_TRUE(test_lib::FileCompare::IdenticalTokens(sExportFile.toStdString(), sExportOrigin.toStdString(), 1E-2));

      QFile::remove(sExportFile);
  }

  // change grid of mesh region (to try in next loop iteration)
  pModel->InvalidateMesh();

  CHexaMeshRegionEntry *pEntry = (CHexaMeshRegionEntry*)(pModel->GraphEntry(MD_HEXA_MESH_ZONE));
  CHexaMeshRegionEntry::TNodeSet stNode = pEntry->EntryNodes();

  for (CHexaMeshRegionEntry::TNodeSet::iterator itMeshRegion = stNode.begin(); itMeshRegion != stNode.end(); ++itMeshRegion)
  {
      if (dynamic_cast<CHexaSubMeshRegion *>(*itMeshRegion))
      {
    CHexaSubMeshRegion *region = static_cast<CHexaSubMeshRegion *>(*itMeshRegion);
    if (region->GridSizeX() == 200)
    {
          int expX = region->ExpX();
          int expY = region->ExpY();

          region->SetSubGrid(expY, expX);
          break;
    }
      }
  }

  }

  test_lib::TestLib::closeModel(&pModel);
}


TEST_F(ModelOperations, FGVRemeshVolumeAssignment)
{
  QString sBase = "FGVRemeshVolumeAssignment";
  QString sModelFile = m_ModelsPath + sBase + ".gm5";

  CModelBase* pModel = test_lib::TestLib::loadModel(sModelFile, &m_Logger);

  EXPECT_TRUE(pModel);

  pModel->GraphEntry(MD_BASE_FORMATION);
  CTetraFormationEntry *pFormationEntry = dynamic_cast<CTetraFormationEntry *>(pModel->GraphEntry(MD_TETRA_FORMATION));
  assert(pFormationEntry);
  CTetraFormationEntry::TNodeSet stFormation = pFormationEntry->EntryNodes();

  typedef std::map<CTetraFormationVolume *, std::pair<QString, TGraphNodeSet> > TVolumeSet;
  TVolumeSet stVolumeBefore;
  for (CTetraFormationEntry::TNodeSet::iterator it = stFormation.begin(); it != stFormation.end(); ++it)
  {
  CFormationBase& formation = **it;
  for (int i = 0; i < formation.ElementSetSize(); ++i)
  {
      CTetraFormationVolume *pVolume = static_cast<CTetraFormationVolume *>(&formation.ElementSet(i));
      stVolumeBefore.insert(std::make_pair(pVolume, std::make_pair(formation.Name(), pVolume->Identifier())));
  }
  }

  pModel->CreateMesh();

  EXPECT_EQ(pFormationEntry->EntryNodes().size(), stFormation.size());

  int no_match = 0;
  int ok = 0;

  for (CTetraFormationEntry::TNodeSet::iterator it = stFormation.begin(); it != stFormation.end(); ++it)
  {
  CFormationBase& formation = **it;
  for (int i = 0; i < formation.ElementSetSize(); ++i)
  {
      CTetraFormationVolume *pVolume = static_cast<CTetraFormationVolume *>(&formation.ElementSet(i));

      TVolumeSet::iterator before = stVolumeBefore.find(pVolume);

      if (before == stVolumeBefore.end())
    ++no_match;
      else if (before->second.first != formation.Name())
    ++no_match;
      else if (pVolume->Identifier().size() != before->second.second.size())
    ++no_match;
      else if ((int)pVolume->Identifier().size() != pVolume->Resemblance(before->second.second))
    ++no_match;
      else
    ++ok;
  }
  }

  EXPECT_EQ(0, no_match);
  EXPECT_EQ((int)stVolumeBefore.size(), ok);

  test_lib::TestLib::closeModel(&pModel);
}

//////////////////////////////////////////////////////////////////////
//
//
//				(ModelOperationsDsa, StartDiana)
//
//
//////////////////////////////////////////////////////////////////////

namespace test{
namespace diana{
  //
  // copies FF to a tmp dir
  //
  QString prepare_FF(const QString& path_, const QString& FF )
  {
    QString FFDIR = CreateTempDirExt( CTempPath::TEMP_GENERAL);
  
    QString path = path_;
    path.chop( 1 );

    bool res1 = QFile::copy( 
      QUtil::url( path, FF ), 
      QUtil::url( FFDIR, FF ) );

    //
    // read and write all
    //
    QFileDevice::Permissions permissions = 
      QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther |		
      QFile::WriteOwner | QFile::WriteUser | QFile::WriteGroup | QFile::WriteOther;

    bool res2 = QFile::setPermissions( QUtil::url( FFDIR, FF ), permissions );

    return FFDIR;
  };
  //
  // sets basoc diana envvars
  //
  void prepare_ENV(const QString& FFDIR, const QString& FF, const QString& BASE )
  {
    CDianaStartUp* dsu = CDianaStartUp::instance();

    dsu->SetDianaEnv( CDianaStartUp::FFDIR, FFDIR );
    dsu->SetDianaEnv( CDianaStartUp::FF, FF );

    //QString FFDIR_ = dsu->GetDianaEnv( CDianaStartUp::FFDIR );
    //QString FF_ = dsu->GetDianaEnv( CDianaStartUp::FF );

    //printer->info("FFDIR: %s ... FF: %s", FFDIR.toStdString().c_str(), FF.toStdString().c_str());

    //
    // vars
    //
    //no need
    //dsu->var( CDianaStartUp::DISPLAY, "yes" );
    dsu->var( CDianaStartUp::BASE, BASE );
  }
  QString prepare(const QString& path_, const QString& FF, const QString& BASE)
  {
    //
    // 1) create tmp dir and copy FF to it 
    //
    QString FFDIR = test::diana::prepare_FF( path_, FF );
    //
    // 2) set diana envvars
    //
    test::diana::prepare_ENV( FFDIR, FF, BASE );

    return FFDIR;
  };

} // namespace diana
} // namespace test
//
// it runs diana directly
// it doesn't use controller, runner and dxw (CDianaXWrapper, CDianaExecuter)
//
TEST_F(ModelOperationsDsa, StartDiana)
{
  // 'test_output.trx' file is created at the end, when all test has been passed
  // DIAPATH, DIASHARE, DIALIB, DIASLIB have been set in main's SetDianaEnvironment
  // next no need:
  // dsu->SetDianaEnv(); //DIAPATH, STP0 and others

  CDianaStartUp* dsu = CDianaStartUp::instance();

  QString FFDIR = test::diana::prepare( m_ModelsPath, "test.ff", "DIANA" );

  //
  // this fails: const char*
  //
  //const char* FF_ = dsu->GetDianaEnv( CDianaStartUp::FF ).toStdString().c_str();
  //QString FF__ = dsu->GetDianaEnv( CDianaStartUp::FF );
  //
  // it can't return a const char*, it's no reliable !!!
  // doing this it also gives problems below -> dia::IDianaRunner::GetResultInfo
  //
  auto get=[=](const QString& k){return dsu->GetDianaEnv( k ).toStdString();}; 
  printer->info("FFDIR: %s ... FF: %s", get(CDianaStartUp::FFDIR).c_str(), get(CDianaStartUp::FF).c_str());

  int result = 1; //error

  if( _g->dsa() )
  {
    CRunAnalysis_CLI ra;
    ra.wait();
    result = ra.res() ? 0:1;
  }
  else
  {
    result = dsu->RunDiana();
  }

  EXPECT_EQ(0, result);

  EXPECT_TRUE( QFile::exists( QUtil::url( FFDIR, dsu->var( CDianaStartUp::BASE ) + ".out" ) ) );

  bool bHaveResults, bCalculationResult;

  dia::IDianaRunner::GetResultInfo(
    get(CDianaStartUp::FFDIR).c_str(), 
    get(CDianaStartUp::FF).c_str(), 
    bHaveResults, 
    bCalculationResult);

  EXPECT_TRUE(bCalculationResult);
  EXPECT_TRUE(bHaveResults);

  QDir( FFDIR ).removeRecursively();
}

//////////////////////////////////////////////////////////////////////
//
//
//				(ModelOperationsDsa, dxw_and_mc41)
//
//
//////////////////////////////////////////////////////////////////////

TEST_F(ModelOperationsDsa, dxw_and_mc41)
{
  CDianaStartUp* dsu = CDianaStartUp::instance();
  //
  QString FFDIR = test::diana::prepare( m_ModelsPath, "Calib.ff", "Calib" );
  //
  auto prepare_ENV_MORE=[=](QString& STP0)
  {
#ifdef WIN32
    STP0 = dsu->GetDianaEnv( CEnvironment::DIAPATH ) + "/binseg/ap/" + "mc41.exe";
#else
    STP0 = dsu->GetDianaEnv( CEnvironment::DIAPATH ) + "/binseg/ap/" + "mc41";
#endif

    dsu->SetDianaEnv( CEnvironment::STP0, STP0 ); // no standard

    // vars
    dsu->var( CDianaStartUp::DISPLAY, "yes" );
    dsu->var( CDianaStartUp::USER_LEADER, "" );
    dsu->var( CDianaStartUp::USER_TRAILER, "" );
  };
  QString STP0;
  prepare_ENV_MORE(STP0);
  //
  // it's not worth to use a lambda here because of the problems returning const char*
  //
  auto get=[=](const QString& k){return dsu->GetDianaEnv( k ).toStdString();};
  printer->info("FFDIR: %s ... FF: %s ... STP0: %s", get( CDianaStartUp::FFDIR ).c_str(), get( CDianaStartUp::FF ).c_str(), get( CDianaStartUp::STP0 ).c_str() );
  //
  //
  //
  int result = 1; //error

  IDianaXWrapper* dxw = new CDianaExecuter();
  if( _g->dsa() )
  {
    CRunAnalysis_CLI ra(dxw);
    ra.wait();
    result = ra.res() ? 0:1;
  }
  else
  {
    result = dxw->ExecuteDiana();
  }

  EXPECT_EQ(0, result);

  EXPECT_TRUE( QFile::exists( QUtil::url( FFDIR, dsu->var( CDianaStartUp::BASE ) + ".out" ) ) );

  bool res = dxw->GetCalculationResult();
  EXPECT_TRUE(res);

  delete dxw;
  //
  //
  //
  QDir( FFDIR ).removeRecursively();
}

//////////////////////////////////////////////////////////////////////
//
//
//				(ModelOperationsIpc, Listeners)
//
//
//////////////////////////////////////////////////////////////////////

namespace test{
namespace listener{

static const int HB_TIMEOUT = 100;
static const int TIMES = 100;

class Sender : public IListener, public IListenerIpc
{
  HB m_hb;

public:
  Sender()
    :	IListener(CEvents::DI_EH, {HeartBeat}), // emitted by hb obj
      IListenerIpc(IListenerIpc::Server)
  {
    m_hb.hb_timeout(HB_TIMEOUT);
  }

  void start(){ m_hb.push_(Start_HB); }

protected:
  virtual void handle(Cmd cmd)
  {
    eCmd cmd_id = (eCmd)cmd.first;
    if(cmd_id ==HeartBeat)
      push_ipc( cmd );
  };
  virtual void received(Cmd cmd) //ipc
  {
    eCmd cmd_id = (eCmd)cmd.first;
    if (cmd_id == HeartBeat_Ack)
      m_hb.push_(HeartBeat_Ack);
  };
};

class Receiver : public IListener, public IListenerIpc
{
  CWait m_wait;

  int m_times;

public:
  Receiver(const std::string& token)
    :	IListener(CEvents::GI_EH, {}), 
      IListenerIpc(IListenerIpc::Client,token),
      m_wait(CEvents::GI_EH),
      m_times(0)
  {
  };
  void wait(){m_wait.wait();}
  int times(){return m_times;}

protected:
  virtual void handle(Cmd cmd)
  {
    eCmd cmd_id = (eCmd)cmd.first;
    if(cmd_id ==HeartBeat)
    {
      if(m_times==TIMES)
      {
        push_ipc( Quit_HB ); 
        m_wait.signal();
      }
      else
      {
        push_ipc( HeartBeat_Ack ); 
        m_times++;
      }
    }
  };
  virtual void received(Cmd cmd) //ipc
  {
    push(cmd);
  };
};

} //namespace listener
} //namespace test

//
// send and receive 100x times a heartbeat command
//
// FIXME: race conditions, mcr 2020-10-07
//	emit an event when resources are available instead of just set a delay and trust it will be enought
//	this is too risky and not determinstic
//
TEST_F(ModelOperationsIpc, Listeners)
{
  using namespace test::listener;

  Sender s;
  //
  std::this_thread::sleep_for(std::chrono::milliseconds( 100 )); // wait for shm to be instantiated before be accessed by Receiver
  //
  Receiver r(s.token());
  //
  std::this_thread::sleep_for(std::chrono::milliseconds( 100 ));  // wait till receiver threads are ready
  //
  s.start();

  r.wait(); // released by receiver after receiving 100 messages

  EXPECT_EQ(100, r.times());
}

//////////////////////////////////////////////////////////////////////
//
//
//				(ModelOperationsIpc, Wait)
//
//
//////////////////////////////////////////////////////////////////////

//
// TODO: move this test to somewhere else as this is not a model specific test
//
TEST_F(ModelOperationsIpc, Wait)
{
  CWait w(CEvents::Tests_EH);

  static const int TIMEOUT = 1000;
  static const int TIME_TO_SIGNAL = 200;

  auto func=[](CWait* w = nullptr, int timme_to_signal=0)
  {
    if(!w)
      return;

    std::this_thread::sleep_for( std::chrono::milliseconds( timme_to_signal ) );
    w->signal();
  };

  ////////////////////////////
  //
  // 1) exit by signal
  //
  ////////////////////////////
  std::thread th( func, &w, TIME_TO_SIGNAL );
  bool res = w.wait( TIMEOUT );
  EXPECT_TRUE( res ); // exit by signal 
  //
  if( th.joinable() ) // is this needed? -> YES, otherwise crash
    th.join();

  ////////////////////////////
  //
  // 2) exit by timeout
  //
  ////////////////////////////
  th = std::thread( func );
  res = w.wait( TIMEOUT );
  EXPECT_TRUE( !res ); // exit by timeout
  //
  if( th.joinable() )
    th.join();
}


//////////////////////////////////////////////////////////////////////
//
//
//				(ModelOperationsIpc, Heartbeat)
//
//
//////////////////////////////////////////////////////////////////////

namespace test{
namespace hb{

static const int HB_TIMEOUT = 100;
static const int TIMES = 5;

class Workflow : public IListener
{
  CWait m_wait;
  HB m_hb;
  int m_count;

public:
  Workflow() 
  :	IListener(CEvents::Tests_EH, {HeartBeat,HeartBeat_Error}), 
    m_wait(CEvents::Tests_EH),
    m_count(0)
  {
    m_hb.hb_timeout( test::hb::HB_TIMEOUT );
    m_hb.push_( Start_HB );
  }

  virtual void handle(Cmd cmd)
  {
    eCmd cmd_id = (eCmd)cmd.first;

    switch(cmd_id)
    {
      case HeartBeat:
      {
        if( m_count == test::hb::TIMES )
          break;

        std::this_thread::sleep_for( std::chrono::milliseconds( test::hb::HB_TIMEOUT / 2 ) );
        m_hb.push_(HeartBeat_Ack);
        m_count++;
        break;
      }
      case HeartBeat_Error:
      {
        m_wait.signal();
        break;
      }
      default:
        break;
    }
  }

  bool wait(int timeout=0){return m_wait.wait(timeout);}
  int count(){return m_count;}
};

} // namespace hb
} // namespace test

//
// TODO: move this test to somewhere else as this is not a model specific test
//
TEST_F(ModelOperationsIpc, Heartbeat)
{
  const int timeout = 2*1000;

  test::hb::Workflow wf; // start hb, sends TIMES ack and then stops to force hb sending HeartBeat_Error

  bool res = wf.wait(timeout);
  //
  EXPECT_TRUE( res ); // exit not by timeout -> in case HeartBeat and/or HeartBeat_Error not received -> to avoid a deadlock
  //
  EXPECT_TRUE( wf.count() == test::hb::TIMES ); // hb error after tic happens 'TIMES' times
}

//////////////////////////////////////////////////////////////////////
//
//
//				(ModelOperationsDsa, KillDianaApp)
//
//
//////////////////////////////////////////////////////////////////////

//
// keeps on listening... when 'RunDiana_Ack_' received, it sends 'QuitDiana' to DI for it to forward it to diana_app in order to terminate the app
//
class Terminator : public IListener
{

public:
  Terminator() : IListener(CEvents::Tests_EH, {RunDiana_Ack_}){}

protected:
  virtual void handle(Cmd cmd)
  {
    eCmd cmd_id = (eCmd)cmd.first;
    if(cmd_id ==RunDiana_Ack_)
    {
      //
      // FIXME: could lead to a race condition with small models whose execution lasts less tha 100 ms.
      //
      std::this_thread::sleep_for(std::chrono::milliseconds( 100 ));
      //
      _e->notify( QuitDiana, CEvents::DI_EH );
    }
  };
};

//
// dsa-only
//
TEST_F(ModelOperationsDsa, KillDianaApp)
{
  if( !_g->dsa() )
    return;

  Terminator t;

  CDianaStartUp* dsu = CDianaStartUp::instance();

  //
  // 1-2) prepare
  //
  QString FFDIR = test::diana::prepare( m_ModelsPath, "test.ff", "DIANA" );
  //
  //printer->info("FFDIR: %s ... FF: %s", FFDIR.toStdString().c_str(), FF.toStdString().c_str());
  //
  // 3) run diana
  //
  CRunAnalysis_CLI ra; // it does: dsu->RunDiana();
  //
  ra.wait();
  //
  // 4) assess results
  //
  EXPECT_TRUE( ra.quit() );
  //
  // 5) delete tmp dir
  //
  QDir( FFDIR ).removeRecursively();
}

//////////////////////////////////////////////////////////////////////
//
//
//				(ModelOperationsDsa, DianaAppFailure, DianaAppSuccess)
//
//
//////////////////////////////////////////////////////////////////////

namespace test{
namespace di{

//
// TODO: use the real wf instead ? -> no, otherwise I can't access DI to override 'received' method
//
// for code clarity it does not:
//	- kill diana
//  - delete DI 
//	- delete tmp dir
// all this is done in the test 'main'
//
class Workflow : public IListener
{
  CWait m_wait;
  eCmd m_cmd;

public:
  Workflow() 
  :	IListener(CEvents::Tests_EH, {
      RunError, // the only one needed
      //
      // just to avoid error logs
      //
      DianaMsg, // memory need to be free
      //
      RunDiana_Ack_,
      RunSuccess,
      HeartBeat_Ack}), 
    m_cmd(DoNothing),
    m_wait(CEvents::Tests_EH) // param just for logs purpose
  {}

  virtual void handle(Cmd cmd)
  {
    eCmd cmd_id = (eCmd)cmd.first;

    switch(cmd_id)
    {
      case RunError:
      {
        m_cmd=cmd_id;
        //
        m_wait.signal();
        break;
      }
      case DianaMsg:
      {
        free(cmd.second);
        break;
      }
      case RunSuccess: // send RunEnd and the execute teh code below -> but we do all in one to simplify
      {
        m_cmd=cmd_id;
        //
        push_(RunEnd);
        break;
      }
      case RunEnd:
      {
        m_wait.signal();
        break;
      }
      case HeartBeat_Ack:
      case RunDiana_Ack_:
      default:
        break;
    }
  }
  bool wait(int timeout=0){return m_wait.wait(timeout);}
  eCmd cmd(){return m_cmd;}
};

//
// same as DI but skipping HeartBeat_Ack and producing RunError
//
class DI_ : public DI
{
public:
  DI_(){}
  virtual void received(Cmd cmd)
  {
    eCmd cmd_id = (eCmd)cmd.first;

    switch(cmd_id)
    {
      //
      // as 'HeartBeat_Ack' is not received, 'RunError' event is triggered
      //
      case HeartBeat_Ack:
        break;
      default:
        DI::received(cmd);
        break;
    }
  }
};

} // namespace di
} // namespace test

//
// hb timeout needs to be forced instead of let it take the one in geomec.ini
//
// also tests:
//	- killing diana_app and its children processes: 'gm42.exe' and 'conhst.exe' (so far windows-only)
//
// diana_app shouldn't need to be killed as in principle no ack received means diana_app failure
//	-> but in order to make a clean test
//		- diana_app is killed
//		- run diana tmp folder is deleted
//
// FIXME: 2) when process finishes Ok
//


template< class C >
void run_di(const QString& path, eCmd cmd )
{
  //
  // protection
  //
  static_assert(std::is_base_of<DI, C>::value, "C must derive from DI");

  //
  // dsa-only
  //
  if( !_g->dsa() )
    return;

  static const int HB_TIMEOUT = 500;
  static const int WF_TIMEOUT = 10*1000;

  test::di::Workflow wf; // waits for 'RunError'/'RunSuccess' events

  CDianaStartUp* dsu = CDianaStartUp::instance();

  QString FFDIR = test::diana::prepare( path, "test.ff", "DIANA" );
  //
  // DI constructor launches diana_app
  //
  // on LaunchDiana_Ack_ received heartbeat mechanism is started (keep alive)
  //
  C* di_ = new C(); // overrides DI::reecived() to skip HeartBeat_Ack and trigger 'RunError' event
  //
  //dsu->di(di_); // -> not needed as delete di is done here and not by the wk obj
  //
  // here it could be a race condition but is not probable as timeout starts after LaunchDiana_Ack_ is sent from diana_app and treated by DI
  //
  di_->hb_timeout( HB_TIMEOUT ); // forces timeout and misses info in geomec.ini
  //
  bool res = wf.wait( WF_TIMEOUT ); // stops execution till process finishes
  EXPECT_TRUE( res );
  //
  _e->notify(QuitDiana,CEvents::DI_EH);
  //
  delete di_;
  //
  EXPECT_TRUE( wf.cmd() == cmd );
  //
  QDir( FFDIR ).removeRecursively();

};

TEST_F(ModelOperationsDsa, DianaAppFailure)
{
  run_di< test::di::DI_ >( m_ModelsPath, RunError );
}

//
// FIXME: still crashing in ~HB deleting m_th that is nullptr
//
TEST_F(ModelOperationsDsa, DianaAppSuccess)
{
  //
  // FIXME: temporarily disabled 
  //
  //run_di< DI >( m_ModelsPath, RunSuccess );
}

//////////////////////////////////////////////////////////////////////
//
//
//				(ModelOperationsGui, GUI)
//
//
//////////////////////////////////////////////////////////////////////

namespace test{
namespace gui{

static const int HB_TIMEOUT = 1000;

//
// GeomecInterface
//
class GI : public IListener, public IListenerIpc
{
  CWait m_wait;
  HB m_hb; // no need to be HB2 as it runs in a separated process

public:
  GI()
    :	IListener(CEvents::Tests_EH, {HeartBeat}), // emitted by hb obj
      IListenerIpc(IListenerIpc::Server),
      m_wait(CEvents::Tests_EH),
      m_hb(CEvents::HB_EH,CEvents::Tests_EH) // id + notify_to
  {
    m_hb.hb_timeout(test::gui::HB_TIMEOUT);
  }

  void start_hb(){ m_hb.push_(Start_HB); }

protected:
  virtual void handle(Cmd cmd)
  {
    auto msg=[](Cmd cmd)
    {
      void* data_ = cmd.second;
      if(!data_)
        return;

      std::string data = *reinterpret_cast< std::string* >(data_);
      delete data_;

      QString msg = "gm test interface feedback : " + QString::fromStdString( data );
      printer->info( msg.toStdString().c_str() );

      //_m()->msg( QString::fromStdString( "+ "+data ) );
      std::cout << ( "+ "+data ) << std::endl;
    };

    eCmd cmd_id = (eCmd)cmd.first;
    switch(cmd_id)
    {
      //
      // 1) from HB
      //
      case HeartBeat:
        push_ipc(cmd);
        break;
      //
      // 2) from ipc (Geomec.exe)
      //
      case Test_Ack:
      {
        msg(cmd);
        //
        start_hb();
        break;
      }
      case HeartBeat_Ack:
        m_hb.push_(HeartBeat_Ack);
        break;
      case Test_End:
        m_wait.signal();
        break;
      case Test_Step:
      {
        msg(cmd);
        break;
      }
      default:
        break;
    }
  };
  virtual void received(Cmd cmd) //ipc
  {
    eCmd cmd_id = (eCmd)cmd.first;
    push(cmd);
  };
public:
  bool wait(int timeout=0){return m_wait.wait(timeout);}
};

} // namespace gui
} // namespace test

//
// windows-only
//
TEST_F(ModelOperationsGui, GUI)
{
#ifndef WIN32
  return;
#endif

  test::gui::GI gi;

  const QString GEOMEC_APP = "Geomec.exe";

  //
  // if %APPDATA%/Geomec/TEST file doesn't exist copy it from plugins folder
  //
  QString TEST_DEFINITION_URL = QString::fromStdString( QUtil::url( QUtil::Goemec, "TEST" ) );
  QString TEST_DEFINITION_INSTALLATION_URL = "../../plugins/TEST";  // only if WorkingDir == $(Project)
  bool bExist = QFile::exists( TEST_DEFINITION_URL );
  if(!bExist)
  {
    bool res = QFile::copy( TEST_DEFINITION_INSTALLATION_URL, TEST_DEFINITION_URL );
  }

  //
  // FIXME: hard-coded
  //
  //std::string TEST_DEFINITION = "C:/Users/Manel.CarreraRuibal/AppData/Roaming/Geomec/TEST_02";
  std::string TEST_DEFINITION = "";

  QStringList params = { QString::asprintf( "test:%s:%s", TEST_DEFINITION.c_str(), gi.token().c_str() ) };

  qint64 pid = gm::di::launch( GEOMEC_APP, params );
  
  const int TIMEOUT = 60*1000; // depending on the test duration
  bool res = gi.wait( TIMEOUT );
  //
  if(!res)  // timeout
    if(gm::di::running(pid))
      gm::di::kill(pid);
}

//////////////////////////////////////////////////////////////////////
//
//
//				(ModelOperationsExport, ... )
//
//	Linear, NonLinear, Heat, Mixture, MixtureContainment
//
//
//////////////////////////////////////////////////////////////////////

namespace tests{
namespace export_{

// Redefine this to change to processing buffer size
#ifndef PRIVATE_BUFFER_SIZE
#define PRIVATE_BUFFER_SIZE  1024
#endif

// Global objects
std::streamsize const  buffer_size = PRIVATE_BUFFER_SIZE;

//
// FIXME: it fails sometimes with flat tiles: .dat
//
// this can fail: it fails to me during the implementation
//
// https://www.boost.org/doc/libs/1_37_0/libs/crc/crc_example.cpp
//
// src\SpecialOperations.cpp(1809): error: Value of: checksum_02_ref
//   Actual: 2782151641
// Expected: checksum_02
// Which is: 1603740816
//
unsigned int checksum( std::string url )
{
  boost::crc_32_type  result;

  std::ifstream  ifs( url, std::ios_base::binary );

  if( !ifs )
    return 0;

  do
  {
    char  buffer[ buffer_size ];

    ifs.read( buffer, buffer_size );
    result.process_bytes( buffer, ifs.gcount() );
  } while ( ifs );

  return result.checksum();
}

//
// Returns empty QByteArray() on failure.
//
// FIXME: it fails sometimes with flat tiles: .dat
//
QByteArray md5(	const QString &fileName )
{
  QFile f(fileName);
  if (f.open(QFile::ReadOnly)) 
  {
    QCryptographicHash hash( QCryptographicHash::Md5 );
    if( hash.addData( &f ) )
      return hash.result();
  }
  return QByteArray();
}

//using TDataV = std::vector< std::pair< QString, std::string > >;
using TData2V = std::vector< std::pair< QString, int > >;

//
// md5 and chacksum strataegies are not valid as diana generate flat files content not always in the same order
//
// so by now, let's just compare with the file size that is not perfect but at least is something
//
void run( 
  test_model_operations::ModelOperationsExport* this_, 
  const QString& file, 
  const CAnalysisType& ana_type,
  //const TDataV& data_v )
  const TData2V& data_v )
{
  struct Accessor : public ModelOperationsExport
  {
    QString path(){ return m_ModelsPath; }
    CAnalysisLogger* logger(){ return &m_Logger; }
  };
  Accessor* accessor = (Accessor*)this_;

  QString url = accessor->path() + file;
  QString name = RemoveExtension(file);

  CModelBase* pModel = test_lib::TestLib::loadModel(url, accessor->logger() );

  EXPECT_TRUE( pModel );

  bool b_Write_InputFiles = true;
  bool b_Write_QuadDat = false;
  bool b_Write_OutputStreamFile = false;

  QString run_dian_tmp_path = GetGeomecTempPathExt( CTempPath::TEMP_CALCULATION );

  controller::Params p = controller::Params{ 
    *pModel, 
    ana_type, 
    b_Write_InputFiles, b_Write_QuadDat, b_Write_OutputStreamFile, 
    run_dian_tmp_path };

  QString results_path = QDir::toNativeSeparators( CreateTempDirExt(CTempPath::TEMP_GENERAL) );

  int res = test_lib::TestLib::run_model( p, url, results_path );

  EXPECT_EQ( 0, res );

  test_lib::TestLib::closeModel(&pModel);

  //
  // check generated files
  //

  auto check_checksum=[]( const QString& url_, unsigned int ref )
  {
    unsigned int checksum = tests::export_::checksum( url_.toStdString() );
    EXPECT_EQ( checksum, ref );
  };
  auto check_md5=[]( const QString& url_, std::string ref )
  {
    std::string md5 = tests::export_::md5( url_ ).toHex().toStdString();
    EXPECT_EQ( md5, ref );
  };
  auto check_size=[]( const QString& url_, int ref )
  {
    QFile file(url_);
    EXPECT_EQ( file.size(), ref );
  };
  auto check_=[=]( const QString& ext, const std::string& ref )
  {
    QString url_ = QString("%1/%2%3").arg(results_path).arg(name).arg(ext); // %3 already comes with '.' ... this gives more flexibility to pass '_D0.dat' for example and not only the extension
    EXPECT_TRUE( QFile::exists( url_ ) );

    check_md5( url_, ref );
    //check_checksum( url_, ref ); //4278532187
  };
  auto check=[=]( const QString& ext, int ref )
  {
    QString url_ = QString("%1/%2%3").arg(results_path).arg(name).arg(ext); // %3 already comes with '.' ... this gives more flexibility to pass '_D0.dat' for example and not only the extension
    EXPECT_TRUE( QFile::exists( url_ ) );

    check_size( url_, ref );
  };

  for( auto x : data_v )
    check( x.first, x.second );
    //check_( x.first, x.second );

  QDir( results_path ).removeRecursively();
}


} // namespace tests
} // namespace export_

//
// md5: https://stackoverflow.com/questions/55070320/how-to-calculate-md5-of-a-file-using-boost
//

TEST_F(ModelOperationsExport, MixtureContainment)
{
  tests::export_::run( 
    this,
    m_model,
    CAnalysisType::AT_MIXTURE_CONTAINMENT,
    /*tests::export_::TDataV
    {
      std::make_pair( ".dcf", "b5d191663733b9c426e3cac0d00a06a2" ),
      std::make_pair( ".dat", "d569645e2d31dde8dc3d307b639927ca" )	// sometimes: a81f380abe4249bdffa3f369aad9bf09
    }*/
    tests::export_::TData2V
    {
      std::make_pair( ".dcf", 788 ),
      std::make_pair( ".dat", 58342 )
    } 
  );
}

TEST_F(ModelOperationsExport, Mixture)
{
  tests::export_::run( 
    this,
    m_model,
    CAnalysisType::AT_MIXTURE,
    /*tests::export_::TDataV
    {
      std::make_pair( ".dcf", "b5d191663733b9c426e3cac0d00a06a2" ),
      std::make_pair( ".dat", "9cefca287282aec121382ffa44a8fe76" )  // sometimes: 0f738da2e4985065b787cd2a40d57792
    }*/ 
    tests::export_::TData2V
    {
      std::make_pair( ".dcf", 788 ),
      std::make_pair( ".dat", 58342 )
    } 
  );
}

TEST_F(ModelOperationsExport, Heat)
{
  tests::export_::run( 
    this,
    m_model,
    CAnalysisType::AT_HEAT,
    /*tests::export_::TDataV
    {
      std::make_pair( ".dcf", "10479539e5ef490e8e8f3c52634bb016" ),
      std::make_pair( ".dat", "c1950374812ae6a1a21a71d94a429f98" ) // sometimes: 06d98535dff8736dd36284a59aec5b31
    }*/ 
    tests::export_::TData2V
    {
      std::make_pair( ".dcf", 500 ),
      std::make_pair( ".dat", 46387 )
    } 
  );
}

TEST_F(ModelOperationsExport, NonLinear)
{
  tests::export_::run( 
    this, // to access: m_ModelsPath, m_Logger
    //
    m_model,
    //
    CAnalysisType::AT_NONLIN,
    //
    /*tests::export_::TDataV
    {
      //
      // binary
      //
      std::make_pair( ".dcf", "1d3609c5671b57edf7a671e63b8e9d6c" ),
      //
      // text
      //
      std::make_pair( "_D0.dat", "259c5100c73acf25d794c879c5b12bda" ),
      std::make_pair( "_D2.dat", "c6093cccee21e54c3355953255a4a892" ),	
      std::make_pair( "_D4.dat", "a21ce879bf343f13ee1ff4451350f92f" )		
    }*/ 
    tests::export_::TData2V
    {
      //
      // binary
      //
      std::make_pair( ".dcf", 2163 ),
      //
      // text
      //
      std::make_pair( "_D0.dat", 44144 ),
      std::make_pair( "_D2.dat", 23369 ),	
      std::make_pair( "_D4.dat", 21605 )		
    } 
  );
}

//
// FIXME: Look at the comments below
//
TEST_F(ModelOperationsExport, Linear)
{
  tests::export_::run( 
    this, // to access: m_ModelsPath, m_Logger
    //
    m_model,
    CAnalysisType::AT_LINEAR,
    //
    /*tests::export_::TDataV
    {
      std::make_pair( ".dcf", "042a06cbedcb69905494f9950db4d730" ),	// binary
      std::make_pair( ".dat", "d6a543ee5f33b471e865a8ab88bb6da4" )	// text		-> 50% of the times "283a06fcf6c06eb84ae0bcfe7c7b12ef" -> so error -> I still don't know why
    }*/
    tests::export_::TData2V
    {
      std::make_pair( ".dcf", 258 ),	// binary
      std::make_pair( ".dat", 49436 )	// text
    } 
  );
}

} // namespace test_model_operations