#pragma once

#include <QtWidgets\QTableView>
#include <QtWidgets\QMenu>
#include <QtGui\QStandardItemModel>
#include <QDialog>

#include <vector>
#include <map>
#include <functional>

#include "RBox.h" // RelativeSize

class QStandardItem;
class CModelBase;
class QStandardItem;
class QKeyEvent;
class QCloseEvent;
class CGradientRow;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QCheckBox;

class CMeshRefinementBoxesHandler;

typedef std::function< void( const QString& ) > TFunction;

class MeshRefinementDlg : public QDialog
{
	Q_OBJECT

	CMeshRefinementBoxesHandler*	m_boxes;
	QMap< QString, CGradientRow* >	m_rows_m;
	
	QVBoxLayout*					m_rows;
	QVBoxLayout* 					m_v_layout;
	QHBoxLayout* 					m_btn_layout;
	QPushButton*					m_add_btn;
	QPushButton*					m_save_btn;
	QPushButton*					m_remesh_btn;
	QPushButton*					m_pdf_btn;

	QCheckBox*						m_show_all;

	TFunction m_function_cb; // to be called when zoom to box is requested 

public:
	enum eError{ OutOfScope, Overlap, NoError };

private:
	TFunction& cb(){ return m_function_cb; }
	void keyPressEvent(QKeyEvent *e);
	bool check_scope( std::vector<QString>& out_v );
	bool check_overlap( std::vector<QString>& over_v );

public:
	// to be accessed by the gradient dlg
	bool check_scope( const QString& name );

	bool check_overlap( 
		const QString& name, 
		std::vector< QString >& over_v );

	bool check( 
		const QString& name, 
		std::vector<QString>& over_v, 
		bool& res1, bool& res2 );

public:
	MeshRefinementDlg( CMeshRefinementBoxesHandler* boxes, QWidget *parent = 0 );
	~MeshRefinementDlg();
	
	void closeEvent( QCloseEvent *event );
	void showEvent(QShowEvent *);
	void show_or_hide_row( const QString& name );

	void on_box_modified_cb( const QString& name );

	// Event handler registers to be requested when zoom to box is needed
	void register_cb( TFunction function_cb ){ m_function_cb = function_cb; };

	std::vector< RelativeSize > gradient( const QString& name );
	bool visible( const QString& name );
	RefinementBox::GradientType_ type( const QString& name ); // gradient type: linear / center point

	eError save();

public slots:
	void on_remove( const QString& box_name );
	void on_add();
	void on_save();
	void on_remesh();
	void on_pdf();
	void on_checkbox_change( int state );
	void on_box_checkbox_change( const QString& name, int state ); //from line
	void on_zoom_to_box_request( const QString& box_name );
	void on_resize( const QString& name, int up_down );
	void on_show_warning( const QString& name );


private:
//	TFunction m_close_cb;

	bool is_changes();
	bool is_changes_gradient();
	bool is_changes_type();
	void update_rows();
	void remove_rows();
	void add_rows();
	TGradientM gradient_m();
	TTypeM type_m();
	QMap< QString, bool > visible_m();
};