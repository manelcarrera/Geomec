#include "stdafx.h"

#include "MeshRefinementDlg.h"
#include "Printer.h"
#include "MeshRefinementBoxesHandler.h"
#include "MeshRefinementGradientDlg.h"
#include "RBox.h"
#include "MeshRefinementModel.h"
#include "GeomecDoc.h"

#include <QtWidgets\QAction>
#include <QtWidgets\QMessageBox>
#include <QtGui\QStandardItemModel>
#include <QtGui\QStandardItem>
#include <QAbstractItemView>
#include <QtWidgets\QHeaderView>
#include <QtWidgets\QApplication>
#include <QtWidgets\QSizePolicy>
#include <QtCore/QTimer>
#include <QtCore/QFile>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QIcon>
#include <QPixmap>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QSpacerItem>
#include <QCheckBox>
#include <QScrollArea>
#include <QFrame>
#include <QKeyEvent>

#include "util_.h"


/*
Notes:
-Undo icon is only set when box is modified from the scene

*/


namespace{
  const QString IMG_BASE_PATH	= QString::fromStdString( util::GetAppPath() ) + QString("/img/");

  static const QString ADD_IMG_URL	= IMG_BASE_PATH + QString("add_16.png" );
  static const QString SAVE_IMG_URL	= IMG_BASE_PATH + QString("save_16.png" );
  static const QString REMESH_IMG_URL = IMG_BASE_PATH + QString("connections_16.png" );
  static const QString PDF_IMG_URL	= IMG_BASE_PATH + QString("pdf_16.png" );

  static const QString TEXT_CONFIRM_TITLE	= QString("Refinement box"); 
  static const QString TEXT_CONFIRM_TEXT	= QString("Keep changes?"); 
  static const QString SAVE_CONFIRMT		= QString("Save changes?"); 

  QPushButton* add_btn( 
    const QString& name, 
    const QString& img, 
    QHBoxLayout* layout )
  {
    QPushButton* btn =  new QPushButton( name );
    btn->setIcon( 
      QIcon( 
        QPixmap( img ) ) );
    btn->setIconSize( QSize(16,16) );
    //m_add_btn->setStyleSheet("border: none");
    layout->addWidget( btn, 0, Qt::AlignLeft ); // stretch, align

    return btn;
  }

  Printer* printer = Printer::instance(Printer::Mr);
}

// pop-up's
namespace
{
void show_warnings( 
  const QString& name,
  const std::vector<QString>& over_v,
  bool res1, bool res2 )
{
  QString msg2;
  for( auto box : over_v )
    msg2 += QString::asprintf("-'%s'<br>", box.toStdString().c_str());
  msg2 = msg2.left( msg2.size()-4 );

  QString msg;
  if( !res1 && !res2 )
    msg = QString::asprintf("<p align='left'>Box out of scope.<br><br>Box overlaps:<br>%s</p>", msg2.toStdString().c_str() );
  if( !res1 && res2 )
    msg = QString::asprintf("<p align='left'>Box out of scope</p>" );
  if( res1 && !res2 )
    msg = QString::asprintf("<p align='left'>Box overlaps:<br>%s</p>", msg2.toStdString().c_str() );

  QMessageBox::warning( 
    nullptr, 
    QString::asprintf("Boxe geometry checks: %s", name.toStdString().c_str()), 
    msg );
}

// all boxes
void show_overlap( std::vector<QString>& over_v )
{
  if( !over_v.size() )
    return;

  QString msg;
  over_v.resize( over_v.size()/2 ); // truncate: second half are duplicated
  for( auto name: over_v )
  {
    QStringList list = name.split("-");
    msg += QString::asprintf("-'%s'-'%s'<br>", 
      list[0].toStdString().c_str(), 
      list[1].toStdString().c_str());
  }
  msg = msg.left( msg.size()-4 );

  QMessageBox::warning( 
    nullptr, 
    "Boxes geometry checks", 
    QString::asprintf(
      "<p align='left'>Box(es) overlap (%dx):<br>%s</p>",
      over_v.size(), 
      msg.toStdString().c_str() ) );
}

// all boxes
void show_out_of_scope( std::vector<QString>& out_v )
{
  if( !out_v.size() )
    return;

  QString msg;
  for( auto name: out_v )
    msg += QString::asprintf("-'%s'<br>", name.toStdString().c_str());
  msg = msg.left( msg.size()-4 );

  QMessageBox::warning( 
    nullptr, 
    "Boxes geometry checks", 
    QString::asprintf(
      "<p align='left'>Box(es) out of scope (%dx):<br>%s</p>", 
      out_v.size(), 
      msg.toStdString().c_str() ) );
}

}

