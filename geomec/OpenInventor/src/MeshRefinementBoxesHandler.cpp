#include "stdafx.h"

#include "MeshRefinementBoxesHandler.h"
#include "MeshRefinementModel.h"
#include "Printer.h"

#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/draggers/SoTabBoxDragger.h> // Does everything but cant rotate
#include <Inventor/draggers/SoDragger.h> //SoDraggerCB

#include <QtWidgets\QMessageBox>

namespace{
Printer* printer = Printer::instance(Printer::Mr);
void print( 
	const SoTabBoxDragger* box, 
	const TGradientV gradient,
	const RefinementBox::GradientType_& type_ )
{
	const SbVec3f& sf = box->scaleFactor.getValue();
	const SbVec3f& t = box->translation.getValue();

	T3da c = { t[ 0 ], t[ 1 ], t[ 2 ] };	//center
	T3da s = { sf[ 0 ] , sf[ 1 ], sf[ 2 ] };	//size
	//T3da r = { 0, 0, 0 };	//rotation : TODO

	QString g_s;
	for( const auto& g : gradient ) 
	{
		QString gs_s_ = QString::asprintf("[%.1f,%.1f]", g.Distance(), g.Size() );
		g_s += gs_s_;
	}

	printer->debug(

		"handler : print : gui box : c: [%.2f,%.2f, %.2f], s: [%.2f,%.2f, %.2f] type:%d g: %s", 

		c[0], c[1], c[2], 
		s[0], s[1], s[2],

		type_,

		g_s.toStdString().c_str() );
}

/*void print( const SoTabBoxDragger* box )
{
	const SbVec3f& sf = box->scaleFactor.getValue();
	const SbVec3f& t = box->translation.getValue();

	T3da c = { t[ 0 ], t[ 1 ], t[ 2 ] };	//center
	T3da s = { sf[ 0 ] , sf[ 1 ], sf[ 2 ] };	//size
	//T3da r = { 0, 0, 0 };	//rotation : TODO

	printer->debug(
		"handler : print : gui box : c: [%.2f,%.2f, %.2f], s: [%.2f,%.2f, %.2f]", 
		c[0], c[1], c[2], 
		s[0], s[1], s[2] );
}

void print( const SoTransform* box )
{
	const SbVec3f& sf = box->scaleFactor.getValue();
	const SbVec3f& t = box->translation.getValue();

	T3da c = { t[ 0 ], t[ 1 ], t[ 2 ] };	//center
	T3da s = { sf[ 0 ] , sf[ 1 ], sf[ 2 ] };	//size
	//T3da r = { 0, 0, 0 };	//rotation : TODO

	printer->debug(
		"handler : print : transform : c: [%.2f,%.2f, %.2f], s: [%.2f,%.2f, %.2f]", 
		c[0], c[1], c[2], 
		s[0], s[1], s[2] );
}*/

void print( const SbVec3f& v, std::string text="" )
{
	printer->debug(
		"handler : print : %s : t: [%.2f,%.2f, %.2f]", 
		text,
		v[0], v[1], v[2] );
}

void print( const T3da& v, std::string text="" )
{
	printer->debug(
		"handler : print : %s : t: [%.2f,%.2f, %.2f]", 
		text,
		v[0], v[1], v[2] );
}

void show_( SoSwitch* s, bool val )
{
	if( s )
		s->whichChild = val ? SO_SWITCH_ALL : SO_SWITCH_NONE; 
	else{} // ERROR
}


bool is_box_changed( 
	const RefinementBox& box, 
	const SoTabBoxDragger* gui_box )
{
	const SbVec3f& sf = gui_box->scaleFactor.getValue();
	const SbVec3f& t = gui_box->translation.getValue();

	T3da c = { t[ 0 ], t[ 1 ], t[ 2 ] };	//center
	T3da s = { sf[ 0 ] , sf[ 1 ], sf[ 2 ] };	//size
	T3da r = { 0, 0, 0 };	//rotation : TODO

	T3da c1 = box.Center();
	T3da s1 = box.Size();
	T3da r1 = box.Rotation();

	return c != c1 || s != s1 || r != r1;
}

RefinementBox refinement_box_gui_to_data( 
	const SoTabBoxDragger* gui_box, 
	const TGradientV gradient, 
	const QString& name, 
	const bool& visible,
	const RefinementBox::GradientType_& type_ )
{
	const SbVec3f& sf = gui_box->scaleFactor.getValue();
	const SbVec3f& t = gui_box->translation.getValue();

	T3da c = { t[ 0 ], t[ 1 ], t[ 2 ] };	//center
	T3da s = { sf[ 0 ] , sf[ 1 ], sf[ 2 ] };	//size
	T3da r = { 0, 0, 0 };	//rotation : TODO

	// This needs to be saved before clearing the data map
	//const std::vector< RelativeSize > b_g = { RelativeSize( 0., 0.50 ), RelativeSize( 0.8, 0.5 ) };

	RefinementBox box = RefinementBox( c, s, r, type_, gradient, name, visible );

	printer->debug("handler : refinement_box_gui_to_data");
	print( gui_box, gradient, type_ );
	box.print();

	return box;
}
} // namespace


