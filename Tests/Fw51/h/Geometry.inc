#define USE_GEO_STREAM_ //stream template functions for basic geometry objects (see GeoStream.h)
#include "dimple.h"
#include "IPoint.h"
#include "IVector.h"
#include "ILine.h"
#include "IPlane.h"
#include "ISurface.h"
#include "GeoStream.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <qtextstream.h>
#include <string>
#include <qfile.h>
#include "ConvexHull_2D.h"
#include "ConvexHull.h"
#include "CylindricHull.h"
#include "Plane.h"
#include "PolyLine.h"
#include "Rectangle.h"
#include "Hexahedron.h"
#include "IProgressBase.h"
#include "IMesh3D.h"
#include "ValueSet.h"
#include "Triangle.h"
#include "Polygon.h"
#include "ISurface.h"
#include "BucketKernel.h"
#include "Tetrahedron.h"
#include "Sphere.h"
#include "RingFactory.h"
#include "BodyGroup.h"
#include "IParallelInitializationCallback.h"

namespace
{


class GeometryTestData
{
  QTextStream &os;
public:
  QTextStream& OS() { return os; }

  GeometryTestData(QTextStream &os) : os(os) {}

  void NewLine(int nr = 1)
  {
    for( int i=0; i<nr; i++ ) os << '\n';
  }

  void PrintPoint(const geo::IPoint& v, const char* c = 0)
  {
    if ( c != 0 ) os << c;
    os << v << "\n\n"; 
  }

  void PrintMatrix(const geo::CMatrix& mat, const char* c = 0)
  {
    int prec  = os.realNumberPrecision();
    os.setRealNumberPrecision( 6 );

    QTextStream::RealNumberNotation rnn = os.realNumberNotation();
    os.setRealNumberNotation( QTextStream::FixedNotation );

    if ( c ) os << c ;
    os << '\n' << mat << '\n';

    os.setRealNumberPrecision( prec );
    os.setRealNumberNotation( rnn );
  }


  void PrintLine(const geo::ILine& l, const char* c = 0)
  {
    if ( c ) os << c;
    os << '\n' << l << '\n';
  }

  void PrintPolyLine(const geo::IPolyLine& polyline, const char* c = 0)
  {
    if ( c ) os << c;
    os << '\n' << polyline << '\n';
  }


  void PrintPlane(const geo::IPlane& plane, const char* c = 0)
  {
    if ( c ) os << c;
    os << '\n' << plane << '\n';
  }

  void PrintObject(const geo::IObject& object, const char* c = 0)
  {
  	const geo::IPoint* point= dynamic_cast<const geo::IPoint*>(&object);
  	if ( point )
	  {
		  PrintPoint(*point,c);
		  return;
	  }

	  const geo::IVector* vector= dynamic_cast<const geo::IVector*>(&object);
	  if(vector)
	  {
		  PrintPoint(geo::CPoint(*vector),c);
		  return;
	  }

	  const geo::ILine* line= dynamic_cast<const geo::ILine*>(&object);
	  if(line)
	  {
		  PrintLine(*line,c);
		  return;
	  }

	  const geo::IPlane* plane= dynamic_cast<const geo::IPlane*>(&object);
	  if(plane)
	  {
		  PrintPlane(*plane,c);
		  return;
	  }

	  const geo::IPolyLine* polyline= dynamic_cast<const geo::IPolyLine*>(&object);
	  if(polyline)
	  {
		  PrintPolyLine(*polyline,c);
		  return;
	  }

  }

};

typedef test_fw51::TestInitInfo<GeometryTestData> GeometryTestInitInfo;
typedef test_fw51::TestInfo<GeometryTestData> GeometryTestInfo;


class Fw51GeometryTest : public test_fw51::Fw51BaseTestTemplate<GeometryTestInfo>
{
protected:
  Fw51GeometryTest()
  {
    m_ModelsPath.append("Geometry/");
    m_OutputPath.append("Geometry/");
  }

};

class Fw51GeometryRingTest : public test_fw51::Fw51InputBaseTest
{
protected:
  Fw51GeometryRingTest()
  {
    m_ModelsPath.append("Geometry/");
    m_OutputPath.append("Geometry/");
  }

  typedef std::vector<int> TIndexVec;
  typedef std::map<int, int> TPointMap;
  TIndexVec SortRing(const geo::CRingFactory::TRing& ring, TPointMap& mpPointRev)
  {
  	// Find the minimumste index
  	int nMinimum = INT_MAX;
  	int nMinIndex = 0;
	  int i;
	  for(i = 0; i < ring.size(); i++) {
		  if(mpPointRev[ring[i]] < nMinimum) {
  			nMinimum = mpPointRev[ring[i]];
	  		nMinIndex = i;
		  }
	  }

	  i = nMinIndex; 
    int idxNext = ring[(i+1)%ring.size()];
    int idxPrev = ring[(i-1+ring.size())%ring.size()];
	  TIndexVec ret;
    bool bForward = (idxPrev > idxNext);
	  do {
		  ret.push_back( mpPointRev[ring[i]] );
      i = (int)(( bForward ? i+1 : (i-1+ring.size()) ) % ring.size());
	  } while(nMinIndex != i);
	  assert(ring.size() == ret.size());

	  return ret;
  }
};


class Fw51GeometryConvexHullTest : public test_fw51::Fw51InputBaseTest
{
protected:
  Fw51GeometryConvexHullTest()
  {
    m_ModelsPath.append("Geometry/");
    m_OutputPath.append("Geometry/");
  }

  bool readHullPoints( QTextStream &os,
                       const char* sPath,
                       std::vector<const geo::IPoint*>& vcPoint,
                       std::vector<const geo::IPoint*>& vcTest )
  {
    // Read from file
    std::ifstream stream;
    stream.open( sPath );
    if ( !stream.is_open() ) {
      os << "Cannot open: " << sPath << endl;
      return false;
    }
    // Read in nodes of file
    bool bTest = false;
    double x, y, z;
    std::string string;
    while ( !stream.eof() ) {
      switch( stream.peek() ) {
      case ' ':
      case '\n':
        stream.get();
        break;
      case 'T':
        stream >> string;
        if ( string != "TESTPOINTS" ) {
          os << "Cannot translate:" << string.c_str() << "\n";
          return false;
        }
        bTest = true;
        break;
      default:
        stream >> x;
        stream >> y;
        stream >> z;
        if ( stream.good() ) {
          if ( bTest )
            vcTest.push_back( new geo::CPoint(x, y, z) );
          else
            vcPoint.push_back( new geo::CPoint(x, y, z) );
        } else {
          if ( !stream.eof() ) {
            os << "Invalid file.\n";
            return false;
          }
        }
      }
    }
    return true;
  }

};

typedef Fw51GeometryConvexHullTest Fw51GeometryConvexHull2DTest;
typedef Fw51GeometryConvexHullTest Fw51GeometryConvexHull3DTest;
typedef Fw51GeometryConvexHullTest Fw51GeometryCylindricHullTest;



class Fw51GeometryPolygonTest : public test_fw51::Fw51InputBaseTest
{
protected:
  Fw51GeometryPolygonTest()
  {
    m_ModelsPath.append("Geometry/");
    m_OutputPath.append("Geometry/");
  }

