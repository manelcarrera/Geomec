#include "ModelOperations.h"
#include "GeomecUtils.h"
#include "MeshDataCacher.h"
#include "ModelBase.h"
#include "ResultCache.h"
#include "ResultRegister.h"
#include "TetraMesh.h"

namespace test_model_operations {

int LibraryLoad_ModelOperations() { return LibraryLoad_SpecialModelOperations(); }

TEST_P(ModelOperationsBatch, Scripts) { execute(); }

TEST_P(ModelOperationsShell, GeomecShell) { ::test_model_operations::execute_geomec_shell(GetParam()); }

const char *scripts[] = {
    "load TestLoadSave.gm4; save_as tmp.gm4; close; load; close",
    "load TestLoadAndCalculate.gm4; run; close",
    "load TestLoadSave.gm4; mesh 0; save_as tmp.gm4; close; load; mesh 1; save; close; load; mesh 0; close",
    "load TestImportMeshFromGoCadFile.gm4; import mesh TestImportMeshFromGoCadFile.so; close",
    "import gocad TestGoCadImport.so 0",
    "import gocad TestGoCadImport.so 1",
    "load TestPetrelHexaModel.gm4; import petrel TestPetrelIncrementZ.txt SI SI; close",
    "load TestPetrelHexaModel.gm4; import petrel TestPetrelDecrementZ.txt SI SI; close",
    "load TestPetrelHexaModel.gm4; import petrel TestPetrelDepthFIELD.txt SI FIELD; close",
    "load TestPetrelHexaModel.gm4; import petrel TestPetrelLateralFIELD.txt FIELD SI; close",
    "load TestPetrelTetraModel.gm4; import petrel TestPetrelIncrementZ.txt SI SI; close",
    "load TestPetrelTetraModel.gm4; import petrel TestPetrelDecrementZ.txt SI SI; close",
    "load TestPetrelTetraModel.gm4; import petrel TestPetrelDepthFIELD.txt SI FIELD; close",
    "load TestPetrelTetraModel.gm4; import petrel TestPetrelLateralFIELD.txt FIELD SI; close",
    "load D2.gm4; run; close",
    "load D2.gm4; run 1; close",
    "load D4.gm4; run; close",
    "load D4.gm4; run 1; close"};

INSTANTIATE_TEST_CASE_P(ModelOperationsBatch, ModelOperationsBatch, ::testing::ValuesIn(scripts));

const char *shell_scripts[] = {"--version"};

INSTANTIATE_TEST_CASE_P(ModelOperationsShell, ModelOperationsShell, ::testing::ValuesIn(shell_scripts));

TEST_F(ModelOperationsCORA, CORA) { some_grouped_tests(); }

TEST_F(ModelOperationsRGI, RGI) { some_grouped_tests(); }

TEST_F(ModelOperationsRGI, MonitoringPoints) { monitoring_points_test(); }

TEST_F(ModelOperationsRGI, MonitoringPointsProblem) { monitoring_points_problem_test(); }

TEST_F(ModelOperationsRGI, LogMessages) { log_messages_test(); }

TEST_F(ModelOperationsSkua, Import) { import_test(); }

} // namespace test_model_operations