std::vector< QString > CMeshRefinementBoxesHandler::changed_boxes()
{
	std::vector< QString > changed_v;

	for( auto name: m_boxes_m.keys() )
	{
		SoTabBoxDragger* gui_box = m_boxes_m[ name ];

		if( m_data_m.contains( name ) )
		{
			const RefinementBox& box = m_data_m[ name ];
			if( is_box_changed( box, gui_box ) )
				changed_v.push_back( name );
		}
	}
	return changed_v;
}

void CMeshRefinementBoxesHandler::data_to_gui_box( const QString& name ) 
{
	//in
	const RefinementBox& data = m_data_m.value( name );
	//out
	SoTabBoxDragger* box = m_boxes_m[ name ];

	SoSFVec3f& sf = box->scaleFactor;
	const SbVec3f& val = sf.getValue();
	const T3da& s = data.Size();
	box->scaleFactor.setValue( s[ 0 ], s[ 1 ], s[ 2 ] );

	SoSFVec3f& t = box->translation;
	const T3da& c = data.Center();
	t.setValue( c[ 0 ], c[ 1 ], c[ 2 ] );

	// TODO: rotation : current box doesn't rotate
}

void CMeshRefinementBoxesHandler::create_box( const QString& name ) 
{
	m_boxes_m[ name ] = new SoTabBoxDragger();

	m_switches_m[ name ] = new SoSwitch();

	SoTabBoxDragger* box = m_boxes_m[ name ];
	m_names_m[ box ] = name;
	box->addFinishCallback( cb_box_modified, this );

	// add to scene + show/hide
	SoSwitch* sw = m_switches_m.value( name );
	m_sw->addChild( sw );
	sw->whichChild = SO_SWITCH_ALL;

	sw->addChild( box );
}

void CMeshRefinementBoxesHandler::create_all_boxes() 
{
	for( auto name : m_data_m.keys() )
	{
		create_box( name );
		data_to_gui_box( name );
		SoSwitch* sw = m_switches_m.value( name );
		sw->whichChild = m_data_m[ name ].IsVisible() ? SO_SWITCH_ALL : SO_SWITCH_NONE;
	}
}

CMeshRefinementBoxesHandler::~CMeshRefinementBoxesHandler()
{
}

CMeshRefinementBoxesHandler::CMeshRefinementBoxesHandler( CMeshRefinementModel* boxes_model ) 
	:	m_boxes_model( boxes_model ), 
		m_data_m( boxes_model->data() )
{
	m_sw = new SoSwitch;
	m_b = new SoCube;

	create_all_boxes();

	addChild( m_sw );
		
	show();
}

// GUI already knows if visible
// The only thing it can't know is the gradient as it is only in a dlg
void CMeshRefinementBoxesHandler::gui_to_data( 
	const TGradientM& g_m,
	const TTypeM& type_m )
{
	printer->debug("handler : gui_to_data");
	//in
	QMap< QString, SoTabBoxDragger* >& b = m_boxes_m;
	QMap< QString, SoSwitch* >& s = m_switches_m;
	//out
	TBoxMap& d = m_data_m;
	
	d.clear();

	for( auto& name : b.keys() )
	{
		SoTabBoxDragger* box = b.value( name );
		d[ name ] = refinement_box_gui_to_data( 
			box, 
			g_m[ name ], 
			name, 
			is_visible( name ),
			type_m[ name ] );
	}
}

TBoxMap CMeshRefinementBoxesHandler::gui_data_m( 
	const TGradientM& g_m,
	const TTypeM& type_m )
{
	//in
	QMap< QString, SoTabBoxDragger* >& b = m_boxes_m;
	//out
	TBoxMap d;

	for( auto name : b.keys() )
	{
		SoTabBoxDragger* box = b.value( name );
		d[ name ] = refinement_box_gui_to_data( box, g_m[ name ], name, is_visible( name ), type_m[ name ] );
	}
	return d;
}

QMap< QString, bool > CMeshRefinementBoxesHandler::data_visibility()
{
	QMap< QString, bool > v;
	for( auto name : m_data_m.keys() )
		v[ name ] = m_data_m[ name ].IsVisible();
	return v;
}

QMap< QString, bool > CMeshRefinementBoxesHandler::gui_visibility()
{
	//in
	QMap< QString, SoTabBoxDragger* >& b = m_boxes_m;
	QMap< QString, SoSwitch* >& s = m_switches_m;
	//out
	QMap< QString, bool > v;

	for( auto name : b.keys() )
		v[ name ] = is_visible( name ); 

	return v;
}

