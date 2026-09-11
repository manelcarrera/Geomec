#include "SelectorDlg.h" 
#include "SelectorHelper.h" 
#include "XML.h" 

// Qt
#include <QStringList>
#include <QMenu>
#include <QMessageBox>
#include <QDir>
#include <QTableWidget>
#include <QLabel>
#include <QTableWidgetItem>
#include <QDesktopServices>
#include <QUrl>
#include <QFile>
#include <QPushButton>
#include <QSize>
#include <QApplication>
#include <QFileInfo>

//img :  +GUI (QtGui)
#include <QIcon>
#include <QPixmap>


#include "Printer.h"
#include "QUtil.h"
#include "util_.h"

#include <vector>

namespace
{
  static const QString TEXT_CB = "Set as default";
  static const QString TEXT_BTN = "Open";
  static const QString TEXT_TITLE = "Geomec selector";

  static const QString IMG_BASE_PATH	= QUtil::url( QString::fromStdString( util::GetAppPath() ), "img");
  static const QString FILE_IMG_URL	= QUtil::url( IMG_BASE_PATH, "file_16.png" );
  static const QString FOLDER_IMG_URL	= QUtil::url( IMG_BASE_PATH, "folder_16.png" );
  static const QString REMOVE_IMG_URL	= QUtil::url( IMG_BASE_PATH, "remove_16.png" );
  static const QString MORE_IMG_URL	= QUtil::url( IMG_BASE_PATH, "more_than_16.png" );
  static const QString LESS_IMG_URL	= QUtil::url( IMG_BASE_PATH, "less_than_16.png" );
  static const QString PDF_IMG_URL	= QUtil::url( IMG_BASE_PATH, "pdf_16.png" );
  static const QString CONFIG_IMG_URL	= QUtil::url( IMG_BASE_PATH, "config_16.png" );
  static const QString LOGS_IMG_URL	= QUtil::url( IMG_BASE_PATH, "logs_16.png" );
  static const QString PYTHON_IMG_URL	= QUtil::url( IMG_BASE_PATH, "python_16.png" );
  static const QString ADD_IMG_URL	= QUtil::url( IMG_BASE_PATH, "add_file_16.png" );

  static const int COL_RB		= 0; // Radio buttons
  static const int COL_PATH	= COL_RB		+ 1;
  static const int COL_FILE	= COL_PATH		+ 1;
  static const int COL_FOLDER	= COL_FILE		+ 1;
  static const int COL_REMOVE	= COL_FOLDER	+ 1;
  static const int COL_LOGS	= COL_REMOVE	+ 1;
  static const int COLS_NUM	= COL_LOGS		+ 1;

  // files URL's
  static const QString CONFIG_BASE_PATH	= QUtil::url( QString( getenv("APPDATA") ), "Geomec" );
  static const QString INI_FILE_URL		= QUtil::url( CONFIG_BASE_PATH, "geomec.ini" );
  static const QString LOGS_URL			= QUtil::url( CONFIG_BASE_PATH, "gm_selector.log" );
  static const QString SELECTOR_CONFIG_URL= QUtil::url( CONFIG_BASE_PATH, "selector.xml" );
  static const QString LOGS_CONFIG_URL	= QUtil::url( CONFIG_BASE_PATH, "logs.xml" );
  static const QString PYTHON_CONFIG_URL	= QUtil::url( CONFIG_BASE_PATH, "plugins\\python.xml" );

  static const QString PACKAGES_PATH		= QUtil::url( QString::fromStdString( util::GetAppPath() ), "packages");

  // FIXME: wrong pdf
  static const QString PDF_URL = QUtil::url( QString::fromStdString( util::GetAppPath() ), "Geomec selector guide.pdf" );

  static const QString DEFAULT_INST_START = getenv("PROGRAMFILES");

  // FIXME: name should be as the folder's one, to avoid human errors
  QString ini_file( const QString& version )
  {
    QString file = INI_FILE_URL;
    return file.replace("ini", version + ".ini" ).replace("_build_","_");
  }