// delete needs to be implemented as the life cicle is the time the model is open
// memeory allocated and released in the evts handler
MeshRefinementDlg::~MeshRefinementDlg()
{
  for( auto& name : m_rows_m.keys() )
    delete m_rows_m[ name ];
  delete m_rows;

  delete m_add_btn;
  delete m_save_btn;
  delete m_remesh_btn;
  delete m_pdf_btn;

  delete m_btn_layout;
  delete m_v_layout;
}

// TODO:
// By now dlg needs to be closed inorder to ave changes
// Otherwise box changes have no efect and refinemt is done in a wrong area
// A save booton needs to be added
MeshRefinementDlg::MeshRefinementDlg( CMeshRefinementBoxesHandler* boxes, QWidget *parent ) : QDialog( parent ), m_boxes( boxes )
{
  setWindowTitle ( "Mesh Refinement" );

  m_v_layout = new QVBoxLayout();
  m_btn_layout = new QHBoxLayout();

  //------------------------------------------------------- Header
  int row=0;

  QHBoxLayout* h_layout = new QHBoxLayout();
  h_layout->addStretch();
  h_layout->addWidget( new CGradientValuesHeader(), Qt::AlignRight );
  h_layout->setMargin(0);

  m_v_layout->addLayout( h_layout );

  //------------------------------------------------------- Rows
  m_rows = new QVBoxLayout();
  add_rows();

  //------------------------------------------------------- btns, check-box

  m_v_layout->addLayout( m_rows );

  m_v_layout->addStretch(); // to have line and buttons allways at bottom

  QFrame* line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken); // thinner

  m_v_layout->addWidget( line );
  m_v_layout->addLayout( m_btn_layout );


  // Do it with a functor



  m_add_btn	= add_btn("Add",	ADD_IMG_URL,	m_btn_layout );
  m_save_btn	= add_btn("Save",	SAVE_IMG_URL,	m_btn_layout );
  m_remesh_btn= add_btn("Remesh", REMESH_IMG_URL, m_btn_layout );

  // show all
  m_show_all = new QCheckBox("Show all");
  m_show_all->setChecked( false );
  m_btn_layout->addWidget( m_show_all );

  //m_btn_layout->addWidget( new QSpacerItem() );
  m_btn_layout->addStretch();

  m_pdf_btn	= add_btn("",		PDF_IMG_URL, m_btn_layout );


  setLayout( m_v_layout );

  connect( m_add_btn,		SIGNAL( clicked() ), this, SLOT( on_add() ) );
  connect( m_save_btn,	SIGNAL( clicked() ), this, SLOT( on_save() ) );
  connect( m_remesh_btn,	SIGNAL( clicked() ), this, SLOT( on_remesh() ) );
  connect( m_pdf_btn,		SIGNAL( clicked() ), this, SLOT( on_pdf() ) );

  connect( m_show_all, SIGNAL( stateChanged(int) ), this, SLOT( on_checkbox_change(int) ) );

  TFunction function_cb = std::bind( &MeshRefinementDlg::on_box_modified_cb, this, std::placeholders::_1 );
  boxes->register_cb( function_cb );

  // on mouse over help
  m_show_all->setToolTip("If checked, also hidden boxes are shown in the list");
  m_add_btn->setToolTip("Add a new refinement box");
  m_save_btn->setToolTip("Save changes");
  m_pdf_btn->setToolTip("Mesh refinement guide (version 1.0).pdf");

}

std::vector< RelativeSize > MeshRefinementDlg::gradient( const QString& name )
{
  return m_rows_m[ name ]->gradient();
}

bool MeshRefinementDlg::visible( const QString& name )
{
  return m_rows_m[ name ]->is_values_visible();
}

RefinementBox::GradientType_ MeshRefinementDlg::type( const QString& name )
{
  return m_rows_m[ name ]->type();
}

// box modified in the scene
// updates:
// -box modified icon (undo)
// -coordinates dlg if open
void MeshRefinementDlg::on_box_modified_cb( const QString& name )
{
  m_rows_m[ name ]->update_box_modified( true );

  // box overlap involves at least one other box 
  for( auto name_ : m_rows_m.keys() )
    m_rows_m[ name_ ]->on_update_warning();
}

