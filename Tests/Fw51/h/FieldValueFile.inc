#include "lbcx.h"
#include <fstream>
#include <qtextstream.h>
#include <QApplication>
#include <qfileinfo.h>
#include <stdio.h>
#include "FvGocadFile.h"
#include "FvEclipseFile.h"
#include "FvGocadWellFile.h"
#include "Mesh.h"
#include "IProgressBase.h"


namespace
{


class Fw51FileTest : public test_fw51::Fw51InputBaseTest
{
protected:

  Fw51FileTest()
  {
    m_ModelsPath.append("FieldValueFile/");
    m_OutputPath.append("FieldValueFile/");
  }

};

typedef Fw51FileTest Fw51GocadTest;
typedef Fw51FileTest Fw51GocadWellTest;
typedef Fw51FileTest Fw51EclipseTest;



TEST_P(Fw51GocadTest, ReadFile)
{
  geo::CMesh mesh;
  CGoCadFile file(mesh);
  IProgressBase progress;
  const char *filename = GetSourceName(m_Input.c_str());
  bool opened = file.Open(filename, progress);

  EXPECT_TRUE(opened) << "Open of " << filename << " failed" << endl << file.ErrorMessage().toStdString() << endl;

  file.Save(m_Output.c_str(), progress);

  EXPECT_TRUE(test_lib::FileCompare::IdenticalTokens(m_Expected, m_Output));
}


TEST_P(Fw51GocadWellTest, ReadFile)
{
  QFile of(m_Output.c_str());
  EXPECT_TRUE(of.open(QIODevice::WriteOnly | QIODevice::Truncate)) << "Failed to open file " << m_Output << std::endl;
  QTextStream os(&of);

  CGoCadWellFile file;
  IProgressBase progress;
  const char *filename = GetSourceName(m_Input.c_str());
  bool opened = file.Open(filename, progress);

  EXPECT_TRUE(opened) << "Open of " << filename << " failed" << endl << file.ErrorMessage().toStdString() << endl;

  // Note the the cocad standard has X and Y swapped
  os << " #WELLNAME" << " X" << " Y" << " Z" << " MD\n";
  for (int i = 0; i < file.wellSize(); ++i) {
    for (int n = 0; n < file.pointSize(i); ++n){
      os << file.wellName(i) << ' ';
      os << file.pointAt(i, n).Y() << ' ';
      os << file.pointAt(i, n).X() << ' ';
      os << file.pointAt(i, n).Z() << ' ';
      os << file.tmdAt(i, n) << '\n';
    }
  }

  of.close();

  EXPECT_TRUE(test_lib::FileCompare::IdenticalText(m_Expected, m_Output));
}


TEST_P(Fw51EclipseTest, ReadFile)
{
  CEclipseFile file;
  IProgressBase progress;
  const char *filename = GetSourceName(m_Input.c_str());
  bool opened = file.Open(filename, progress);

  EXPECT_TRUE(opened) << "Open of " << filename << " failed" << endl << file.ErrorMessage().toStdString() << endl;

  file.Save(m_Output.c_str(), progress);

  EXPECT_TRUE(test_lib::FileCompare::IdenticalText(m_Expected, m_Output));
}





INSTANTIATE_TEST_CASE_P(Gocads, Fw51GocadTest, ::testing::Values(
    "gocad_f1h2solid.so",
    "gocad_fault2horz.mx",
    "gocad_fault2horzas.mx",
    "gocad_lensetop.ts",
    "gocad_mix_test.mx",
    "gocad_pie.so",
    "gocad_solid.so",
    "gocad_surface.ts",
    "gocad_test.ts",
    "gocad_topegga.ts",
    "gocad_topsele.ts"
  ));

INSTANTIATE_TEST_CASE_P(GocadWells, Fw51GocadWellTest, ::testing::Values(
    "gocad_well1.mx"
  ));

INSTANTIATE_TEST_CASE_P(Eclipses, Fw51EclipseTest, ::testing::Values(
    "eclipse_basetest.grid",
    "eclipse_small.grid",
    "eclipse_test26.grid"
  ));



}