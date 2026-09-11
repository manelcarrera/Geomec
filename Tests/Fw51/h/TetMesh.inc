#include "dimple.h"
#include <fstream>
#include <qtextstream.h>
#include <QApplication>
#include <qfileinfo.h>
#include <stdio.h>
#include "FvGocadFile.h"
#include "TetMeshBase.h"
#include "IProgressBase.h"
#include "SurfaceDesc.h"
#include "ElementGroup.h"
#include "TetSurface.h"
#include "IBody.h"
#include <map>



namespace
{


double volume_size( const geo::IElementSet& element_set )
{
  double dRet = 0;
  for( int  i  = 0; i < element_set.ElementSize(); i++) {
    if( dynamic_cast<const geo::IBody*>( &element_set.Element(i) ) )
      dRet += element_set.Element(i).Size();
  }
  return dRet;
}

double surface_size( const geo::IElementSet& element_set )
{
  double dRet = 0;
  for( int  i  = 0; i < element_set.ElementSize(); i++) {
    dRet += element_set.Element(i).Size();
  }
  return dRet;
}




class Fw51TetMeshTest : public test_fw51::Fw51InputBaseTest
{
protected:

  double *pTolerance;

  Fw51TetMeshTest() : pTolerance(0)
  {
    m_ModelsPath.append("TetMesh/");
    m_OutputPath.append("TetMesh/");

    std::string param = GetParam();

    std::size_t pos = param.find(' ');
    if (pos != std::string::npos)
    {
      param = param.substr(pos + 1);

      pTolerance = new double(atof(param.c_str()));

      std::cout << "Using tolerance: " << *pTolerance << std::endl;
    }

  }

