#include "Fw51.h"
#include <gtest/gtest.h>

namespace test_fw51 {

int LibraryLoad_Fw51() { return 0; }

} // namespace test_fw51

// the VS linker discards empty object files, so we include them here instead of in project

#include "FieldValueFile.inc"
#include "Geometry.inc"
#include "Model.inc"
#include "Tensor.inc"
#include "TetMesh.inc"
#include "Triangle.inc"