void MeshRefinementDlg::on_resize( const QString& name, int up_down )
{ 
  m_boxes->resize( name, up_down ); 
  m_rows_m[ name ]->update_box_modified( true );
}

void MeshRefinementDlg::show_or_hide_row( const QString& name )
{
  // FIXME: This happens during rows load
  if( !m_rows_m.contains( name ) )
    return;

  CGradientRow* row = m_rows_m[ name ];

  bool s1 = row->m_checkbox->isChecked(); //status

  bool s2 = m_show_all->isChecked();

  if( !s1 && !s2 )
    row->hide();
  else
    row->show();
}


void MeshRefinementDlg::on_box_checkbox_change( const QString& name, int state ) // signaled by line
{
  show_or_hide_row( name );

  for( auto name_ : m_rows_m.keys() )
    m_rows_m[ name_ ]->on_update_warning();
}

void MeshRefinementDlg::on_checkbox_change( int state )
{ 
  for( auto name : m_rows_m.keys() )
    show_or_hide_row( name );
}

void MeshRefinementDlg::on_add()
{
  int id = rand() % 1000;

  QString name = QString::asprintf("box_%d", id);

  m_boxes->add( name );

  CGradientRow* item = new CGradientRow( m_boxes, name, this );

  m_rows_m[ name ] = item;

  m_rows->addWidget( item );
}

void MeshRefinementDlg::on_pdf()
{
  AfxGetApp()->GetMainWnd()->PostMessage( WM_COMMAND, ID_MR_MANUAL, 0 );
}

// res:
// 0: rno error
// 1: some box out of scope error
// 1: two boxes overlapping error
MeshRefinementDlg::eError MeshRefinementDlg::save()
{
  m_boxes->gui_to_data( gradient_m(), type_m() );

  std::vector<QString> out_v;
  bool res1 = check_scope( out_v );//, true );
  if( !res1 )
    return OutOfScope;

  std::vector<QString> over_v;
  bool res2 = check_overlap( over_v );//, true );
  if( !res2 )
    return Overlap;

  return NoError;
}

void MeshRefinementDlg::on_remesh()
{
  m_boxes->gui_to_data( gradient_m(), type_m() );

  std::vector<QString> out_v;
  bool res1 = check_scope( out_v );//, true );
  if( !res1 )
  {
    show_out_of_scope( out_v );
    return;
  }

  std::vector<QString> over_v;
  bool res2 = check_overlap( over_v );//, true );
  if( !res2 )
  {
    show_overlap( over_v );
    return;
  }

  /*QMessageBox::StandardButton res = QMessageBox::critical( 
    nullptr, 
    "Remesh", 
    "Sure to remesh?",  
    QMessageBox::Yes | QMessageBox::No );

  if( res == QMessageBox::Yes )*/
  {
    AfxGetApp()->GetMainWnd()->PostMessage( WM_COMMAND, ID_INVALIDATE_MESH, 0 );
    AfxGetApp()->GetMainWnd()->PostMessage( WM_COMMAND, ID_CREATE_MESH, 0 );
  }
}

// clears all undo icons 
void MeshRefinementDlg::on_save()
{
  if( is_changes() ) // boxes + gradient
  {
    /*QMessageBox::StandardButton res = QMessageBox::critical( 
      nullptr, 
      TEXT_CONFIRM_TITLE, 
      SAVE_CONFIRMT,  
      QMessageBox::Yes | QMessageBox::No );

    if( res == QMessageBox::Yes )*/
    {
      m_boxes->gui_to_data( gradient_m(), type_m() );

      for( auto name : m_rows_m.keys() )
        m_rows_m[ name ]->update_box_modified( false );
    }
  }
  else
  {
    QMessageBox::warning( 
      nullptr, 
      TEXT_CONFIRM_TITLE, 
      QString("No changes made so nothing to be saved") );
  }
}

void MeshRefinementDlg::on_remove(  const QString& box_name  )
{
  CGradientRow* row = m_rows_m[ box_name ];
  m_rows->removeWidget( row );	// remove from the table
  delete row;
  m_rows_m.remove( box_name );	// remove from the widget list

  //OIV wrapper
  m_boxes->remove( box_name ); 

  // update warning icon
  // box overlap involves at least one other box 
  for( auto name_ : m_rows_m.keys() )
    m_rows_m[ name_ ]->on_update_warning();
}

