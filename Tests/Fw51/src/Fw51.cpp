#include <gtest/gtest.h>
#include "Fw51.h"


namespace test_fw51
{

int LibraryLoad_Fw51() { return 0; }

}



// the VS linker discards empty object files, so we include them here instead of in project

#include "Triangle.inc"
#include "Tensor.inc"
#include "Geometry.inc"
#include "FieldValueFile.inc"
#include "TetMesh.inc"
#include "Model.inc"