  QString version_logs_url( const QString& version )
  {
    QString v = version;
    QString ver = CONFIG_BASE_PATH + "/" "gm_" + v.replace("_build_","_") + ".log";
    return ver;
  }

  QString version_logs_file( const QString& version )
  {
    QString v = version;
    QString ver = "gm_" + v.replace("_build_","_") + ".log";
    return ver;
  }

  static const QString RB_BOLD	= "QRadioButton { font-weight: bold }";
  static const QString RB_BLUE	= "QRadioButton { color: blue }";
  static const QString RB_BLACK	= "QRadioButton { color: black }";

  // bold: ProgramFiles only version
  // blue: default version (in 'selector.xml' file)
  // they are not exclusive, can be bold and color
  void set_style( QRadioButton_Custom* rb, const QString& path, const QString& version )
  {
    bool is_default_inst = path.startsWith( DEFAULT_INST_START );
    bool is_default = version == version::read();

    rb->setStyleSheet( is_default ? RB_BLUE : RB_BLACK );

    if( is_default_inst )
      rb->setStyleSheet( rb->styleSheet().append( RB_BOLD ) );
  }

  // img 
  // bold / no bold
  // fixed size to 16x16
  void set_btn_img( QPushButton* btn, const QString& img_url, bool is_border = true )
  {
    btn->setIcon( QIcon( QPixmap( img_url ) ) );
    btn->setIconSize( QSize(16,16) );

    if( !is_border )
      btn->setStyleSheet("border: none");
  }

  bool url_exists( const QString& url )
  {
    return	QFileInfo( url ).exists() || QDir( url ).exists();
  }

  void add_style( QWidget* widget, const QString& style )
  {
    widget->setStyleSheet( widget->styleSheet().append( style ) );
  }

  Printer* printer = Printer::instance(Printer::Sel);
}

//////////////////////////////////////////////////////////////////////////
//
//                      CSelectorDlg
//
//////////////////////////////////////////////////////////////////////////

void CSelectorDlg::update_style()
{
  for( int i = 0; i < m_versions_v.size(); i++ )
  {
    QLayoutItem* item = m_grid_layout.itemAtPosition( i, 0 );
    if( item && item->widget() )
      set_style( 
        qobject_cast< QRadioButton_Custom* >( item->widget() ), 
        m_paths_v[ i ], 
        m_versions_v[ i ] );
  }

}


CSelectorDlg::~CSelectorDlg()
{
  for( int i = 0; i < m_versions_v.size(); i++ )
  {
    for( int j = 0; j < COLS_NUM ; j++ )
    {
      QLayoutItem* item = m_grid_layout.itemAtPosition( i, j );
      if( item && item->widget() )
        delete item->widget();
    }
  }
}

