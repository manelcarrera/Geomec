#include "stdafx.h"

#include <QtWidgets>
#include <QWidget>
#include <QObject>
#include <QString>

#include "MeshRefinementBoxDlg.h"
#include "custombtn.h"
#include "MeshRefinementBox.h"

CMeshRefinementBox* CMeshRefinementBoxDlg::m_box;

namespace
{
	std::vector< QString > g_labels_s = { "Eastern", "Northern", "Depth", "Rx", "Ry", "Rz", "Sx", "Sy", "Sz" };

	static const double STEP_POSITION	= 100;
	static const double STEP_ROTATION	= 5;
	static const double STEP_SCALE		= 2;

	static const QString QS_TRANSLATE	= QString("Translate");
	static const QString QS_ROTATE		= QString("Rotate");
	static const QString QS_SCALE		= QString("Scale");
	static const QString QS_PLUS		= QString("+");
	static const QString QS_MINUS		= QString("-");
	static const QString QS_STEP		= QString("Step");

	//static const QString QS_STYLE		= QString("border-bottom-width: 1px; border-top-width: 1px; border-left-width: 1px; border-right-width: 1px; border-bottom-style: solid; border-radius: 0px;");
	static const QString QS_STYLE		= QString("border-width: 1px; border-style: solid; border-radius: 0px;");

	static const QSize SIZE_BTN			= QSize( 22, 22 );
	static const QSize SIZE_EDIT		= QSize( 66, 22 );
	static const QSize SIZE_EDIT_SMALL	= QSize( 44, 22 );
}

void CMeshRefinementBoxDlg::box( CMeshRefinementBox* box ){ m_box = box; };
CMeshRefinementBox* CMeshRefinementBoxDlg::box(){ return m_box; };

CMeshRefinementBoxDlg* CMeshRefinementBoxDlg::instance()
{
	static CMeshRefinementBoxDlg _instance;
	return &_instance;
}

bool CMeshRefinementBoxDlg::is_show(){ return m_box && m_box->is_show(); }

void CMeshRefinementBoxDlg::exec_()
{
	if( m_box && !m_box->is_show() )
	{
		m_box->show(); 
		exec(); 
		m_box->hide(); 
	}
}

//CMeshRefinementBoxDlg::CMeshRefinementBoxDlg( CMeshRefinementBox* box, QWidget *parent ) : QDialog( parent ), m_box( box )
CMeshRefinementBoxDlg::CMeshRefinementBoxDlg( QWidget *parent ) : QDialog( parent )
{
	// Controls: create and layout
	QGridLayout* layout1 = new QGridLayout( this );
    //layout1->setColumnStretch(1, 1);

	QLabel* label = new QLabel( QS_TRANSLATE );
	label->setStyleSheet( QS_STYLE );
	layout1->addWidget( label, 0, 0 );
	m_labels_2.push_back( label );

	label = new QLabel( QS_STEP );
	layout1->addWidget( label, 0, 4 );
	m_labels_2.push_back( label );

	label = new QLabel( QS_ROTATE );
	label->setStyleSheet( QS_STYLE );
	layout1->addWidget( label, 5, 0 );
	m_labels_2.push_back( label );

	label = new QLabel( QS_SCALE );
	label->setStyleSheet( QS_STYLE );
	layout1->addWidget( label, 10, 0 );
	m_labels_2.push_back( label );

	for( int i=0; i <= Sz; i++ )
	{
		m_labels[	i ] = new QLabel( g_labels_s[ i ] );
		//m_labels.push_back( new QLabel( g_labels_s[ i ] ) );

		m_edits[	i ] = new QLineEdit;
		m_edits[	i ]->setFixedSize( SIZE_EDIT );

		m_steps[	i ] = new QLineEdit;
		m_steps[	i ]->setFixedSize( SIZE_EDIT_SMALL );

		m_btns_m[	i ] = new CBtn( QS_MINUS, i, CBtn::Minus );
		//m_btns_m[ i ]->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred ); // Doesn't work 
		//QSizePolicy::Minimum // Doesn't work 
		m_btns_m[	i ]->setFixedSize( SIZE_BTN );

		m_btns_p[	i ] = new CBtn( QS_PLUS, i, CBtn::Plus );
		m_btns_p[	i ]->setFixedSize( SIZE_BTN );

		int row =	i >= Sx ?	i+5 :	// scale		-> 3
					i >= Rx ?	i+3 :	// rotate		-> 2
								i+1 ;	// translate	-> 1

		int col=0;
	    layout1->addWidget( m_labels[	i ],	row, col++ );
	    layout1->addWidget( m_edits[	i ],	row, col++ );
	    layout1->addWidget( m_btns_m[	i ],	row, col++ );
	    layout1->addWidget( m_btns_p[	i ],	row, col++ );
		layout1->addWidget( m_steps[	i ],	row, col );
	}

	update_fields();

	for( int i=0; i<=Pd; i++ )	step( i, STEP_POSITION );
	for( int i=Rx; i<=Rz; i++ )	step( i, STEP_ROTATION );
	for( int i=Sx; i<=Sz; i++ )	step( i, STEP_SCALE );

	QPushButton* btn_t = new QPushButton( QS_TRANSLATE );
	layout1->addWidget( btn_t, 4, 1 );

	QPushButton* btn_r = new QPushButton( QS_ROTATE );
	layout1->addWidget( btn_r, 9, 1 );

	QPushButton* btn_s = new QPushButton( QS_SCALE );
	layout1->addWidget( btn_s, 14, 1 );

	btn_t->setFixedSize( SIZE_EDIT );
	btn_r->setFixedSize( SIZE_EDIT );
	btn_s->setFixedSize( SIZE_EDIT );

	m_btns = { btn_t, btn_r, btn_s };
	

	// Events
	connect( btn_t, &QAbstractButton::clicked, this, &CMeshRefinementBoxDlg::translate );
	connect( btn_r, &QAbstractButton::clicked, this, &CMeshRefinementBoxDlg::rotate );
	connect( btn_s, &QAbstractButton::clicked, this, &CMeshRefinementBoxDlg::scale );

	for( int i=0; i <= Sz; i++ )
	{
		connect( m_btns_m[ i ], &QAbstractButton::clicked, this, &CMeshRefinementBoxDlg::btn );
		connect( m_btns_p[ i ], &QAbstractButton::clicked, this, &CMeshRefinementBoxDlg::btn );
	}

	//m_box->show( true );
}

