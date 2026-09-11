#include "stdafx.h"

#include "mytreeviewdlg.h"

#include <QtWidgets\QPushButton>
#include <QtWidgets\QLabel>
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QHBoxLayout>
#include <QtWidgets\QCheckBox>

#include "MyDlg.h"

#include "GeomecDoc.h"

namespace
{
  static const QString TEXT_OLD_MESHER_DLG_TITLE	= QString("Models using the old FGV mesher found"); 

  static const QString TEXT_OLD_MESHER_DLG_TEXT	= QString(	"<p>Upgrading to the new CM2 mesher can be performed now.</p> \
                                <p>If needed, the model can be also converted from hexa to tetra.</p> \
                                <p>This can be handled now or <font color='red'>later on</font></p> \
                                <p style='text-align:center'><b>Handle this now?</p> \
                                <p></p>"); 
                                //<p style='text-align:center'><b>Handle this now?</b><a href='http://google.com/'>(Help)</a></p>"); 

  static const QString TEXT_OLD_MESHER_DLG_BTN_YES	= QString("Handle now"); 
  static const QString TEXT_OLD_MESHER_DLG_BTN_NO		= QString("Skip"); 
}

void MyDlg::yes()
{
  MyTreeViewDlg::instance()->show();
  close();
}

void MyDlg::no(){ close(); }


void MyDlg::keyPressEvent( QKeyEvent* e )
{
  if( e->type() == QEvent::KeyPress && e->key() == Qt::Key_F1 )
  {
    AfxGetApp()->GetMainWnd()->PostMessage( WM_COMMAND, WM_USER + 200, 0 );
  }
  else
  {
    QDialog::keyPressEvent( e );
  }
}

void MyDlg::cb_( int state )
{ 
  int a = 0; 

  CModelBase* model = dynamic_cast< CModelBase* >( GetGeomecDoc()->Model() );
  model->m_show_cm2_dlg = ( Qt::CheckState)state == Qt::Unchecked;
}

MyDlg::MyDlg()
{
  lb1	= new QLabel();
  b2 = new QPushButton();
  b3 = new QPushButton();

  cb = new QCheckBox();
  cb->setText("Don't show the dialog again for this model");

  lv = new QVBoxLayout();
  lh = new QHBoxLayout();

  lv->addWidget( lb1 );
  lv->addLayout( lh );
  lv->addWidget( cb );

  lh->addStretch();
  lh->addWidget( b2 );
  lh->addWidget( b3 );

  setLayout( lv );

  lb1->setText(	TEXT_OLD_MESHER_DLG_TEXT );

  b2->setText(	TEXT_OLD_MESHER_DLG_BTN_YES );
  b3->setText(	TEXT_OLD_MESHER_DLG_BTN_NO );

  connect( b2, SIGNAL( clicked() ), this, SLOT( yes() ) );
  connect( b3, SIGNAL( clicked() ), this, SLOT( no() ) );

  connect( cb, SIGNAL( stateChanged( int ) ), this, SLOT( cb_( int ) ) );
}

MyDlg::~MyDlg()
{
  //FIXME: At the end
  QObject* objs[] = { lb1, b2, b3, lh, lv, (QObject*)(-1) };
  int i=0;
  while( objs[ i ] != (QObject*)(-1) )
    delete objs[ i++ ];
}