void MeshRefinementDlg::on_zoom_to_box_request( const QString& box_name )
{
  cb()( box_name ); // notify event handler
}

void MeshRefinementDlg::on_show_warning( const QString& name )
{
  std::vector<QString> over_v;
  bool res1;
  bool res2;

  bool res = check( name, over_v, res1, res2 );
  if( !res )
    show_warnings( name, over_v, res1, res2 );
}

void MeshRefinementDlg::showEvent(QShowEvent *)
{
  m_boxes->show();
}

// needed ? 
bool MeshRefinementDlg::is_changes_type()
{
  // in
  TTypeM _type_m = type_m();
  TBoxMap& data_m = m_boxes->data_m();

  // out
  for( auto name : _type_m.keys() )
  {
    if( data_m.contains( name ) ) // very dangerous not doing this !!!
    {
      RefinementBox& d = data_m[ name ];
      if( _type_m[ name ] != d.GradientType() )
        return false;
    }
  }
  return true;
}

bool MeshRefinementDlg::is_changes_gradient()
{
  TBoxMap gui_data_m = m_boxes->gui_data_m( gradient_m(), type_m() );

  TBoxMap& data_m = m_boxes->data_m();

  bool equal=true;

  if( gui_data_m.size() != data_m.size() )
  {
    equal=false;
  }
  else
  {
    for( auto name : gui_data_m.keys() )
    {
      if( data_m.contains( name ) )
      {
        RefinementBox& d1 = gui_data_m[ name ];
        RefinementBox& d2 = data_m[ name ];

        if( d1.Profile() != d2.Profile() || 
          d1.IsVisible() != d2.IsVisible()  )
        {
          equal=false;
          break;
        }
      }
      else
      {
          equal=false;
          break;
      }
    }
  }
  return !equal;
}


bool MeshRefinementDlg::is_changes()
{
  // boxes + gradient
  // Is not a reference so changes are just local
  TBoxMap gui_data_m = m_boxes->gui_data_m( gradient_m(), type_m() ); // boxes data + gradient + visibility

  TBoxMap& data_m = m_boxes->data_m();

  bool equal=true;
  if( gui_data_m.size() != data_m.size() )
  {
    equal=false;
  }
  else
  {
    for( auto name : gui_data_m.keys() )
    {
      if( data_m.contains( name ) )
      {
        if( gui_data_m[ name ] != data_m[ name ]  )
        {
          equal=false;
          break;
        }
      }
      else
      {
        equal=false;
        break;
      }
    }
  }

  /*printer->debug("/////////////////// m_boxes->gui_data_m()");
  for( auto box: gui_data_m )
    box->print();
  printer->debug("/////////////////// m_boxes->data_m()");
  for( auto box: m_boxes->data_m() )
    box->print();*/

  // visibility: not needed, already included above as a box member

  return !equal;
}

void MeshRefinementDlg::remove_rows()
{
  // dlg rows
  for( int i = 0; i < m_rows->count(); i++ )
  {
    QLayoutItem* item = m_rows->itemAt( 0 ); // !!! Attention: this is Ok
    if( dynamic_cast< QWidgetItem* >( item ) )
      delete item->widget();
  }
  m_rows_m.clear();

  // GUI Boxes
  for( auto name : m_boxes->boxes_m().keys() )
    on_remove( name ); // TODO: verify this
}

void MeshRefinementDlg::add_rows()
{
  if( !m_boxes->boxes_m().size() ) // if remove_rows han been executed before
    m_boxes->create_all_boxes();

  for( auto name : m_boxes->boxes_m().keys() )
  {
    CGradientRow* item = new CGradientRow( m_boxes, name, this );
    m_rows_m[ name ] = item;
    m_rows->addWidget( item );

    if( !m_boxes->is_visible( name ) )
      item->hide();
  }
}


void MeshRefinementDlg::update_rows()
{
  remove_rows();
  add_rows();
}

QMap< QString, bool > MeshRefinementDlg::visible_m()
{
  QMap< QString, bool > v_m;
  for( auto name : m_rows_m.keys() )
    v_m[ name ] = visible( name );
  return v_m;
}