CMeshRefinementBoxDlg::~CMeshRefinementBoxDlg()
{
	m_box->hide();

	for( int i=0; i < m_btns.size(); i++ )
		delete m_btns[ i ];

	std::map< int, QLineEdit*  >::iterator it;
	for ( it = m_edits.begin(); it != m_edits.end(); it++ )
		delete it->second;

	for( int i=0; i < m_labels_2.size(); i++ )
		delete m_labels_2[ i ];

	std::map< int, QLabel*  >::iterator it2;
	for ( it2 = m_labels.begin(); it2 != m_labels.end(); it2++ )
		delete it2->second;

	std::map< int, QPushButton*  >::iterator it3;
	for ( it3 = m_btns_m.begin(); it3 != m_btns_m.end(); it3++ )
		delete it3->second;

	for ( it3 = m_btns_p.begin(); it3 != m_btns_p.end(); it3++ )
		delete it3->second;
}

void CMeshRefinementBoxDlg::update_fields()
{
	// Fill fields

	// Position
	std::vector< float > t_v = m_box->translation();
	val( Pe, t_v[ Pe ] );
	val( Pn, t_v[ Pn ] );
	val( Pd, t_v[ Pd ] );
	
	// Rotation
	std::vector< int > r_v = m_box->rotation();
	val( Rx, r_v[ X ] );
	val( Ry, r_v[ Y ] );
	val( Rz, r_v[ Z ] );
	
	// Scale
	std::vector< float > s_v = m_box->scale();
	val( Sx, s_v[ X ] );
	val( Sy, s_v[ Y ] );
	val( Sz, s_v[ Z ] );
}

void CMeshRefinementBoxDlg::btn()
{
	CBtn* _btn = qobject_cast< CBtn* >( sender() ); // retrieve the button you have clicked

	int idx = _btn->m_idx;
	int action = _btn->m_action;

	CBtn::eAction a_ = (CBtn::eAction)action;

	int step_ = step( idx ); 
	/*int step =	idx <= CMeshRefinementBoxDlg::Pd ? STEP_POSITION : 
				idx <= CMeshRefinementBoxDlg::Rz ? STEP_ROTATION :
									STEP_SCALE;*/

	if( idx <= CMeshRefinementBoxDlg::Pd ) //FIXME
	{
		m_box->translate( idx, step_, a_ );
	}
	else if( idx <= CMeshRefinementBoxDlg::Rz )
		m_box->rotate( idx, step_, a_ );
	else if( idx <= CMeshRefinementBoxDlg::Sz )
		m_box->scale( idx, step_, a_ );

	update_fields();
};

int CMeshRefinementBoxDlg::step( int idx ){ return m_steps[ idx ]->text().toInt(); }
void CMeshRefinementBoxDlg::step( int idx, int val ){ m_steps[ idx ]->setText( QString::number( val ) ); }
float CMeshRefinementBoxDlg::val( int idx ){ return m_edits[ idx ]->text().toFloat(); }
void CMeshRefinementBoxDlg::val( int idx, float val ){ m_edits[ idx ]->setText( QString::number( val ) ); }
void CMeshRefinementBoxDlg::val( int idx, int val ){ m_edits[ idx ]->setText( QString::number( val ) ); }

std::vector< float > CMeshRefinementBoxDlg::vals( eVals type_ )
{ 
	int i = type_ == Position ? Pe : type_ == Rotation ? Rx : Sx; 
	return std::vector< float >{ val( i ), val( i+1 ), val( i+2 ) };
}

void CMeshRefinementBoxDlg::translate(){	m_box->translate(	vals( Position ) ); };
void CMeshRefinementBoxDlg::rotate(){		m_box->rotate(		vals( Rotation ) ); }
void CMeshRefinementBoxDlg::scale(){		m_box->scale(		vals( Scale ) ); }