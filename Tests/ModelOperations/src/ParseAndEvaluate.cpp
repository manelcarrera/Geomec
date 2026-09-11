#include "GeomecUtils.h"
#include "ModelBase.h"
#include "ModelOperations.h"
#include "NodalValueSet.h"
#include "TetraEntryTypes.h"
#include "TetraFormation.h"
#include "TetraMesh.h"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>

#include "ImportPetrel.h"
#include "NewArchiveStdStringStream.h"
#include "NewWellPath.h"
#include "NewWellPathInput.h"
#include "PropertyMetaData.h"
#include "RGInterface.h"
#include "RockMechProcessor.h"
#include "VersionNumbers.h"

#include <QDir>

#include "Printer.h"

#ifdef SKUA_NEW
#include "SKUAParseData.h"
#include "SKUAParserDataIO.h"
#endif

namespace {
Printer *printer = Printer::instance(Printer::Tests);
}

namespace test_model_operations {

#ifdef _WIN32
#define PATH_DELIM "\\"
#else
#define PATH_DELIM "/"
#endif

///////////////////////////////////////////////////////////////////////////////
//
//
//						ModelOperationsCORA
//
//
///////////////////////////////////////////////////////////////////////////////

ModelOperationsCORA::ModelOperationsCORA()
    : m_ModelsPath(test_lib::TestLib::ModelPath()),
      m_OutputPath(GetGeomecTempPathExt(CTempPath::TEMP_GENERAL) + PATH_DELIM) {}

static QStringList CORATestGetLines(const QString &fileName) {
  QStringList list;

  QFile file(fileName);
  if (file.open(QFile::ReadOnly)) {
    QString data = file.readAll();
    file.close();

    // easy fixes for difference in how linux and windows write doubles
    data.replace("e-0", "e-");
    data.replace("e+0", "e+");

    data.replace("\r", "");

    list = data.split("\n", QString::SkipEmptyParts);

    // linux and windows give us different orders of the parameters
    // sorting allows us to compare
    // although theoretically we could get false positives this way, we assume that doesn't happen
    list.sort();
  }

  return list;
}

void ModelOperationsCORA::some_grouped_tests() {
  QString glob_path = QString(test_lib::TestLib::ModelPath()) + "CORA";
  std::vector<QString> files;
  Glob(glob_path, ".gm4$", files);

  QString output;

  QRegExp reInt("\\d+");
  QRegExp reWP("^well:New Wellpath");

  //
  // loop files in Tests/Model/CORA
  //
  for (std::vector<QString>::const_iterator it = files.begin(); it != files.end(); ++it) {
    QString path, file, base, outputBase;
    SplitPathAndFileName(*it, path, file);

    std::cout << "TEST geomec_shell --interface cora FOR " << file.toStdString() << std::endl;

    path += PATH_DELIM;

    base = file.left(file.length() - 4);
    outputBase = m_OutputPath + base;

    QString cmd = "--interface cora \"" + *it + "\" getmodelinfo \"" + outputBase + ".modelinfo\" \"" + outputBase +
                  ".modelinfo_result_summary\"";

    //
    // geomec_shell
    //
    ::test_model_operations::execute_geomec_cora(cmd.toStdString().c_str(), &output);
    //
    //
    //

    EXPECT_TRUE(QFile::exists(outputBase + ".modelinfo"));
    EXPECT_TRUE(QFile::exists(outputBase + ".modelinfo_result_summary"));

    //
    // model results to be compared with expected ones in 'xxx.reference' files
    //
    // only XMLTest?
    //
    if (QFile::exists(path + base + ".reference")) {
      EXPECT_TRUE(CORATestGetLines(path + base + ".reference") == CORATestGetLines(outputBase + ".modelinfo"));

      cmd = "--interface cora \"" + *it + "\" getmodelinfo 1.7 \"" + outputBase + ".modelinfo_1.7\" \"" + outputBase +
            ".modelinfo_result_summary_1.7\"";
      //
      // geomec_shell
      //
      ::test_model_operations::execute_geomec_cora(cmd.toStdString().c_str(), &output);
      //
      //
      //

      EXPECT_TRUE(QFile::exists(outputBase + ".modelinfo_1.7"));
      EXPECT_TRUE(QFile::exists(outputBase + ".modelinfo_result_summary_1.7"));

      EXPECT_TRUE(CORATestGetLines(path + base + ".reference") == CORATestGetLines(outputBase + ".modelinfo_1.7"));

      QFile::remove(outputBase + ".modelinfo_1.7");
      QFile::remove(outputBase + ".modelinfo_result_summary_1.7");

#ifndef _DEBUG
      // because of BUG 171162 the debug version breaks on geo::CValue::Value()

      cmd = "--interface cora \"" + *it + "\" getmodelinfo 2.0 \"" + outputBase + ".modelinfo_2.0\" \"" + outputBase +
            ".modelinfo_result_summary_2.0\"";
      //
      // geomec_shell
      //
      ::test_model_operations::execute_geomec_cora(cmd.toStdString().c_str(), &output);
      //
      //
      //

      EXPECT_TRUE(QFile::exists(outputBase + ".modelinfo_2.0.xml"));
      EXPECT_TRUE(QFile::exists(outputBase + ".modelinfo_result_summary_2.0"));

      EXPECT_TRUE(CORATestGetLines(path + base + ".reference.xml") ==
                  CORATestGetLines(outputBase + ".modelinfo_2.0.xml"));

      QFile::remove(outputBase + ".modelinfo_2.0.xml");
      QFile::remove(outputBase + ".modelinfo_result_summary_2.0");
#endif
    }

    if (QFile::exists(path + base + ".parameterfile")) {
      // cmd = "--interface cora \"" + *it + "\" runmodel 1.7 \"" + path + base + ".parameterfile\" \"" + path + base +
      // ".selectedlsfs\" " + outputBase + ".response_parameters " + outputBase + ".runmodel_result_summary
      // minimum_output";
      //
      // this si the one with a bad format
      //
      cmd = "--interface cora \"" + *it + "\" runmodel 1.7 \"" +
            // input files
            path + base + ".parameterfile\" \"" + path + base + ".selectedlsfs\" \"" +
            // output files
            outputBase + ".response_parameters\" \"" + outputBase + ".runmodel_result_summary\" " +
            //
            "minimum_output";

      //
      // geomec_shell
      //
      ::test_model_operations::execute_geomec_cora(cmd.toStdString().c_str(), &output);
      //
      //
      //

      EXPECT_TRUE(QFile::exists(outputBase + ".response_parameters"));
      EXPECT_TRUE(QFile::exists(outputBase + ".runmodel_result_summary"));

      QFile::remove(outputBase + ".response_parameters");
      QFile::remove(outputBase + ".runmodel_result_summary");
    }
    //
    // wellpaths ???
    //
    // 'H.WP.*.gm4' files in 'WellPathTests' folder
    //
    else {
      int numberExpected = 0, numberFound = 0;
      int index = 0;
      while ((index = reInt.indexIn(base, index)) != -1) {
        numberExpected += reInt.cap().toInt();
        index += reInt.matchedLength();
      }

      QStringList data = CORATestGetLines(outputBase + ".modelinfo");
      for (QStringList::const_iterator line = data.begin(); line != data.end(); ++line) {
        if (reWP.indexIn(*line) != -1)
          ++numberFound;
      }

      EXPECT_EQ(numberExpected, numberFound);
    }

    //
    // remove generated tmp data
    //
    QFile::remove(outputBase + ".modelinfo");
    QFile::remove(outputBase + ".modelinfo_result_summary");
  }
}

#ifndef SKUA_NEW
namespace test_gocad {
void compareProperties(const std::vector<double> &oldProperties, const std::vector<double> &newProperties) {
  EXPECT_TRUE(oldProperties.size() == newProperties.size());

  for (size_t property = 0; property < newProperties.size(); ++property) {
    EXPECT_TRUE(oldProperties[property] == newProperties[property]);
  }
}

void compareNode(const CGocadData::CNode &oldNode, const CGocadData::CNode &newNode) {
  EXPECT_TRUE(oldNode.ID() == newNode.ID());
  EXPECT_TRUE(oldNode.PropertySize() == newNode.PropertySize());

  for (int property = 0; property < newNode.PropertySize(); ++property) {
    compareProperties(oldNode.Property(property), newNode.Property(property));
  }
}

void compareVertex(const CGocadData::CVertex &oldVertex, const CGocadData::CVertex &newVertex) {
  compareNode(oldVertex, newVertex);

  EXPECT_TRUE(oldVertex.X() == newVertex.X());
  EXPECT_TRUE(oldVertex.Y() == newVertex.Y());
  EXPECT_TRUE(oldVertex.Z() == newVertex.Z());
}

void compareAtom(const CGocadData::CAtom &oldAtom, const CGocadData::CAtom &newAtom) {
  compareVertex(oldAtom.Vertex(), newAtom.Vertex());
}

void compareTetra(const CGocadData::CTetra &oldTetra, const CGocadData::CTetra &newTetra) {
  for (int node = 0; node < 4; ++node) {
    compareNode(oldTetra.Node(node), newTetra.Node(node));
  }

  EXPECT_TRUE(oldTetra.PropertySize() == newTetra.PropertySize());

  for (int property = 0; property < newTetra.PropertySize(); ++property) {
    compareProperties(oldTetra.Property(property), newTetra.Property(property));
  }
}

void compareVolume(const CGocadData::CTVolume &oldVolume, const CGocadData::CTVolume &newVolume) {
  EXPECT_TRUE(oldVolume.Name() == newVolume.Name());
  EXPECT_TRUE(oldVolume.VertexSize() == newVolume.VertexSize());

  for (int vertex = 0; vertex < newVolume.VertexSize(); ++vertex) {
    compareVertex(oldVolume.Vertex(vertex), newVolume.Vertex(vertex));
  }

  EXPECT_TRUE(oldVolume.AtomSize() == newVolume.AtomSize());

  for (int atom = 0; atom < newVolume.AtomSize(); ++atom) {
    compareAtom(oldVolume.Atom(atom), newVolume.Atom(atom));
  }

  EXPECT_TRUE(oldVolume.TetraSize() == newVolume.TetraSize());

  for (int tetra = 0; tetra < newVolume.TetraSize(); ++tetra) {
    compareTetra(oldVolume.Tetra(tetra), newVolume.Tetra(tetra));
  }
}

void compareVolumes(CGocadData::CTSolid &oldSolid, CGocadData::CTSolid &newSolid) {
  EXPECT_TRUE(oldSolid.VolumeSize() == newSolid.VolumeSize());

  for (int volume = 0; volume < newSolid.VolumeSize(); ++volume) {
    compareVolume(oldSolid.Volume(volume), newSolid.Volume(volume));
  }
}

void compareTriangle(const CGocadData::CTriangle &oldTriangle, const CGocadData::CTriangle &newTriangle) {
  for (int node = 0; node < 3; ++node) {
    compareNode(oldTriangle.Node(node), newTriangle.Node(node));
  }
}

void compareFace(const CGocadData::CTFace &oldFace, const CGocadData::CTFace &newFace) {
  EXPECT_TRUE(oldFace.Name() == newFace.Name());
  EXPECT_TRUE(oldFace.TriangleSize() == newFace.TriangleSize());

  for (size_t face = 0; face < newFace.TriangleSize(); ++face) {
    compareTriangle(oldFace.Triangle(face), newFace.Triangle(face));
  }
}

void compareSurface(const CGocadData::CSurface &oldSurface, const CGocadData::CSurface &newSurface) {
  EXPECT_TRUE(oldSurface.Name() == newSurface.Name());
  EXPECT_TRUE(oldSurface.TFaceSize() == newSurface.TFaceSize());

  for (int face = 0; face < newSurface.TFaceSize(); ++face) {
    compareFace(oldSurface.TFace(face), newSurface.TFace(face));
  }
}

void compareSurfaces(CGocadData::CTSolid &oldSolid, CGocadData::CTSolid &newSolid) {
  EXPECT_TRUE(oldSolid.SurfaceSize() == newSolid.SurfaceSize());

  for (int surface = 0; surface < newSolid.SurfaceSize(); ++surface) {
    compareSurface(oldSolid.Surface(surface), newSolid.Surface(surface));
  }
}

void comparePropertyNames(CGocadData::CTSolid &oldSolid, CGocadData::CTSolid &newSolid) {
  EXPECT_TRUE(oldSolid.PropertiesSize() == newSolid.PropertiesSize());

  for (int property = 0; property < newSolid.PropertiesSize(); ++property) {
    EXPECT_TRUE(oldSolid.PropertyName(property) == newSolid.PropertyName(property));
  }
}

void compareNoDataValues(CGocadData::CTSolid &oldSolid, CGocadData::CTSolid &newSolid) {
  EXPECT_TRUE(oldSolid.NoDataValueSize() == newSolid.NoDataValueSize());

  for (int noDataValue = 0; noDataValue < newSolid.NoDataValueSize(); ++noDataValue) {
    EXPECT_TRUE(oldSolid.NoDataValue(noDataValue) == newSolid.NoDataValue(noDataValue));
  }
}

void compareESizes(CGocadData::CTSolid &oldSolid, CGocadData::CTSolid &newSolid) {
  EXPECT_TRUE(oldSolid.ESizeSize() == newSolid.ESizeSize());

  for (int eSize = 0; eSize < newSolid.ESizeSize(); ++eSize) {
    EXPECT_TRUE(oldSolid.ESize(eSize) == newSolid.ESize(eSize));
  }
}

void compareTetraProperties(CGocadData::CTSolid &oldSolid, CGocadData::CTSolid &newSolid) {
  EXPECT_TRUE(oldSolid.TetraPropertiesSize() == newSolid.TetraPropertiesSize());

  for (int tetraProperty = 0; tetraProperty < newSolid.TetraPropertiesSize(); ++tetraProperty) {
    EXPECT_TRUE(oldSolid.TetraPropertyName(tetraProperty) == newSolid.TetraPropertyName(tetraProperty));
  }
}

void compareTetraNoDataValues(CGocadData::CTSolid &oldSolid, CGocadData::CTSolid &newSolid) {
  EXPECT_TRUE(oldSolid.TetraNoDataValueSize() == newSolid.TetraNoDataValueSize());

  for (int tetraNoDataValue = 0; tetraNoDataValue < newSolid.TetraNoDataValueSize(); ++tetraNoDataValue) {
    EXPECT_TRUE(oldSolid.TetraNoDataValue(tetraNoDataValue) == newSolid.TetraNoDataValue(tetraNoDataValue));
  }
}

void compareTetraESize(CGocadData::CTSolid &oldSolid, CGocadData::CTSolid &newSolid) {
  EXPECT_TRUE(oldSolid.TetraESizeSize() == newSolid.TetraESizeSize());

  for (int tetraESize = 0; tetraESize < newSolid.TetraESizeSize(); ++tetraESize) {
    EXPECT_TRUE(oldSolid.TetraESize(tetraESize) == newSolid.TetraESize(tetraESize));
  }
}

void compareVertexIDs(CGocadData::CTSolid &oldSolid, CGocadData::CTSolid &newSolid) {
  EXPECT_TRUE(oldSolid.VertexIDsSize() == newSolid.VertexIDsSize());

  int vertexID = 0;

  for (int count = 0; count < newSolid.VertexIDsSize(); ++count) {
    const CGocadData::CVertex *oldVertex;
    const CGocadData::CVertex *newVertex;

    do {
      oldVertex = oldSolid.VertexID(vertexID);
      newVertex = newSolid.VertexID(vertexID);

      if (newVertex) {
        compareVertex(*oldVertex, *newVertex);
      }

      ++vertexID;
    } while (newVertex == 0);
  }
}

void compareAtomIDs(CGocadData::CTSolid &oldSolid, CGocadData::CTSolid &newSolid) {
  EXPECT_TRUE(oldSolid.AtomIDsSize() == newSolid.AtomIDsSize());

  int atomID = 0;

  for (int count = 0; count < newSolid.AtomIDsSize(); ++count) {
    const CGocadData::CAtom *oldAtom;
    const CGocadData::CAtom *newAtom;

    do {
      oldAtom = oldSolid.AtomID(atomID);
      newAtom = newSolid.AtomID(atomID);

      if (newAtom) {
        compareAtom(*oldAtom, *newAtom);
      }

      ++atomID;
    } while (newAtom == 0);
  }
}

void compareSolids(CGocadData::CTSolid &oldSolid, CGocadData::CTSolid &newSolid) {
  EXPECT_TRUE(oldSolid.ZFactor() == newSolid.ZFactor());
  EXPECT_TRUE(oldSolid.UnitFactor(0) == newSolid.UnitFactor(0));
  EXPECT_TRUE(oldSolid.UnitFactor(1) == newSolid.UnitFactor(1));
  EXPECT_TRUE(oldSolid.UnitFactor(2) == newSolid.UnitFactor(2));
  EXPECT_TRUE(oldSolid.Name() == newSolid.Name());

  comparePropertyNames(oldSolid, newSolid);
  compareNoDataValues(oldSolid, newSolid);
  compareESizes(oldSolid, newSolid);
  compareTetraProperties(oldSolid, newSolid);
  compareTetraNoDataValues(oldSolid, newSolid);
  compareTetraESize(oldSolid, newSolid);
  compareVertexIDs(oldSolid, newSolid);
  compareAtomIDs(oldSolid, newSolid);
  compareVolumes(oldSolid, newSolid);
  compareSurfaces(oldSolid, newSolid);
}

} // namespace test_gocad
#endif

namespace test_petrel {

void verifyUnit(const CImportPetrel::t_unit &petrelUnit, const QString &unit) {
  EXPECT_TRUE(petrelUnit == (unit == "SI" ? CImportPetrel::e_si_unit : CImportPetrel::e_field_unit));
}

} // namespace test_petrel

///////////////////////////////////////////////////////////////////////////////
//
//
//						ModelOperationsBatch
//
//
///////////////////////////////////////////////////////////////////////////////

Script::Script(const char *script) {
  commands.reserve(16);

  QStringList l = QString(script).split(";", QString::SkipEmptyParts);
  for (QStringList::const_iterator it = l.begin(); it != l.end(); ++it) {
    Command cmd;
    QStringList c = it->split(' ', QString::SkipEmptyParts);
    if (c.length() > 0) {
      if (c[0] == "load")
        cmd.opcode = Command::LOAD;
      else if (c[0] == "save")
        cmd.opcode = Command::SAVE;
      else if (c[0] == "save_as")
        cmd.opcode = Command::SAVE_AS;
      else if (c[0] == "close")
        cmd.opcode = Command::CLOSE;
      else if (c[0] == "run")
        cmd.opcode = Command::RUN;
      else if (c[0] == "import")
        cmd.opcode = Command::IMPORT;
      else if (c[0] == "mesh")
        cmd.opcode = Command::MESH;
    }
    if (c.length() > 1) {
      c.removeFirst();
      cmd.params = c.join(' ');
    }

    if (cmd.opcode != Command::NOP)
      commands.push_back(cmd);
  }
}

ModelOperationsBatch::ModelOperationsBatch()
    : m_ModelsPath(test_lib::TestLib::ModelPath()), m_OutputPath(GetGeomecTempPathExt(CTempPath::TEMP_GENERAL)) {}

#ifdef SKUA_NEW
bool selectGocadSolidDlg(const std::vector<const gm_skua::SKUAParseData *> &solids, std::vector<int> &vcSolids) {
  EXPECT_TRUE(solids.size() == 1);

  vcSolids.clear();
  vcSolids.push_back(1);

  return true;
}
#else
bool selectGocadSolidDlg(const std::vector<QSharedPointer<CGocadData::CTSolid>> &solids, std::vector<int> &vcSolids) {
  EXPECT_TRUE(solids.size() == 1);

  vcSolids.clear();
  vcSolids.push_back(1);

  return true;
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
//
//						ModelOperationsShell
//
//
///////////////////////////////////////////////////////////////////////////////

void ModelOperationsBatch::execute() {
  CModelBase *pModel = 0;
  QString path;

  int tmpFlag;

  Script script(GetParam());

  static int i = 0;
  static const char *opcode_cp[] = {"nop", "load", "save", "save_as", "close", "run", "import", "mesh"};
  printer->info("batch : script: %d", i);
  i++;

  for (std::vector<Command>::const_iterator command = script.commands.begin(); command != script.commands.end();
       ++command) {
    printer->info("batch : cmd: %s %s", opcode_cp[command->opcode], command->params.toStdString().c_str());
    switch (command->opcode) {
    case Command::LOAD:
      if (!command->params.isEmpty())
        path = m_ModelsPath + command->params;
      EXPECT_TRUE(QFile::exists(path));
      pModel = test_lib::TestLib::loadModel(path, &m_Logger);
      EXPECT_TRUE(pModel);
      break;
    case Command::SAVE:
      ASSERT_TRUE(!path.startsWith(m_ModelsPath));
      EXPECT_TRUE(test_lib::TestLib::saveModel(*pModel, path));
      EXPECT_TRUE(QFile::exists(path));
      break;
    case Command::SAVE_AS:
      path = m_OutputPath + PATH_DELIM + command->params;
      EXPECT_TRUE(test_lib::TestLib::saveModel(*pModel, path));
      EXPECT_TRUE(QFile::exists(path));
      break;
    case Command::CLOSE:
      test_lib::TestLib::closeModel(&pModel);
      break;
      //
      // no need to end run as it is automatically done at the runStepsModel end
      //
    case Command::RUN:
      if (!command->params.isEmpty() && command->params.toInt() == 1)
        EXPECT_EQ(0, test_lib::TestLib::runStepsModel(pModel, path));
      else
        EXPECT_EQ(0, test_lib::TestLib::runModel(pModel, path));
      break;
    case Command::IMPORT:
      if (command->params.startsWith("mesh ")) {
        QStringList l = command->params.split(' ', QString::SkipEmptyParts);
        QString sGocadFile = m_ModelsPath + l[1];

        CTetraMesh &mesh = static_cast<CTetraMesh &>(pModel->Mesh());

        EXPECT_TRUE(mesh.ImportMeshFromGoCadFile(sGocadFile, selectGocadSolidDlg));
      } else if (command->params.startsWith("gocad ")) {
        QStringList l = command->params.split(' ', QString::SkipEmptyParts);
        QString sGocadFile = m_ModelsPath + l[1];

        tmpFlag = l.length() > 2 ? l[2].toInt() : 0;

        CGocadImport gocadImport(tmpFlag);

        EXPECT_TRUE(gocadImport.Import(sGocadFile.toStdString()));

        if (l.length() < 4 || l[3].toInt()) {
#ifdef SKUA_NEW
          std::vector<const gm_skua::SKUAParseData *> &solids = gocadImport.getSolids();
          CNewArchiveStdStringStream file(std::fstream::in | std::fstream::out | std::fstream::binary);
          CStreamVersion version(VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION);
          IProgressBase progress;

          gm_skua::SaveStream(file, progress, *solids[0]);
          file.SetPosition(0);

          gm_skua::SKUAParseData newSolid(gm_skua::SKUAParseData::Unknown);

          gm_skua::LoadStream(file, version, progress, newSolid);

          EXPECT_TRUE(*solids[0] == newSolid);
#else
          std::vector<QSharedPointer<CGocadData::CTSolid>> &solids = gocadImport.getSolids();
          CNewArchiveStdStringStream file(std::fstream::in | std::fstream::out | std::fstream::binary);
          CStreamVersion version(VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION);
          IProgressBase progress;

          solids[0]->SaveStream(file, progress);
          file.SetPosition(0);

          CGocadData::CTSolid newSolid;

          newSolid.LoadStream(file, version, progress);

          test_gocad::compareSolids(*solids[0], newSolid);
#endif
        }
      } else if (command->params.startsWith("petrel ")) {
        QStringList l = command->params.split(' ', QString::SkipEmptyParts);
        QString sPetrelFile = m_ModelsPath + l[1];

        CNewWellPathInput *input = new CNewWellPathInput(sPetrelFile, *pModel);
        IProgressBase progress;

        const CQuantity::UNIT defaultLateralUnit = CQuantity::SI_UNIT, defaultDepthUnit = CQuantity::SI_UNIT;

        CImportPetrel importPetrel(sPetrelFile, input, progress, defaultLateralUnit, defaultDepthUnit);

        EXPECT_TRUE(importPetrel.Import());

        test_petrel::verifyUnit(importPetrel.getLateralUnit(), l[2]);
        test_petrel::verifyUnit(importPetrel.getDepthUnit(), l[3]);

        std::cout << std::endl << importPetrel.Message().toStdString().c_str() << std::endl;
      }
      break;
    case Command::MESH:
      if (command->params.isEmpty())
        tmpFlag = pModel->CanCreateMesh() ? 1 : 0;
      else
        tmpFlag = command->params.toInt();
      if (tmpFlag) {
        EXPECT_TRUE(pModel->CanCreateMesh());
        pModel->CreateMesh();
        EXPECT_FALSE(pModel->CanCreateMesh());
      } else {
        EXPECT_FALSE(pModel->CanCreateMesh());
        pModel->InvalidateMesh();
        EXPECT_TRUE(pModel->CanCreateMesh());
      }
      break;
    }
  }

  if (path.startsWith(m_OutputPath))
    QFile::remove(path);
}

//
// executes geomec_shell
//
// used by CORA, RGI a GM interfaces
//
void execute(const char *executable_, const char *params, QString *captureOutput) {
  char buffer[256];
  char exec_string[4096];
  memset(exec_string, 0, 4096);

  size_t index = 0;

#ifndef _WIN32
  const char *linuxBuildExecPath = "";
  size_t linuxBuildExecPathSize = strlen(linuxBuildExecPath);

  // memcpy(exec_string, "../../geomec_shell/", linuxBuildExecPathSize);
  memcpy(exec_string, "", linuxBuildExecPathSize);
  index += linuxBuildExecPathSize;
#endif

  const char *execPath = test_lib::TestLib::ExecPath();
  size_t execPathSize = strlen(execPath);

  memcpy(exec_string + index, execPath, execPathSize);
  index += execPathSize;

  const char *executable = executable_;
  size_t executableSize = strlen(executable);

  memcpy(exec_string + index, executable, executableSize);
  index += executableSize;

  exec_string[index++] = ' ';

  memcpy(exec_string + index, params, strlen(params));

  test_lib::TestLib::Log(exec_string);
  printer->info("--cmd-- %s", exec_string);

#ifdef _WIN32
  FILE *pipe = _popen(exec_string, "r");
#else
  FILE *pipe = popen(exec_string, "r");
#endif

  EXPECT_TRUE(pipe);

  while (!feof(pipe)) {
    if (fgets(buffer, 256, pipe)) {
      //
      // enable here std out
      //
      if (captureOutput)
        *captureOutput += buffer;
      else
        std::cout << buffer;
    }
  }

#ifdef _WIN32
  _pclose(pipe);
#else
  pclose(pipe);
#endif

  test_lib::TestLib::Log(captureOutput);
}

void execute_geomec_shell(const char *params, QString *captureOutput) {
  execute("geomec_shell", params, captureOutput);
}
void execute_geomec_cora(const char *params, QString *captureOutput) { execute("geomec_cora", params, captureOutput); }

///////////////////////////////////////////////////////////////////////////////
//
//
//						ModelOperationsRGI
//
//
///////////////////////////////////////////////////////////////////////////////

ModelOperationsRGI::ModelOperationsRGI()
    : m_ModelsPath(test_lib::TestLib::ModelPath()),
      m_OutputPath(GetGeomecTempPathExt(CTempPath::TEMP_GENERAL) + PATH_DELIM) {}

void ModelOperationsRGI::some_grouped_tests() {
  auto PRINT_ERROR = [=](const RGInterface *rgInterface) {
    int nE = rgInterface->getNumErrorMsgs();
    for (int i = 0; i < nE; ++i) {
      printer->error("%d: %s", i, rgInterface->getNthErrorMsg(i));
    }
  };

  QString rescue = m_OutputPath + "rgi";
  QString rgitmp = m_OutputPath + "rgitmp";

  QDir(m_OutputPath).mkdir("rgitmp");

  QString cleanPath = QDir::current().absoluteFilePath(
      m_ModelsPath); // TODO: taken from geomec_shell; should be moved somewhere more global
  cleanPath = QDir::current().cleanPath(cleanPath);
  cleanPath = QDir::toNativeSeparators(cleanPath);

  printer->debug("RGI : clean path:%s ... m_OutputPath:%s", cleanPath.toStdString().c_str(),
                 m_OutputPath.toStdString().c_str());

  // We only test error messages that are generated before we open the RGInterface/RESCUE file
  // If these work, normal error messages should also work
  struct {
    int type; // 0 = error without running Diana, 1 = error with running Diana, 2 = no error with running Diana
    QString model;
    QString logfile;
    QString tmpdir;
    QString error;
  } input[] = {
      {2, "TestRGIOk.gm5", "", rgitmp, ""},
      {0, "NOTEXISTENT.gm5", "", "",
       QString("Failed to load model '%1'").arg(cleanPath + PATH_DELIM + "NOTEXISTENT.gm5")},
      {0, "TestRGINotEmpty.gm5", "", "", "Geomec model hasn't been cleaned up; please resolve the following issues:"},
#ifdef WIN32
      {0, "TestRGILog.gm5", "C:\\AUX", "", "Unable to open log file 'C:\\AUX'"},
#else
      {0, "TestRGILog.gm5", "/log.txt", "", "Unable to open log file '/log.txt'"}, // assume we can't write in /
#endif
      {0, "TestRGIInvalid.gm5", "", "", "Invalid Geomec model, not all calculation criteria are met!"},
      {1, "TestRGIDianaFailure.gm5", "", "", "Analysis failed"},
      {0, "", "", "", ""}};

  for (size_t testNr = 0; !input[testNr].model.isEmpty(); ++testNr) {
    QDir(rescue).removeRecursively();

    QString cmd =
        "--interface rgi /ROCKMECHFILE:" + rescue + PATH_DELIM + "rgi.bin " + m_ModelsPath + input[testNr].model;

    if (!input[testNr].logfile.isEmpty())
      cmd += " /LOG:" + input[testNr].logfile;

    if (!input[testNr].tmpdir.isEmpty())
      cmd += " /TEMPDIR:" + input[testNr].tmpdir;

    QString output;

    RGInterface *rgInterface = nullptr;

    if (input[testNr].type == 0) {
      rgInterface = new RGInterface(rescue.toStdString());
      rgInterface->setCurrentDepletionStage(RGDepletionStage(0, 0));
      rgInterface->addCommand(GMCommand(typeCommandDefineInitialDate, RGDate(1971, 10, 16)));
      rgInterface->addCommand(GMCommand(typeCommandCalculate));
      rgInterface->addCommand(GMCommand(typeCommandQuit));
      rgInterface->dumpModel();
    } else // input[testNr].type >= 1
    {
      rgInterface = new RGInterface(rescue.toStdString());
      rgInterface->setCurrentDepletionStage(RGDepletionStage(0, 0));
      rgInterface->addCommand(GMCommand(typeCommandDefineInitialDate, RGDate(1971, 10, 16)));
      rgInterface->addCommand(GMCommand(typeCommandSaveGrid));
      // getFormationName no longer needs the next command to be available
      // rgInterface->addCommand(GMCommand(typeCommandSaveProperty, RGProperty(RGPropertyType::propElementFormation)));
      rgInterface->addCommand(GMCommand(typeCommandSaveProperty, RGProperty(RGPropertyType::propElementYoung)));
      rgInterface->addCommand(GMCommand(typeCommandQuit));
      rgInterface->dumpModel();

      EXPECT_TRUE(rgInterface->getNumErrorMsgs() == 0);
      PRINT_ERROR(rgInterface);

      delete rgInterface;

      ::test_model_operations::execute_geomec_shell(cmd.toStdString().c_str(), &output);

      rgInterface = new RGInterface(rescue.toStdString());

      std::string f = rgInterface->getFormationName(0);
      EXPECT_TRUE(!f.empty());

      int elts = input[testNr].type == 1 ? 1 : 1000; // larger model in 2, just to prevent any warnings in Diana (seems
                                                     // it can't handle 1 element models well)

      if (input[testNr].type == 2) {
        std::vector<double> values;
        rgInterface->loadProperty(RGProperty(RGPropertyType::propElementYoung), values);

        EXPECT_TRUE(values.size() == elts);
        int count = 0;
        for (int i = 0; i < elts; ++i) {
          if (fabs(values[i] - 8.5E9) < 1000)
            ++count;
        }
        EXPECT_TRUE(count == elts);

        rgInterface->deletePropertiesForDepletionStage(0);
        rgInterface->dumpModel();

        delete rgInterface;

        rgInterface = new RGInterface(rescue.toStdString());

        rgInterface->saveProperty(RGProperty(RGPropertyType::propElementYoung), std::vector<double>(elts, 9.5E9));
        rgInterface->clearCommandList();
        rgInterface->addCommand(GMCommand(typeCommandLoadProperty, RGProperty(RGPropertyType::propElementYoung)));
        rgInterface->addCommand(GMCommand(typeCommandSaveProperty, RGProperty(RGPropertyType::propElementYoung)));
        rgInterface->addCommand(GMCommand(typeCommandQuit));
        rgInterface->dumpModel();

        EXPECT_TRUE(rgInterface->getNumErrorMsgs() == 0);

        delete rgInterface;

        cmd = cmd.replace(".gm5", "_D0.gm5");

        output.clear();
        ::test_model_operations::execute_geomec_shell(cmd.toStdString().c_str(), &output);

        rgInterface = new RGInterface(rescue.toStdString());

        values.clear();
        rgInterface->loadProperty(RGProperty(RGPropertyType::propElementYoung), values);

        EXPECT_TRUE(values.size() == elts);
        count = 0;
        for (int i = 0; i < elts; ++i) {
          if (fabs(values[i] - 9.5E9) < 1000)
            ++count;
        }
        EXPECT_TRUE(count == elts);

        cmd = cmd.replace(".gm5", "_D0.gm5");
      }

      rgInterface->setCurrentDepletionStage(RGDepletionStage(1, 3.2E7));
      rgInterface->saveProperty(RGProperty(RGPropertyType::propElementFluidPressure), std::vector<double>(elts, 50));
      rgInterface->saveProperty(RGProperty(RGPropertyType::propElementYoung), std::vector<double>(elts, 9.5E9));
      rgInterface->saveProperty(RGProperty(RGPropertyType::propElementMaterialType), std::vector<int>(elts, 1));
      rgInterface->clearCommandList();
      // rgInterface->addCommand(GMCommand(typeCommandLoadProperty,
      // RGProperty(RGPropertyType::propElementMaterialType)));
      rgInterface->addCommand(GMCommand(typeCommandLoadProperty, RGProperty(RGPropertyType::propElementFluidPressure)));
      rgInterface->addCommand(GMCommand(typeCommandLoadProperty, RGProperty(RGPropertyType::propElementYoung)));
      rgInterface->addCommand(GMCommand(typeCommandCalculate));
      rgInterface->addCommand(GMCommand(typeCommandQuit));
      rgInterface->dumpModel();
    }

    EXPECT_TRUE(rgInterface->getNumErrorMsgs() == 0);
    PRINT_ERROR(rgInterface);

    delete rgInterface;

    output.clear();
    ::test_model_operations::execute_geomec_shell(cmd.toStdString().c_str(), &output);

    if (input[testNr].type > 0)
      std::cout << output.toStdString() << std::endl;

    rgInterface = new RGInterface(rescue.toStdString());

    if (input[testNr].error.isEmpty()) {
      EXPECT_TRUE(rgInterface->getNumErrorMsgs() == 0);
      PRINT_ERROR(rgInterface);
    } else {
      EXPECT_TRUE(rgInterface->getNumErrorMsgs() >= 1);
      PRINT_ERROR(rgInterface);

      std::cout << "Expect: " << input[testNr].error.toStdString() << std::endl;

      bool haveExpectedError = false;
      for (size_t i = 0; i < rgInterface->getNumErrorMsgs(); ++i) {
        std::string error = rgInterface->getNthErrorMsg(i);
        std::cout << "Received: " << rgInterface->getNthErrorMsg(i) << std::endl;

        if (error == input[testNr].error.toStdString())
          haveExpectedError = true;
      }

      EXPECT_TRUE(haveExpectedError);
    }

    delete rgInterface;

    QDir(rescue).removeRecursively();
    QDir(rgitmp).removeRecursively();
  }
}

std::string transform_monps_propname(std::string prop) {
  QString exportLabel(prop.c_str());

  // biggest match first
  if (exportLabel.endsWith("NN") || exportLabel.endsWith("EE") || exportLabel.endsWith("VV") ||
      exportLabel.endsWith("NE") || exportLabel.endsWith("EV") || exportLabel.endsWith("NV")) {
    QString last = exportLabel.right(2);
    exportLabel = exportLabel.remove(exportLabel.length() - 2, 2);
    exportLabel += "_D1__" + last;
  } else if (exportLabel != "GammaV" &&
             (exportLabel.endsWith("N") || exportLabel.endsWith("E") || exportLabel.endsWith("V"))) {
    QString last = QString(exportLabel.back());
    exportLabel = exportLabel.remove(exportLabel.length() - 1, 1);
    exportLabel += "_D1__" + last;
  } else {
    exportLabel += "_D1";
  }

  return exportLabel.toStdString();
}

std::string transform_monps_propname(const CNodalValueSet &nvs) {
  QString name;

  const CValueComponent *component = nvs.Component();

  if (component) {
    const IValueComposite &vt = component->Parent();

    name = vt.Name();

    if (vt.ComponentSize() > 1)
      name += "__" + component->Name();
  }

  return name.toStdString();
}

void ModelOperationsRGI::monitoring_points_test() {
  QString rescue = m_OutputPath + "rgi";
  QString rgitmp = m_OutputPath + "rgitmp";

  QDir(m_OutputPath).mkdir("rgitmp");

  QString cleanPath = QDir::current().absoluteFilePath(
      m_ModelsPath); // TODO: taken from geomec_shell; should be moved somewhere more global
  cleanPath = QDir::current().cleanPath(cleanPath);
  cleanPath = QDir::toNativeSeparators(cleanPath);

  struct {
    int type; // 0 = default
    QString model;
    QString logfile;
    QString tmpdir;
    QString error;
  } input[] = {{0, "TestRGIMonitoringPoints.gm5", "", rgitmp, ""}, {0, "", "", "", ""}};

  for (size_t testNr = 0; !input[testNr].model.isEmpty(); ++testNr) {
    QDir(rescue).removeRecursively();

    QString cmd =
        "--interface rgi /ROCKMECHFILE:" + rescue + PATH_DELIM + "rgi.bin " + m_ModelsPath + input[testNr].model;

    if (!input[testNr].logfile.isEmpty())
      cmd += " /LOG:" + input[testNr].logfile;

    if (!input[testNr].tmpdir.isEmpty())
      cmd += " /TEMPDIR:" + input[testNr].tmpdir;

    QString output;

    RGInterface *rgInterface = nullptr;

    int last = 0;

    std::vector<RGGeneralProperty> props1;
    std::vector<RGGeneralProperty> props2;

    if (input[testNr].type == 0) {
      rgInterface = new RGInterface(rescue.toStdString());

      // WORKAROUND for LookupTable problems in RGInterface
      /*
      std::map<std::string, const IValueComponentBase *> properties;
      std::vector<RGGeneralProperty> RGProps;

      CModelBase *pModel = test_lib::TestLib::loadModel(m_ModelsPath + input[testNr].model, &m_Logger);
      EXPECT_TRUE(pModel);

      GeomecRGI::CRockMechProcessor RMProc(rescue, rgitmp, rgitmp);
      RMProc.SetModel(*pModel, true);
      RMProc.CollectMonitorablePropertiesFromModel(properties, false);
      RMProc.TransformMonitorableProperties(properties, RGProps);

      EXPECT_TRUE(RGProps.size() > 0);

      rgInterface->SetAvailableMonitorableProperties(RGProps);

      std::vector<double> xWorkaround(4, 250);
      std::vector<double> yWorkaround(4, 300);
      std::vector<double> zWorkaround(4, 0);

      for (int depth = 0; depth < 4; ++depth)
    zWorkaround[depth] = 200 * (depth + 1);

      RGPointSet ptSet1Workaround("MonPS1", xWorkaround, yWorkaround, zWorkaround);
      RGPointSet ptSet2Workaround("MonPS1", yWorkaround, xWorkaround, zWorkaround);

      std::vector<RGGeneralProperty> props1Workaround;
      props1Workaround.reserve(RGProps.size() / 2);

      std::vector<RGGeneralProperty> props2Workaround;
      props2Workaround.reserve(RGProps.size() / 2);

      for (std::vector<RGGeneralProperty>::const_iterator it = RGProps.begin(); it != RGProps.end(); ++it)
      {
    if (it->GetProperty().front() < 'N')
          props1Workaround.push_back(*it);
    else
          props2Workaround.push_back(*it);
      }

      RGMonitorPointSets monPtSetsWorkaround;

      monPtSetsWorkaround.Add(ptSet1Workaround, props1Workaround);
      monPtSetsWorkaround.Add(ptSet2Workaround, props2Workaround);

      rgInterface->SetSelectedMonitoringPointSets(monPtSetsWorkaround);
      */
      // END WORKAROUND

      rgInterface->setCurrentDepletionStage(RGDepletionStage(0, 0));
      rgInterface->addCommand(GMCommand(typeCommandDefineInitialDate, RGDate(1971, 10, 16)));
      rgInterface->addCommand(GMCommand(typeCommandListMonitorableProperties));
      rgInterface->addCommand(GMCommand(typeCommandSaveGrid));
      rgInterface->addCommand(GMCommand(typeCommandQuit));
      rgInterface->dumpModel();

      EXPECT_TRUE(rgInterface->getNumErrorMsgs() == 0);

      for (int e = 0; e < rgInterface->getNumErrorMsgs(); ++e) {
        std::string m = rgInterface->getNthErrorMsg(e);
      }

      delete rgInterface;

      ::test_model_operations::execute_geomec_shell(cmd.toStdString().c_str(), &output);

      rgInterface = new RGInterface(rescue.toStdString());

      EXPECT_EQ(0, rgInterface->getNumErrorMsgs());

      int elts = 1000;

      std::vector<double> x(4, 250);
      std::vector<double> y(4, 300);
      std::vector<double> z(4, 0);

      for (int depth = 0; depth < 4; ++depth)
        z[depth] = 200 * (depth + 1);

      RGPointSet ptSet1("MonPS1", x, y, z);
      RGPointSet ptSet2("MonPS2", y, x, z);

      std::vector<RGGeneralProperty> props = rgInterface->GetAvailableMonitorableProperties();

      EXPECT_TRUE(props.size() > 0);

      props1.reserve(props.size() / 2);

      props2.reserve(props.size() / 2);

      for (std::vector<RGGeneralProperty>::const_iterator it = props.begin(); it != props.end(); ++it) {
        if (it->GetProperty().front() < 'N')
          props1.push_back(*it);
        else
          props2.push_back(*it);
      }

      last = (int)props2.size() - 1;

      RGMonitorPointSets monPtSets;

      monPtSets.Add(ptSet1, props1);
      monPtSets.Add(ptSet2, props2);

      rgInterface->setCurrentDepletionStage(RGDepletionStage(1, 3.2E7));
      rgInterface->saveProperty(RGProperty(RGPropertyType::propElementFluidPressure), std::vector<double>(elts, 50));
      rgInterface->saveProperty(RGProperty(RGPropertyType::propElementYoung), std::vector<double>(elts, -1));
      rgInterface->SetSelectedMonitoringPointSets(monPtSets);
      rgInterface->clearCommandList();
      rgInterface->addCommand(GMCommand(typeCommandLoadProperty, RGProperty(RGPropertyType::propElementFluidPressure)));
      rgInterface->addCommand(GMCommand(typeCommandLoadProperty, RGProperty(RGPropertyType::propElementYoung)));
      rgInterface->addCommand(GMCommand(typeCommandLoadMonitorPointSets));
      rgInterface->addCommand(GMCommand(typeCommandCalculate));
      rgInterface->addCommand(GMCommand(typeCommandEvaluateMonitorValues));
      rgInterface->addCommand(GMCommand(typeCommandQuit));
      rgInterface->dumpModel();

      EXPECT_TRUE(rgInterface->getNumErrorMsgs() == 0);

      delete rgInterface;

      rgInterface = new RGInterface(rescue.toStdString());

      RGMonitorPointSets ptSets = rgInterface->GetSelectedMonitoringPointSets();

      EXPECT_EQ(monPtSets.GetNumPointSets(), ptSets.GetNumPointSets());

      if (monPtSets.GetNumPointSets() == ptSets.GetNumPointSets()) {
        EXPECT_EQ(ptSet1.GetNumPoints(), ptSets.GetPointSet(0).GetNumPoints());
        EXPECT_EQ(ptSet2.GetNumPoints(), ptSets.GetPointSet(1).GetNumPoints());
        EXPECT_EQ(props1.size(), ptSets.GetProperties(0).size());
        EXPECT_EQ(props2.size(), ptSets.GetProperties(1).size());

        if (props1.size() == ptSets.GetProperties(0).size()) {
          size_t same = 0;
          size_t size = props1.size();

          const std::vector<RGGeneralProperty> RGprops = ptSets.GetProperties(0);

          for (size_t i = 0; i < size; ++i) {
            if (props1[i].GetProperty() != RGprops[i].GetProperty())
              continue;
            if (props1[i].GetQuantity() != RGprops[i].GetQuantity())
              continue;
            if (props1[i].GetSupport() != RGprops[i].GetSupport())
              continue;
            ++same;
          }

          EXPECT_EQ(same, size);
        }

        if (props2.size() == ptSets.GetProperties(1).size()) {
          size_t same = 0;
          size_t size = props2.size();

          const std::vector<RGGeneralProperty> RGprops = ptSets.GetProperties(1);

          for (size_t i = 0; i < size; ++i) {
            if (props2[i].GetProperty() != RGprops[i].GetProperty())
              continue;
            if (props2[i].GetQuantity() != RGprops[i].GetQuantity())
              continue;
            if (props2[i].GetSupport() != RGprops[i].GetSupport())
              continue;
            ++same;
          }

          EXPECT_EQ(same, size);
        }

        if (ptSet1.GetNumPoints() == ptSets.GetPointSet(0).GetNumPoints()) {
          for (size_t i = 0; i < ptSet1.GetNumPoints(); ++i) {
            double x0, y0, z0, x1, y1, z1;
            ptSet1.GetNthPoint(i, x0, y0, z0);
            ptSets.GetPointSet(0).GetNthPoint(i, x1, y1, z1);
            EXPECT_EQ(x0, x1);
            EXPECT_EQ(y0, y1);
            EXPECT_EQ(z0, z1);
          }
        }
        if (ptSet2.GetNumPoints() == ptSets.GetPointSet(1).GetNumPoints()) {
          for (size_t i = 0; i < ptSet2.GetNumPoints(); ++i) {
            double x0, y0, z0, x1, y1, z1;
            ptSet2.GetNthPoint(i, x0, y0, z0);
            ptSets.GetPointSet(1).GetNthPoint(i, x1, y1, z1);
            EXPECT_EQ(x0, x1);
            EXPECT_EQ(y0, y1);
            EXPECT_EQ(z0, z1);
          }
        }
      }
    }

    delete rgInterface;

    output.clear();
    ::test_model_operations::execute_geomec_shell(cmd.toStdString().c_str(), &output);

    std::cout << output.toStdString() << std::endl;

    rgInterface = new RGInterface(rescue.toStdString());

    EXPECT_TRUE(rgInterface->getNumErrorMsgs() == 0);

    if (rgInterface->getNumErrorMsgs() == 0) {
      RGMonitorValues values = rgInterface->GetMonitorValues();

      EXPECT_EQ(1, values.GetDepletionStage().getDepletionStage());
      EXPECT_EQ(2, values.GetPointSets().GetNumPointSets());

      for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(1E10, values.Get(1, i, last));
      }

      // TODO: check more values
    } else {
      for (size_t e = 0; e < rgInterface->getNumErrorMsgs(); ++e)
        std::cout << "Unexpected RGInterface error: " << rgInterface->getNthErrorMsg(e) << std::endl;
    }

    rgInterface->setCurrentDepletionStage(RGDepletionStage(2, 6.4E7));
    rgInterface->clearCommandList();
    rgInterface->addCommand(GMCommand(typeCommandCalculate));
    rgInterface->addCommand(GMCommand(typeCommandEvaluateMonitorValues));
    rgInterface->addCommand(GMCommand(typeCommandQuit));
    rgInterface->dumpModel();

    delete rgInterface;

    // We're not in hibernation mode, so we can't run this without having the previously generated model available.
    // We might set up a D1 model, but then we still cannot test it very well for the pointsets, as these are created
    // anew. And if we can't test it well, it doesn't make a lot of sense.

    /*
    output.clear();
    ::test_model_operations::execute_geomec_shell(cmd.toStdString().c_str(), &output);

    std::cout << output.toStdString() << std::endl;

    rgInterface = new RGInterface(rescue.toStdString());

    EXPECT_TRUE(rgInterface->getNumErrorMsgs() == 0);

    {
        RGMonitorValues values = rgInterface->GetMonitorValues();

        EXPECT_EQ(2, values.GetDepletionStage().getDepletionStage());
        EXPECT_EQ(2, values.GetPointSets().GetNumPointSets());

        // TODO: check values
    }

    delete rgInterface;
    */

    // Test for Bug 399915: Monitoring points: missing properties and component names + some icons wrong
    // Test is not yet complete, first check missing properties

    QString gm5File = QString(m_ModelsPath + input[testNr].model).replace(".gm5", "_D1.gm5");

    CModelBase *pModel = test_lib::TestLib::loadModel(gm5File);

    EXPECT_TRUE(pModel);

    if (pModel) {
      std::set<std::string> inputProps, outputProps;

      for (std::vector<RGGeneralProperty>::iterator it = props1.begin(); it != props1.end(); ++it)
        inputProps.insert(transform_monps_propname(it->GetProperty()));
      for (std::vector<RGGeneralProperty>::iterator it = props2.begin(); it != props2.end(); ++it)
        inputProps.insert(transform_monps_propname(it->GetProperty()));

      int nonComponent_count = 0;
      int unusedComponent_count = 0;

      const IPointSet *monPS1 = test_lib::TestLib::getPointSet(pModel, "MonPS1");

      EXPECT_TRUE(monPS1);

      if (monPS1) {
        int nvsSize = monPS1->NodalValueSetSize();

        outputProps.insert("Northing_D1");
        outputProps.insert("Easting_D1");
        outputProps.insert("Depth_D1");

        for (int i = 3; i < nvsSize; ++i) {
          const CNodalValueSet &nvs = monPS1->NodalValueSet(i);

          std::string name = transform_monps_propname(nvs);
          if (name.empty())
            ++nonComponent_count;
          else if (name.back() == '-')
            ++unusedComponent_count;
          else
            outputProps.insert(name);
        }
      }

      const IPointSet *monPS2 = test_lib::TestLib::getPointSet(pModel, "MonPS2");

      EXPECT_TRUE(monPS2);

      if (monPS2) {
        int nvsSize = monPS2->NodalValueSetSize();

        for (int i = 3; i < nvsSize; ++i) {
          const CNodalValueSet &nvs = monPS2->NodalValueSet(i);

          std::string name = transform_monps_propname(nvs);
          if (name.empty())
            ++nonComponent_count;
          else if (name.back() == '-')
            ++unusedComponent_count;
          else
            outputProps.insert(name);
        }
      }

      std::set<std::string> missing, extra;
      std::set_difference(inputProps.begin(), inputProps.end(), outputProps.begin(), outputProps.end(),
                          std::inserter(missing, missing.end()));
      std::set_difference(outputProps.begin(), outputProps.end(), inputProps.begin(), inputProps.end(),
                          std::inserter(extra, extra.end()));

      if (!missing.empty()) {
        for (std::set<std::string>::iterator it = missing.begin(); it != missing.end(); ++it)
          std::cout << "Input property '" << (*it) << "' missing in model pointsets" << std::endl;
      }

      if (!extra.empty()) {
        for (std::set<std::string>::iterator it = extra.begin(); it != extra.end(); ++it)
          std::cout << "Pointset property '" << (*it) << "' not present in input" << std::endl;
      }

      // Uncomment these once bug is resolved
      EXPECT_EQ(0, nonComponent_count);     // all NodalValueSets should be linked to a component
      EXPECT_EQ(42, unusedComponent_count); // 14 tensors only have the main/diagonal components with values, the three
                                            // other components are named "-": 14 * 3 = 42
      EXPECT_EQ(props1.size() + props2.size(), inputProps.size());
      EXPECT_EQ(props1.size() + props2.size(), outputProps.size());
      EXPECT_TRUE(missing.empty());
      EXPECT_TRUE(extra.empty());

      test_lib::TestLib::closeModel(&pModel);
    }

    QDir(rescue).removeRecursively();
    QDir(rgitmp).removeRecursively();
  }
}

// Ruby's unit test has SetAvailableMonitorableProperties in the first block, and that works fine
// But we need it to be in the second block, because that is the actual workflow.
// And that doesn't work. The properties ARE written to the rescue file, but we can't read them in the third block.
// Why???
// Because LookupTables in RESCUE don't change after initial creation:
// http://w3.energistics.org/rescue/overview_docs/rescue6.htm#RescueLookup
// If the tables are dropped first, then it works.
// (Interesting that the data DOES show up in the file.)
// THIS PROBLEM CAN BE ADJUSTED ONCE WE GET A NEW RGINTERFACE; and let's keep it around.
//
// Update: RGInterface has been fixed.
void ModelOperationsRGI::monitoring_points_problem_test() {
  QString rescue = m_OutputPath + "rgi";
  QString rgitmp = m_OutputPath + "rgitmp";

  QDir(m_OutputPath).mkdir("rgitmp");

  QString cleanPath = QDir::current().absoluteFilePath(
      m_ModelsPath); // TODO: taken from geomec_shell; should be moved somewhere more global
  cleanPath = QDir::current().cleanPath(cleanPath);
  cleanPath = QDir::toNativeSeparators(cleanPath);

  QDir(rescue).removeRecursively();

  RGInterface *rgInterface = nullptr;

  std::vector<RGGeneralProperty> propList = {{"prop1", "TimeD", RGSupport::Element},
                                             {"prop2", "VolumetricHeat", RGSupport::IntegrationPoint},
                                             {"prop3", "Temperature", RGSupport::IntegrationPoint},
                                             {"prop4", "Percentage", RGSupport::Node}};

  std::string testModelName = rescue.toStdString();

  { // Flow writes
    RGInterface rgi(testModelName);
    rgi.setCurrentDepletionStage(RGDepletionStage(RG_INITIAL_STAGE, 0));
    rgi.addCommand(GMCommand(typeCommandDefineInitialDate, RGDate(1971, 10, 16)));
    rgi.addCommand(GMCommand(typeCommandListMonitorableProperties));
    rgi.addCommand(GMCommand(typeCommandQuit));
    rgi.dumpModel();
  }
  { // Geomec writes
    RGInterface rgi(testModelName);
    rgi.SetAvailableMonitorableProperties(propList);
    rgi.dumpModel();
  }
  { // Flow reads
    RGInterface rgi(testModelName);
    std::vector<RGGeneralProperty> propTempList = rgi.GetAvailableMonitorableProperties();

    EXPECT_EQ(propList.size(), propTempList.size());

    if (propList.size() == propTempList.size()) {
      for (size_t i = 0; i < propTempList.size(); ++i) {
        EXPECT_EQ(propList[i].GetProperty(), propTempList[i].GetProperty());
        EXPECT_EQ(propList[i].GetQuantity(), propTempList[i].GetQuantity());
        EXPECT_EQ(propList[i].GetSupport(), propTempList[i].GetSupport());
      }
    }
  }

  QDir(rescue).removeRecursively();
  QDir(rgitmp).removeRecursively();
}

void ModelOperationsRGI::log_messages_test() {
  QString rescue = m_OutputPath + "rgi";
  QString rgitmp = m_OutputPath + "rgitmp";

  QDir(m_OutputPath).mkdir("rgitmp");

  QString cleanPath = QDir::current().absoluteFilePath(
      m_ModelsPath); // TODO: taken from geomec_shell; should be moved somewhere more global
  cleanPath = QDir::current().cleanPath(cleanPath);
  cleanPath = QDir::toNativeSeparators(cleanPath);

  QDir(rescue).removeRecursively();

  QString cmd =
      "--interface rgi /ROCKMECHFILE:" + rescue + PATH_DELIM + "rgi.bin " + m_ModelsPath + "TestRGILogMessages.gm5";

  QString output;

  RGInterface *rgInterface = new RGInterface(rescue.toStdString());
  rgInterface->setCurrentDepletionStage(RGDepletionStage(0, 0));
  rgInterface->addCommand(GMCommand(typeCommandDefineInitialDate, RGDate(1971, 10, 16)));
  rgInterface->addCommand(GMCommand(typeCommandSaveGrid));
  rgInterface->addCommand(GMCommand(typeCommandQuit));
  rgInterface->dumpModel();

  EXPECT_TRUE(rgInterface->getNumErrorMsgs() == 0);

  delete rgInterface;

  ::test_model_operations::execute_geomec_shell(cmd.toStdString().c_str(), &output);

  rgInterface = new RGInterface(rescue.toStdString());
  EXPECT_TRUE(rgInterface->getNumErrorMsgs() == 0);

  rgInterface->setCurrentDepletionStage(RGDepletionStage(1, 3.2E7));
  rgInterface->saveProperty(RGProperty(RGPropertyType::propElementFluidPressure), std::vector<double>(224, 50));
  rgInterface->clearCommandList();
  rgInterface->addCommand(GMCommand(typeCommandLoadProperty, RGProperty(RGPropertyType::propElementFluidPressure)));
  rgInterface->addCommand(GMCommand(typeCommandCalculate));
  rgInterface->addCommand(GMCommand(typeCommandQuit));
  rgInterface->dumpModel();

  delete rgInterface;

  output.clear();
  ::test_model_operations::execute_geomec_shell(cmd.toStdString().c_str(), &output);

  rgInterface = new RGInterface(rescue.toStdString());
  EXPECT_TRUE(rgInterface->getNumErrorMsgs() > 1);

  bool foundExpected = false;
  for (size_t i = 0; i < rgInterface->getNumErrorMsgs(); ++i) {
    // this message can only come from the global message observer
    if (rgInterface->getNthErrorMsg(i) ==
        "Hexamodel with submeshregions does not support quadratic elements per formation; please adjust your input")
      foundExpected = true;
  }

  delete rgInterface;

  EXPECT_TRUE(foundExpected);

  QDir(rescue).removeRecursively();
  QDir(rgitmp).removeRecursively();
}

///////////////////////////////////////////////////////////////////////////////
//
//
//						ModelOperationsSkua
//
//
///////////////////////////////////////////////////////////////////////////////

ModelOperationsSkua::ModelOperationsSkua()
    : m_ModelsPath(test_lib::TestLib::ModelPath()),
      m_OutputPath(GetGeomecTempPathExt(CTempPath::TEMP_GENERAL) + PATH_DELIM) {}

void ModelOperationsSkua::import_test() {
  QString soBaseFile = "TestSkuaImport_1";

  QString inputFile = m_ModelsPath + soBaseFile + ".so";
  QString outputFile = m_OutputPath + soBaseFile + ".gm5";

  QString cmd = "--interface gm \"" + inputFile + "\" \"" + outputFile + "\"";

  QString output;
  ::test_model_operations::execute_geomec_shell(cmd.toStdString().c_str(), &output);

  std::cout << output.toStdString() << std::endl;

  CModelBase *pModel = test_lib::TestLib::loadModel(outputFile, &m_Logger);

  EXPECT_TRUE(pModel);

  if (!pModel)
    return;

  EXPECT_EQ(25824, pModel->Mesh().Mesh().ElementSize());

  CTetraFormationEntry *tetraFormationEntry =
      static_cast<CTetraFormationEntry *>(pModel->GraphEntry(MD_TETRA_FORMATION));
  CTetraFormationEntry::TNodeSet stNodes = tetraFormationEntry->EntryNodes();

  for (CTetraFormationEntry::TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
    const CTetraFormation *formation = *it;

    switch (formation->Order()) {
    case 0:
      EXPECT_TRUE("Zone-Generated_Top_Horizon" == formation->Name());
      break;
    case 1:
      EXPECT_TRUE("Zone-ZONEID_1_2" == formation->Name());
      break;
    case 2:
      EXPECT_TRUE("Zone-ZONEID_2_3" == formation->Name());
      break;
    case 3:
      EXPECT_TRUE("Zone-Bottom" == formation->Name());
      break;
    default:
      EXPECT_TRUE(false);
    }
  }

  test_lib::TestLib::closeModel(&pModel);

  QFile::remove(outputFile);
}

}; // namespace test_model_operations