TGradientM MeshRefinementDlg::gradient_m()
{
  TGradientM g_m;
  for( auto name : m_rows_m.keys() )
    g_m[ name ] = gradient( name );
  return g_m;
}

QMap< QString, RefinementBox::GradientType_ > MeshRefinementDlg::type_m()
{
  TTypeM t_m;
  for( auto name : m_rows_m.keys() )
    t_m[ name ] = type( name );
  return t_m;
}

void MeshRefinementDlg::closeEvent( QCloseEvent *event )
{
  if( is_changes() )
  {
    QMessageBox::StandardButton res = 
      QMessageBox::critical( 
        nullptr, 
        TEXT_CONFIRM_TITLE, 
        TEXT_CONFIRM_TEXT,  
        QMessageBox::Yes | QMessageBox::No );

    if( res == QMessageBox::Yes )
      m_boxes->gui_to_data( gradient_m(), type_m() );
    else 
    {
      //undo changes: (1)gradient + (2)boxes (size & position)
      if( is_changes_gradient() )
        update_rows();

      if( m_boxes->changed_boxes().size() )
      {
        for( auto name: m_boxes->changed_boxes() )
        {
          m_boxes->remove( name ); 
          m_boxes->create_box( name );
        }
      }
    }
  }
  m_boxes->show( false );
}

void MeshRefinementDlg::keyPressEvent(QKeyEvent *e) 
{
  if(e->key() != Qt::Key_Escape)
    QDialog::keyPressEvent(e);
  else {}
}

///////////////////////////////////////// checks //////////////////////////////////////////////////////

// GUI boxes before being trespassed to data
// one box
bool MeshRefinementDlg::check_scope( const QString& name )
{
  //model
  CGeomecDoc* doc = GetGeomecDoc();
  TBox bbox = doc->bbox();
  //box
  TBoxMap d = m_boxes->gui_data_m( gradient_m(), type_m() );
  RefinementBox& b1 = d[ name ];
  return bbox.contains( b1.box() );
}

// GUI boxes before being trespassed to data
// one box
bool MeshRefinementDlg::check_overlap( 
  const QString& name, 
  std::vector< QString >& over_v )
{
  TBoxMap d1 = m_boxes->gui_data_m( gradient_m(), type_m() );
  TBoxMap d2 = d1;
  RefinementBox& b1 = d1[ name ];
  for( auto n2 : d2.keys() )
  {
    if( name == n2 )
      continue;

    RefinementBox& b2 = d2[ n2 ];
    if( !b2.IsVisible() )
      continue;
    bool res = ( (TBox)b1.box() ).overlaps( b2.box() );
    if( res )
      over_v.push_back( n2 );
  }
  return !over_v.size();
}


// one box
bool MeshRefinementDlg::check( 
  const QString& name, 
  std::vector<QString>& over_v, 
  bool& res1, 
  bool& res2 )
{
  // scope
  res1 = check_scope( name );

  // overlaps
  res2 = check_overlap( name, over_v );

  return res1 && res2;
}

// all boxes
bool MeshRefinementDlg::check_overlap( std::vector<QString>& over_v )
{
  TBoxMap d1 = m_boxes->gui_data_m( gradient_m(), type_m() );
  TBoxMap d2 = d1;
  
  for( auto n1 : d1.keys() )
  {
    RefinementBox& b1 = d1[ n1 ];
    if( !b1.IsVisible() )
      continue;
    for( auto n2 : d2.keys() )
    {
      if( n1 == n2 )
        continue;
      RefinementBox& b2 = d2[ n2 ];
      if( !b2.IsVisible() )
        continue;
      bool res = ( (TBox)b1.box() ).overlaps( b2.box() );
      if( res )
        over_v.push_back( n1 + "-" + n2 );
    }
  }
  return over_v.size() == 0;
}

// all boxes
// boxes out of scope
bool MeshRefinementDlg::check_scope( std::vector<QString>& out_v )
{
  CGeomecDoc* doc = GetGeomecDoc();
  TBox bbox = doc->bbox();

  TBoxMap d = m_boxes->gui_data_m( gradient_m(), type_m() );
  for( auto name : d.keys() )
  {
    RefinementBox& b1 = d[ name ];
    if( !b1.IsVisible() )
      continue;
    bool res = bbox.contains( b1.box() );
    if( !res )
      out_v.push_back( name );
  }
  return out_v.size() == 0;
}
