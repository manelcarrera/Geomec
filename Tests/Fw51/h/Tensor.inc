#include "mfStressTensor.h"
#include "mfTensor.h"
#include "MF.h"
#include "Vector.h"
#include <qtextstream.h>
#include <string>
#include <qfile.h>



namespace
{


class Fw51TensorCommonTest
{
protected:
  void PrintMatrix(QTextStream &os, const geo::IMatrix& matrix)
  {
    os.setRealNumberNotation( QTextStream::ScientificNotation );
    for(int i = 0; i < matrix.ColumnSize(); i++) {
      for(int j = 0; j < matrix.RowSize(); j++) {
        os << matrix.Value(j, i) << '\t';
      }
      os << '\n';
    }
  }

  void PrintTensor(QTextStream &os, const ITensor& tensor)
  {
    if(tensor.Empty())
      os << "Empty Tensor\n";
    else
      PrintMatrix(os, tensor.Matrix());
  }
};


class Fw51TensorFileTest : public test_fw51::Fw51InputBaseTest, public Fw51TensorCommonTest
{
protected:

  Fw51TensorFileTest()
  {
    m_ModelsPath.append("Tensor/");
    m_OutputPath.append("Tensor/");
  }

};


class Fw51TensorTest : public test_fw51::Fw51BaseTest, public Fw51TensorCommonTest
{
protected:

