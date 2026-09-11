#include "MeshRefinementModel.h"

#include "RBox.h"

#include "Printer.h"

#include "ArchiveInterface.h"

#include <tuple>

typedef std::tuple< QString, bool, RefinementBox* > TBoxTuple;

namespace
{
  Printer* printer = Printer::instance(Printer::Mr);
}


CMeshRefinementModel::CMeshRefinementModel()
{
}

/////////////////////////////////////////////// util ////////////////////////////////////////////////////////////

TBoxV CMeshRefinementModel::data_v()
{
  TBoxMap& d = m_data_m;
  TBoxV data_v;

  for( auto& name : d.keys() )
  {
    const RefinementBox& box = d.value( name );

    if( d[ name ].IsVisible() )
      data_v.push_back( box );
  }

  return data_v; 
}

/////////////////////////////////////////////// logs ////////////////////////////////////////////////////////////

void CMeshRefinementModel::print()
{
  TBoxMap& d = m_data_m;

  printer->info("boxes: %d", d.size() );
  for( auto& name : d.keys() )
  {
    printer->info("box name : %s", name.toStdString().c_str() );
    const RefinementBox& box = d.value( name );
    ( (RefinementBox&)box ).print();
  }
}

/////////////////////////////////////////////// to_stream ////////////////////////////////////////////////////////////

void to_stream_( CArchiveInterface& stream, const RefinementBox& box )
{
  stream << box.Name();
  stream << (box.IsVisible() ? 1:0);

  const T3da& c = box.Center();
  const T3da& s = box.Size();
  const T3da& r = box.Rotation();

  RefinementBox::GradientType_ g = box.RefinementBox::GradientType();

  stream << c[ 0 ] << c[ 1 ] << c[ 2 ];
  stream << s[ 0 ] << s[ 1 ] << s[ 2 ];
  stream << r[ 0 ] << r[ 1 ] << r[ 2 ];

  stream << g;

  stream << box.Profile().size(); //number of gradient steps
  for( auto gradient_step : box.Profile() )
  {
    stream << gradient_step.Distance();
    stream << gradient_step.Size();
  }
}

void CMeshRefinementModel::to_stream( CArchiveInterface& stream )
{
  TBoxMap& d = m_data_m;

  printer->info("to_stream ->");
  print();

  stream << d.size(); //number of boxes
  for( auto name : d.keys() )
  {
    const RefinementBox& box = d.value( name );
    to_stream_( stream, box );
  }
}

/////////////////////////////////////////////// from_stream ////////////////////////////////////////////////////////////

RefinementBox box_from_stream( CArchiveInterface& stream )
{
  QString name;
  int visible;

  RefinementBox box = RefinementBox();

  stream >> name;
  box.SetName( name );

  stream >> visible;
  box.SetVisible(  visible );

  T3da a;

  stream >> a[0] >> a[1] >> a[2];
  box.SetCenter( a );

  stream >> a[0] >> a[1] >> a[2];
  box.SetSize( a );

  stream >> a[0] >> a[1] >> a[2];
  box.SetRotation( a );

  unsigned int g;
  stream >> g;

  box.SetGradientType( (RefinementBox::GradientType_)g );

  int num_gradient_steps;
  stream >> num_gradient_steps;

  std::vector<RelativeSize> gradient_steps_v;
  int distance;
  double size;
  for( int i=0; i < num_gradient_steps; i++ )
  {
    stream >> distance;
    stream >> size;
    gradient_steps_v.push_back( RelativeSize( distance, size ) );
  }
  box.SetProfile( gradient_steps_v );

  return box;
}

void CMeshRefinementModel::from_stream( CArchiveInterface& stream )
{
  TBoxMap& d = m_data_m;

  d.clear();

  int num_boxes;
  stream >> num_boxes;

  for( int i=0; i < num_boxes; i++ )
  {
    RefinementBox box = box_from_stream( stream );
    m_data_m[ box.Name() ] = box;
  }

  printer->info("from_stream ->");
  print();
}