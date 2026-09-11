#include "TriangleSurface.h"
#include "TriSurface.h"

#include <fstream>


namespace
{

class TriangleTestData
{
public:
  CTriangleSurface surface;
  CTriangleSurface::TPointVec vcPoints;
	CTriangleSurface::TSegmentVec vcSegments;
  CRegion *region1;
  CRegion *region2;

  bool area_constraint_value;
  bool create_mesh;

  CTriangleSurface *surfacePtr;

  TriangleTestData() : region1(0), region2(0), area_constraint_value(true), create_mesh(true), surfacePtr(0)
  {
    vcPoints.reserve(8);
    vcSegments.reserve(8);
  }
};


typedef test_fw51::TestInitInfo<TriangleTestData> TriangleTestInitInfo;
typedef test_fw51::TestInfo<TriangleTestData> TriangleTestInfo;




class Fw51TriangleTest : public test_fw51::Fw51BaseTestTemplate<TriangleTestInfo>
{
protected:

  Fw51TriangleTest()
  {
    m_ModelsPath.append("Triangle/");
    m_OutputPath.append("Triangle/");
  }

  void DumpMesh(std::ofstream& os, const CTriangleSurface &surface)
  {
    os.precision(6);
    os.setf(std::ios::fixed, std:: ios::floatfield);

	  os << "Number of nodes: " << surface.PointSize() << std::endl;
	  for(int i = 0; i < surface.PointSize(); ++i)
    	os << "node " << i << ": x = " << surface.Point(i).X() << ", y = " << surface.Point(i).Y() << std::endl;

    os << "Number of triangles: " << surface.FaceSize() << std::endl;

	  for(int i = 0; i < surface.FaceSize(); ++i)
	  {
      os << "triangle " << i << ": ";
		  const geo::IFace &face = surface.Face(i);
		
      EXPECT_EQ(3, face.NrOfPoints()); // can we do this here?

		  for(int j = 0; j < face.NrOfPoints(); ++j)
        os << face.PointIndex(j) << " ";

		  os << std::endl;
	  }

    os << "Number of regions: " << surface.RegionSize() << std::endl;

	  for(int i = 0; i < surface.RegionSize(); ++i)
	  {
		  const CRegion &region = surface.Region(i);
		  os << "Region " << i << ", number of elements: " << region.TriangleSize() << std::endl;

		  for(size_t j = 0; j < region.TriangleSize(); ++j)
		  {
			  EXPECT_EQ(&region.Triangle(j), &surface.Face(region.Triangle(j).Index()));
			  os << "triangle nr. " << region.Triangle(j).Index() << std::endl;
		  }
	  }
  }

};

typedef Fw51TriangleTest Fw51TriangleLoadTest;




TEST_P(Fw51TriangleTest, CreateMesh)
{
  const TriangleTestInitInfo *inits = GetParam().inits;

  std::ofstream os(m_Output.c_str(), std::fstream::trunc);

  EXPECT_TRUE(os.is_open()) << "Failed to open output file " << m_Output << std::endl;

  TriangleTestData testData;

  for (int i = 0; i < TEST_INIT_INFO_MAXSIZE; ++i)
  {
    if (!inits[i].init)
      break;

    inits[i].init(testData);

    if (testData.region1)
      EXPECT_TRUE(testData.region1->AreaConstraint() == testData.area_constraint_value);
    if (testData.region2)
      EXPECT_TRUE(testData.region2->AreaConstraint() == testData.area_constraint_value);

    if (testData.create_mesh)
      testData.surface.CreateMesh();

    if (inits[i].message)
      os << inits[i].message << std::endl;

    DumpMesh(os, testData.surface);
  }

  os.close();

  EXPECT_TRUE(test_lib::FileCompare::IdenticalText(m_Expected, m_Output));
}

TEST_P(Fw51TriangleLoadTest, Load)
{
  const TriangleTestInitInfo *inits = GetParam().inits;

  std::ofstream os(m_Output.c_str(), std::fstream::trunc);

  EXPECT_TRUE(os.is_open()) << "Failed to open output file " << m_Output << std::endl;

  TriangleTestData testData;

  for (int i = 0; i < TEST_INIT_INFO_MAXSIZE; ++i)
  {
    if (!inits[i].init)
      break;

    inits[i].init(testData);

    if (inits[i].message)
      os << inits[i].message << std::endl;

    if (testData.surfacePtr)
    {
      DumpMesh(os, *testData.surfacePtr);

      delete testData.surfacePtr;
    }
    else
    {
      EXPECT_TRUE(false) << "No surface found" << std::endl;
    }
  }

  os.close();

  EXPECT_TRUE(test_lib::FileCompare::IdenticalText(m_Expected, m_Output));
}





void init_basic(TriangleTestData &testData)
{
	testData.vcPoints.push_back(geo::CPoint(0, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(10, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(10, 10, 0));
	testData.vcPoints.push_back(geo::CPoint(0, 10, 0));

	testData.vcSegments.push_back(std::make_pair(0, 1));
	testData.vcSegments.push_back(std::make_pair(1, 2));
	testData.vcSegments.push_back(std::make_pair(2, 3));
	testData.vcSegments.push_back(std::make_pair(3, 0));

	testData.surface.Points(testData.vcPoints);
	testData.surface.Segments(testData.vcSegments);
}


void init_flat(TriangleTestData &testData)
{
	testData.vcPoints.push_back(geo::CPoint(0, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(1000, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(1000, 10, 0));
	testData.vcPoints.push_back(geo::CPoint(0, 10, 0));

	testData.vcSegments.push_back(std::make_pair(0, 1));
	testData.vcSegments.push_back(std::make_pair(1, 2));
	testData.vcSegments.push_back(std::make_pair(2, 3));
	testData.vcSegments.push_back(std::make_pair(3, 0));

	testData.surface.Points(testData.vcPoints);
	testData.surface.Segments(testData.vcSegments);
}


void init_areaconstraint(TriangleTestData &testData)
{
	testData.vcPoints.push_back(geo::CPoint(0, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(10, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(10, 10, 0));
	testData.vcPoints.push_back(geo::CPoint(0, 10, 0));

	testData.vcSegments.push_back(std::make_pair(0, 1));
	testData.vcSegments.push_back(std::make_pair(1, 2));
	testData.vcSegments.push_back(std::make_pair(2, 3));
	testData.vcSegments.push_back(std::make_pair(3, 0));

	testData.surface.Points(testData.vcPoints);
	testData.surface.Segments(testData.vcSegments);

	CRegion::TSegments rsegs;
	int i;
	for(i = 0; i < 4; i++) rsegs.push_back(i); // put all segments in this region

	testData.region1 = &testData.surface.NewRegion(rsegs, 2);
}


void init_oneregion(TriangleTestData &testData)
{
	testData.vcPoints.push_back(geo::CPoint(0, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(10, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(10, 10, 0));
	testData.vcPoints.push_back(geo::CPoint(0, 10, 0));

	testData.vcSegments.push_back(std::make_pair(0, 1));
	testData.vcSegments.push_back(std::make_pair(3, 0));
	testData.vcSegments.push_back(std::make_pair(1, 2));
	testData.vcSegments.push_back(std::make_pair(2, 3));

	testData.surface.Points(testData.vcPoints);
	testData.surface.Segments(testData.vcSegments);

	CRegion::TSegments rsegs;
	rsegs.push_back(0);
	rsegs.push_back(1);
	rsegs.push_back(2);
	rsegs.push_back(3);

	testData.region1 = &testData.surface.NewRegion(rsegs, 2);
}


void init_twoparts(TriangleTestData &testData)
{
	testData.vcPoints.push_back(geo::CPoint(0, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(10, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(20, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(20, 10, 0));
	testData.vcPoints.push_back(geo::CPoint(10, 10, 0));
	testData.vcPoints.push_back(geo::CPoint(0, 10, 0));

	// ring
	testData.vcSegments.push_back(std::make_pair(0, 1));
	testData.vcSegments.push_back(std::make_pair(1, 2));
	testData.vcSegments.push_back(std::make_pair(2, 3));
	testData.vcSegments.push_back(std::make_pair(3, 4));
	testData.vcSegments.push_back(std::make_pair(4, 5));
	testData.vcSegments.push_back(std::make_pair(5, 0));

	// vertical in the middle
	testData.vcSegments.push_back(std::make_pair(1, 4));

	testData.surface.Points(testData.vcPoints);
	testData.surface.Segments(testData.vcSegments);
}


void init_tworegions(TriangleTestData &testData)
{
	testData.vcPoints.push_back(geo::CPoint(0, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(10, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(20, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(20, 10, 0));
	testData.vcPoints.push_back(geo::CPoint(10, 10, 0));
	testData.vcPoints.push_back(geo::CPoint(0, 10, 0));

	// ring
	testData.vcSegments.push_back(std::make_pair(0, 1));
	testData.vcSegments.push_back(std::make_pair(1, 2));
	testData.vcSegments.push_back(std::make_pair(2, 3));
	testData.vcSegments.push_back(std::make_pair(3, 4));
	testData.vcSegments.push_back(std::make_pair(4, 5));
	testData.vcSegments.push_back(std::make_pair(5, 0));

	// vertical in the middle
	testData.vcSegments.push_back(std::make_pair(1, 4));

	testData.surface.Points(testData.vcPoints);
	testData.surface.Segments(testData.vcSegments);

	CRegion::TSegments rsegs1;
	rsegs1.push_back(0);
	rsegs1.push_back(6);
	rsegs1.push_back(4);
	rsegs1.push_back(5);

	testData.region1 = &testData.surface.NewRegion(rsegs1, 2);

	CRegion::TSegments rsegs2;
	rsegs2.push_back(1);
	rsegs2.push_back(2);
	rsegs2.push_back(3);
	rsegs2.push_back(6);

	testData.region2 = &testData.surface.NewRegion(rsegs2, 2);
}


void init_sharpangle(TriangleTestData &testData)
{
	testData.vcPoints.push_back(geo::CPoint(0, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(1000, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(1000, 10, 0));
	testData.vcPoints.push_back(geo::CPoint(0, 10, 0));

	// ring
	testData.vcSegments.push_back(std::make_pair(0, 1));
	testData.vcSegments.push_back(std::make_pair(1, 2));
	testData.vcSegments.push_back(std::make_pair(2, 3));
	testData.vcSegments.push_back(std::make_pair(3, 0));

	// diagonal
	testData.vcSegments.push_back(std::make_pair(1, 3));

	testData.surface.Points(testData.vcPoints);
	testData.surface.Segments(testData.vcSegments);

	CRegion::TSegments rsegs1;
	rsegs1.push_back(0);
	rsegs1.push_back(4);
	rsegs1.push_back(3);

	testData.region1 = &testData.surface.NewRegion(rsegs1);

	CRegion::TSegments rsegs2;
	rsegs2.push_back(1);
	rsegs2.push_back(2);
	rsegs2.push_back(4);

	testData.region2 = &testData.surface.NewRegion(rsegs2);

  testData.area_constraint_value = false;
}


void init_refinetworegions_0(TriangleTestData &testData)
{
	testData.vcPoints.push_back(geo::CPoint(0, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(10, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(20, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(20, 10, 0));
	testData.vcPoints.push_back(geo::CPoint(10, 10, 0));
	testData.vcPoints.push_back(geo::CPoint(0, 10, 0));

	// ring
	testData.vcSegments.push_back(std::make_pair(0, 1));
	testData.vcSegments.push_back(std::make_pair(1, 2));
	testData.vcSegments.push_back(std::make_pair(2, 3));
	testData.vcSegments.push_back(std::make_pair(3, 4));
	testData.vcSegments.push_back(std::make_pair(4, 5));
	testData.vcSegments.push_back(std::make_pair(5, 0));

	// vertical in the middle
	testData.vcSegments.push_back(std::make_pair(1, 4));

	testData.surface.Points(testData.vcPoints);
	testData.surface.Segments(testData.vcSegments);

	CRegion::TSegments rsegs1;
	rsegs1.push_back(0);
	rsegs1.push_back(6);
	rsegs1.push_back(4);
	rsegs1.push_back(5);

	// request region without area constraint
	testData.region1 = &testData.surface.NewRegion(rsegs1);

	CRegion::TSegments rsegs2;
	rsegs2.push_back(1);
	rsegs2.push_back(2);
	rsegs2.push_back(3);
	rsegs2.push_back(6);

	// request region without area constraint
	testData.region2 = &testData.surface.NewRegion(rsegs2);

  testData.area_constraint_value = false;
	//assert(!region1.AreaConstraint());
	//assert(!region2.AreaConstraint());
}

void init_refinetworegions_1(TriangleTestData &testData)
{
	// now set some area constraints and remesh
	testData.region1->MaximumArea(2);
	testData.region2->MaximumArea(2);

  testData.area_constraint_value = true;
	//assert(region1.AreaConstraint());
	//assert(region2.AreaConstraint());
}


void init_multiplerefinements_0(TriangleTestData &testData)
{
	testData.vcPoints.push_back(geo::CPoint(0, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(10, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(20, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(20, 10, 0));
	testData.vcPoints.push_back(geo::CPoint(10, 10, 0));
	testData.vcPoints.push_back(geo::CPoint(0, 10, 0));

	// ring
	testData.vcSegments.push_back(std::make_pair(0, 1));
	testData.vcSegments.push_back(std::make_pair(1, 2));
	testData.vcSegments.push_back(std::make_pair(2, 3));
	testData.vcSegments.push_back(std::make_pair(3, 4));
	testData.vcSegments.push_back(std::make_pair(4, 5));
	testData.vcSegments.push_back(std::make_pair(5, 0));

	// vertical in the middle
	testData.vcSegments.push_back(std::make_pair(1, 4));

	testData.surface.Points(testData.vcPoints);
	testData.surface.Segments(testData.vcSegments);

	CRegion::TSegments rsegs1;
	rsegs1.push_back(0);
	rsegs1.push_back(6);
	rsegs1.push_back(4);
	rsegs1.push_back(5);

	// request region without area constraint
	testData.region1 = &testData.surface.NewRegion(rsegs1);

	CRegion::TSegments rsegs2;
	rsegs2.push_back(1);
	rsegs2.push_back(2);
	rsegs2.push_back(3);
	rsegs2.push_back(6);

	// request region without area constraint
	testData.region2 = &testData.surface.NewRegion(rsegs2);

  testData.area_constraint_value = false;
	//assert(!region1.AreaConstraint());
	//assert(!region2.AreaConstraint());
}


void init_multiplerefinements_1(TriangleTestData &testData)
{
	// now set some area constraints and remesh
	testData.region1->MaximumArea(2);
	testData.region2->MaximumArea(2);

  testData.area_constraint_value = true;
	//assert(region1.AreaConstraint());
	//assert(region2.AreaConstraint());
}

void init_multiplerefinements_2(TriangleTestData &testData)
{
	// coarsen
	testData.region1->MaximumArea(10);
	testData.region2->MaximumArea(10);

	//assert(region1.AreaConstraint());
	//assert(region2.AreaConstraint());
}


void init_clearandremesh_0(TriangleTestData &testData)
{
	testData.vcPoints.push_back(geo::CPoint(0, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(10, 0, 0));
	testData.vcPoints.push_back(geo::CPoint(10, 10, 0));
	testData.vcPoints.push_back(geo::CPoint(0, 10, 0));

	testData.vcSegments.push_back(std::make_pair(0, 1));
	testData.vcSegments.push_back(std::make_pair(1, 2));
	testData.vcSegments.push_back(std::make_pair(2, 3));
	testData.vcSegments.push_back(std::make_pair(3, 0));

	testData.surface.Points(testData.vcPoints);
	testData.surface.Segments(testData.vcSegments);
}

void init_clearandremesh_1(TriangleTestData &testData)
{
	testData.surface.Clear();

  testData.create_mesh = false;
}

void init_clearandremesh_2(TriangleTestData &testData)
{
	testData.surface.Points(testData.vcPoints);
	testData.surface.Segments(testData.vcSegments);

  testData.create_mesh = true;
}



void BuildSurface(std::vector<geo::CPoint> &vcPoints, std::vector<std::vector<int> > &vcTriangles)
{
  vcPoints.clear();
	vcTriangles.clear();

	// 0    1    2
	// +----+----+
	// |   /|   /|
	// | /  | /  |
	// +----+----+
	// 5    4    3

	vcPoints.push_back(geo::CPoint(0, 0, 0));
	vcPoints.push_back(geo::CPoint(10, 0, 0));
	vcPoints.push_back(geo::CPoint(20, 0, 0));
	vcPoints.push_back(geo::CPoint(20, 10, 0));
	vcPoints.push_back(geo::CPoint(10, 10, 0));
	vcPoints.push_back(geo::CPoint(0, 10, 0));

	// triangles from left to right
	std::vector<int> coords(3);
	coords[0] = 0;
	coords[1] = 1;
	coords[2] = 5;
	vcTriangles.push_back(coords);

  coords[0] = 1;
	coords[1] = 4;
	coords[2] = 5;
	vcTriangles.push_back(coords);

	coords[0] = 1;
	coords[1] = 2;
	coords[2] = 4;
	vcTriangles.push_back(coords);

	coords[0] = 2;
	coords[1] = 3;
  coords[2] = 4;
  vcTriangles.push_back(coords);
}





void init_loadmesh(TriangleTestData &testData)
{
	std::vector<geo::CPoint> vcPoints;
	std::vector<std::vector<int> > vcTriangles;

	BuildSurface(vcPoints, vcTriangles);

	CTriangleSurface *surface = new CTriangleSurface(vcPoints, vcTriangles);

  testData.surfacePtr = surface;
}


void init_surfaceconstruct(TriangleTestData &testData)
{
	// tests the construction of a triangle surface throught the ISurface interface
	std::vector<geo::CPoint> vcPoints;
	std::vector<std::vector<int> > vcTriangles;

	BuildSurface(vcPoints, vcTriangles);

	geo::CTriSurface trisurf(vcPoints, vcTriangles);

  testData.surfacePtr = new CTriangleSurface(trisurf);
}



const TriangleTestInfo mesh_tests[] = {
  { "basic", { { 0, init_basic }, { 0, 0 } } },
  { "flat", { { 0, init_flat }, { 0, 0 } } },
  { "areaconstraint", { { 0, init_areaconstraint }, { 0, 0 } } },
  { "oneregion", { { 0, init_oneregion }, { 0, 0 } } },
  { "twoparts", { { 0, init_twoparts }, { 0, 0 } } },
  { "tworegions", { { 0, init_tworegions }, { 0, 0 } } },
  { "sharpangle", { { 0, init_sharpangle }, { 0, 0 } } },
  { "refinetworegions", { { "Mesh after first run:", init_refinetworegions_0 },
                          { "Mesh after second run:", init_refinetworegions_1 },
                          { 0, 0 } } },
  { "multiplerefinements", { { "Mesh after first run:", init_multiplerefinements_0 },
                             { "Mesh after second run:", init_multiplerefinements_1 },
                             { "Mesh after third run:", init_multiplerefinements_2 },
                             { 0, 0 } } },
  { "clearandremesh", { { "After first run:", init_clearandremesh_0 },
                          { "After Clear:", init_clearandremesh_1 },
                          { "After second run:", init_clearandremesh_2 },
                          { 0, 0 } } }
};


INSTANTIATE_TEST_CASE_P(MainRun, Fw51TriangleTest, ::testing::ValuesIn(mesh_tests));



const TriangleTestInfo load_tests[] = {
  { "loadmesh", { { 0, init_loadmesh }, { 0, 0 } } },
  { "surfaceconstruct", { { 0, init_surfaceconstruct }, { 0, 0 } } }
};


INSTANTIATE_TEST_CASE_P(MainRun, Fw51TriangleLoadTest, ::testing::ValuesIn(load_tests));










}