  Fw51TensorTest()
  {
    m_ModelsPath.append("Tensor/");
    m_OutputPath.append("Tensor/");
  }

};

typedef Fw51TensorTest Fw51TensorMechFunTest;


TEST_P(Fw51TensorFileTest, Principals)
{
  QFile of(m_Output.c_str());

  EXPECT_TRUE(of.open(QIODevice::WriteOnly | QIODevice::Truncate)) << "Failed to open file " << m_Output << std::endl;
  QTextStream os(&of);

  os << "\n\n";
  os << "     ************************* TENSOR TEST **********************\n";
  os << "     * Mechanical functions test file                           *\n";
  os << "     * testing the principal values of the tensor               *\n";
  os << "     ************************************************************\n\n";

  QFile file(m_Input.c_str());
  EXPECT_TRUE(file.open(QIODevice::ReadOnly | QIODevice::Text)) << "Failed to open file " << m_Input << std::endl;
  QTextStream stream( &file );

  os << "XX\tYY\tZZ\tXY\tYZ\tXZ\n";  


  for (;;) {
    stream.skipWhiteSpace();
    if (stream.atEnd()) break;

    double xx, yy, zz, xy, yz, xz;
    stream >> xx;
    stream >> yy;
    stream >> zz;
    stream >> xy;
    stream >> yz;
    stream >> xz;
    CTensor tensor(xx, yy, zz, xy, yz, xz);
    os << "TEN\t";
    os << tensor.XX() << "\t";
    os << tensor.YY() << "\t";
    os << tensor.ZZ() << "\t";
    os << tensor.XY() << "\t";
    os << tensor.YZ() << "\t";
    os << tensor.XZ() << "\n";
    
    geo::CMatrix matrix1 = tensor.GetPrincipalDir();
    geo::CMatrix matrix2 = tensor.GetPrincipalStr();

    geo::CVector vcmax(matrix1.Value(0, 2), matrix1.Value(1, 2), matrix1.Value(2, 2));
    geo::CVector vcmed(matrix1.Value(0, 1), matrix1.Value(1, 1), matrix1.Value(2, 1));
    geo::CVector vcmin(matrix1.Value(0, 0), matrix1.Value(1, 0), matrix1.Value(2, 0));

    double Eigen1 = matrix2.Value(0, 0);
    double Eigen2 = matrix2.Value(1, 1);
    double Eigen3 = matrix2.Value(2, 2);

    os << "MAX (" << vcmax.X() << ", " << vcmax.Y() << ", " << vcmax.Z() << ") LENGTH " << Eigen3 << "\n";
    os << "MED (" << vcmed.X() << ", " << vcmed.Y() << ", " << vcmed.Z() << ") LENGTH " << Eigen2 << "\n";
    os << "MIN (" << vcmin.X() << ", " << vcmin.Y() << ", " << vcmin.Z() << ") LENGTH " << Eigen1 << "\n";    

  }
  

  file.close();
  of.close();

  EXPECT_TRUE(test_lib::FileCompare::IdenticalTokens(m_Expected, m_Output));
}



TEST_P(Fw51TensorTest, Tensors)
{
  QFile of(m_Output.c_str());

  EXPECT_TRUE(of.open(QIODevice::WriteOnly | QIODevice::Truncate)) << "Failed to open file " << m_Output << std::endl;
  QTextStream os(&of);

  os << "\n\n";
  os << "     ************************* TENSOR TEST **********************\n";
  os << "     * Mechanical functions test file                           *\n";
  os << "     * testing of the tensor                                    *\n";
  os << "     ************************************************************\n\n";


  double angledeg = 35;
  double anglerad = 0.610865;
  geo::CVector rotationaxis(1,1,1); 

  geo::CMatrix mattest1;
  mattest1.ReSize(5,5);

  mattest1.Value(0,0,-10);
  mattest1.Value(0,1,-9);
  mattest1.Value(0,2,-8);
  mattest1.Value(0,3,-7);
  mattest1.Value(0,4,0);
  mattest1.Value(1,0,0);
  mattest1.Value(1,1,-4);
  mattest1.Value(1,2,-3);
  mattest1.Value(1,3,-2);
  mattest1.Value(1,4,-1);
  mattest1.Value(2,0,0);
  mattest1.Value(2,1,1);
  mattest1.Value(2,2,0);
  mattest1.Value(2,3,3);
  mattest1.Value(2,4,4);
  mattest1.Value(3,0,5);
  mattest1.Value(3,1,6);
  mattest1.Value(3,2,7);
  mattest1.Value(3,3,0);
  mattest1.Value(3,4,9);
  mattest1.Value(4,0,0);
  mattest1.Value(4,1,11);
  mattest1.Value(4,2,12);
  mattest1.Value(4,3,13);
  mattest1.Value(4,4,14);

  geo::CMatrix mattest3(3,3);
  mattest3.Value(0,0,2.731821289);
  mattest3.Value(1,0,0.619763867);
  mattest3.Value(2,0,-0.319602942);
  mattest3.Value(0,1,0.619763867);
  mattest3.Value(1,1,4.649066665);
  mattest3.Value(2,1,0.738605815);
  mattest3.Value(0,2,-0.319602942);
  mattest3.Value(1,2,0.738605815);
  mattest3.Value(2,2,2.619112046);
  CTensor tenstest1;
  os << "The tensor below is empty, no values should be printed.\n";
  PrintTensor(os, tenstest1);
  os <<  "\n";

////Non-default constructor////////////////////////////////////////////////////////////
  geo::CMatrix mattest11(3,3);

  mattest11.Value(0,0,1.0);
  mattest11.Value(0,1,2.0);

  mattest11.Value(0,2, 3.0);
  mattest11.Value(1,0, 2.0); //mattest11.Value(1,0, 4.0);

  mattest11.Value(1,1,5.0);
  mattest11.Value(1,2,6.0);

  mattest11.Value(2,0,3.0); //mattest11.Value(2,0,7.0);

  mattest11.Value(2,1,6.0); //mattest11.Value(2,1,8.0);

  mattest11.Value(2,2,9.0);

  CTensor tenstest2(mattest11);
  os <<  "The tensor below should be the same as mattest11.\n";
  PrintTensor(os, tenstest2);
  os <<  "\n";

////Stress tensor from stress vector////////////////////////////////////////////////////
  geo::CMatrix mattest12(1,6);
  mattest12.Value(0,0,1.0);
  mattest12.Value(0,1,2.0);
  mattest12.Value(0,2,3.0);
  mattest12.Value(0,3,4.0);
  mattest12.Value(0,4,5.0);
  mattest12.Value(0,5,6.0);

  CTensor tenstest3;
  tenstest3.CreateStressTensorFromVector(mattest12);
  os <<  "The tensor below should be mattest12 in stress tensor form.\n";
  PrintTensor(os, tenstest3);
  os <<  "\n";

////Strain tensor from strain tensor///////////////////////////////////////////////////
  tenstest3.CreateStrainTensorFromVector(mattest12);
  os <<  "The tensor below should be mattest12 in strain tensor form.\n";
  PrintTensor(os, tenstest3);
  os <<  "\n";

////Rotated tensor ("Rtransposed * sigma * R", R=rotation matrix, sigma is tensor) angle in degrees/////
  CTensor tensrot = tenstest3.RotateTensorDeg(rotationaxis,angledeg );
  os <<  "The tensor below is the rotated tensor by 35 degrees around (1,1,1).\n";
  PrintTensor(os, tensrot);
  os <<  "\n";

////Rotated tensor ("Rtransposed * sigma * R", R=rotation matrix, sigma is tensor) angle in radians/////
  tensrot = tenstest3.RotateTensorRad(rotationaxis,anglerad );
  os <<  "The tensor below is the rotated tensor by 0.610865 radians around (1,1,1).\n";
  PrintTensor(os, tensrot);
  os <<  "\n";

////Principal values of tensor//////////////////////////////////////////////////////////////////////////
  CTensor tenstest4(mattest3);
  geo::CMatrix principval = tenstest4.GetPrincipalStr();
  os <<  "The matrix below has the principal values of the tensor on its diagonal.\n";
  PrintMatrix(os, principval);
  os <<  "\n";


////Principal directions of tensor//////////////////////////////////////////////////////////////////////
  geo::CMatrix principdir = tenstest4.GetPrincipalDir();
  os <<  "The matrix below has the principal directions of the tensor in its columns.\n";
  PrintMatrix(os, principdir);
  os <<  "\n\n";

  os << "     ********************** END TENSOR TEST *******************" << '\n';

  of.close();

  EXPECT_TRUE(test_lib::FileCompare::IdenticalTokens(m_Expected, m_Output));
}


TEST_P(Fw51TensorMechFunTest, MechanicalFunctions)
{
  QFile of(m_Output.c_str());

  EXPECT_TRUE(of.open(QIODevice::WriteOnly | QIODevice::Truncate)) << "Failed to open file " << m_Output << std::endl;
  QTextStream os(&of);

  os << "     ************************* MECH. FUNC. TEST **********************\n";
  os << "     * Mechanical functions test file                                *\n";
  os << "     * testing of the Mechanical functions                           *\n";
  os << "     *****************************************************************\n";
  os << "\n";

  double dInitPress = 500.0;
  double dCurrPress = 250.0;

  CStressTensor InitStress(100, 250, 500, 30, 40, 50);
  CStressTensor CurrStress(80, 200, 400, 30, 40, 50);

  os <<  "Gamma Max.\n";
  os << MF::GammaMax(InitStress, CurrStress, dInitPress, dCurrPress, MF::EFFECTIVE).Value() << "\n";

  os <<  "Gamma Med.\n";
  os << MF::GammaMed(InitStress, CurrStress, dInitPress, dCurrPress, MF::EFFECTIVE).Value() << "\n";

  os <<  "Gamma Min.\n";
	os << MF::GammaMin(InitStress, CurrStress, dInitPress, dCurrPress, MF::EFFECTIVE).Value() << "\n";

	os <<  "Gamma Vertical.\n";
	os << MF::GammaVertical(InitStress, CurrStress, dInitPress, dCurrPress, MF::EFFECTIVE).Value() << "\n";

	os <<  "Gamma Volume.\n";
	os << MF::GammaVolume(InitStress, CurrStress, dInitPress, dCurrPress, MF::EFFECTIVE).Value() << "\n";

	os <<  "Shear Capacity.\n";
	os << MF::ShearCapacity(InitStress, 30, 200, MF::DEG).Value() << "\n";

  of.close();

  EXPECT_TRUE(test_lib::FileCompare::IdenticalTokens(m_Expected, m_Output));
}




INSTANTIATE_TEST_CASE_P(MainRun, Fw51TensorFileTest, ::testing::Values(
    "principal_I.dat"
  ));

INSTANTIATE_TEST_CASE_P(MainRun, Fw51TensorTest, ::testing::Values(
    "tensor_test"
  ));

INSTANTIATE_TEST_CASE_P(MainRun, Fw51TensorMechFunTest, ::testing::Values(
    "functions"
  ));



}