  bool readPoints(QTextStream &os,
                   const char* path,
                   std::vector<geo::CPoint>& polygon,
                   std::vector<geo::CPoint>& sample )
  {
    // Read from file
    std::ifstream stream;
    stream.open( path );
    if ( !stream.is_open() ) {
      os << "Cannot open: " << path << endl;
      return false;
    }
    // Read in polygon nodes
    double x, y, z;
    int n, i;
    stream >> n;
    polygon.resize( n );
    for ( i = 0; i < n; ++i ) {
      stream >> x >> y >> z;
      polygon[i] = geo::CPoint( x, y, z );
    }
    // Read in sample points
    stream >> n;
    sample.resize( n );
    for ( i = 0; i < n; ++i ) {
      stream >> x >> y >> z;
      sample[i] = geo::CPoint( x, y, z );
    }
    return stream.good();
  }

};





TEST_P(Fw51GeometryTest, BasicFunctionality)
{
  const GeometryTestInitInfo *inits = GetParam().inits;

  QFile of(m_Output.c_str());
  EXPECT_TRUE(of.open(QIODevice::WriteOnly | QIODevice::Truncate)) << "Failed to open output file " << m_Output << std::endl;
  QTextStream os(&of);

  GeometryTestData testData(os);

  for (int i = 0; i < TEST_INIT_INFO_MAXSIZE; ++i)
  {
    if (!inits[i].init)
      break;

    inits[i].init(testData);
  }

  of.close();

  EXPECT_TRUE(test_lib::FileCompare::IdenticalTokens(m_Expected, m_Output));
}



TEST_P(Fw51GeometryRingTest, Ring)
{
  QFile of(m_Output.c_str());
  EXPECT_TRUE(of.open(QIODevice::WriteOnly | QIODevice::Truncate)) << "Failed to open output file " << m_Output << std::endl;
  QTextStream os(&of);

	TPointMap mpPointRev;
	geo::CRingFactory factory;
	bool bInputCorrect = false;
	std::ifstream stream(m_Input.c_str());
	if(stream.is_open())
	{
		std::string sKeyword;
		stream >> sKeyword;
		if(sKeyword != "BEGIN")
		{
			os << "Keyword BEGIN not found" << endl;
			return;
		}

		TPointMap mpPoint;

		os << "EVALUATE INPUT\n\n";

		while(!stream.eof() && !bInputCorrect)
		{
			stream >> sKeyword;
			if(sKeyword == "VRTX")
			{
				// Read vertex
				int nIndex;
				double dX, dY;
				stream >> nIndex >> dX >> dY;
				geo::CPoint* pPoint = new geo::CPoint(dX, dY);
				std::pair<TPointMap::iterator, bool> ret =mpPoint.insert(TPointMap::value_type(nIndex, factory.AddPoint(*pPoint)));
				if(!ret.second)
				{
					os << "Duplicate point index" << endl;
					return;
				}
				VERIFY(mpPointRev.insert(TPointMap::value_type(ret.first->second, nIndex)).second);
				os << "VRTX\t" << nIndex << "\t" << dX << "\t" << dY << endl;
			}else if(sKeyword == "SEGM")
			{
				if(factory.SegmentSize() == 0)
					os << endl;
				int nFirst, nSecond;
				stream >> nFirst >> nSecond;
				if((mpPoint.find(nFirst) == mpPoint.end()) || (mpPoint.find(nSecond) == mpPoint.end()))
				{
					os << "Point index not found" << endl;
					return;
				}
				if(!factory.AddSegment(mpPoint[nFirst], mpPoint[nSecond]))
					os << "Segment is not added to factory" << endl;
				os << "SEGM\t" << nFirst << "\t" << nSecond << endl;
			}
			else if(sKeyword == "END")
			{
				bInputCorrect = true;
			}
			else
			{
				// Bull shit
				os << "Syntax error: " << sKeyword.c_str() << endl;
				return;

			}
		}
	}
	else
		os << "Cannot open file" << endl;

	if(bInputCorrect)
	{
		os << "\nFIND RINGS...\n\n";
		if(!factory.CreateRings())
			os << "Create rings fails" << endl << endl;
		// Sort rings
		std::set<TIndexVec> stFactory;
		int i = 0;
		for(i = 0; i < factory.RingSize(); i++)
			stFactory.insert(SortRing(factory.Ring(i), mpPointRev));
        // Print rings
		i = 0;
		os << factory.RingSize() << " RINGS FOUND" << endl << endl;
		std::set<TIndexVec>::iterator it;
		for(it = stFactory.begin(); it != stFactory.end(); it++)
		{
			os << "R" << i + 1 << " (";
			for(size_t j = 0; j < it->size(); j++)
			{
				if(j != 0) os << ", ";
				os << (*it)[j];
			}
			os << ")" << endl;
			i++;
		}
		os << endl;

		// Sort boundary
		stFactory.clear();
		for(i = 0; i < factory.BoundarySize(); i++)
			stFactory.insert(SortRing(factory.Boundary(i), mpPointRev));
        // Print boundary
		i = 0;
		os << factory.BoundarySize() << " BOUNDARIES FOUND" << endl << endl;
		for( it = stFactory.begin(); it != stFactory.end(); it++)
		{
			os << "B" << i + 1 << " (";
			for(size_t j = 0; j < it->size(); j++)
			{
				if(j != 0) os << ", ";
				os << (*it)[j];
			}
			os << ")" << endl;
			i++;
		}		
		

	}
	else
		os << "\nEND statement not found\n" << endl;

  of.close();

  EXPECT_TRUE(test_lib::FileCompare::IdenticalTokens(m_Expected, m_Output));
}



TEST_P(Fw51GeometryConvexHull2DTest, BasicFunctionality)
{
  QFile of(m_Output.c_str());
  EXPECT_TRUE(of.open(QIODevice::WriteOnly | QIODevice::Truncate)) << "Failed to open output file " << m_Output << std::endl;
  QTextStream os(&of);

  GeometryTestData testData(os);

  std::string param = GetParam();
  std::size_t pos   = param.find(' ');
  int nDisabledCoordinate = pos != std::string::npos ? atoi(param.substr(pos).c_str()) : 0;
    


  os << "     ***************** CONVEXHULL 2D TEST ***********************"
     << endl;
  os << "     * Geometry test file                                       *"
     << endl;
  os << "     * testing of the 2d convex hull functionality of the       *"
     << endl;
  os << "     * geometry lib                                             *"
     << endl;
  os << "     ************************************************************"
     << endl;

  std::vector<const geo::IPoint*> vcPoint;
  std::vector<const geo::IPoint*> vcTest;

  //if ( !readHullPoints( sPath, vcPoint, vcTest ) ) return;
  EXPECT_TRUE(readHullPoints(os, m_Input.c_str(), vcPoint, vcTest ));

  // Print the points
  os << "\nInput point" << endl;
  for(size_t i = 0; i < vcPoint.size(); i++)
    os << "P" << i << " (" << vcPoint[i]->X() << ", " << vcPoint[i]->Y()
       << ")" << endl;

  geo::CConvexHull_2D hull(vcPoint, nDisabledCoordinate);
  os << "\nHull point" << endl;
  for(int i = 0; i < hull.NrOfPoints(); i++)
    testData.PrintObject(hull.Point(i), "P = ");

  // Now we're testing some points

  os << "\nPoint in hul" << endl;
  for (size_t i = 0; i < vcTest.size(); i++ ) {
    os << "Point " << i << "  (" << vcTest[i]->X() << "," << vcTest[i]->Y()
       << ") is " ;
    if ( !hull.ContainsXY(*vcTest[i], true) ) {
      os << "not ";
    }
    os << "in hull." << endl;
  }

  testData.NewLine(2);
  os << "     ******************* END CONVEXHULL 2D TEST *****************"
     << endl;

  of.close();

  EXPECT_TRUE(test_lib::FileCompare::IdenticalTokens(m_Expected, m_Output));
}


TEST_P(Fw51GeometryConvexHull3DTest, BasicFunctionality)
{
  QFile of(m_Output.c_str());
  EXPECT_TRUE(of.open(QIODevice::WriteOnly | QIODevice::Truncate)) << "Failed to open output file " << m_Output << std::endl;
  QTextStream os(&of);

  GeometryTestData testData(os);

  os << "     ***************** CONVEXHULL 3D TEST ***********************"
     << endl;
  os << "     * Geometry test file                                       *"
     << endl;
  os << "     * testing of the 3d convex hull functionality of the       *"
     << endl;
  os << "     * geometry lib                                             *"
     << endl;
  os << "     ************************************************************"
     << endl;

  std::vector<const geo::IPoint*> vcPoint;
  std::vector<const geo::IPoint*> vcTest;
  //if ( !readHullPoints( sPath, vcPoint, vcTest ) ) return;
  EXPECT_TRUE(readHullPoints(os, m_Input.c_str(), vcPoint, vcTest ));

  // Print the points
  os << "\nInput point" << endl;
  for(size_t i = 0; i < vcPoint.size(); i++)
    os << "P" << i << " (" << vcPoint[i]->X() << ", " << vcPoint[i]->Y()
       << ", " << vcPoint[i]->Z() << ")" << endl;

  geo::CConvexHull hull( vcPoint );
  if ( !hull.Calculate( 0 ) ) {
    os << "Hull could not be created." << endl;
    return;
  }
  
  os << "\nHull point" << endl;
  for(int i = 0; i < hull.NrOfPoints(); i++)
    testData.PrintObject(hull.GlobalPoint(i), "P = ");

  // Now we're testing some points

  os << "\nPoint in hull" << endl;
  for (size_t i = 0; i < vcTest.size(); i++ ) {
    os << "Point " << i << "  (" << vcTest[i]->X() << "," << vcTest[i]->Y()
       << "," << vcTest[i]->Z() << ") is " ;
    if ( !hull.Contains(*vcTest[i], true) ) {
      os << "not ";
    }
    os << "in hull." << endl;
  }

  testData.NewLine(2);
  os << "     ******************* END CONVEXHULL 3D TEST *****************"
     << endl;


  of.close();

  EXPECT_TRUE(test_lib::FileCompare::IdenticalTokens(m_Expected, m_Output));
}

TEST_P(Fw51GeometryCylindricHullTest, BasicFunctionality)
{
  QFile of(m_Output.c_str());
  EXPECT_TRUE(of.open(QIODevice::WriteOnly | QIODevice::Truncate)) << "Failed to open output file " << m_Output << std::endl;
  QTextStream os(&of);

  GeometryTestData testData(os);

  std::string param = GetParam();
  std::size_t pos   = param.find(' ');
  double dRadius = pos != std::string::npos ? atof(param.substr(pos).c_str()) : 0;
    


  os << "     ***************** CYLINDRIC HULL TEST **********************"
     << endl;
  os << "     * Geometry test file                                       *"
     << endl;
  os << "     * testing of the cylindric hull functionality of the       *"
     << endl;
  os << "     * geometry lib                                             *"
     << endl;
  os << "     ************************************************************"
     << endl;

  os << endl << "Radius " << dRadius << endl;

  std::vector<const geo::IPoint*> vcPoint;
  std::vector<const geo::IPoint*> vcTest;

  EXPECT_TRUE(readHullPoints(os, m_Input.c_str(), vcPoint, vcTest ));

  // Print the points
  os << "\nInput point" << endl;
  for(size_t i = 0; i < vcPoint.size(); i++)
    os << "P" << i << " (" << vcPoint[i]->X() << ", " << vcPoint[i]->Y() << ", " << vcPoint[i]->Z() << ")" << endl;

  geo::CCylindricHull hull(vcPoint, dRadius);
#if 0
  os << "\nHull point" << endl;
  for(int i = 0; i < hull.NrOfPoints(); i++)
    testData.PrintObject(hull.Point(i), "P = ");
#endif

  // Now we're testing some points

  os << "\nPoint in hull" << endl;
  for (size_t i = 0; i < vcTest.size(); i++ ) {
    os << "Point " << i << "  (" << vcTest[i]->X() << "," << vcTest[i]->Y() << "," << vcTest[i]->Z() << ") is " ;
    if ( !hull.Contains(*vcTest[i], true) ) {
      os << "not ";
    }
    os << "in hull (inclusive)." << endl;
    os << "Point " << i << "  (" << vcTest[i]->X() << "," << vcTest[i]->Y() << "," << vcTest[i]->Z() << ") is " ;
    if ( !hull.Contains(*vcTest[i], false) ) {
      os << "not ";
    }
    os << "in hull (exclusive)." << endl;
  }

  testData.NewLine(2);
  os << "     ******************* END CYLINDRIC HULL TEST ****************"
     << endl;

  of.close();

  EXPECT_TRUE(test_lib::FileCompare::IdenticalTokens(m_Expected, m_Output));
}


TEST_P(Fw51GeometryPolygonTest, BasicFunctionality)
{
  QFile of(m_Output.c_str());
  EXPECT_TRUE(of.open(QIODevice::WriteOnly | QIODevice::Truncate)) << "Failed to open output file " << m_Output << std::endl;
  QTextStream os(&of);

  os << "     ********************** POLYGON TEST ************************"
     << endl;
  os << "     * Geometry test file                                       *"
     << endl;
  os << "     * testing of the polygon interior/exterior algorithms      *"
     << endl;
  os << "     * geometry lib                                             *"
     << endl;
  os << "     ************************************************************"
     << endl;

  std::vector<geo::CPoint> polygonPoints;
  std::vector<geo::CPoint> sample;

  //if ( !readPoints( path, polygonPoints, sample ) ) return;
  EXPECT_TRUE(readPoints(os, m_Input.c_str(), polygonPoints, sample ) );

  // Print the points
  os << "\nPolygon point\n";
  for(size_t i = 0; i < polygonPoints.size(); i++)
    os << "Node " << i << " (" << polygonPoints[i].X() << ", "
                               << polygonPoints[i].Y() << ", "
                               << polygonPoints[i].Z() << ")\n";

    geo::CPolygon polygon( polygonPoints );

    os << "\nSample point\n";
    for (size_t i = 0; i < sample.size(); i++ ) {
      os << "Point " << QString( "%1" ).arg( i, 4 ) << " ";
      QString coord = QString( "(%1,%2,%3)" ).arg( sample[i].X(), 10 )
                                             .arg( sample[i].Y(), 10 )
                                             .arg( sample[i].Z(), 10 );
    os << coord;
    bool inside         = polygon.Contains(          sample[i], false );
    bool insideOrEdge   = polygon.Contains(          sample[i], true  );
    bool insideXY       = polygon.ContainsInXYPlane( sample[i], false );
    bool insideXYOrEdge = polygon.ContainsInXYPlane( sample[i], true  );
    os << ( inside                      ? " inside"
                                        : "       " );
    os << ( insideOrEdge && !inside     ? " edge"
                                        : "     " );
    os << ( insideXY                    ? " inside(XY)" 
                                        : "           " );
    os << ( insideXYOrEdge && !insideXY ? " edge(XY)" 
                                        : "         " );
    os << endl;
  }

  os << endl 
     << endl
     << "     ******************* END POLYGON TEST ********************"
     << endl;

  of.close();

  EXPECT_TRUE(test_lib::FileCompare::IdenticalTokens(m_Expected, m_Output));
}


void init_bucket(GeometryTestData &testData)
{

  struct BucketBuild
  {
    BucketBuild(QTextStream &os, geo::CBucketKernel& bk, const geo::IPoint& min, const geo::IPoint& max, int nBuckets)
    {
    	os << "Requested for box ("<< min.X() << "," << min.Y() << "," << min.Z() << ") (";
	    os << max.X() << "," << max.Y() << "," << max.Z() << ") for " << nBuckets << " buckets" << endl;

	    bk.Resize(min, max, nBuckets);

	    os << "Size X = " << bk.SizeX() << ", Size Y = " << bk.SizeY() << ", Size Z = " << bk.SizeZ() << endl;
    }
  };

  struct LocationTest
  {
    LocationTest(QTextStream &os, const geo::CBucketKernelTempl<geo::CPoint>& kernel, const geo::IPoint& point)
    {
	    os << "Point(" << point.X() << ", " << point.Y() << ", " << point.Z() << ") in box(";
	    os << kernel.BucketX( point.X() ) << ", " << kernel.BucketY( point.Y() ) << ", " << kernel.BucketZ( point.Z()) << ")" << endl;
    }
  };

  struct AddPoint
  {
    AddPoint(QTextStream &os, geo::CBucketKernelTempl<geo::CPoint>& kernel, const geo::IPoint& point)
    {
	    os << "Point(" << point.X() << ", " << point.Y() << ", " << point.Z() << ") ";
	    geo::CPoint p(point);
	    if(!kernel.Insert(point, p)) os << "not ";
	    os << "added" << endl;
    }
  };

  struct PrintBucket
  {
    PrintBucket(QTextStream &os, const geo::CBucketKernelTempl<geo::CPoint>::TBucket& bucket)
    {
	    geo::CBucketKernelTempl<geo::CPoint>::TBucket::const_iterator it;
	    if(bucket.size() == 0)
		    os << "-";
	    else
	  {
		  os << "( ";
		  for(it = bucket.begin(); it != bucket.end(); it++)
		  {
  			const geo::IPoint& point = *it;
  			  os << "(" << point.X() << ", " << point.Y() << ", " << point.Z() << ") ";
		    }
		    os << ")";
	    }
    }
  };

  struct PointTest
  {
    PointTest(QTextStream &os, const geo::CBucketKernelTempl<geo::CPoint>& kernel, const geo::IPoint& point)
    {
	    os << "At Point(" << point.X() << ", " << point.Y() << ", " << point.Z() << ") in box(";
	    os << kernel.BucketX( point.X() ) << ", " << kernel.BucketY( point.Y() ) << ", " << kernel.BucketZ( point.Z()) << ")" << endl;
    }
  };



	QTextStream &os = testData.OS();

	os << "     ************************ BUCKET TEST ***********************" << endl;
	os << "     * Bucket engine test file                                  *" << endl ;
	os << "     * Testing the functionality of the bucket method           *" << endl;
	os << "     ************************************************************" << endl;
	os << endl << endl;

	// We make bucket kernel of 3 x 2 x 2 on (-2, -2, -2) to (2, 2, 2) of points
	geo::CBucketKernelTempl<geo::CPoint> bk_point;
	bk_point.Resize(geo::CPoint(0.5, 0.5, -1), 3, 2, 2, 1);
	os << "Kernel is in box ("<< bk_point.Min().X() << "," << bk_point.Min().Y() << "," << bk_point.Min().Z() << ") (";
	os << bk_point.Max().X() << "," << bk_point.Max().Y() << "," << bk_point.Max().Z() << ")" << endl;
	os << "Edgelength is " << bk_point.EdgeSize() << endl << endl;
	LocationTest(os, bk_point, geo::CPoint(2, 1, 0.5));
	LocationTest(os, bk_point, geo::CPoint(3, 1, -0.25));
	LocationTest(os, bk_point, geo::CPoint(3, 2, 0.25));	
	LocationTest(os, bk_point, geo::CPoint(-10, -10, -10));	// Testing delimiters
	LocationTest(os, bk_point, geo::CPoint(10, 10, 10));	// Testing delimiters
	LocationTest(os, bk_point, geo::CPoint(2, -10, 10));
	os << endl;
	double dx, dy, dz;
	for(dx = 1; dx <= 2; dx++)
		for(dy = 1; dy <= 2; dy++)
			for(dz = -0.5; dz <= 0.5; dz++)
				AddPoint(os, bk_point, geo::CPoint( dx,  dy,  dz));
	AddPoint(os, bk_point, geo::CPoint( 10, 10,  0.25));
	AddPoint(os, bk_point, geo::CPoint( 10, -10,  -0.25));
	AddPoint(os, bk_point, geo::CPoint( 1,  1,  0.5));
	os << endl;
	int x, y, z;	
	for(x = 0; x < bk_point.SizeX(); x++)
	{
		for(y = 0; y < bk_point.SizeY(); y ++)
		{
			for(z = 0; z < bk_point.SizeZ(); z++)
			{
			  os << "Bucket(" << x << ", " << y << ", " << z << ") : ";
        os.flush();
			  PrintBucket(os, bk_point.Bucket(x, y, z));
			  os << endl;
			}
		}
	}
	os << endl;
	os << "Expand = 0 on (5, -5,  -2)\nPoints: ";
  os.flush();
	geo::CSetExpandCallBack<geo::CPoint> *pCallBack = new geo::CSetExpandCallBack<geo::CPoint>( );
	bk_point.Expand( *pCallBack, geo::CPoint(5, -5,  -2), 0);
	PrintBucket(os,  pCallBack->Bucket() );
	os << endl;
	delete pCallBack;
	os << "Expand = 1 on (5, -5,  -2)\nPoints: ";	
  os.flush();
	pCallBack = new geo::CSetExpandCallBack<geo::CPoint>( );
	bk_point.Expand( *pCallBack, geo::CPoint(5, -5,  -2), 1);
	PrintBucket(os,  pCallBack->Bucket() );
	os << endl;
	delete pCallBack;
	os << "Expand = 0 on XY (0, 0, 10)\nPoints: ";
  os.flush();
	pCallBack = new geo::CSetExpandCallBack<geo::CPoint>( );
	bk_point.ExpandXY( *pCallBack, geo::CPoint(0, 0, 10), 0);
	PrintBucket(os,  pCallBack->Bucket() );
	os << endl;
	delete pCallBack;
	os << endl;

	os << "Bounding box (0, 0, 0) (1.4, 10, 1)\nPoints: ";
  os.flush();
	PrintBucket(os, bk_point.Buckets(geo::CPoint(0, 0, 0), geo::CPoint(1.4, 10, 1)));
	os << endl;

	os << "Bounding box (0, 0, 0) (6, 1, 100)\nPoints: ";
  os.flush();
	PrintBucket(os, bk_point.Buckets(geo::CPoint(0, 0, 0), geo::CPoint(6, 1, 100)));
	os << endl << endl;

	os << "Point (1, 1, 0.5) ";
	geo::CPoint pt(1, 1, 0.5);
	if(!bk_point.Remove(pt, pt) ) os << "not ";
	os << "removed.\nPoints at (1, 1, 0.5) : ";
  os.flush();
	PrintBucket(os,  bk_point.Bucket( pt ));
	os << endl;

	os << "Point (1, 1, 0.5) ";
	if(!bk_point.Remove(pt, pt)) os << "not ";
	os << "removed.\nPoints at (1, 1, 0.5) : ";
  os.flush();
	PrintBucket(os,  bk_point.Bucket( pt ));
	os << endl;

	os << "Point (2, 1, 0.5) ";
	pt = geo::CPoint(2, 1, 0.5);
	if(!bk_point.Remove(geo::CPoint(0, 0, 0), geo::CPoint(2, 2, 2), pt)) os << "not ";
	os << "removed from box (0, 0, 0)-(2, 2, 2).\nPoints at (2, 1, 0.5) : ";
  os.flush();
	PrintBucket(os,  bk_point.Bucket( pt ));
	os << endl;

	os << "Point (2, 1, -0.5) ";
	pt = geo::CPoint(2, 1, -0.5);
	if(!bk_point.Remove(geo::CPoint(0, 0, 0), geo::CPoint(2, 2, 2), pt)) os << "not ";
	os << "removed from box (0, 0, 0)-(2, 2, 2).\nPoints at (2, 1, -0.5) : ";
  os.flush();
	PrintBucket(os,  bk_point.Bucket( pt ));
	os << endl << endl;
	
	geo::CBucketKernel bk(geo::CPoint(0, 0, 0), geo::CPoint(1, 1, 1), 0);

	// Filling buckets
	BucketBuild(os, bk, geo::CPoint(10, 20, 30), geo::CPoint(110, 70, 50), 1000);	// Rectangle
	BucketBuild(os, bk, geo::CPoint(0, 0, 0), geo::CPoint(10, 10, 10), 1000);		// Box
	BucketBuild(os, bk, geo::CPoint(0, 0, 0), geo::CPoint(10, 10, 0), 100);			// 2D
	BucketBuild(os, bk,geo::CPoint(0, 0, 0), geo::CPoint(70, 70, 0), 4);
}

void init_face(GeometryTestData &testData)
{
  struct ContainsTest
  {
    ContainsTest(QTextStream &os, const char* pName, const geo::IFace& face, const geo::CPoint* pPoint, int nPoint)
    {
    	for(int i = 0; i < nPoint; i++)
	    {
		    os << pName;
		    if(!face.Contains(pPoint[i], false)) os << " not";
		    os << " contains (" << pPoint[i] << ") excluding edge" << endl;
		    os << pName;
		    if(!face.Contains(pPoint[i], true)) os << " not";
		    os << " contains (" << pPoint[i] << ") including edge" << endl;
	    }
    }
  };

  struct TriangleTest
  {
    TriangleTest(QTextStream &os, const geo::IPoint& p1, const geo::IPoint& p2, const geo::IPoint& p3, const geo::IPoint& test_point)
    {
    	geo::CTriangle tri(p1, p2, p3);

	    os << "Triangle nodes:\n";
	    os << "p1: (" << p1 << ")\n";
	    os << "p2: (" << p2 << ")\n";
	    os << "p3: (" << p3 << ")\n";
	    os << "Triangle is ";
	    if(!tri.IsConvex()) os << "not ";
	    os << "convex." << endl;

	    // create a point that should lie in the triangle
	    geo::CPoint p_test(test_point);
	    os << endl;
	    ContainsTest(os, "Triangle", tri, &p_test, 1);
	    os << endl;
    }
  };

	QTextStream &os = testData.OS();

	geo::CPoint *pPoint;
	pPoint = new geo::CPoint[10];

	os << "Face test\n\n";

	// create a face (triangle) and check if a point on the edge is contained
	geo::CPoint p1(6327807.5258980, 623557.52589803, 3600.0000000000);
	geo::CPoint p2(6328000.0000000, 624000.00000000, 3600.0000000000);
	geo::CPoint p3(6326000.0000000, 624000.00000000, 3597.0000000000);
	geo::CPoint p_test(6326500.0000000, 624000.00000000, 3597.7500000000);
	TriangleTest(os, p1, p2, p3, p_test);

	p1 = geo::CPoint(1, 1);
	p2 = geo::CPoint(0, 0);
	p3 = geo::CPoint(0, 2);
	p_test = geo::CPoint(1, -2e15);
	TriangleTest(os, p1, p2, p3, p_test);

	p1 = geo::CPoint(2e10, 1);
	p2 = geo::CPoint(0, 0);
	p3 = geo::CPoint(0, 2);
	p_test = geo::CPoint(1, 1);
	TriangleTest(os, p1, p2, p3, p_test);

	p1 = geo::CPoint(1, 1, 0);
	p2 = geo::CPoint(0, 0, 0);
	p3 = geo::CPoint(0, 2, 0);
	p_test = geo::CPoint(0.5, 0.5, 2e10);
	TriangleTest(os, p1, p2, p3, p_test);

	p1 = geo::CPoint(1, 1, 0);
	p2 = geo::CPoint(0, 0, 0);
	p3 = geo::CPoint(0, 2, 0);
	p_test = geo::CPoint(0.5, 0.5, 2*EPS);
	TriangleTest(os, p1, p2, p3, p_test);	

	// Test intersection with rection
	geo::CRectangle rect(geo::CPoint(-1, -1), geo::CPoint(1, 1));
	os << "Rectangle((-1, -1), (1, 1))" << endl;
	os << "Rectangle is ";
	if(!rect.IsConvex()) os << "not ";
	os << "convex." << endl;

	// Extra contains test for rectangle (16-08-2004)
	pPoint[0] = geo::CPoint(0, 0);
	pPoint[1] = geo::CPoint(0, -1);
	pPoint[2] = geo::CPoint(0, -1.01);
	pPoint[3] = geo::CPoint(0, 0.999);
	pPoint[4] = geo::CPoint(-1, -1);
	pPoint[5] = geo::CPoint(0.8, -0.4);
	pPoint[6] = geo::CPoint(-1, -2);
	pPoint[7] = geo::CPoint(2, 1);
	pPoint[8] = geo::CPoint(0, 1 - (2 * EPS));
	os << endl;
	ContainsTest(os, "Rectangle", rect, pPoint, 9);
	os << endl;

	// Test intersection with a very big rectangle
	rect = geo::CRectangle(geo::CPoint(-10000, -10000), geo::CPoint(10000, 10000));
	os << "Rectangle((-10000, -10000), (10000, 10000))" << endl;
	os << "Rectangle is ";
	if(!rect.IsConvex()) os << "not ";
	os << "convex." << endl;

	// Extra contains test for rectangle (16-08-2004)
	pPoint[0] = geo::CPoint(0, 10000 - (100 * EPS));
	os << endl;
	ContainsTest(os, "Rectangle", rect, pPoint, 1);
	os << endl;		

	// Test first test line
	geo::CLine line1(geo::CPoint(-2, 0, -1), geo::CPoint(0, 0, 1));
	os << "Intersection line: \n" << line1;
	os << "Intersection point: " << rect.Intersection(line1) << endl << endl;

	// Test second test line
	geo::CLine line2(geo::CPoint(1, 1, -1), geo::CPoint(-1, -1, 1));
	os << "Intersection line: \n" << line2;
	os << "Intersection point: " << rect.Intersection(line2) << endl << endl;

	// Test polygon
	geo::CPolygon polygon;
	polygon.PushBack(geo::CPoint( 3, 3, 0)); // po
	polygon.PushBack(geo::CPoint( 0, 0, 0)); // p1
	polygon.PushBack(geo::CPoint( 3,-3, 0)); // p2
	polygon.PushBack(geo::CPoint( 0,-6, 0)); // p3
	polygon.PushBack(geo::CPoint(-3,-6, 0)); // p4
	polygon.PushBack(geo::CPoint( 0,-3, 0)); // p5
	polygon.PushBack(geo::CPoint(-3, 0, 0)); // p6
	polygon.PushBack(geo::CPoint( 0, 3, 0)); // p7
	os << "Polygon: " << endl;
	for(int i = 0; i < polygon.NrOfPoints(); i++)
		os << "p" << i << " (" << polygon.Point(i) << ")\n";
	os << "Polygon is ";
	if(!polygon.IsConvex()) os << "not ";
	os << "convex." << endl;

	// Extra contains test for rectangle (16-08-2004)
	pPoint[0] = geo::CPoint(1, 0);
	pPoint[1] = geo::CPoint(3, 3);
	pPoint[2] = geo::CPoint(0, -2);
	pPoint[3] = geo::CPoint(0, 1);
	pPoint[4] = geo::CPoint(-3, 3);
	pPoint[5] = geo::CPoint(0.9, 0.9);
	pPoint[6] = geo::CPoint(0, -2.999);
	pPoint[7] = geo::CPoint(-2.001, 1);
	os << endl;
	ContainsTest(os, "Polygon", polygon, pPoint, 8);		
	os << endl;
	
	// Test plane intersection I
	geo::CPlane plane = geo::CPlane::PlaneYZ;
	geo::IPolygon::TLineVec vcLine = polygon.Intersection(plane);
	os << "Intersection planeI: " << endl;
	for(size_t j = 0; j < vcLine.size(); j++)
	{
		os << "Intersection Line " << j << endl;
		os << vcLine[j];
	}
	os << endl;

	// Test plane intersection II
	plane = geo::CPlane::PlaneYZ;
	plane.Move(geo::CVector(1, 0, 0));
	vcLine = polygon.Intersection(plane);
	os << "Intersection planeII: " << endl;
	for(size_t j = 0; j < vcLine.size(); j++)
	{
		os << "Intersection Line " << j << endl;
		os << vcLine[j];
	}
	os << endl;

	// Test plane intersection III
	plane = geo::CPlane::PlaneYZ;
	plane.Move(geo::CVector(3, 0, 0));
	vcLine = polygon.Intersection(plane);
	os << "Intersection planeII: " << endl;
	for(size_t j = 0; j < vcLine.size(); j++)
	{
		os << "Intersection Line " << j << endl;
		os << vcLine[j];
	}
	os << endl;

	delete [] pPoint;

}

void init_hexahedron_0(GeometryTestData &testData)
{
	QTextStream &os = testData.OS();

	os << "Hexahedron test I" << endl << endl;
	geo::CMesh3D mesh;

	geo::CPoint points[8];
	points[0] = geo::CPoint(0, 0, 0);
	points[1] = geo::CPoint(10, 0, 0);
	points[2] = geo::CPoint(10, 10, 0);
	points[3] = geo::CPoint(0, 10, 0);
	points[4] = geo::CPoint(0, 0, 10);
	points[5] = geo::CPoint(10, 0, 10);
	points[6] = geo::CPoint(10, 10, 10);
	points[7] = geo::CPoint(0, 10, 10);

	std::vector<const geo::IPoint*> pts;
	int i;
	for(i = 0; i < 8; i++) pts.push_back(&points[i]);

	geo::CHexahedron hexa(mesh, pts);
	
	os << "Faces of the hexahedron" << endl;

	for(i = 0; i < hexa.NrOfFaces(); i++)
	{
		os << "Face " << i  << "{";
		int j;
		for(j = 0; j < hexa.Face(i).NrOfPoints(); j++)
		{
			os << " P" << j << "(" << hexa.Face(i).Point(j).X() << ",";
			os << hexa.Face(i).Point(j).Y() << "," << hexa.Face(i).Point(j).Z() << ")";
		}
		os << " } " << endl;
		os << "Face " << i  << "{";
		for(j = 0; j < hexa.Face(i).NrOfLines(); j++)
		{
			os << " L" << j << "(" << hexa.Face(i).Line(j).First().X() << ",";
			os << hexa.Face(i).Line(j).First().Y() << "," << hexa.Face(i).Line(j).First().Z() << ")(";
			os << hexa.Face(i).Line(j).Second().X() << "," << hexa.Face(i).Line(j).Second().Y();
			os << "," << hexa.Face(i).Line(j).Second().Z() << ")";
		}
		os << " } " << endl;
	}

	os << endl;

	os << "Hexahedron volume: " << endl;
	os << hexa.Volume() << endl << endl;

	os << "Hexahedron face areas:" << endl;

	for(i = 0; i < hexa.NrOfFaces(); i++)
	{
		os << "Face " << i + 1 << ": " << hexa.Face(i).Area() <<
					endl;
	}

	os << endl;

	os << "Hexahedron line lengths:" << endl;
	for(i = 0; i < hexa.NrOfLines(); i++)
	{
		os << "Line " << i + 1 << ": " << hexa.Line(i).Length() <<
					endl;
	}
	os << endl;

}

void init_hexahedron_1(GeometryTestData &testData)
{
	QTextStream &os = testData.OS();

	os << "Hexahedron test II" << endl << endl;
	geo::CMesh3D mesh;

	geo::CPoint points[8];
	// Create a new hexahedron
	points[0] = geo::CPoint(-1, -1, -1);
	points[1] = geo::CPoint(1 , -1, -3);
	points[2] = geo::CPoint(1 ,  1, -3);
	points[3] = geo::CPoint(-1,  1, -1);
	points[4] = geo::CPoint(-1, -1,  1);
	points[5] = geo::CPoint(1 , -1,  3);
	points[6] = geo::CPoint(1 ,  1,  3);
	points[7] = geo::CPoint(-1,  1,  1);

	os << "Points:" << endl;

	int i;
	for(i = 0; i < 8; i++)
	{
		os << "Point " << i + 1 << ": (" << points[i].X() << "," <<
					points[i].Y() << "," << points[i].Z() << ")" << endl;
	}

	std::vector<const geo::IPoint*> pts;
	for(i = 0; i < 8; i++) pts.push_back(&points[i]);

	geo::CHexahedron hexa(mesh, pts);

	// Cutting X-Plane
	os << "\nCutting element with Y-Plane\n";
	geo::CPolygon y_poly = hexa.IntersectionConv( geo::CPlane::PlaneYZ );

	for(i = 0; i < y_poly.NrOfPoints(); i++)
	{
		os << "Point " << i + 1 << ": (" << y_poly.Point(i).X() << "," <<
					y_poly.Point(i).Y() << "," << y_poly.Point(i).Z() << ")" << endl;
	}

	os << "\nCutting element with X-Plane\n";
	geo::CPolygon x_poly = hexa.IntersectionConv( geo::CPlane::PlaneZX );

	for(i = 0; i < x_poly.NrOfPoints(); i++)
	{
		os << "Point " << i + 1 << ": (" << x_poly.Point(i).X() << "," <<
					x_poly.Point(i).Y() << "," << x_poly.Point(i).Z() << ")" << endl;
	}
}

void init_hexahedron_2(GeometryTestData &testData)
{
	QTextStream &os = testData.OS();

	os << "Hexahedron test III" << endl << endl;
	geo::CMesh3D mesh;

	geo::CPoint points[8];
	// Create a new hexahedron
	points[0] = geo::CPoint(6.321e+006, 616000, 4970); // 28
	points[1] = geo::CPoint(6.323e+006, 616000, 4923); // 30
	points[2] = geo::CPoint(6.323e+006, 618000, 4700); // 42
	points[3] = geo::CPoint(6.321e+006, 618000, 4768); // 40
	points[4] = geo::CPoint(6.321e+006, 616000, 5082); // 29
	points[5] = geo::CPoint(6.323e+006, 616000, 4993); // 31
	points[6] = geo::CPoint(6.323e+006, 618000, 4821); // 43
	points[7] = geo::CPoint(6.321e+006, 618000, 4842); // 41

	os << "Points:" << endl;

	int i;
	for(i = 0; i < 8; i++)
	{
		os << "Point " << i + 1 << ": (" << points[i].X() << "," <<
					points[i].Y() << "," << points[i].Z() << ")" << endl;
	}

//  N  6319914.56002355    6325630.3924349
//  E  612000              624000
	geo::CPoint p1(6319914.56002355, 612000, 0);
	geo::CPoint p2(6325630.3924349,  624000, 0);
	geo::CPoint p3(6325630.3924349,  624000, 1);
		   
	std::vector<const geo::IPoint*> pts;
	for(i = 0; i < 8; i++) pts.push_back(&points[i]);

	geo::CHexahedron hexa(mesh, pts);
	
	// Cutting X-Plane
	os << "\nCutting element with user defined Plane\n";
	geo::CPolygon y_poly = hexa.IntersectionConv( geo::CPlane(p1, p2, p3) );

	for(i = 0; i < y_poly.NrOfPoints(); i++)
	{
		os << "Point " << i + 1 << ": (" << y_poly.Point(i).X() << "," <<
					y_poly.Point(i).Y() << "," << y_poly.Point(i).Z() << ")" << endl;
	}
}

void init_line(GeometryTestData &testData)
{
	QTextStream &os = testData.OS();

  testData.NewLine(2);
  os << "     ************************* LINE TEST ************************" << endl;
  os << "     * Geometry test file                                       *" << endl ;
  os << "     * testing of the line functionality of the geometry lib    *" << endl;
  os << "     ************************************************************" << endl;
  testData.NewLine(2);

  const geo::CPoint p1=geo::CPoint(0,0,0);
  const geo::CPoint p2=geo::CPoint(1,1,0);
  const geo::CPoint p3=geo::CPoint(1,0,0);
  const geo::CPoint p4=geo::CPoint(0,1,0);

  testData.PrintObject(p1,"p1 := ");
  testData.PrintObject(p2,"p2 := ");
  testData.PrintObject(p3,"p3 := ");
  testData.PrintObject(p4,"p4 := ");

  const geo::CLine l1(p1,p2);
  const geo::CLine l2(p3,p4);

  testData.PrintObject(l1,"l1(p1,p2) := ");
  testData.PrintObject(l2,"l2(p3,p4) := ");

  os << "l1.Dir().AngleDeg(l2.Dir()) = " << l1.Dir().AngleDeg(l2.Dir()) << endl;
  os << "(l1==l2) = " << (l1==l2) << endl;
  os << "(l1.IsParallel(l2)) = " << (l1.IsParallel(l2)) << endl;
  testData.PrintObject(l1.Intersection(l2),"l1.Intersection(l2) := ");

  geo::CLine l3=l1;
  testData.PrintObject(l3,"l3 := ");
  os << "(l3==l1) = " << (l3==l1) << endl;
  os << "(l3.IsParallel(l1)) = " << (l3.IsParallel(l1)) << endl;
  testData.PrintObject(l3.Intersection(l1),"l3.Intersection(l1) = ");

  
  l3=geo::CLine(p1,p3);
  testData.PrintObject(l3,"l3(p1,p3) := ");
  int Index1,Index2;
  os << "(l1.IsConnected(l3,Index1,Index2)) = " << (l1.IsConnected(l3,Index1,Index2)) << endl;
  os << "Index1 = " << Index1 << endl;
  os << "Index2 = " << Index2 << endl;

  l3 = geo::CLine(p3,p1);
  testData.PrintObject(l3,"l3(p3,p1) := ");
  os << "(l1.IsConnected(l3,Index1,Index2)) = " << (l1.IsConnected(l3,Index1,Index2)) << endl;
  os << "Index1 = " << Index1 << endl;
  os << "Index2 = " << Index2 << endl;

  os << "l2.Distance(geo::CPoint::NullPoint) = " << l2.Distance(geo::CPoint::NullPoint) << endl;
  
  os << "l1.Contains(p2,true) = " << l1.Contains(p2,true) << endl;
  os << "l1.Contains(p2,false) = " << l1.Contains(p2,false) << endl;

  testData.PrintObject(l2.Project(geo::CPoint::NullPoint),"l2.Project(geo::CPoint::NullPoint) = ");
  os << "l1.SquareLength() = " << l1.SquareLength() << endl;

  

  testData.NewLine(2);
  os << "     ********************** END LINE TEST *********************" << endl;
  
}

void init_mapping(GeometryTestData &testData)
{
  QTextStream &os = testData.OS();

  // create the element (rect)
  geo::CPoint pt(10, 10, 0);
  geo::CVector v1(10, 0);
  geo::CVector v2(0, 10);
  geo::CRectangle rect(pt, v1, v2);

  os << "BEGIN MAPPING TEST\n";

  os << "rectangle: ("
    << rect.Point(0).X() << "," << rect.Point(0).Y() << "), ("
    << rect.Point(1).X() << "," << rect.Point(1).Y() << "), ("
    << rect.Point(2).X() << "," << rect.Point(2).Y() << "), ("
    << rect.Point(3).X() << "," << rect.Point(3).Y() << ")\n\n";

  // same value for each corner, equally weighted, Voight, returns same value for each corner
  {
    std::vector<double> WeightFactors(rect.NrOfPoints());
    geo::IElement::TSourceValueVector values(rect.NrOfPoints());
    os << "CASE 1\n";
    os << "same value for each corner, equally weighted, Voight, returns same value for each corner\n";
    os << "input values: ";
    for(int i = 0; i < rect.NrOfPoints(); i++)
    {
      std::pair<const geo::IPoint*, geo::CValue> prValue;
      prValue.first = &rect.Point(i);
      prValue.second = geo::CValue(10);
      values[i] = prValue;
      os << values[i].second.Value() << " ";
      WeightFactors[i] = 10;
    }
    os << "\n";

    std::vector<geo::CValue> newvalues = rect.MapValues(values, geo::WT_VOIGHT, WeightFactors);

    os << "output values: "
      << newvalues[0].Value() << " "
      << newvalues[1].Value() << " "
      << newvalues[2].Value() << " "
      << newvalues[3].Value() << "\n";
  }

  os << "\n";

  // same value for each corner, equally weighted, Reuss, returns same value for each corner
  {
    std::vector<double> WeightFactors(rect.NrOfPoints());
    geo::IElement::TSourceValueVector values(rect.NrOfPoints());
    os << "CASE 2\n";
    os << "same value for each corner, equally weighted, Reuss, returns same value for each corner\n";
    os << "input values: ";
    for(int i = 0; i < rect.NrOfPoints(); i++)
    {
      std::pair<const geo::IPoint*, geo::CValue> prValue;
      prValue.first = &rect.Point(i);
      prValue.second = geo::CValue(10);
      values[i] = prValue;
      os << values[i].second.Value() << " ";
      WeightFactors[i] = 10;
    }
    os << "\n";

    std::vector<geo::CValue> newvalues = rect.MapValues(values, geo::WT_REUSS, WeightFactors);

    os << "output values: "
       << newvalues[0].Value() << " "
       << newvalues[1].Value() << " "
       << newvalues[2].Value() << " "
       << newvalues[3].Value() << "\n";
  }

  os << "\n";

  // different values in each corner, equally weighted, Voight, returns for each corner input value
  {
    std::vector<double> WeightFactors(rect.NrOfPoints());
    geo::IElement::TSourceValueVector values(rect.NrOfPoints());
    os << "CASE 3\n";
    os << "different values in each corner, equally weighted, Voight, returns for each corner input value\n";
    os << "input values: ";
    for(int i = 0; i < rect.NrOfPoints(); i++)
    {
      std::pair<const geo::IPoint*, geo::CValue> prValue;
      prValue.first = &rect.Point(i);
      prValue.second = geo::CValue(1 + 10 * i);
      values[i] = prValue;
      os << values[i].second.Value() << " ";
      WeightFactors[i] = 10;
    }
    os << "\n";

    std::vector<geo::CValue> newvalues = rect.MapValues(values, geo::WT_VOIGHT, WeightFactors);

    os << "output values: "
      << newvalues[0].Value() << " "
      << newvalues[1].Value() << " "
      << newvalues[2].Value() << " "
      << newvalues[3].Value() << "\n";
  }

  os << "\n";

  // different values in each corner, equally weighted, Reuss, returns for each corner input value
  {
    std::vector<double> WeightFactors(rect.NrOfPoints());
    geo::IElement::TSourceValueVector values(rect.NrOfPoints());
    os << "CASE 4\n";
    os << "different values in each corner, equally weighted, Reuss, returns for each corner input value\n";
    os << "input values: ";
    for(int i = 0; i < rect.NrOfPoints(); i++)
    {
      std::pair<const geo::IPoint*, geo::CValue> prValue;
      prValue.first = &rect.Point(i);
      prValue.second = geo::CValue(1 + 10 * i);
      values[i] = prValue;
      os << values[i].second.Value() << " ";
      WeightFactors[i] = 10;
    }
    os << "\n";

    std::vector<geo::CValue> newvalues = rect.MapValues(values, geo::WT_REUSS, WeightFactors);

    os << "output values: "
      << newvalues[0].Value() << " "
      << newvalues[1].Value() << " "
      << newvalues[2].Value() << " "
      << newvalues[3].Value() << "\n";
  }

  os << "\n";

  // different values in each corner, different weight factor in each corner, Voight, returns for each corner input value
  {
    std::vector<double> WeightFactors(rect.NrOfPoints());
    geo::IElement::TSourceValueVector values(rect.NrOfPoints());
    os << "CASE 5\n";
    os << "different values in each corner, different weight factor in each corner, Voight, returns for each corner input value\n";
    os << "input values: ";
    for(int i = 0; i < rect.NrOfPoints(); i++)
    {
      std::pair<const geo::IPoint*, geo::CValue> prValue;
      prValue.first = &rect.Point(i);
      prValue.second = geo::CValue(1 + 10 * i);
      values[i] = prValue;
      os << values[i].second.Value() << " ";
      WeightFactors[i] = 1 + 10 * i;
    }
    os << "\n";

    std::vector<geo::CValue> newvalues = rect.MapValues(values, geo::WT_VOIGHT, WeightFactors);

    os << "output values: "
      << newvalues[0].Value() << " "
      << newvalues[1].Value() << " "
      << newvalues[2].Value() << " "
      << newvalues[3].Value() << "\n";
  }

  os << "\n";

  // different values in each corner, different weight factor in each corner, Reuss, returns for each corner input value
  {
    std::vector<double> WeightFactors(rect.NrOfPoints());
    geo::IElement::TSourceValueVector values(rect.NrOfPoints());
    os << "CASE 6\n";
    os << "different values in each corner, different weight factor in each corner, Reuss, returns for each corner input value\n";
    os << "input values: ";
    for(int i = 0; i < rect.NrOfPoints(); i++)
    {
      std::pair<const geo::IPoint*, geo::CValue> prValue;
      prValue.first = &rect.Point(i);
      prValue.second = geo::CValue(1 + 10 * i);
      values[i] = prValue;
      os << values[i].second.Value() << " ";
      WeightFactors[i] = 1 + 10 * i;
    }
    os << "\n";

    std::vector<geo::CValue> newvalues = rect.MapValues(values, geo::WT_REUSS, WeightFactors);

    os << "output values: "
      << newvalues[0].Value() << " "
      << newvalues[1].Value() << " "
      << newvalues[2].Value() << " "
      << newvalues[3].Value() << " ";
  }

  os << "\n";

  // same value for each corner, same one in middle, equally weighted, Voight, returns same value for each corner
  {
    std::vector<double> WeightFactors(rect.NrOfPoints() + 1);
    geo::IElement::TSourceValueVector values(rect.NrOfPoints() + 1);
    os << "CASE 7\n";
    os << "same value for each corner, same one in middle, equally weighted, Voight, returns same value for each corner\n";
    os << "input values: ";
    for(int i = 0; i < rect.NrOfPoints(); i++)
    {
      std::pair<const geo::IPoint*, geo::CValue> prValue;
      prValue.first = &rect.Point(i);
      prValue.second = geo::CValue(10);
      values[i] = prValue;
      os << values[i].second.Value() << " ";
      WeightFactors[i] = 10;
    }

    std::pair<const geo::IPoint*, geo::CValue> prValue;
    geo::CPoint point(
      (rect.Point(0).X() + rect.Point(1).X() + rect.Point(2).X() + rect.Point(3).X()) / 4,
      (rect.Point(0).Y() + rect.Point(1).Y() + rect.Point(2).Y() + rect.Point(3).Y()) / 4,
      (rect.Point(0).Z() + rect.Point(1).Z() + rect.Point(2).Z() + rect.Point(3).Z()) / 4);

    prValue.first = &point;
    prValue.second = geo::CValue(10);
    values[4] = prValue;
    WeightFactors[4] = 10;
    os << "\n";

    std::vector<geo::CValue> newvalues = rect.MapValues(values, geo::WT_VOIGHT, WeightFactors);

    os << "output values: "
      << newvalues[0].Value() << " "
      << newvalues[1].Value() << " "
      << newvalues[2].Value() << " "
      << newvalues[3].Value() << "\n";
  }

  os << "\n";

  // same value for each corner, same one in middle, equally weighted, Reuss, returns same value for each corner
  {
    std::vector<double> WeightFactors(rect.NrOfPoints() + 1);
    geo::IElement::TSourceValueVector values(rect.NrOfPoints() + 1);
    os << "CASE 8\n";
    os << "same value for each corner, same one in middle, equally weighted, Reuss, returns same value for each corner\n";
    os << "input values: ";
    for(int i = 0; i < rect.NrOfPoints(); i++)
    {
      std::pair<const geo::IPoint*, geo::CValue> prValue;
      prValue.first = &rect.Point(i);
      prValue.second = geo::CValue(10);
      values[i] = prValue;
      os << values[i].second.Value() << " ";
      WeightFactors[i] = 10;
    }

    std::pair<const geo::IPoint*, geo::CValue> prValue;
    geo::CPoint point(
      (rect.Point(0).X() + rect.Point(1).X() + rect.Point(2).X() + rect.Point(3).X()) / 4,
      (rect.Point(0).Y() + rect.Point(1).Y() + rect.Point(2).Y() + rect.Point(3).Y()) / 4,
      (rect.Point(0).Z() + rect.Point(1).Z() + rect.Point(2).Z() + rect.Point(3).Z()) / 4);

    prValue.first = &point;
    prValue.second = geo::CValue(10);
    values[4] = prValue;
    WeightFactors[4] = 10;
    os << "\n";

    std::vector<geo::CValue> newvalues = rect.MapValues(values, geo::WT_REUSS, WeightFactors);

    os << "output values: "
      << newvalues[0].Value() << " "
      << newvalues[1].Value() << " "
      << newvalues[2].Value() << " "
      << newvalues[3].Value() << "\n";
  }

  os << "\n";

  // same value for each corner, different one in middle, equally weighted, Voight,
  // returns plane located at one fifth distance between corner input an midpoint input above corner input values
  {
    std::vector<double> WeightFactors(rect.NrOfPoints() + 1);
    geo::IElement::TSourceValueVector values(rect.NrOfPoints() + 1);
    os << "CASE 9\n";
    os << "same value for each corner, different one in middle, equally weighted, Voight,\n";
    os << "returns plane located at one fifth distance between corner input an midpoint input above corner input values\n";
    os << "input values: ";
    for(int i = 0; i < rect.NrOfPoints(); i++)
    {
      std::pair<const geo::IPoint*, geo::CValue> prValue;
      prValue.first = &rect.Point(i);
      prValue.second = geo::CValue(10);
      values[i] = prValue;
      os << values[i].second.Value() << " ";
      WeightFactors[i] = 10;
    }

    std::pair<const geo::IPoint*, geo::CValue> prValue;
    geo::CPoint point(
      (rect.Point(0).X() + rect.Point(1).X() + rect.Point(2).X() + rect.Point(3).X()) / 4,
      (rect.Point(0).Y() + rect.Point(1).Y() + rect.Point(2).Y() + rect.Point(3).Y()) / 4,
      (rect.Point(0).Z() + rect.Point(1).Z() + rect.Point(2).Z() + rect.Point(3).Z()) / 4);

    prValue.first = &point;
    prValue.second = geo::CValue(20);
    values[4] = prValue;
    WeightFactors[4] = 10;
    os << "\n";

    std::vector<geo::CValue> newvalues = rect.MapValues(values, geo::WT_VOIGHT, WeightFactors);

    os << "output values: "
      << newvalues[0].Value() << " "
      << newvalues[1].Value() << " "
      << newvalues[2].Value() << " "
      << newvalues[3].Value() << "\n";
  }

  os << "\n";

  // same value for each corner, different one in middle, equally weighted, Reuss
  {
    std::vector<double> WeightFactors(rect.NrOfPoints() + 1);
    geo::IElement::TSourceValueVector values(rect.NrOfPoints() + 1);
    os << "CASE 10\n";
    os << "same value for each corner, different one in middle, equally weighted, Reuss\n";
    os << "input values: ";
    for(int i = 0; i < rect.NrOfPoints(); i++)
    {
      std::pair<const geo::IPoint*, geo::CValue> prValue;
      prValue.first = &rect.Point(i);
      prValue.second = geo::CValue(10);
      values[i] = prValue;
      os << values[i].second.Value() << " ";
      WeightFactors[i] = 10;
    }

    std::pair<const geo::IPoint*, geo::CValue> prValue;
    geo::CPoint point(
      (rect.Point(0).X() + rect.Point(1).X() + rect.Point(2).X() + rect.Point(3).X()) / 4,
      (rect.Point(0).Y() + rect.Point(1).Y() + rect.Point(2).Y() + rect.Point(3).Y()) / 4,
      (rect.Point(0).Z() + rect.Point(1).Z() + rect.Point(2).Z() + rect.Point(3).Z()) / 4);

    prValue.first = &point;
    prValue.second = geo::CValue(20);
    values[4] = prValue;
    WeightFactors[4] = 10;
    os << "\n";

    std::vector<geo::CValue> newvalues = rect.MapValues(values, geo::WT_REUSS, WeightFactors);

    os << "output values: "
      << newvalues[0].Value() << " "
      << newvalues[1].Value() << " "
      << newvalues[2].Value() << " "
      << newvalues[3].Value() << "\n";
  }

  os << "\n";

  // same value for each corner, equally weighted, different one in middle, weighted twice as heavy, Voight,
  // returns plane located at two sixth (1/3) distance between corner input an midpoint input above corner input values
  {
    std::vector<double> WeightFactors(rect.NrOfPoints() + 1);
    geo::IElement::TSourceValueVector values(rect.NrOfPoints() + 1);
    os << "CASE 11\n";
    os << "same value for each corner, equally weighted, different one in middle, weighted twice as heavy, Voight,\n";
    os << "returns plane located at two sixth (1/3) distance between corner input an midpoint input above corner input values\n";
    os << "input values: ";
    for(int i = 0; i < rect.NrOfPoints(); i++)
    {
      std::pair<const geo::IPoint*, geo::CValue> prValue;
      prValue.first = &rect.Point(i);
      prValue.second = geo::CValue(10);
      values[i] = prValue;
      os << values[i].second.Value() << " ";
      WeightFactors[i] = 10;
    }

    std::pair<const geo::IPoint*, geo::CValue> prValue;
    geo::CPoint point(
      (rect.Point(0).X() + rect.Point(1).X() + rect.Point(2).X() + rect.Point(3).X()) / 4,
      (rect.Point(0).Y() + rect.Point(1).Y() + rect.Point(2).Y() + rect.Point(3).Y()) / 4,
      (rect.Point(0).Z() + rect.Point(1).Z() + rect.Point(2).Z() + rect.Point(3).Z()) / 4);

    prValue.first = &point;
    prValue.second = geo::CValue(20);
    values[4] = prValue;
    WeightFactors[4] = 20;
    os << "\n";

    std::vector<geo::CValue> newvalues = rect.MapValues(values, geo::WT_VOIGHT, WeightFactors);

    os << "output values: "
      << newvalues[0].Value() << " "
      << newvalues[1].Value() << " "
      << newvalues[2].Value() << " "
      << newvalues[3].Value() << "\n";
  }

  os << "\n";

  // same value for each corner, equally weighted, different one in middle, weighted twice as heavy, Reuss
  {
    std::vector<double> WeightFactors(rect.NrOfPoints() + 1);
    geo::IElement::TSourceValueVector values(rect.NrOfPoints() + 1);
    os << "CASE 12\n";
    os << "same value for each corner, equally weighted, different one in middle, weighted twice as heavy, Reuss\n";
    os << "input values: ";
    for(int i = 0; i < rect.NrOfPoints(); i++)
    {
      std::pair<const geo::IPoint*, geo::CValue> prValue;
      prValue.first = &rect.Point(i);
      prValue.second = geo::CValue(10);
      values[i] = prValue;
      os << values[i].second.Value() << " ";
      WeightFactors[i] = 10;
    }

    std::pair<const geo::IPoint*, geo::CValue> prValue;
    geo::CPoint point(
      (rect.Point(0).X() + rect.Point(1).X() + rect.Point(2).X() + rect.Point(3).X()) / 4,
      (rect.Point(0).Y() + rect.Point(1).Y() + rect.Point(2).Y() + rect.Point(3).Y()) / 4,
      (rect.Point(0).Z() + rect.Point(1).Z() + rect.Point(2).Z() + rect.Point(3).Z()) / 4);

    prValue.first = &point;
    prValue.second = geo::CValue(40);
    values[4] = prValue;
    WeightFactors[4] = 40;
    os << "\n";

    std::vector<geo::CValue> newvalues = rect.MapValues(values, geo::WT_REUSS, WeightFactors);

    os << "output values: "
      << newvalues[0].Value() << " "
      << newvalues[1].Value() << " "
      << newvalues[2].Value() << " "
      << newvalues[3].Value() << "\n";
  }

  os << "\n";

  os << "END MAPPING TEST\n";
}

void init_matrix(GeometryTestData &testData)
{
  QTextStream &os = testData.OS();

  testData.NewLine(2);
  os << "     ************************* MATRIX TEST **********************" << endl;
  os << "     * Geometry test file                                       *" << endl ;
  os << "     * testing of the matrix                                    *" << endl;
  os << "     ************************************************************" << endl;
  testData.NewLine(2);

  double angledeg = 35;
  double anglerad = 0.610865;
  geo::CVector rotationaxis(1,1,1); 
  geo::CMatrix rotation;


  ////MATRIX TESTS///////////////////////////////////////////////////////////////////////
  //Default constructor en ReSize///////////////////////////////////////////////////////

    geo::CMatrix mattest1;
  mattest1.ReSize(5,5);
  
  mattest1.Value(0,0, -10);
  mattest1.Value(0,1, -9);
  mattest1.Value(0,2, -8);
  mattest1.Value(0,3, -7);
  mattest1.Value(0,4, 0);
  mattest1.Value(1,0, 0);
  mattest1.Value(1,1, -4);
  mattest1.Value(1,2, -3);
  mattest1.Value(1,3, -2);
  mattest1.Value(1,4, -1);
  mattest1.Value(2,0, 0);
  mattest1.Value(2,1, 1);
  mattest1.Value(2,2, 0);
  mattest1.Value(2,3, 3);
  mattest1.Value(2,4, 4);
  mattest1.Value(3,0, 5);
  mattest1.Value(3,1, 6);
  mattest1.Value(3,2, 7);
  mattest1.Value(3,3, 0);
  mattest1.Value(3,4, 9);
  mattest1.Value(4,0, 0);
  mattest1.Value(4,1, 11);
  mattest1.Value(4,2, 12);
  mattest1.Value(4,3, 13);
  mattest1.Value(4,4, 14);

  os << "The test matrix is printed below.\n";
  testData.PrintMatrix(mattest1);
  os << "\n";

/////////////////////////////////////////////////////////////////////////////////////////

  geo::CMatrix mattest2(5,5);

  mattest2.Value(0,0, -10);
  mattest2.Value(0,1, -9);
  mattest2.Value(0,2, -8);
  mattest2.Value(0,3, -7);
  mattest2.Value(0,4, 0);
  mattest2.Value(1,0, 0);
  mattest2.Value(1,1, -4);
  mattest2.Value(1,2, -3);
  mattest2.Value(1,3, -2);
  mattest2.Value(1,4, -1);
  mattest2.Value(2,0, 0);
  mattest2.Value(2,1, 1);
  mattest2.Value(2,2, 0);
  mattest2.Value(2,3, 3);
  mattest2.Value(2,4, 4);
  mattest2.Value(3,0, 5);
  mattest2.Value(3,1, 6);
  mattest2.Value(3,2, 7);
  mattest2.Value(3,3, 0);
  mattest2.Value(3,4, 9);
  mattest2.Value(4,0, 0);
  mattest2.Value(4,1, 11);
  mattest2.Value(4,2, 12);
  mattest2.Value(4,3, 13);
  mattest2.Value(4,4, 14);

////Upper matrix of matrix///////////////////////////////////////////////////////////////

  geo::CMatrix mattest2upper = mattest2.UpperMatrix();

  os << "The Upper Matrix of the test matrix is printed below.\n";
  testData.PrintMatrix(mattest2upper);
  os << "\n";

////Transposed of matrix/////////////////////////////////////////////////////////////////

  geo::CMatrix mattest2transposed = mattest2.GetTransposed();
  testData.PrintMatrix(mattest2transposed);
  os << "\n";

////Determinant of matrix///////////////////////////////////////////////////////////////
  
  double determinant = mattest2.GetDeterminant();
  os << "The determinant of the test matrix is printed below.\n";
  os << determinant;
  os << "\n";

////Inverse of matrix///////////////////////////////////////////////////////////////////
  
  geo::CMatrix mattest2inverse = mattest2.GetInverse();
  os << "The inverse of the test matrix is printed below.\n";
  testData.PrintMatrix(mattest2inverse);
  os << "\n";

////Make rotation matrix around X axis angle = 35 degrees///////////////////////////////


  rotation.CreateRotationMatrixDeg(geo::CVector::Xaxis, angledeg);
  os << "Rotation matrix around X axis by 35 degrees is printed below.\n";
  testData.PrintMatrix(rotation);
  os << "\n";

////Make rotation matrix around Y axis angle = 35 degrees///////////////////////////////

  rotation.CreateRotationMatrixDeg(geo::CVector::Yaxis, angledeg);
  os << "Rotation matrix around Y axis by 35 degrees is printed below.\n";
  testData.PrintMatrix(rotation);
  os << "\n";

////Make rotation matrix around Z axis angle = 35 degrees///////////////////////////////

  rotation.CreateRotationMatrixDeg(geo::CVector::Zaxis, angledeg);
  os << "Rotation matrix around Z axis by 35 degrees is printed below.\n";
  testData.PrintMatrix(rotation);
  os << "\n";

////Make rotation matrix around X axis angle = 0.610865 rad/////////////////////////////

  rotation.CreateRotationMatrixRad(geo::CVector::Xaxis, anglerad);
  os << "Rotation matrix around X axis by 0.610865 rad is printed below.\n";
  testData.PrintMatrix(rotation);
  os << "\n";

////Make rotation matrix around Y axis angle = 0.610865 rad/////////////////////////////

  rotation.CreateRotationMatrixRad(geo::CVector::Yaxis, anglerad);
  os << "Rotation matrix around Y axis by 0.610865 rad is printed below.\n";
  testData.PrintMatrix(rotation);
  os << "\n";

////Make rotation matrix around Z axis angle = 0.610865 rad/////////////////////////////


  rotation.CreateRotationMatrixRad(geo::CVector::Zaxis, anglerad);
  os << "Rotation matrix around Z axis by 0.610865 rad is printed below.\n";
  testData.PrintMatrix(rotation);
  os << "\n";

////Make rotation matrix around vector (1,1,1) angle = 35 degrees///////////////////////

  rotation.CreateRotationMatrixDeg(rotationaxis, angledeg);
  os << "Rotation matrix around (1,1,1) by 35 degrees is printed below.\n";
  testData.PrintMatrix(rotation);
  os << "\n";

////Make rotation matrix around vector (1,1,1) angle = 0.610865 rad//////////////////////////

  rotation.CreateRotationMatrixRad(rotationaxis, anglerad);
  os << "Rotation matrix around (1,1,1) by 0.610865 rad is printed below.\n";
  testData.PrintMatrix(rotation);
  os << "\n";

////Make Identity matrix///////////////////////////////////////////////////////////////

  geo::CMatrix identity;

  identity.CreateIdentityMatrix(5);

  os << "A 5 by 5 Identity matrix is printed below.\n";
  testData.PrintMatrix(identity);
  os << "\n";

////Make 6 by 6 Stress rotation matrix, angle in degrees////////////////////////////////
  rotation.CreateRotationMatrixDeg(rotationaxis, angledeg);
  geo::CMatrix stressrotationdeg = rotation.StressRotation();
  os << "The stress rotation matrix around (1,1,1) by 35 degrees is printed below.\n";
  testData.PrintMatrix(stressrotationdeg);
  os << "\n";

////Make 6 by 6 Stress rotation matrix, angle in radians////////////////////////////////
  rotation.CreateRotationMatrixRad(rotationaxis, anglerad);
  geo::CMatrix stressrotationrad = rotation.StressRotation();
  os << "The stress rotation matrix around (1,1,1) by 0.610865 rad is printed below.\n";
  testData.PrintMatrix(stressrotationrad);
  os << "\n";
  //TODO change definition of element stressrotationmatrix[2][5], stressrotationmatrix[4][5]


////Make 6 by 6 Strain rotation matrix, angle in degrees////////////////////////////////
  rotation.CreateRotationMatrixDeg(rotationaxis, angledeg);
  geo::CMatrix strainrotationdeg = rotation.StrainRotation();
  os << "The strain rotation matrix around (1,1,1) by 35 degrees is printed below.\n";
  testData.PrintMatrix(strainrotationdeg);
  os << "\n";


////Make 6 by 6 Strain rotation matrix, angle in radians////////////////////////////////
  rotation.CreateRotationMatrixRad(rotationaxis, anglerad);
  geo::CMatrix strainrotationrad = rotation.StrainRotation();
  os << "The strain rotation matrix around (1,1,1) by 0.610865 rad is printed below.\n";
  testData.PrintMatrix(strainrotationrad);
  os << "\n";

////Eigenvalues and eigenvectors////////////////////////////////////////////

  geo::CMatrix mattest3(3,3);

  mattest3.Value(0,0, 2.731821289);
  mattest3.Value(1,0, 0.619763867);
  mattest3.Value(2,0, -0.319602942);
  mattest3.Value(0,1, 0.619763867);
  mattest3.Value(1,1, 4.649066665);
  mattest3.Value(2,1, 0.738605815);
  mattest3.Value(0,2, -0.319602942);
  mattest3.Value(1,2, 0.738605815);
  mattest3.Value(2,2, 2.619112046);

  geo::CMatrix eigenvalues = mattest3.EigenValues();
  
  os << "The eigenvalues can be found in the diagonal matrix below.\n";
  testData.PrintMatrix(eigenvalues);
  os << "\n";

  geo::CMatrix eigenvectors = mattest3.EigenVectors();

  os << "The eigenvectors can be found in the columns of the matrix below.\n";
  testData.PrintMatrix(eigenvectors);
  os << "\n";

////Operator "=" ///////////////////////////////////////////////////////////////
  geo::CMatrix mattest4;
  geo::CMatrix mattest5(2,3);

  mattest5.Value(0,0, 1.0);
  mattest5.Value(1,0, 2.0);
  mattest5.Value(0,1, 3.0);
  mattest5.Value(1,1, 4.0);
  mattest5.Value(0,2, 5.0);
  mattest5.Value(1,2, 6.0);

  os << "mattest5 = \n";
  testData.PrintMatrix(mattest5);
  os << "\n";

  mattest4 = mattest5;
  os << "The matrix below should be the same as mattest5.\n";
  testData.PrintMatrix(mattest4);
  os << "\n";

////Operator "-" ////////////////////////////////////////////////////////////////

  geo::CMatrix mattest6;
  mattest6 = mattest5 - mattest4;
  os << "The matrix below should contain zeros only.\n";
  testData.PrintMatrix(mattest6);
  os << "\n";


////Operator "+" ////////////////////////////////////////////////////////////////

  geo::CMatrix mattest7;
  mattest7 = mattest5 + mattest4;
  os << "The matrix below should print the sum of mattest4 and mattest5.\n";
  testData.PrintMatrix(mattest7);
  os << "\n";


////Operator "*" with double/////////////////////////////////////////////////////

  geo::CMatrix mattest8;
  mattest8 = mattest5 * 2.0;

  os << "The matrix below should print mattest5 times 2.0.\n";
  testData.PrintMatrix(mattest8);
  os << "\n";

////Operator "*" with another matrix////////////////////////////////////////////

  geo::CMatrix mattest9(3,2);
  geo::CMatrix mattest10;

  mattest9.Value(0,0, 1.0);
  mattest9.Value(1,0, 1.0);
  mattest9.Value(2,0, 1.0);
  mattest9.Value(0,1, 1.0);
  mattest9.Value(1,1, 1.0);
  mattest9.Value(2,1, 1.0);

  mattest10 = mattest5 * mattest9;
  os << "The matrix below should print the multiplication between mattest5 and mattest9.\n";
  testData.PrintMatrix(mattest10);
  os << "\n";

/////////////////////////////////////////////////////////////////////////////////

  // 2x2 matrix test (inverse is calculated differently):
  geo::CMatrix mat2(2, 2);
  mat2.Value(0, 0, 10);
  mat2.Value(0, 1, 5);
  mat2.Value(1, 0, 4);
  mat2.Value(1, 1, -6);
  os << "2 x 2 matrix:\n";
  testData.PrintMatrix(mat2);
  os << "\n";

  double det = mat2.GetDeterminant();
  os << "determinant = " << det << "\n", det;

  geo::CMatrix mat2inv = mat2.GetInverse();
  os << "inverse:\n";
  testData.PrintMatrix(mat2inv);
  os << "\n";

  geo::CMatrix ident = mat2 * mat2inv;
  os << "original matrix * inverse (should give identity):\n";
  testData.PrintMatrix(ident);

  // also 3x3 matrix inverse is calculated differently
  geo::CMatrix mat3(3, 3);
  mat3.Value(0, 0, 11);
  mat3.Value(0, 1, 12);
  mat3.Value(0, 2, 13);
  mat3.Value(1, 0, -21);
  mat3.Value(1, 1, -22);
  mat3.Value(1, 2, -23);
  mat3.Value(2, 0, 131);
  mat3.Value(2, 1, 32);
  mat3.Value(2, 2, 33);
  os << "3 x 3 matrix:\n";
  testData.PrintMatrix(mat3);
  os << "\n";

  geo::CMatrix mat3inv = mat3.GetInverse();
  os << "inverse:\n";
  testData.PrintMatrix(mat3inv);
  os << "\n";

  geo::CMatrix ident3 = mat3 * mat3inv;
  os << "original matrix * inverse (should give identity):\n";
  testData.PrintMatrix(ident3);
  os << "\n";

  ident3 = mat3inv * mat3;
  os << "inverse * original (should also give identity):\n";
  testData.PrintMatrix(ident3);

  testData.NewLine(2);
  os << "     ********************** END MATRIX TEST *******************" << endl;



}

void init_plane(GeometryTestData &testData)
{
  QTextStream &os = testData.OS();
  
  os << "     ******************* PLANE TEST *****************************" << endl;
  os << "     * Geometry test file                                       *" << endl;
  os << "     * testing of the plane functionality of the geometry lib   *" << endl;
  os << "     ************************************************************" << endl;

  testData.NewLine(2);

  const geo::CPoint p1=geo::CPoint(1,0,0);
  const geo::CPoint p2=geo::CPoint(0,1,0);
  const geo::CPoint p3=geo::CPoint(0,0,1);


  testData.PrintObject(p1,"p1 = ");
  testData.PrintObject(p2,"p2 = ");
  testData.PrintObject(p3,"p3 = ");

  
  geo::CPlane plane1(p1,p2,p3);


  testData.PrintObject( plane1,"plane1(p1,p2,p3) := ");
  geo::CLine l1(geo::CPoint(0,0,0),geo::CPoint(10,10,10));
  testData.PrintObject(l1,"l1 := ");
  testData.PrintObject(plane1.Intersection(l1),"plane1.Intersection(l1) = ");
  testData.PrintObject(plane1.Project(geo::CPoint::NullPoint),"plane1.Project(geo::CPoint::NullPoint) = ");
  os << "plane1.Distance(geo::CPoint::NullPoint) = " << plane1.Distance(geo::CPoint::NullPoint) << endl;
  os << "plane1.SquareDistance(geo::CPoint::NullPoint) = " << plane1.SquareDistance(geo::CPoint::NullPoint) << endl;
  geo::CPlane plane2(geo::CPlane::PlaneXY);
  testData.PrintObject(plane2,"plane2(geo::CPlane::PlaneXY) := ");
  testData.PrintObject(plane1.Intersection(plane2),"plane1.Intersection(plane2) = ");

  os << "plane2.Rotate(geo::CVector::Xaxis,90)" << endl;
  plane2.Rotate(geo::CVector::Xaxis,90);
  os << "plane2.IsParallel(geo::CPlane::PlaneZX) = " << plane2.IsParallel(geo::CPlane::PlaneZX) << endl;
  testData.PrintObject(geo::CPlane::PlaneZX.Intersection(geo::CPlane::PlaneZX),"geo::CPlane::PlaneZX.Intersection(geo::CPlane::PlaneZX) = ");
  

  testData.NewLine(2);
  os << "     ********************** END PLANE TEST *********************" << endl;
}

void init_point(GeometryTestData &testData)
{
  QTextStream &os = testData.OS();
  
  os << "     ******************* POINT TEST *****************************" << endl;
  os << "     * Geometry test file                                       *" << endl ;
  os << "     * testing of the point functionality of the geometry lib   *" << endl;
  os << "     ************************************************************" << endl;

  geo::CPoint p1=geo::CPoint(1,1,1);
  geo::CPoint p2=geo::CPoint(2,2,2);

  testData.PrintObject(p1,"p1 := ");
  testData.PrintObject(p2,"p2 := ");

  os << "(p1==p2) = " << (p1==p2) << endl;
  os << "(p1<p2) = "  << (p1<p2) << endl;
  os << "(p1>p2) = "  << (p1>p2) << endl;
  

  os << "p1.Distance(p2) = " << p1.Distance(p2) << endl;
  os << "p1.SquareDistance(p2) = " << p1.SquareDistance(p2) << endl;

  testData.PrintObject(p1+p2,"p1 + p2 = ");
  testData.PrintObject(p2-p1,"p2 - p1 = ");
  testData.PrintObject(p1*3.0 ,"p1*3.0 = ");
  testData.PrintObject(p1/2.0 ,"p1/2.0 = ");

  os << "p1.Rotate(geo::Zaxis,90.0)" << endl;
  p1.Rotate(geo::CVector::Zaxis,90.0);
  testData.PrintObject(p1 ,"p1 = ");

  os << "p1.Rotate(geo::CVector::Zaxis,-90.0)" << endl;
  p1.Rotate(geo::CVector::Zaxis,-90.0);
  testData.PrintObject(p1 ,"p1 = ");

  os << "p1.Rotate(geo::CVector(3,8,6),33.6)" << endl;
  p1.Rotate(geo::CVector(3,8,6),33.6);
  testData.PrintObject(p1 ,"p1 = ");

  os << "p1.Rotate(geo::CVector(3,8,6),-33.6);)" << endl;
  p1.Rotate(geo::CVector(3,8,6),-33.6);
  testData.PrintObject(p1 ,"p1 = ");

  geo::CMatrix mat(3,4);
  mat.Value(0,3,1.0);
  mat.Value(1,3,2.0);
  mat.Value(2,3,3.0);
  mat.Value(0,0,1.0);
  mat.Value(1,1,1.0);
  mat.Value(2,2,1.0);

  testData.PrintMatrix(mat,"mat := ");

  os << "p1.Transform(mat)" << endl;
  p1.Transform(mat);
  testData.PrintObject(p1 ,"p1 = ");

  os << "p1.Move(geo::CVector(1,1,1))" << endl;
  p1.Move(geo::CVector(1,1,1));
  testData.PrintObject(p1 ,"p1 = ");
  
  geo::CPoint p3;
  p3=p1;
  os << "p3 = p1" << endl;
  testData.PrintObject(p3 ,"p3 = ");
  
  testData.NewLine(2);
  os << "     ********************** END POINT TEST *********************" << endl;
  
}

void init_polyline(GeometryTestData &testData)
{
  QTextStream &os = testData.OS();

  testData.NewLine(2);
  os << "     ************************* POLYLINE TEST ********************" << endl;
  os << "     * Geometry test file                                       *" << endl ;
  os << "     * testing of the polyline                                  *" << endl;
  os << "     ************************************************************" << endl;
  testData.NewLine(2);


  geo::CArray<geo::CPoint> arr;
  geo::CPoint p0(4,2,1);
  arr.PushBack(p0);
  p0 = geo::CPoint(6.5,3,2);
  arr.PushBack(p0);
  p0 = geo::CPoint(2.5,6,3);
  arr.PushBack(p0);
  p0 = geo::CPoint(7,9,4);
  arr.PushBack(p0);

  geo::CPolyLine polyline1(arr);
  testData.PrintObject(polyline1,"polyline1 :=");

  geo::CPoint p1(6,1,0);
  geo::CPoint p2(3,9,0);
  geo::CPoint p3(6,1,10);

  geo::CPlane plane;
  testData.PrintObject(plane,"plane = ");

  plane = geo::CPlane(p1,p2,p3);
  testData.PrintObject(plane,"plane := ");

  geo::CArray<geo::CPoint> ret;
  ret = polyline1.Intersection(plane);

  for(size_t i=0;i<ret.Size();i++)
  {
    std::stringstream str;
    str << "intersection point[" << i << "] = ";
    testData.PrintObject(ret.Object(i),str.str().c_str());
  }

  testData.NewLine(2);
  os << "     ********************** END POLYLINE TEST *******************" << endl;
}

void init_setmapping(GeometryTestData &testData)
{


  struct MakeHexMesh
  {
    MakeHexMesh(int nX, int nY, int nZ, double sizeX, double sizeY, double sizeZ,
                double offsetX, double offsetY, double offsetZ, geo::IMesh &mesh)
    {
      std::vector<std::vector<std::vector<geo::CPoint*> > > vcNodes;
      vcNodes.resize(nX + 1);

      // nodes
      int i;
      for(i = 0; i < nX + 1; i++)
      {
        vcNodes[i].resize(nY + 1);
        for(int j = 0; j < nY + 1; j++)
        {
          vcNodes[i][j].resize(nZ + 1);
          for(int k = 0; k < nZ + 1; k++)
          {
            vcNodes[i][j][k] = new geo::CPoint(sizeX * i + offsetX, sizeY * j + offsetY, sizeZ * k + offsetZ);
          }
        }
      }

      // elements
      for(i = 0; i < nX; i++)
      {
        for(int j = 0; j < nY; j++)
        {
          for(int k = 0; k < nZ; k++)
          {
            std::vector<const geo::IPoint *> vcHexaNodes(8);
            vcHexaNodes[0] = vcNodes[i][j][k];
            vcHexaNodes[1] = vcNodes[i+1][j][k];
            vcHexaNodes[2] = vcNodes[i+1][j+1][k];
            vcHexaNodes[3] = vcNodes[i][j+1][k];
            vcHexaNodes[4] = vcNodes[i][j][k+1];
            vcHexaNodes[5] = vcNodes[i+1][j][k+1];
            vcHexaNodes[6] = vcNodes[i+1][j+1][k+1];
            vcHexaNodes[7] = vcNodes[i][j+1][k+1];

            new geo::CHexahedron(mesh, vcHexaNodes);
          }
        }
      }

      // points are copied by the mesh, so can be deleted now
      // (elements are deleted automagically by the mesh)
      for(i = 0; i < nX + 1; i++)
      {
        for(int j = 0; j < nY + 1; j++)
        {
          for(int k = 0; k < nZ + 1; k++) delete vcNodes[i][j][k];
        }
      }
    }
  };

  class CProgress : public IProgressBase
  {
  private:
   int m_nSteps;
   int m_nCurrentStep;
   QTextStream *m_os;
  public:
    CProgress(QTextStream *os): m_nSteps(0), m_nCurrentStep(0), m_os(os) {}
    ~CProgress() {}

    virtual void AddSteps(int nSteps) { m_nSteps = nSteps; }
    virtual void Step(int nSteps = 1) { m_nCurrentStep += nSteps; *m_os << "step " << m_nCurrentStep << " of " << m_nSteps << "...\n"; }
  };


  QTextStream &os = testData.OS();


  // target mesh
  geo::CMesh3D targetmesh;

  // 2 x 2 x 2 elements (8), each 5 x 5 x 5 in size, offset 5, 5, 5
  MakeHexMesh(2, 2, 2, 5, 5, 5, 0, 0, 0, targetmesh);


  // source mesh
  geo::CMesh3D sourcemesh;

  // 3 x 3 x 3 elements (27), each 30 in size
  MakeHexMesh(3, 3, 3, 30, 30, 30, 5, 5, 5, sourcemesh);

  // values on latter mesh
  std::vector<geo::CValueSet::TValueVec> vcValues(sourcemesh.ElementSize());
  os << "Input values:\n";
  int i;
  for(i = 0; i < sourcemesh.ElementSize(); i++)
  {
    os << "Source element " << i+1 << ":\n";
    vcValues[i].resize(sourcemesh.Element(i).NrOfPoints());
    for(int j = 0; j < sourcemesh.Element(i).NrOfPoints(); j++)
    {
      vcValues[i][j] = geo::CValue(sourcemesh.Element(i).Point(j).X());

      os << "Node " << j + 1 << " ("
        << sourcemesh.Element(i).Point(j).X() << ","
        << sourcemesh.Element(i).Point(j).Y() << ","
        << sourcemesh.Element(i).Point(j).Z() << "), value = "
        << vcValues[i][j].Value() << "\n";
    }
  }

  // make valueset
  geo::CValueSet vssource(sourcemesh, vcValues);

  // empty valueset for target mesh
  geo::CValueSet vstarget(targetmesh);

  CProgress prg(&os);
  vssource.MapValues(vstarget, geo::WT_NONE, geo::IParallelInitializationCallback::Sequential, &prg);

  os << "\nOutput values:\n";

  for(i = 0; i < targetmesh.ElementSize(); i++)
  {
    os << "Target element " << i+1 << ":\n";
    geo::CValueSet::TValueVec values(targetmesh.Element(i).NrOfPoints());
    vstarget.ElementValues(values, i, geo::IParallelInitializationCallback::Sequential);
    assert(values.size() == targetmesh.Element(i).NrOfPoints());
    for(int j = 0; j < targetmesh.Element(i).NrOfPoints(); j++)
    {
        os << "Node " << j+1 << " ("
        << targetmesh.Element(i).Point(j).X() << ","
        << targetmesh.Element(i).Point(j).Y() << ","
        << targetmesh.Element(i).Point(j).Z() << "), value = "
        << values[j].Representation().c_str() << "\n";
    }
  }

}

void init_sphere(GeometryTestData &testData)
{
  QTextStream &os = testData.OS();

	os << endl;
    double dRadius = 0.060299;
    geo::CPoint middle = geo::CPoint(0.599999999999, 0.060000001, 0);
    geo::CSphere sphere( middle, dRadius );
	os << "Sphere at (" << sphere.MidPoint() << ") with radius " << sphere.Radius() << "\n";

	geo::CLine line( middle, geo::CPoint(0.0018296784308, 0.0053179, 0.009058400));
	os << "Intersection with line (" << line.First() << ")-(" << line.Second() << ") at\n";
	geo::CArray<geo::CPoint> arPoint = sphere.Intersection( line );
	for(size_t i = 0; i < arPoint.Size(); i++)
		os << "P" << i << "(" << arPoint.Object(i) << ")\n";
		
}

void init_tetrahedron(GeometryTestData &testData)
{

  struct TestTetrahedron
  {
    TestTetrahedron(QTextStream &os, geo::CPoint* points, geo::CPoint* p)
    {
	
	    geo::CMesh3D mesh;

	    os << "Tetrahedron:" << endl;

	    int i;
	    for(i = 0; i < 4; i++)
	    {
		    os << "Point " << i + 1 << ": (" << points[i].X() << "," <<
				    	points[i].Y() << "," << points[i].Z() << ")" << endl;
	    }

	    os << endl;

    	geo::CTetrahedron tetra(mesh, points[0], points[1], points[2], points[3]);

	    os << "Tetrahedron volume: " << endl;
	    os << tetra.Volume() << endl << endl;

    	os << "Tetrahedron face areas:" << endl;

	    for(i = 0; i < tetra.NrOfFaces(); i++)
	    {
		    os << "Face " << i + 1 << ": " << tetra.Face(i).Area() <<
				    	endl;
	    }

	    os << endl;

	    os << "Tetrahedron line lengths:" << endl;
	    for(i = 0; i < tetra.NrOfLines(); i++)
	    {
    		os << "Line " << i + 1 << ": " << tetra.Line(i).Length() <<
		    			endl;
	    }

	    os << endl;
	    os << "Containment of corner points:" << endl;
	    for(i = 0; i < tetra.NrOfPoints(); i++)
    	{
		    os << "Point " << i + 1 << " is ";
		    if(! tetra.Contains(tetra.Point(i), true) ) os << "not ";
    		os << "contained including edge and ";
		    if(! tetra.Contains(tetra.Point(i), false) ) os << "not ";
		    os << "contained excluding edge.\n";
	    }

	    os << endl;
	    os << "Containment of faces' midpoints:" << endl;
	    for(i = 0; i < tetra.NrOfFaces(); i++)
	    {
		    bool b = tetra.Contains(tetra.Face(i).MidPoint(), true);
		    os << "Contains midpoint of face " << i + 1 << " is " <<
				    	(b?"TRUE":"FALSE") << endl;
		    if(p)
		    {
			    b = tetra.Face(i).Contains(*p, true);
			    os << "Face " << i + 1 << " " <<
     					(b?"CONTAINS":"DOES NOT CONTAIN") <<
			        		" user point" << endl;
			    os << "SignedDistance = " << tetra.Face(i).SignedDistance(*p, true) << endl;
		    }
	    }

	    os << endl;
	    os << "Containment of lines' midpoints:" << endl;
	    for(i = 0; i < tetra.NrOfLines(); i++)
	    {
		    bool b = tetra.Contains(tetra.Line(i).MidPoint(), true);
		    os << "Contains midpoint of line " << i + 1 << " is " <<
					   (b?"TRUE":"FALSE") << endl;
		    if(p)
		    {
			    b = tetra.Line(i).Contains(*p, true);
			    os << "Line " << i + 1 << " " <<
    					(b?"CONTAINS":"DOES NOT CONTAIN") <<
					    " user point" << endl;
		    }
	    }

	    if(p)
	    {
		    os << endl;
		    os << "Point (" << p->X() << "," << p->Y() << "," << p->Z() << ") is ";
		    if(! tetra.Contains(*p, true) ) os << "not ";
		    os << "contained including edge and ";
		    if(! tetra.Contains(*p, false) ) os << "not ";
		    os << "contained excluding edge.\n";
  	  }
    }

  };

  QTextStream &os = testData.OS();

	os << "****   Tetrahedron test ****" << endl << endl;
	
	geo::CPoint points[4];
	points[0] = geo::CPoint(0, 0, 0);
	points[1] = geo::CPoint(10, 0, 0);
	points[2] = geo::CPoint(0, 10, 0);
	points[3] = geo::CPoint(0, 0, 10);
	geo::CPoint p(5, 5, 5);
	TestTetrahedron(os, points, &p);

	points[0] = geo::CPoint(3414.47, 500, 1771.38);
	points[1] = geo::CPoint(3097.613, -459.2219, 2451.854);
	points[2] = geo::CPoint(3394.74, 0, 1651.32);
	points[3] = geo::CPoint(3487.371, 175.8415, 2845.912);
	p = geo::CPoint(3249.6367711090, 1, 2125.3716321760);
	TestTetrahedron(os, points, &p);
}

void init_tetra_mesh(GeometryTestData &testData)
{

  struct InsertPoint
  {
    InsertPoint(QTextStream &os, geo::IMesh& mesh, ftn_double_t x, ftn_double_t y, ftn_double_t z)
    {
	    int nIndex = mesh.RegisterNode(geo::CPoint(x, y, z));
	    os << "Insert point(" << x << "," << y << "," << z << ") at index " << nIndex << endl;
    }
  };


  struct InsertTetra
  {
    InsertTetra(QTextStream &os, geo::IMesh& mesh, int p0, int p1, int p2, int p3)
    {
    	geo::CTetrahedron* pElement = new geo::CTetrahedron(mesh, p0, p1, p2, p3);
    	os << "Insert " << pElement->Type().c_str() << " (" << p0 << "," << p1 << "," << p2 << "," << p3 << ") at index " << pElement->Index() << endl;
    }
  };
    
  struct BuildTetraMesh
  {
    BuildTetraMesh(QTextStream &os, geo::IMesh& mesh)
    {
    	// Insert points
    	InsertPoint(os, mesh, 100, 100, 0);		// 0
    	InsertPoint(os, mesh, 100, 100, 100);	// 1
    	InsertPoint(os, mesh, 0, 100, 100);		// 2
    	InsertPoint(os, mesh, 0, 100, 0);		// 3
    	InsertPoint(os, mesh, 0, 0, 100);		// 4
    	InsertPoint(os, mesh, 0, 0, 0);			// 5
    	InsertPoint(os, mesh, 100, 0, 100);		// 6
    	InsertPoint(os, mesh, 100, 0, 0);		// 7
    	InsertPoint(os, mesh, 50, 50, 50);		// 8
    	os << endl;
    	// Insert tetra elements
    	InsertTetra(os, mesh, 0, 2, 8, 1);  // 0
    	InsertTetra(os, mesh, 5, 7, 0, 6);	// 1
    	InsertTetra(os, mesh, 4, 5, 8, 6);	// 2
    	InsertTetra(os, mesh, 3, 5, 0, 2);	// 3
    	InsertTetra(os, mesh, 5, 0, 8, 6);	// 4
    	InsertTetra(os, mesh, 4, 2, 8, 5);	// 5
    	InsertTetra(os, mesh, 2, 0, 8, 5);	// 6
    	InsertTetra(os, mesh, 6, 8, 4, 1);	// 7
    	InsertTetra(os, mesh, 1, 2, 8, 4);	// 8
    	InsertTetra(os, mesh, 0, 1, 8, 6);	// 9
    
    	os << endl;
    	os << "ElementSize = " << mesh.ElementSize() << endl;
    	os << "PointSize = " << mesh.PointSize() << endl;
    	os << "Min = (" << mesh.Min().X() << "," << mesh.Min().Y() << "," << mesh.Min().Z() << ")" << endl;
    	os << "Max = (" << mesh.Max().X() << "," << mesh.Max().Y() << "," << mesh.Max().Z() << ")" << endl;
    	os << endl;
    
    	for(int i = 0; i < mesh.PointSize(); i++)
    	{
    		const geo::INode& node = dynamic_cast<const geo::INode&>(mesh.Point(i));
    		os << "Point " << node.Index() << " connected to element ";
    		for(int j = 0; j < node.AttachedElementSize(); j++)
    		{
    			if(j != 0)
    				os << ", ";
    			os << node.AttachedElement(j).Index();
    		}
    		os << endl;
    	}
    }
  };
    
  struct PointInBody
  {
    PointInBody(QTextStream &os, geo::CMesh3D& mesh, double x, double y, double z)
    {
    	geo::CMesh3D::TBodyVec vcRet = mesh.PointInMesh(geo::CPoint(x, y, z));
    	os << "Point (" << x << "," << y << "," << z << ") ";
    	if(vcRet.size() == 0)
    		os << "not in mesh" << endl;
    	else
    	{
    		os << "in body ";
    		for(size_t i = 0; i < vcRet.size(); i++)
    		{
    			if(i != 0)
    				os << ", ";
    			os << vcRet[i]->Index();
    		}
    		os << endl;
    	}
    }
    
    PointInBody(QTextStream &os, geo::CBodyGroup& group, double x, double y, double z)
    {
    	geo::CBodyGroup::TBodyVec vcRet = group.PointInVolume(geo::CPoint(x, y, z));
    	os << "Point (" << x << "," << y << "," << z << ") ";
    	if(vcRet.size() == 0)
    		os << "not in body group" << endl;
    	else
    	{
    		os << "in body ";
    		for(size_t i = 0; i < vcRet.size(); i++)
    		{
    			if(i != 0)
    				os << ", ";
    			os << vcRet[i]->Index();
    		}
    		os << endl;
    	}
    }
  };
    
  struct PointInVolume
  {
    PointInVolume(QTextStream &os, geo::CBodyGroup& group, double x, double y, double z, bool bIncludeEdge)
    {
    	os << "Point (" << x << "," << y << "," << z << ") ";
    	if(group.Contains(geo::CPoint(x, y, z), bIncludeEdge))
    		os << "in mesh" << endl;
    	else
    		os << "not in mesh" << endl;
    
    }
  };
    
  struct LineIntersection
  {
    LineIntersection(QTextStream &os, geo::CBodyGroup& group, const geo::ILine& line)
    {
    	os << "Line (" << line.First().X() << "," << line.First().Y() << "," << line.First().Z() << ") ";
    	os << "Line (" << line.Second().X() << "," << line.Second().Y() << "," << line.Second().Z() << ") intersects\n";
    	std::set<geo::CPoint> stPoint;
    	group.IntersectionWithEdgeFaces(line, stPoint);
    	int i = 0;
    	for(std::set<geo::CPoint>::iterator it = stPoint.begin(); it != stPoint.end(); it++)
    	{
    		os << "P" << i << " (" << it->X() << "," << it->Y() << "," << it->Z() << ") ";
    		i++;
    	}
    	os << "\n\n";
    }
  };

  struct PointInMesh
  {
    PointInMesh(QTextStream &os, geo::CMesh3D& mesh, double x, double y, double z, bool bIncludeEdge)
    {
    	os << "Point (" << x << "," << y << "," << z << ") ";
    	if(mesh.Contains(geo::CPoint(x, y, z), bIncludeEdge))
    		os << "in mesh" << endl;
    	else
    		os << "not in mesh" << endl;
    
    }
  };

  struct PointInTetra
  {
    PointInTetra(QTextStream &os, const geo::IPoint& point,
		    		  const geo::IPoint& p0,
				      const geo::IPoint& p1,
				      const geo::IPoint& p2,
				      const geo::IPoint& p3)
    {
	    geo::CMesh mesh;
	    geo::CTetrahedron *pTetra = new geo::CTetrahedron(mesh, p0, p1, p2, p3);
	    os << "Point(" << point.X() << "," << point.Y() << "," << point.Z() << ") is ";
	    if(!pTetra->Contains(point, true))
		    os << "not ";
	    os << "in tetra : \n";
	    for(int i = 0; i < 4; i++)
		    os << "P"<< i << " (" << pTetra->Point(i).X() << "," << pTetra->Point(i).Y() << "," << pTetra->Point(i).Z() << ")\n";
	    os << "\n";
    }
  };

    
  QTextStream &os = testData.OS();

	os << "     ********************** TETRA MESH TEST *********************" << endl;
	os << "     * Mesh test file                                           *" << endl ;
	os << "     * Testing the functionnality of the tetra mesh             *" << endl;
	os << "     ************************************************************" << endl;
	os << endl;
	os << "Testing the contains function of the tetrahedron.\n";
  os.flush();
	PointInTetra(os, geo::CPoint(69.208, -10.1999, 115.471),
				 geo::CPoint(100,0,100),
				 geo::CPoint(36.5693,-21.0115,131.87),
				 geo::CPoint(25,-100,-100),
				 geo::CPoint(87.5,-100,-100));
	PointInTetra(os, geo::CPoint(92.4334, -60.5329, -21.0657),
				 geo::CPoint(100,0,100),
				 geo::CPoint(36.5693,-21.0115,131.87),
				 geo::CPoint(25,-100,-100),
				 geo::CPoint(87.5,-100,-100));
	PointInTetra(os, geo::CPoint(92.4334, -60.5329, -21.0657),
				 geo::CPoint(100,0,100),
				 geo::CPoint(36.5693,-21.0115,131.87),
				 geo::CPoint(25,-100,-100),
				 geo::CPoint(87.5,-100,-100));
	PointInTetra(os, geo::CPoint(69.208, -10.1999, 115.471),
				 geo::CPoint(100,0,100),
				 geo::CPoint(36.5693,-21.0115,131.87),
				 geo::CPoint(25,-100,-100),
				 geo::CPoint(87.5,-100,-100));
	PointInTetra(os, geo::CPoint(48.6254, 34.4056, 332.933),
				 geo::CPoint(100,100,100),
				 geo::CPoint(22.8006,37.0027,239.576),
				 geo::CPoint(45.8227,30.8272,345.64),
				 geo::CPoint(26.9347,92.215,364.815));
	PointInTetra(os, geo::CPoint(36.9096, 59.7957, 354.689),
				 geo::CPoint(100,100,100),
				 geo::CPoint(22.8006,37.0027,239.576),
				 geo::CPoint(45.8227,30.8272,345.64),
				 geo::CPoint(26.9347,92.215,364.815));
	PointInTetra(os, geo::CPoint(48.6254, 34.4056, 332.933),
				 geo::CPoint(100,100,100),
				 geo::CPoint(22.8006,37.0027,239.576),
				 geo::CPoint(45.8227,30.8272,345.64),
				 geo::CPoint(26.9347,92.215,364.815));
	PointInTetra(os, geo::CPoint(69.208, -10.1999, 115.471),
				 geo::CPoint(0,0,100),
				 geo::CPoint(36.5693,-21.0115,131.87),
				 geo::CPoint(25,-100,-100),
				 geo::CPoint(100,0,100));
	PointInTetra(os, geo::CPoint(49.383, 32.7639, 350.916),
				 geo::CPoint(45.8227,30.8272,345.64),
				 geo::CPoint(97.1655,59.679,358.511),
				 geo::CPoint(150,87.5,500),
				 geo::CPoint(150,25,500));
	PointInTetra(os, geo::CPoint(49.383, 32.7639, 350.916),
				 geo::CPoint(45.8227,30.8272,345.64),
				 geo::CPoint(97.1655,59.679,358.511),
				 geo::CPoint(150,87.5,500),
				 geo::CPoint(150,25,500));
	geo::CMesh3D mesh;
	BuildTetraMesh(os, mesh);
	os << endl << "Point in mesh testing including edge" << endl << endl;
	PointInMesh(os, mesh, 0, 0, 0, true);
	PointInMesh(os, mesh, 50, 50, 50, true);
	PointInMesh(os, mesh, 150, 0, 200, true);
	PointInMesh(os, mesh, 0.1, 0.1, 0.1, true);
	PointInMesh(os, mesh, 50, 25, 10, true); 
	PointInMesh(os, mesh, 100, 100, 100, true);
//	os << endl << "Point in mesh testing excluding edge" << endl << endl;
//	PointInMesh(os, mesh, 0, 0, 0, false);
//	PointInMesh(os, mesh, 50, 50, 50, false);
//	PointInMesh(os, mesh, 150, 0, 200, false);
//	PointInMesh(os, mesh, 0.1, 0.1, 0.1, false);
//	PointInMesh(os, mesh, 50, 25, 10, false); 
//	PointInMesh(os, mesh, 100, 100, 100, false);
	os << endl << "Point in body testing" << endl << endl;
	PointInBody(os, mesh, 0, 0, 0);
	PointInBody(os, mesh, 50, 50, 50);
	PointInBody(os, mesh, 150, 0, 200);
	PointInBody(os, mesh, 0.1, 0.1, 0.1);
	PointInBody(os, mesh, 50, 25, 10); 
	PointInBody(os, mesh, 100, 100, 100);
	os << endl << "Testing bodygroup \n\n";
	geo::CBodyGroup* pBodyGroup  = new geo::CBodyGroup(mesh);
	for(int i = 0; i < mesh.ElementSize(); i++)
	{
		const geo::IBody* pBody = dynamic_cast<const geo::IBody*>(&mesh.Element(i));
		if(pBody)
		{
			pBodyGroup->AddBody(*pBody);
			os << "Body " << pBody->Index() << " added to body group.\n";
		}
	}
	os << endl << "Point in body group testing including edge" << endl << endl;
	PointInVolume(os, *pBodyGroup, 0, 0, 0, true);
	PointInVolume(os, *pBodyGroup, 50, 50, 50, true);
	PointInVolume(os, *pBodyGroup, 150, 0, 200, true);
	PointInVolume(os, *pBodyGroup, 0.1, 0.1, 0.1, true);
	PointInVolume(os, *pBodyGroup, 50, 25, 10, true); 
	PointInVolume(os, *pBodyGroup, 100, 100, 100, true);
	os << endl << "Point in body group testing excluding edge" << endl << endl;
	PointInVolume(os, *pBodyGroup, 0, 0, 0, false);
	PointInVolume(os, *pBodyGroup, 50, 50, 50, false);
	PointInVolume(os, *pBodyGroup, 150, 0, 200, false);
	PointInVolume(os, *pBodyGroup, 0.1, 0.1, 0.1, false);
	PointInVolume(os, *pBodyGroup, 50, 25, 10, false); 
	PointInVolume(os, *pBodyGroup, 100, 100, 100, false);
	os << endl << "Point in bodygroup testing" << endl << endl;
	PointInBody(os, *pBodyGroup, 0, 0, 0);
	PointInBody(os, *pBodyGroup, 50, 50, 50);
	PointInBody(os, *pBodyGroup, 150, 0, 200);
	PointInBody(os, *pBodyGroup, 0.1, 0.1, 0.1);
	PointInBody(os, *pBodyGroup, 50, 25, 10); 
	PointInBody(os, *pBodyGroup, 100, 100, 100);
	os << endl << "Line intersection with bodygroup" << endl << endl;
	LineIntersection(os, *pBodyGroup, geo::CLine(geo::CPoint(25, -50, 25), geo::CPoint(25, 125, 25)));
	LineIntersection(os, *pBodyGroup, geo::CLine(geo::CPoint(50, -50, 75), geo::CPoint(50, 125, 75)));
	LineIntersection(os, *pBodyGroup, geo::CLine(geo::CPoint(5, 5, -25), geo::CPoint(5, 5, 125)));
	LineIntersection(os, *pBodyGroup, geo::CLine(geo::CPoint(-50, 0, 25), geo::CPoint(150, 100, 25)));		

}

static void test_triangle(const geo::CTriangle& triangle, GeometryTestData& testData)
{
  QTextStream &os = testData.OS();

  os << "Triangle area: " << triangle.Area() << endl;
  os << endl;

  os << "Triangle line lengths:" << endl;

  int i;
  for(i = 0; i < triangle.NrOfLines(); i++)
  {
    os << "Line " << i + 1 << ": " << triangle.Line(i).Length() <<
          endl;
  }

  os << endl;
  os << "Isoparametric coordinates of nodes:" << endl;
  QTextStream::RealNumberNotation notation = os.realNumberNotation();
  os.setRealNumberNotation( QTextStream::FixedNotation );

  geo::IElement::TDoubleVec vcIso0 = triangle.WorldToIso(triangle.Point(0));
  geo::IElement::TDoubleVec vcIso1 = triangle.WorldToIso(triangle.Point(1));
  geo::IElement::TDoubleVec vcIso2 = triangle.WorldToIso(triangle.Point(2));

  assert(vcIso0.size() == 2);
  assert(vcIso1.size() == 2);
  assert(vcIso2.size() == 2);

  os << "Point 1: (" << vcIso0[0] << "," << vcIso0[1] << ")" << endl;
  os << "Point 2: (" << vcIso1[0] << "," << vcIso1[1] << ")" << endl;
  os << "Point 3: (" << vcIso2[0] << "," << vcIso2[1] << ")" << endl;

  os << endl;
  geo::CPoint mid = triangle.MidPoint();
  os << "Isoparametric coordinates of midpoint (" << mid.X() << "," <<
    mid.Y() << "," << mid.Z() << "):" << endl;

  geo::IElement::TDoubleVec vcIsoMid = triangle.WorldToIso(mid);
  assert(vcIsoMid.size() == 2);
  os << "xi : " << vcIsoMid[0] << endl;
  os << "eta: " << vcIsoMid[1] << endl;

  os.setRealNumberNotation(notation);
}

void init_triangle1(GeometryTestData &testData)
{
  QTextStream &os = testData.OS();

  os.setRealNumberPrecision(6);
  os << "Triangle test 1" << endl << endl;
  geo::CPoint p1(0, 0, 0);
  geo::CPoint p2(10, 0, 0);
  geo::CPoint p3(0, 10, 0);

  geo::CTriangle triangle(p1, p2, p3);
  test_triangle(triangle, testData);
}

void init_triangle2(GeometryTestData &testData)
{
  QTextStream &os = testData.OS();

  os.setRealNumberPrecision(6);
  os << "Triangle test 2" << endl << endl;
  geo::CPoint p1(0, 0, 0);
  geo::CPoint p2(0, 10, 0);
  geo::CPoint p3(0, 0, 10);

  geo::CTriangle triangle(p1, p2, p3);
  test_triangle(triangle, testData);
}

void init_triangle3(GeometryTestData &testData)
{
  QTextStream &os = testData.OS();

  os.setRealNumberPrecision(6);
  os << "Triangle test 3" << endl << endl;
  geo::CPoint p1(0, 0, 0);
  geo::CPoint p2(10, 0, 0);
  geo::CPoint p3(0, 0, 10);

  geo::CTriangle triangle(p1, p2, p3);
  test_triangle(triangle, testData);
}

void init_triangle4(GeometryTestData &testData)
{
  QTextStream &os = testData.OS();

  os.setRealNumberPrecision(6);
  os << "Triangle test 4" << endl << endl;
  geo::CPoint p1(0, 0, 0);
  geo::CPoint p2(10, 10, 0);
  geo::CPoint p3(0, 0, 10);

  geo::CTriangle triangle(p1, p2, p3);
  test_triangle(triangle, testData);
}

void init_vector(GeometryTestData &testData)
{
 	QTextStream &os = testData.OS();

  os << "     ******************* VECTOR TEST ****************************"
     << endl;
  os << "     * Geometry test file                                       *"
     << endl;
  os << "     * testing of the vector functionality of the geometry lib  *"
     << endl;
  os << "     ************************************************************"
     << endl;

  const geo::CVector v1(1,0,0);
  const geo::CVector v2(0,1,0);

  testData.PrintObject(v1,"v1 := ");
  testData.PrintObject(v2,"v2 := ");
  

  geo::CVector v3;
  testData.PrintObject(v1.CrossProduct(v2),"v1.CrossProduct(v2) = ");
  testData.PrintObject(v2.CrossProduct(v1),"v2.CrossProduct(v1) = ");


  os << "v2.DotProduct(v1) = " << v2.DotProduct(v1) << endl;
  os << "v1.AngleDeg(v2) = "   << v1.AngleDeg(v2)   << endl;
  os << "v2.AngleDeg(v1) = "   << v2.AngleDeg(v1)   << endl;
  os << "v1.Length() = "       << v1.Length()       << endl;
  os << "v2.Length() = "       << v2.Length()       << endl;
  os << "(v1 == v2) = "        << (v1 == v2)        << endl;
  testData.PrintObject(v1.UnitVector(),"v1.UnitVector() = ");
  testData.PrintObject(v1 + v2,"v1 + v2 = ");
  testData.PrintObject(v1 - v2,"v1 - v2 = ");
  testData.PrintObject(v1*3.0,"v1*3.0 = ");
  testData.PrintObject(v1/2.0,"v1/2.0 = ");

  v3 = v1;
  testData.PrintObject(v3,"v3 := ");
  os << "v3.Flip()" << endl;
  v3.Flip();
  testData.PrintObject(v3,"v3 = ");

  v3 = v1;
  os << "v3 := v1" << endl;
  testData.PrintObject(v3,"v3 := ");
  os << "v3.Rotate(v1.CrossProduct(v2),v1.AngleDeg(v2))" << endl;
  v3.Rotate(v1.CrossProduct(v2),v1.AngleDeg(v2));
  testData.PrintObject(v3,"v3 = ");

  v3 = v2;
  os << "v3 := v2" << endl;
  testData.PrintObject(v3,"v3 := ");
  os << "v3.Rotate(v2.CrossProduct(v1),v2.AngleDeg(v1))" << endl;
  v3.Rotate(v2.CrossProduct(v1),v2.AngleDeg(v1));
  testData.PrintObject(v3,"v3 = ");


  os << "mat = v1.GetRotationMatrix(90)" << endl;
  geo::CMatrix mat;
  v1.GetRotationMatrix(90,mat);
  testData.PrintMatrix(mat,"mat = ");


  v3 = v2;
  os << "v3 := v2" << endl;
  testData.PrintObject(v3,"v3 := ");
  os << "v3.Transform(mat)" << endl;
  v3.Transform(mat);
  testData.PrintObject(v3,"v3 = ");


  geo::CVector v4(2,8,9);
  testData.PrintObject(v4,"v4 := ");
  os << "v3=v4*2.0" << endl;
  v3=v4*2.0;
  testData.PrintObject(v3,"v3 = ");
  os << "v3.IsParallel(v4) = " << v3.IsParallel(v4) << endl;
  os << "v3.Flip()" << endl;
  testData.PrintObject(v3,"v3 = ");
  os << "v3.IsParallel(v4) = " << v3.IsParallel(v4) << endl;
  

  testData.NewLine(2);
  os << "     ********************** END VECTOR TEST ********************" << endl;
}

#ifdef _MSC_VER
// not sure what they want to test with the division by 0, but apparently Visual Studio is now clever enough to evaluate that to 0
// so let's just disable the warning here, as apparently it's intentional
#pragma warning(push)
#pragma warning(disable: 4723)
#endif
void init_value(GeometryTestData &testData)
{
  QTextStream &os = testData.OS();

  testData.NewLine(2);
  os << "     ************************* VALUE TEST ***********************" << endl;
  os << "     * Geometry test file                                       *" << endl ;
  os << "     * testing of the value functionality of the geometry lib   *" << endl;
  os << "     ************************************************************" << endl;
  testData.NewLine(2);

  double a= 12345; 
  double b= 12; // avoid compile time divide by zero error
  double c= 3;
  geo::CValue value(a/(b-4*c));
  if ( value.Valid() )
    os << "12345/0.0 is valid" << endl;
  else
    os << "12345/0.0 is invalid" << endl;

  geo::CValue value2(sqrt(-1.0));
  if ( value2.Valid() )
    os << "sqrt(-1.0) is valid" << endl;
  else
    os << "sqrt(-1.0) is invalid" << endl;

  geo::CValue value3(0);
  if ( value3.Valid() )
    os << "0 is valid" << endl;
  else
    os << "0 is invalid" << endl;

  geo::CValue value4(-1.0);
  if ( value4.Valid() )
    os << "-1.0 is valid" << endl;
  else
    os << "-1.0 is invalid" << endl;

  testData.NewLine(2);
  os << "     ********************** END VALUE TEST ********************" << endl;
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif




const GeometryTestInfo geo_tests[] = {
  { "value", { { 0, init_value }, { 0, 0 } } },
  { "bucket", { { 0, init_bucket }, { 0, 0 } } },
  { "face", { { 0, init_face }, { 0, 0 } } },
  { "hexahedron", { { 0, init_hexahedron_0 },
                    { 0, init_hexahedron_1 },
                    { 0, init_hexahedron_2 },
                    { 0, 0 } } },
  { "line", { { 0, init_line }, { 0, 0 } } },
  { "mapping", { { 0, init_mapping }, { 0, 0 } } },
  { "matrix", { { 0, init_matrix }, { 0, 0 } } },
  { "plane", { { 0, init_plane }, { 0, 0 } } },
  { "point", { { 0, init_point }, { 0, 0 } } },
  { "polyline", { { 0, init_polyline }, { 0, 0 } } },
  { "setmapping", { { 0, init_setmapping }, { 0, 0 } } },
  { "sphere", { { 0, init_sphere }, { 0, 0 } } },
  { "tetrahedron", { { 0, init_tetrahedron }, { 0, 0 } } },
  { "tetra_mesh", { { 0, init_tetra_mesh }, { 0, 0 } } },
  { "triangle", { { 0, init_triangle1 },
				  { 0, init_triangle2 },
				  { 0, init_triangle3 },
				  { 0, init_triangle4 },
				  { 0, 0 } } },
  { "vector", { { 0, init_vector }, { 0, 0 } } }
};
INSTANTIATE_TEST_CASE_P(MainRun, Fw51GeometryTest, ::testing::ValuesIn(geo_tests));


INSTANTIATE_TEST_CASE_P(MainRun, Fw51GeometryRingTest, ::testing::Values(
    "ring_model_i.dat",
    "ring_model_ii.dat",
    "ring_model_iii.dat",
    "ring_model_iv.dat",
    "ring_model_v.dat",
    "ring_model_vi.dat",
    "ring_model_vii.dat",
    "ring_model_viii.dat",
    "ring_model_ix.dat",
    "ring_model_x.dat",
    "ring_model_xi.dat"
  ));


INSTANTIATE_TEST_CASE_P(MainRun, Fw51GeometryConvexHull2DTest, ::testing::Values(
    "convexhull_2d_I.dat 3",
    "convexhull_2d_II.dat 2",
    "convexhull_2d_III.dat 0"
  ));


INSTANTIATE_TEST_CASE_P(MainRun, Fw51GeometryConvexHull3DTest, ::testing::Values(
    "convexhull_3d_large.dat",
    "convexhull_3d_medium.dat",
    "convexhull_3d_tolerance.dat"
  ));

  INSTANTIATE_TEST_CASE_P(MainRun, Fw51GeometryCylindricHullTest, ::testing::Values(
    "cylindric_hull_empty.dat 1",
	"cylindric_hull_1point.dat 1",
	"cylindric_hull_2point.dat 1",
	"cylindric_hull_multipoint.dat 1",
	"cylindric_hull_equalpoint.dat 1",
	"cylindric_hull_radius0.dat 0",
	"cylindric_hull_radius2.dat 2"
  ));


INSTANTIATE_TEST_CASE_P(MainRun, Fw51GeometryPolygonTest, ::testing::Values(
    "polygon.dat"
  ));



}
