#pragma once

#include <QWidget>
#include <QFrame>
#include <QDialog>
#include <QLineEdit>

#include <vector>

#include "RBox.h"

class QHBoxLayout;
class QLabel;
class QCheckBox;
class QMouseEvent;
class QMenu;
class QAction;
class QPushButton;
class QKeyEvent;
class QLineEdit;

class CGradientValues;
class CMeshRefinementBoxesHandler;
class CBoxEdit;
class QGridLayout;
class QDoubleValidator;
class QFocusEvent;

///////////////////////////////////////////////////////////////////////////////////////
//
//									CGradientRow
//
///////////////////////////////////////////////////////////////////////////////////////

class CGradientRow : public QFrame
{
    Q_OBJECT

	enum eActions{	New, Delete, Go, Show_Hide, Num_Actions };

	QHBoxLayout*	m_layout;
	QLabel*			m_img_label;
	QPushButton*	m_warning_btn;
	QPushButton*	m_modified;
	QPushButton*	m_zoom;
	QLineEdit*		m_name_edit;
	QString			m_name;
	CGradientValues* m_values;
	QPushButton*	m_delete_btn;
	QPushButton*	m_enlarge;
	QPushButton*	m_decrease;
	QPushButton*	m_edit_btn;
	QPushButton*	m_type_btn;
	QPushButton*	m_rotate_btn;

	std::map< int, QAction* > m_actions;
	std::map< int, QMenu* > m_menus;

	QMenu* m_menu;

	CMeshRefinementBoxesHandler* m_boxes_handler;

	CBoxEdit* m_edit_dlg;

	RefinementBox::GradientType_ m_type;

public:

	QCheckBox*		m_checkbox;

public:

	CGradientRow( 
		CMeshRefinementBoxesHandler* boxes_handler, 
		const QString& box_name, 
		QWidget *parent = 0 );

	~CGradientRow();

	std::vector< RelativeSize > gradient();

	bool is_values_visible();
	void show_values( bool val );
	void update_box_modified( bool val );

	void update_warning( bool val );

	QString name(){ return m_name; };

	RefinementBox::GradientType_ type(){ return m_type; }

private:
	void keyPressEvent( QKeyEvent *e );
	void show_btns( bool val = true );


signals:
	// to parent (dlg)
	void remove( const QString& box_name );
	void add();
	void checkbox_change( const QString& name, int state );
	void zoom_to_box_request( const QString& box_name );
	void resize( const QString&, int );
	void show_warning_evt( const QString& );

public slots:
	void on_update_warning();

private slots:

	void action( eActions action_ );

	void on_custom_context_menu( const QPoint &point );
	void on_delete();
	void on_undo();
	void on_zoom_to_box();
	void on_checkbox( int state );

	void on_new();

	void on_rename();

	void on_toggle_type();

	void on_size_up();
	void on_size_down();

	void on_edit();

	void on_apply();
	void on_ok();
	void on_cancel();

	void on_rotate();

	void on_show_warning();

};

///////////////////////////////////////////////////////////////////////////////////////
//
//									QLineEdit_Zero, QLineEdit_Others
//
///////////////////////////////////////////////////////////////////////////////////////

class QLineEdit_Center : public QLineEdit
{
    Q_OBJECT
    protected:
		void focusOutEvent( QFocusEvent* e );
	public:
		QLineEdit_Center( QWidget *parent ) : QLineEdit( parent ){}
};
class QLineEdit_Others : public QLineEdit
{
    Q_OBJECT
    protected:
		void focusOutEvent( QFocusEvent* e );
	public:
		QLineEdit_Others( QWidget *parent ) : QLineEdit( parent ){}
};

///////////////////////////////////////////////////////////////////////////////////////
//
//									CGradientValues
//
///////////////////////////////////////////////////////////////////////////////////////

class CGradientValues : public QFrame
{
    Q_OBJECT

	std::vector< int > m_sizes;

	std::vector< QLineEdit* > m_edits_v;
	QMap< QLineEdit*, int > m_index_m;

signals:
	void update_warning_evt();


public:

	CGradientValues( QWidget *parent = 0 );
	~CGradientValues();

	std::vector< RelativeSize > gradient();
	void gradient( const TGradientV& gradient_v );

	bool gradient_check();
};

///////////////////////////////////////////////////////////////////////////////////////
//
//									CGradientValuesHeader
//
///////////////////////////////////////////////////////////////////////////////////////

class CGradientValuesHeader : public QWidget
{
    Q_OBJECT

	QGridLayout* m_layout;

public:

	CGradientValuesHeader( QWidget *parent = 0 );
	~CGradientValuesHeader();
};

///////////////////////////////////////////////////////////////////////////////////////
//
//									CBoxEdit
//
///////////////////////////////////////////////////////////////////////////////////////

class CBoxEdit : public QDialog
{
    Q_OBJECT

	// pos
	QLineEdit* m_px;
	QLineEdit* m_py;
	QLineEdit* m_pz;
	
	// size
	QLineEdit* m_sx;
	QLineEdit* m_sy;
	QLineEdit* m_sz;

	QGridLayout* m_layout;

	QDoubleValidator* m_validator_pos;
	QDoubleValidator* m_validator_size;

	T3da m_pos_ini; 
	T3da m_size_ini;

public:

	T3da pos();
	T3da size();

	T3da pos_ini();
	T3da size_ini();

	void update(	T3da pos,
					T3da size );

signals:
	void close();

private:

	void closeEvent( QCloseEvent *event );

public:

	CBoxEdit(	T3da pos,
				T3da size,
				QWidget *parent = 0 );
		
	~CBoxEdit();
};