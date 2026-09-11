#include "stdafx.h"

#include <QtWidgets\QPushButton>
#include <QtWidgets\QLabel>
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QHBoxLayout>
#include <QProcess>
#include <QEvent>
#include <QKeyEvent>

#include "MeshConverterDlg.h"

#include "meshconvertertreeview.h"

namespace
{
  static const QString TEXT	= QString(	"<p> \
                        Click right button to: <br><br> \
                        - Swap between old and new mesher <br> \
                        - Do it for one model or for all <br> \
                        - Convert a model from hexa to tetra \
                      </p> \
                      <p> \
                        <font color='red'>\
                          Performing these actions involves \
                        </font>\
                      </p> \
                      <p> \
                        <font color='red'>\
                          loosing mesh and results!!!\
                        </font>\
                      </p>"); 
}

MeshConverterDlg *MeshConverterDlg::instance()
{
  static MeshConverterDlg _instance;
  _instance.update();
  return &_instance;
}

void MeshConverterDlg::update(){ m_tree->update(); }

void MeshConverterDlg::help(){ AfxGetApp()->GetMainWnd()->PostMessage( WM_COMMAND, WM_USER + 200, 0 ); }

MeshConverterDlg::MeshConverterDlg()
{
  b1 = new QPushButton();

  m_tree = new MeshConverterTreeView();

  lv = new QVBoxLayout();
  lh = new QHBoxLayout();

  lv->addWidget( m_tree );
  lv->addLayout( lh );

  lh->addStretch();
  lh->addWidget( b1 );

  setLayout( lv );

  b1->setText( "Help" );

  m_tree->setWhatsThis( TEXT );

  connect( b1, SIGNAL( clicked() ), this, SLOT( help() ) );

  //b1->setWhatsThis( "Go to general help" );

  //QPixmap image("img/help_icon_256.jpg");

  /*b1->setIcon( QIcon( "img/help_icon.png" ) );
  b1->setIconSize( QSize( 100, 100 ) );
  b1->setFixedSize( QSize( 100, 100 ) );*/

  /*connect( b1, &QPushButton::clicked,	this, [](){ 
    AfxGetApp()->GetMainWnd()->PostMessage( WM_COMMAND, ID_HELP_FINDER, 0 ); 
  });*/

  /*
  //https://www.walletfox.com/course/qhelpengineexample.php
  connect( b1, &QPushButton::clicked,	this, [](){ 

    QProcess *process = new QProcess;
    
    QStringList args;
    args	<< QLatin1String("-collectionFile")
        << QLatin1String("docs/wateringmachine.qhc")
        //<< QLatin1String("E:/dev/Geomec_20170920_2/geomec3/docs/wateringmachine.qhc")
        << QLatin1String("-enableRemoteControl");

    //args	<< QLatin1String("-enableRemoteControl");

    process->start(QLatin1String("assistant"), args);
    
    if (!process->waitForStarted())
      return;
  });*/

  //setWindowFlags( windowFlags() |= Qt::WindowStaysOnTopHint );
}


MeshConverterDlg::~MeshConverterDlg()
{
  QObject* objs[] = { m_tree, b1, lh, lv, (QObject*)(-1) };
  int i=0;
  while( objs[ i ] != (QObject*)(-1) )
    delete objs[ i++ ];
}

void MeshConverterDlg::keyPressEvent(QKeyEvent *e) 
{
  if(e->key() != Qt::Key_Escape)
    QDialog::keyPressEvent(e);
  else {}
}