// Example:
// -version: 5.7.0.4_build_19245.2 
// -path: C:\Users\%usr%\AppData\Roaming\Geomec 
// 
// Style:
// -Bold: installed one
// -Blue: default one
CSelectorDlg::CSelectorDlg( 
  const std::pair< 
    std::vector< QString >, 
    std::vector< QString > >& versions_p )
{
  CXml cfg( app::CONFIG_FILE, {}, {"delete"} );
  m_b_delete	= cfg.exist( "delete" );

  m_versions_v = versions_p.first;
  m_paths_v = versions_p.second;

  setWindowTitle( TEXT_TITLE );
  setLayout( &m_grid_layout );

  m_rb_group.setExclusive(true);

  for( int i = 0; i < m_versions_v.size(); i++ ) 
  {
    const QString& version = m_versions_v[ i ];
    const QString& path = m_paths_v[ i ];

    printer->debug("dir: %s", version.toStdString().c_str() );

    // rb
    QRadioButton_Custom* rb = new QRadioButton_Custom( version );
    rb->setAutoExclusive( true );
    rb->setToolTip( path );
    set_style( rb, path, version );
    m_rb_group.addButton( rb, i );
    m_grid_layout.addWidget( rb, i, 0 );

    // version installation folder
    QLabel* label = new QLabel( "-- " + path + " --" );
    //label->setStyleSheet("QLabel { font-style: italic; }");
    m_grid_layout.addWidget( label, i, 1 );

    // btns: 3x
    bool border = false;
    bool b_version_ini_file = QFile::exists( ini_file( version ) );

    bool b_version_logs = QFile::exists( version_logs_url( version ) );
    
    // 1)config file : 1)present 2)to be created
    QPushButton_Custom* btn1 = new QPushButton_Custom( QPushButton_Custom::Version_Config_File, i, this, b_version_ini_file );
    set_btn_img( btn1, b_version_ini_file ? CONFIG_IMG_URL : ADD_IMG_URL , border );
    m_grid_layout.addWidget( btn1, i, COL_FILE );
    btn1->setToolTip( b_version_ini_file ? "Version config file" : "Create config file for this version" );

    // 2)installation folder
    QPushButton_Custom* btn2 = new QPushButton_Custom( QPushButton_Custom::Installation_Folder, i, this );
    set_btn_img( btn2, FOLDER_IMG_URL, border );
    m_grid_layout.addWidget( btn2, i, COL_FOLDER );
    btn2->setToolTip("Version installation folder");

    // 3)remove version
    // default version can't be removed
    QPushButton_Custom* btn3 = nullptr;
    if( !path.startsWith( DEFAULT_INST_START ) )
    {
      btn3 = new QPushButton_Custom( QPushButton_Custom::Remove_Version, i, this );
      set_btn_img( btn3, REMOVE_IMG_URL, border );
      m_grid_layout.addWidget( btn3, i, COL_REMOVE );
      btn3->setToolTip("Remove version");
    }

    // 4)version log file
    QPushButton_Custom* btn4 = nullptr;
    if( b_version_logs )
    {
      btn4 = new QPushButton_Custom( QPushButton_Custom::Version_Log_File, i, this );
      set_btn_img( btn4, LOGS_IMG_URL, border );
      m_grid_layout.addWidget( btn4, i, COL_LOGS );
      btn4->setToolTip( version_logs_file( version ) );
    }

    connect( btn1, SIGNAL( clicked() ), this, SLOT( on_version_ini_file() ) );

    connect( btn2, SIGNAL( clicked() ), this, SLOT( on_version_folder() ) );

    if( !path.startsWith( DEFAULT_INST_START ) )
      connect( btn3, SIGNAL( clicked() ), this, SLOT( on_remove() ) );

    if( b_version_logs )
      connect( btn4, SIGNAL( clicked() ), this, SLOT( on_version_logs() ) );

    if( i == m_versions_v.size()-1 )
      rb->setChecked( true );
  }

  // checkbox row -----------------------------------
  int row = (int)m_versions_v.size();
  m_cb.setText( TEXT_CB );
  m_grid_layout.addWidget( &m_cb, row, COL_RB );

  // general ini file
  set_btn_img( &m_btn_ini, CONFIG_IMG_URL, false );
  m_grid_layout.addWidget( &m_btn_ini, row, COL_FILE );

  // logs folder

  set_btn_img( &m_btn_logs_config, CONFIG_IMG_URL, false );
  m_grid_layout.addWidget( &m_btn_logs_config, row, COL_LOGS );
  if( !url_exists( LOGS_CONFIG_URL ) )
    m_btn_logs_config.setStyleSheet("background-color: rgba(255, 0, 0, 0.2); border: none"); // border: none -> needed, otherwise a padding around is added

  // open btn row --------------------------------
  row++;
  m_btn_open.setText( TEXT_BTN );
  m_grid_layout.addWidget( &m_btn_open, row, COL_RB );

  // logs config
  set_btn_img( &m_btn_logs_folder, FOLDER_IMG_URL, false );
  m_grid_layout.addWidget( &m_btn_logs_folder, row, COL_LOGS );

  // icons row ----------------------------------
  row++;

  // help
  set_btn_img( &m_btn_pdf, PDF_IMG_URL, false );
  m_h_layout_btns_left.addWidget( &m_btn_pdf );
  // xml file
  set_btn_img( &m_btn_config, CONFIG_IMG_URL, false );
  m_h_layout_btns_left.addWidget( &m_btn_config );
  // logs file
  set_btn_img( &m_btn_logs, LOGS_IMG_URL, false );
  m_h_layout_btns_left.addWidget( &m_btn_logs );
  // python config file
  set_btn_img( &m_btn_python, PYTHON_IMG_URL, false );
  m_h_layout_btns_left.addWidget( &m_btn_python );

  m_h_layout_btns_left.addStretch();

  m_h_layout_btns.addLayout( &m_h_layout_btns_left );


  // more
  set_btn_img( &m_btn_more, MORE_IMG_URL, false );
  m_h_layout_btns.addWidget( &m_btn_more, 0, Qt::AlignRight );

  

  m_grid_layout.addLayout( &m_h_layout_btns, row, COL_RB );



  // less
  set_btn_img( &m_btn_less, LESS_IMG_URL, false );
  m_grid_layout.addWidget( &m_btn_less, row, COLS_NUM-1 );
  

  // https://stackoverflow.com/questions/28021656/qt-margin-spacing-between-widgets
  //m_grid_layout.setMargin( 0 ); // this works but does too much
  m_grid_layout.setVerticalSpacing( 5 ); // this also works

  

  connect( &m_btn_open, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( &m_btn_more, SIGNAL( clicked() ), this, SLOT( on_more() ) );
  connect( &m_btn_less, SIGNAL( clicked() ), this, SLOT( on_less() ) );
  connect( &m_btn_pdf,  SIGNAL( clicked() ), this, SLOT( on_pdf() ) );
  connect( &m_btn_config,  SIGNAL( clicked() ), this, SLOT( on_config() ) );
  connect( &m_btn_logs,  SIGNAL( clicked() ), this, SLOT( on_logs() ) );
  connect( &m_btn_ini,  SIGNAL( clicked() ), this, SLOT( on_ini_file() ) );
  connect( &m_btn_logs_folder,  SIGNAL( clicked() ), this, SLOT( on_logs_folder() ) );
  connect( &m_btn_logs_config,  SIGNAL( clicked() ), this, SLOT( on_logs_config() ) );
  connect( &m_btn_python,  SIGNAL( clicked() ), this, SLOT( on_python() ) );


  // on mouse over help
  m_btn_open.setToolTip("Launch selected version");
  m_btn_less.setToolTip("Show less info");
  m_btn_more.setToolTip("Show more info");
  m_btn_pdf.setToolTip("Geomec selector guide.pdf");
  m_btn_config.setToolTip("Open selector xml config file");
  m_btn_logs.setToolTip("Show logs");
  m_btn_ini.setToolTip("Default ini file");
  m_btn_logs_folder.setToolTip("Logs and config files location");
  m_btn_logs_config.setToolTip("Logs config");
  m_btn_python.setToolTip("Python config file");
  m_cb.setToolTip("Make the selected version the default one");


  on_less();
}

void CSelectorDlg::on_open( const QString& url )
{
  if( url_exists( LOGS_CONFIG_URL ) )
    QDesktopServices::openUrl( QUrl::fromLocalFile( url ) );
  else
  {
    QMessageBox::warning( 
      nullptr, 
      "Open resource", 
      "Resource not found !!!" );
  }
}

void CSelectorDlg::on_python(){			on_open( PYTHON_CONFIG_URL ); }
void CSelectorDlg::on_logs_folder(){	on_open( CONFIG_BASE_PATH ); }
void CSelectorDlg::on_ini_file(){		on_open( INI_FILE_URL ); }
void CSelectorDlg::on_logs(){			on_open( LOGS_URL ); }
void CSelectorDlg::on_config(){			on_open( SELECTOR_CONFIG_URL ); }
void CSelectorDlg::on_pdf(){			on_open( PDF_URL ); }

void CSelectorDlg::on_logs_config()
{
  if( url_exists( LOGS_CONFIG_URL ) )
    on_open( LOGS_CONFIG_URL ); 
  else
  {
    QMessageBox::StandardButton res = QMessageBox::critical( 
      nullptr, 
      "Create file", 
      "Create logs config file?",  
      QMessageBox::Yes | QMessageBox::No );

    if( res == QMessageBox::No )
      return;
    else
    {
      bool res_ = QFile::copy( 
        QUtil::url( PACKAGES_PATH,"logs.xml"),
        LOGS_CONFIG_URL );

      QFile::setPermissions( 
        LOGS_CONFIG_URL, 
        QFile::ReadOther | QFile::WriteOther);

      if( res_ )
      {
        m_btn_logs_config.setStyleSheet(
          "background-color: rgba(255, 255, 255, 0); border: none"); 
        // border: none -> needed, otherwise a padding around is added
      }
      else
      {
        QMessageBox::warning( 
          nullptr, 
          "Create logs config file", 
          "File not created<br>Something went wrong!" );
      }
    }
  }
}

void CSelectorDlg::on_version_logs()
{
  QPushButton_Custom* btn = qobject_cast<QPushButton_Custom*>( sender() );
  int id = btn->id();
  QDesktopServices::openUrl( QUrl::fromLocalFile( 
    version_logs_url( QString( m_versions_v[ id ] ) ) ) );
}


//version config file
void CSelectorDlg::on_version_ini_file()
{
  QPushButton_Custom* btn = qobject_cast<QPushButton_Custom*>( sender() );
  int id = btn->id();

  QString file_url = ini_file( m_versions_v[ id ] );
  // show it
  if( QFile::exists( ini_file( m_versions_v[ id ] ) ) ) 
    QDesktopServices::openUrl( QUrl::fromLocalFile( file_url ) );
  // create it
  else
  {
    QFile::copy( 
      // general config file
      INI_FILE_URL,
      // blank version config file from packages folder
      //QUtil::url( PACKAGES_PATH, "geomec.ini" ), 
      file_url );

    QFile::setPermissions( 
      file_url, 
      QFile::ReadOther | QFile::WriteOther);

    if( QFile::exists( file_url ) ) 
    {
      delete btn;

      btn = new QPushButton_Custom( QPushButton_Custom::Version_Config_File, id, this, true );
      set_btn_img( btn, CONFIG_IMG_URL, false );
      m_grid_layout.addWidget( btn, id, COL_FILE );
      btn->setToolTip( "Version config file" );
      connect( btn, SIGNAL( clicked() ), this, SLOT( on_version_ini_file() ) );
    }
    else
    {
      QMessageBox::warning( 
        nullptr, 
        "Create version config file", 
        "Config file not created<br>Something went wrong!" );
    }
  }
}

void CSelectorDlg::on_version_folder()
{
  QPushButton_Custom* btn = qobject_cast<QPushButton_Custom*>( sender() );
  int id = btn->id();

  QString path;
  QString bin = "explorer.exe";
  QString params = QUtil::url( m_paths_v[ id ], m_versions_v[ id ] );
  TMap envvars_m = {};

  app::launch( path, bin,	params,	envvars_m );
}

void CSelectorDlg::on_remove()
{
  QPushButton_Custom* btn = qobject_cast<QPushButton_Custom*>( sender() );
  int id = btn->id();
  delete__( id );
}

// FIXME: refer btns other way, not by index
void CSelectorDlg::on_more()
{
  for( int i = 0; i < m_versions_v.size(); i++ )
  {
    for( int j = COL_PATH; j < COLS_NUM ; j++ )
    {
      // version config file not always present
      QLayoutItem* item = m_grid_layout.itemAtPosition( i, j );
      if( item )
        item->widget()->show();
    }
  }

  m_btn_less.show();
  m_btn_more.hide();

  m_btn_ini.show();
  m_btn_logs_folder.show();
  m_btn_logs_config.show();

  QApplication::processEvents();
  adjustSize();
}

void CSelectorDlg::on_less()
{
  for( int i = 0; i < m_versions_v.size(); i++ )
  {
    for( int j = COL_PATH; j < COLS_NUM ; j++ )
    {
      // version config file not always present
      QLayoutItem* item = m_grid_layout.itemAtPosition( i, j );
      if( item )
        item->widget()->hide();
    }
  }

  m_btn_less.hide();
  m_btn_more.show();

  m_btn_ini.hide();
  m_btn_logs_folder.hide();
  m_btn_logs_config.hide();

  // it works!
  QApplication::processEvents();
  adjustSize();
}

bool CSelectorDlg::check()
{
  return m_cb.isChecked();
}

int CSelectorDlg::id()
{
  return m_rb_group.checkedId();
}

bool CSelectorDlg::delete_config_file( int id )
{
  QString file_url = ini_file( m_versions_v[ id ] );
  QFile file( file_url );
  QFileInfo fileInfo( file.fileName() );

  QString msg = "Sure to remove version config file?<br><br>-File: " + fileInfo.fileName();

  QMessageBox::StandardButton res = QMessageBox::critical( 
    nullptr, 
    "Geomec versions", 
    msg,  
    QMessageBox::Yes | QMessageBox::No );

  if( res == QMessageBox::No )
    return false;

  bool res_ = file.remove();

  if( res_ )
  {
    QPushButton_Custom* btn = qobject_cast< QPushButton_Custom* >( 
      m_grid_layout.itemAtPosition( id, COL_FILE )->widget() );

    delete btn;

    btn = new QPushButton_Custom( QPushButton_Custom::Version_Config_File, id, this, false );
    set_btn_img( btn, ADD_IMG_URL , false );
    m_grid_layout.addWidget( btn, id, COL_FILE );
    btn->setToolTip( "Create config file for this version" );
    connect( btn, SIGNAL( clicked() ), this, SLOT( on_version_ini_file() ) );
  }
  else
  {
    QMessageBox::warning( 
      nullptr, 
      "Delete version config file", 
      "Config file not deleted<br>Something went wrong!" );
  }
  return res_;
}

// deletes also dir and files
// add maybe a checkbox to double check?
bool CSelectorDlg::delete_( QRadioButton_Custom* rb )
{
  int id = m_rb_group.id( rb );
  return delete__( id );
}

// 4x: 
// -delete folder
// -remove radio button
// -update vectors
// -update selector.xml location key
bool CSelectorDlg::delete__( int id )
{
  if( !m_b_delete )
  {
    QMessageBox::warning( 
      nullptr, 
      "Remove version", 
      "Deletion is disable by default<br>Enable it first in the config file" );
    return false;
  }

  const QString& path = m_paths_v[ id ];
  const QString& version = m_versions_v[ id ];

  QString msg = "Sure to <b>completely remove</b>?<br><br>-Version: " + version + "<br>-Path: " + path;

  QMessageBox::StandardButton res = QMessageBox::critical( 
    nullptr, 
    "Geomec versions", 
    msg,  
    QMessageBox::Yes | QMessageBox::No );

  if( res == QMessageBox::No )
    return false;

  QAbstractButton* rb = m_rb_group.button( id );
  m_rb_group.removeButton( rb );
  delete rb;

  for( int j = COL_PATH; j < COLS_NUM; j++ )
  {
      QLayoutItem* item = m_grid_layout.itemAtPosition( id, j );
      if( item && item->widget() )
        delete item->widget();
  }

  if( version == version::read() ) // default version
    version::unset();


  // delete files
  // FIXME: version folder not deleted, why?????
  // path not updated is selector.xml -> this can produce a crash
  bool res1 = QDir( QUtil::url( path, version ) ).removeRecursively();
  // it seems to me that folder is not removed in the operation before
  // but maybe not, the folder is not removed sometimes because folder is open in the explorer?
  //bool res2 = QDir( path ).rmdir( version ); 

  m_versions_v.erase( m_versions_v.begin() + id);
  m_paths_v.erase( m_paths_v.begin() + id);

  app::paths::tidy(); //remove locations with no installations

  QMessageBox::warning( 
    nullptr, 
    "Remove version", 
    "Result: Ok!<br><br>-- Version deleted --<br>-version: '" + version + "'<br>-path: '" + path + "'" );
  
  return res1;
  //return res1 && res2;
}

bool CSelectorDlg::set_as_default( QRadioButton_Custom* rb )
{
  bool res1 = false;

  int id = m_rb_group.id( rb );

  const QString& path = m_paths_v[ id ];
  const QString& version = m_versions_v[ id ];

  version::set( version );

  update_style();

  /*QMessageBox::warning( 
    nullptr, 
    QString("Version default"), 
    QString("Result: Ok!<br><br>-- Default version set --<br>-version: '" + version + "'<br>-path: '" + path + "'" ) );*/

  return true;
}

//////////////////////////////////////////////////////////////////////////
//
//                      QPushButton_Custom
//
//////////////////////////////////////////////////////////////////////////

void QPushButton_Custom::on_custom_context_menu( const QPoint &point )
{
  m_menu->exec( mapToGlobal( point ) ); 
}
void QPushButton_Custom::on_delete()
{
  ( (CSelectorDlg*)parentWidget() )->delete_config_file( m_id );
}

QPushButton_Custom::~QPushButton_Custom()
{
  if( m_menu )
  {	
    for( auto& action : m_menu->actions() )
      delete action;
    delete m_menu;
  }
}

QPushButton_Custom::QPushButton_Custom( 
  eType_ type_,
  const int id, 
  QWidget* parent,
  bool has_context_menu )
  :	m_type( type_ ),
    m_id( id ),
    m_has_context_menu( has_context_menu ),
    m_menu( nullptr )
{
  if( has_context_menu )
  {
    m_menu = new QMenu(); 

    setContextMenuPolicy( Qt::CustomContextMenu );

    connect(	this,	SIGNAL( customContextMenuRequested( const QPoint & ) ), 
          this,	SLOT(	on_custom_context_menu( const QPoint &) ) );

    if( type_ == Version_Config_File )
    {
      QAction* action = new QAction( "Delete", this );
      m_menu->addAction( action );

      connect(	action,	SIGNAL( triggered() ),	
            this,	SLOT(	on_delete() ) );
    }
  }
}


//////////////////////////////////////////////////////////////////////////
//
//                      QRadioButton_Custom
//
//////////////////////////////////////////////////////////////////////////

void QRadioButton_Custom::on_custom_context_menu( const QPoint &point )
{ 
  m_menu->exec( mapToGlobal( point ) ); 
}

void QRadioButton_Custom::on_delete()
{
  ( (CSelectorDlg*)parentWidget() )->delete_( this );
}

void QRadioButton_Custom::on_set_as_default()
{
  ( (CSelectorDlg*)parentWidget() )->set_as_default( this );
}

QRadioButton_Custom::QRadioButton_Custom( 
  const QString& caption, 
  QWidget* parent ) 
  : QRadioButton( caption, parent )
{
  m_menu = new QMenu(); 

  QAction* action = new QAction( "Delete", this );
  m_menu->addAction( action );

  QAction* action2 = new QAction( "Set as default", this );
  m_menu->addAction( action2 );

  setContextMenuPolicy( Qt::CustomContextMenu );

  connect(	this,	SIGNAL( customContextMenuRequested( const QPoint & ) ), 
        this,	SLOT(	on_custom_context_menu( const QPoint &) ) );

  connect(	action,	SIGNAL( triggered() ),	
        this,	SLOT(	on_delete() ) );

  connect(	action2,SIGNAL( triggered() ),	
        this,	SLOT(	on_set_as_default() ) );
};

QRadioButton_Custom::~QRadioButton_Custom()
{
  for( auto& action : m_menu->actions() )
    delete action;
  delete m_menu;
}