void CMeshRefinementBoxesHandler::resize( const QString& name, int up_down )
{
	SoTabBoxDragger* box = m_boxes_m[ name ];

	SoSFVec3f& sf = box->scaleFactor;
	const SbVec3f& val = sf.getValue();
	// '0': up / '1': down
	// FIXME: HC
	double factor = up_down == 0 ? 10.0 : 0.1;

	const std::array< double, 3 > s = { factor*val[ 0 ], factor*val[ 1 ], factor*val[ 2 ] };

	box->scaleFactor.setValue( s[ 0 ], s[ 1 ], s[ 2 ] );
}

T3da CMeshRefinementBoxesHandler::size( const QString& name )
{
	SoTabBoxDragger* box = m_boxes_m[ name ];

	SoSFVec3f& sf = box->scaleFactor;
	const SbVec3f& val = sf.getValue();

	return T3da{ val[0], val[1], val[2] };
}

T3da CMeshRefinementBoxesHandler::pos( const QString& name )
{
	SoTabBoxDragger* box = m_boxes_m[ name ];

	SoSFVec3f& pos_ = box->translation;
	const SbVec3f& val = pos_.getValue();

	return T3da{ val[0], val[1], val[2] };
}


void CMeshRefinementBoxesHandler::size( const QString& name, T3da size_ )
{
	m_boxes_m[ name ]->scaleFactor.setValue( size_[ 0 ], size_[ 1 ], size_[ 2 ] );
}

void CMeshRefinementBoxesHandler::pos( const QString& name, T3da pos_ )
{
	m_boxes_m[ name ]->translation.setValue( pos_[ 0 ], pos_[ 1 ], pos_[ 2 ] );
}


void CMeshRefinementBoxesHandler::add( const QString& name )
{
	create_box( name );
}

// restore box position and dimensions
void CMeshRefinementBoxesHandler::undo( const QString& name )
{
	data_to_gui_box( name );
}

void CMeshRefinementBoxesHandler::remove( const QString& name )
{
	// TODO: control duplicates

	//data
	// don't remove anything from data, only from GUI

	
	//GUI
	//SoTabBoxDragger* box = m_boxes_m[ name ]; //delete box; // OIV objects can't be deleted
	m_names_m.remove( m_boxes_m[ name ] );
	m_sw->removeChild( m_switches_m[ name ] ); //dont need to remove its child (the box itself)
	//delete m_boxes_OIV_m[ name ]; // can't be deleted so I imagine it's done internaly
	m_switches_m.remove( name );
	m_boxes_m.remove( name );
}

void CMeshRefinementBoxesHandler::rename( const QString& old_name, const QString& new_name )
{
	m_boxes_m[ new_name ] = m_boxes_m[ old_name ];
	m_boxes_m.remove( old_name );

	m_switches_m[ new_name ] = m_switches_m[ old_name ];
	m_switches_m.remove( old_name );

	m_data_m[ new_name ] = m_data_m[ old_name ];
	m_data_m.remove( old_name );

	// names_m: (pointer, name)
	m_names_m[ m_boxes_m.value( new_name ) ] = new_name;

}

void CMeshRefinementBoxesHandler::show( bool val )
{ 
	show_( m_sw, val ); 
}

void CMeshRefinementBoxesHandler::show( const QString& box_name, bool val )
{ 
	if( m_switches_m.contains( box_name ) )
	{
		SoSwitch* sw = m_switches_m[ box_name ];
		show_( sw, val ); 
	}
	else{} // ERROR
}

bool CMeshRefinementBoxesHandler::is_visible( const QString& box_name )
{ 
	if( m_switches_m.contains( box_name ) )
	{
		SoSwitch* sw = m_switches_m[ box_name ];
		if( sw )
			return sw->whichChild.getValue() == SO_SWITCH_ALL; 
		else{} // ERROR
	}
	else{} // ERROR

	return false;
}


bool CMeshRefinementBoxesHandler::is_modified( const QString& name )
{
	if( m_boxes_m.contains( name ) )
		return m_modified_set.contains( m_boxes_m[ name ] );
	else
		return false;
}

void CMeshRefinementBoxesHandler::cb_box_modified( void *userData, SoDragger *f )
{
	CMeshRefinementBoxesHandler* box_handler = (CMeshRefinementBoxesHandler*)userData;
	SoTabBoxDragger* box = static_cast< SoTabBoxDragger* >( f );
	box_handler->m_modified_set.insert( static_cast< SoTabBoxDragger* >( box ) );

	// notify dlg
	box_handler->cb()( box_handler->names_m()[ box ] ); // function doesn't need to be public
}

// triggered on rotate btn clicked
void CMeshRefinementBoxesHandler::rotate( const QString& name )
{
	// TODO
}