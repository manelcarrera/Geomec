 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "TestSoilModelFactory.h"

#include "TestMaterial.h"
#include "TestFormation.h"
#include "TestHorizon.h"
#include "TestLib.h"


namespace test_lib
{

//static CSoilModelFactory soilModelFactory;

CSoilModelFactory::CSoilModelFactory() : m_os(0)
{
}

IModelObject* CSoilModelFactory::createObject( const char* className ) const
{
  QString s( className );
  IModelObject* item = 0;

  if (!m_os) m_os = TestLib::CurrentOutput();

  if ( s == "test_lib::CMaterial" ) {
  item = new CMaterial(*m_os);
  } else if ( s == "test_lib::CMaterialContainer" ) {
  item = new CMaterialContainer(*m_os);
  } else if ( s == "test_lib::CHorizon" ) {
  item = new CHorizon(*m_os);
  } else if ( s == "test_lib::CHorizonContainer" ) {
  item = new CHorizonContainer(*m_os);
  } else if ( s == "test_lib::CFormation" ) {
  item = new CFormation(*m_os);
  } else if ( s == "test_lib::CFormationContainer" ) {
  item = new CFormationContainer(*m_os);
  }
  return item;
}


}