#pragma once

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSelection.h>

class SoScale;
class SoTranslation;
class SoCube;
class SoRotationXYZ;
class SoSwitch;
class SoSelection;
class SoPath;
class SoDragger;
class SoTabBoxDragger;

#include <QMap>

#include "RBox.h"

class CMeshRefinementModel;

#include <functional>
typedef std::function< void( const QString& ) > TFunction;

class CMeshRefinementBoxesHandler : public SoSelection
{
	SoCube* m_b;
	SoSwitch* m_sw;

	CMeshRefinementModel* m_boxes_model;

	// data: references to maps in mesh refinement model (2x) 
	TBoxMap& m_data_m;

	// gui
	QMap< QString, SoTabBoxDragger* > m_boxes_m;
	QMap< SoTabBoxDragger*, QString > m_names_m;
	QMap< QString, SoSwitch* > m_switches_m;

	QSet< SoTabBoxDragger* > m_modified_set;

	TFunction m_function_cb; // to be called on box modified

private:
	
	void data_to_gui_box( const QString& name );
	TFunction& cb(){ return m_function_cb; }

public:

	CMeshRefinementBoxesHandler( CMeshRefinementModel* boxes_model );
	~CMeshRefinementBoxesHandler();

	TBoxMap& data_m(){ return m_data_m; };
	QMap< QString, SoTabBoxDragger* >& boxes_m(){ return m_boxes_m; };

	QMap< SoTabBoxDragger*, QString >& names_m(){ return m_names_m; };

	// callback on box modified
	static void cb_box_modified( void *userData, SoDragger *f );

	void add( const QString& name );
	void remove( const QString& name );
	void rename( const QString& old_name, const QString& new_name );

	void show( bool val = true );
	void show( const QString& box_name, bool val = true );
	bool is_visible( const QString& box_name );

	void gui_to_data( 
		const TGradientM& g_m, 
		const TTypeM& type_m );

	QMap< QString, bool > gui_visibility();
	QMap< QString, bool > data_visibility();

	TBoxMap gui_data_m( 
		const TGradientM& g_m,
		const TTypeM& type_m );

	bool is_modified( const QString& name );

	void register_cb( TFunction function_cb ){ m_function_cb = function_cb; };

	void undo( const QString& name );

	void resize( const QString& name, int up_down );

	void size( const QString& name, T3da size );
	void pos( const QString& name, T3da pos );

	T3da size( const QString& name );
	T3da pos( const QString& name );

	void create_all_boxes();

	std::vector< QString > changed_boxes();

	void create_box( const QString& name );

	void rotate( const QString& name );
};