  ~Fw51TetMeshTest()
  {
    if (pTolerance)
      delete pTolerance;
  }
};



TEST_P(Fw51TetMeshTest, ReadFile)
{
  QFile of(m_Output.c_str());

  EXPECT_TRUE(of.open(QIODevice::WriteOnly | QIODevice::Truncate)) << "Failed to open file " << m_Output << std::endl;
  QTextStream os(&of);

  geo::CMesh mesh;
  CGoCadFile file(mesh);
  IProgressBase progress;

  //EXPECT_TRUE(file.Open(m_Input.c_str(), progress)) << "Failed to open file " << m_Input << std::endl;
  if (!file.Open(m_Input.c_str(), progress))
  {
    os << "Failed: " << file.ErrorMessage() << endl;
    return;
  }


/*      if( !test_env( "DIAPATH"  ) ||
        !test_env( "DIASHARE" ) ||
        !test_env( "DIALIB"   ) ||
        !test_env( "DIASLIB"  ) )
      return;*/

  {
    geo::CTetMeshBase& mesher = *geo::CTetMeshBase::GetTetMesher();
    if( pTolerance ) mesher.Tolerance( *pTolerance );
    typedef std::vector<const geo::ISurface*> TSurfaceVec;
    typedef std::map<QString, TSurfaceVec> TSurfaceMap;
    TSurfaceMap mpSurface;
    // Put everything in the mesher

    for(size_t i = 0; i < file.SurfaceSize(); i++) {
      // Put surfaces in mesher
      QString sSurfaceName( file.Surface(i).Header().Name() );
      if( sSurfaceName.left(2).toUpper() == "F_" ) {
        // Insert front surface
        sSurfaceName = sSurfaceName.mid(2);
        TSurfaceVec vcSurface(2);
        vcSurface[0] = 0;
        vcSurface[1] = 0;        
        TSurfaceMap::iterator it = mpSurface.insert(TSurfaceMap::value_type(sSurfaceName, vcSurface)).first;
        if(it->second[0]) {
          // Failure
          os << "Duplicate front surface " << sSurfaceName << "\n";
          return;
        }
        it->second[0] = &file.Surface(i);
      } else if( sSurfaceName.left(2).toUpper() == "B_" ) {
        // Insert back and front surface
        sSurfaceName = sSurfaceName.mid(2);
        TSurfaceVec vcSurface(2);
        vcSurface[0] = 0;
        vcSurface[1] = 0;
        TSurfaceMap::iterator it = mpSurface.insert(TSurfaceMap::value_type(sSurfaceName, vcSurface)).first;
        if(it->second[1]) {
          // Failure
          os << "Duplicate back surface " << sSurfaceName << "\n";
          return;
        }
        it->second[1] = &file.Surface(i);
      } else {
        // Just a normal surface
        if( mpSurface.find( sSurfaceName ) != mpSurface.end() ) {
          os << "Duplicate surface " << sSurfaceName << "\n";
          return;
        }
        TSurfaceVec vcSurface(1);
        vcSurface[0] = &file.Surface(i);
        mpSurface.insert(TSurfaceMap::value_type(sSurfaceName, vcSurface)).first;
      }
    }

    // Inserting surfaces in mesher
    int i = 0;
    for(TSurfaceMap::iterator it = mpSurface.begin(); it != mpSurface.end(); it++) {
      i++;
      // Do we have a fault?
      if(it->second.size() == 2) {
        // Fault
        if(!it->second[0]) {
          os << "Front surface of " << it->first << " is missing.\n";
          return ;
        }
        if(!it->second[1]) {
          os << "Back surface of " << it->first << " is missing.\n";
          return ;
        }
        os << "Slipping surface " << i << " : " << it->first << endl;
        mesher.AddSurface(*it->second[0], it->first.toLatin1().data(), true);
      } else {
        assert(it->second.size() == 1);
        os << "Surface " << i << " : " << it->first << endl;
        mesher.AddSurface(*it->second[0], it->first.toLatin1().data(), false);
      }
    }

    geo::CTetMeshProgress progressCreateMesh;
    typedef geo::CTetMeshBase::TRet TRet;
    TRet ret =  mesher.CreateMesh( getenv( "DIAPATH"  ),
                                   getenv( "DIASHARE" ),
                                   getenv( "DIALIB"   ),
                                   getenv( "DIASLIB"  ),
                                   "fgv4gm",
                                   progressCreateMesh);
    if(ret.second) {
       // No failure
       os << "\nMesher generated:\n";
       os << mesher.NrOfSurfaces() << " Input surfaces.\n";
       os << mesher.NrOfVolumes() << " Bodies generated.\n";
       os << "Total square " << volume_size( mesher ) << "\n";

       if( ret.first.size() > 0 ) {
         os << "\nMesher generated following warning(s):\n";
         for(size_t j = 0; j < ret.first.size(); j++) {
           os << ret.first[j].c_str() << "\n";
         }
       }

       for(i = 0; i < mesher.NrOfVolumes(); i++) {
         os << "\nBody " << i + 1 << " volume " << volume_size(mesher.Volume(i)) << "\n";
         for(int j = 0; j < mesher.Volume(i).SideSurfaceSize(); j++) {
           os << "Sidesurface " << j + 1 << " area " << surface_size(mesher.Volume(i).SideSurface(j))<< "\n";
         }
       }
       
       for(i = 0; i < mesher.NrOfSurfaces(); i++) {
         const geo::CSurfaceDesc& surface = mesher.SurfaceDesc(i);
         os << "\nInput surface " << surface.Name().c_str() << "\n";
         if( surface.Slip() ) {
           // Contact surface front and back
           os << "Contact surface front has " << surface.interfaceElements().ElementSize() << " contact elements.\n";
           os << "Contact surface back  has " << surface.interfaceElements().ElementSize() << " contact elements.\n";
         } else {
           for(int j = 0; j < surface.TetSurfaceSize(); j++) {
             os << "Tetsurface " << j + 1 << " square " << surface_size(surface.TetSurface(j)) << "\n";
           }
         }
       }       
    } else {
      // Failure
      os << "\nMesher failed:\n";
      for(size_t j = 0; j < ret.first.size(); j++) {
        os << ret.first[j].c_str() << "\n";
      }
    }


  }

  of.close();


  if (pTolerance)
  {
    EXPECT_TRUE(test_lib::FileCompare::IdenticalTokens(m_Expected, m_Output, *pTolerance));
  }
  else
  {
    EXPECT_TRUE(test_lib::FileCompare::IdenticalTokens(m_Expected, m_Output));
  }
}



INSTANTIATE_TEST_CASE_P(FirstRun, Fw51TetMeshTest, ::testing::Values(
    "Bodies2Fault.mx",
    "Bodies2FF.mx",
    "Bodies2NF.mx",
    "Bodies2NN.mx",
    "Bodies2Simple.mx",
    "Bodies4Simple.mx",
    "BodiesNested1.mx",
    "BodiesNested2.mx",
    "BodiesNested3.mx",
    "BodiesNested4.mx",
    "BodiesNested5.mx",
    "BodiesNested6.mx",
    "BodiesNested7.mx",
    "BodiesNested8.mx",
    "BodiesNested9.mx",
    "BodiesNested10.mx",
    "DegenerateTriangle1.mx",
    "DegenerateTriangle2.mx",
    "DisconnectedFault.mx",
    "DuplicateSurface.mx",
    "EmptyModel.mx",
    "Fault2Squares.mx",
    "FaultEmpty.mx",
    "FaultHorizon1.mx",
    "FaultHorizon3.mx",
    "FaultHorizon4.mx",
    "FaultHorizon6.mx",
    "FaultHorizon7.mx",
    "FaultHorizon8.mx",
    "FaultHorizon9.mx",
    "FaultHorizon10.mx",
    "FaultHorizon11.mx",
    "FaultHorizon12.mx",
    "FaultHorizon13.mx",
    "FaultHorizon14.mx",
    "FaultHorizon15.mx",
    "FaultHorizon16.mx",
    "FaultHorizon17.mx",
    "FaultHorizon18.mx",
    "FaultHorizon19.mx",
    "FaultHorizon20.mx",
    "FaultHorizon21.mx",
    "FaultHorizon22.mx",
    "FaultHorizon23.mx",
    "FaultHorizon24.mx",
    "FaultHorizon25.mx"
  ));
  
INSTANTIATE_TEST_CASE_P(SecondRun, Fw51TetMeshTest, ::testing::Values(
    "FaultLens1.mx",
    "FaultLens2.mx",
    "FaultLens3.mx",
    "FaultLens4.mx",
    "FaultLens5.mx",
    "FaultLens6.mx",
    "FaultLens7.mx",
    "FaultLens8.mx",
    "FaultLens9.mx",
    "FaultOneTriangle.mx",
    "FaultOuter01.mx",
    "FaultOuter02.mx",
    "FaultOuter04.mx",
    "FaultOuter05.mx",
    "FaultOuter06.mx",
    "FaultOuter07.mx 1e-5",
    "FaultOuter08.mx",
    "Faults30.mx",
    "FaultsCross.mx",
    "FaultsTouch1.mx",
    "FaultsTouch10.mx",
    "FaultsTouch11.mx",
    "FaultsTouch12.mx",
    "FaultsTouch2.mx",
    "FaultsTouch3.mx",
    "FaultsTouch4.mx",
    "FaultsTouch5.mx",
    "FaultsTouch6.mx",
    "FaultsTouch7.mx",
    "FaultsTouch8.mx",
    "FaultsTouch9.mx",
    "FourBodies1.mx",
    "FourBodies4.mx",
    "GocadError1.mx",
    "GocadError2.mx",
    "GocadError3.mx",
    "IdenticalTriangle.mx",
    "IntersectingTriangles1.mx",
    "IntersectingTriangles2.mx",
    "IntersectingTriangles3.mx",
    "IntersectingTriangles4.mx",
    "IsolatedEdge1.mx",
    "IsolatedEdge2.mx",
    "IsolatedEdge3.mx",
    "IsolatedEdge4.mx",
    "IsolatedEdge5.mx"
  ));
  
INSTANTIATE_TEST_CASE_P(ThirdRun, Fw51TetMeshTest, ::testing::Values(
    "large.mx",
    "NonUniqueFault1.mx",
    "OnlyFront.mx",
    "OrmenLange.mx",
    "OverConnected.mx",
    "SelfIntersectBound1.mx",
    "SelfIntersectBound2.mx",
    "Shearwater1.mx",
    "Shearwater2.mx",
    //"Shearwater3.mx", // error isolated edge
    "SimpleModel.mx",
    "simple_box.mx",
    "simple_Fault.mx",
    "SingleFault.mx",
    "SingleFault2.mx",
    "SingleFault3.mx",
    "Yibal1.mx",
    "Yibal2.mx"
  ));


}
