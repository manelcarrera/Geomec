#include "stdafx.h"

#include <QtWidgets\QAction>
#include <QtWidgets\QMessageBox>
#include <QtGui\QStandardItemModel>
#include <QtGui\QStandardItem>
#include <QAbstractItemView>

#include "meshconvertertreeview.h"

#include "WellZoomInModel.h"
#include "ZoominModelEntry.h"
#include "NewWellPathInput.h"
#include "WellPath.h"
#include "NewWellPath.h"
#include "ZoominModelPlaceHolder.h"

#include "WellCasingModel.h"

#include "GeomecDoc.h"
#include "TetraMesh.h"
#include "TetraModel.h"

#include "modelbase.h"

#include <QtWidgets\QHeaderView>
#include <QtWidgets\QApplication>
#include <QtWidgets\QSizePolicy>
#include <QtCore/QTimer>
#include <QtCore/QFile>


#include "HexaGUI.h"

#include "Printer.h"
#include "util_.h"


namespace
{
  enum eModelType{	Tetra_M, Hexa_M, Unknown_M	};
};

class QStandardItem_Custom : public QStandardItem
{
public:
  QStandardItem_Custom( const QString &text, CTetraMesh* mesh = nullptr ) : QStandardItem( text ), m_mesh( mesh ), m_model_type( Unknown_M ){};
  QStandardItem_Custom( const QIcon& icon, const QString &text, CTetraMesh* mesh = nullptr ) : QStandardItem( icon, text ), m_mesh( mesh ), m_model_type( Unknown_M ){};
  QStandardItem_Custom( const QIcon& icon, const QString &text, eModelType model_type ) : QStandardItem( icon, text ), m_mesh( nullptr ), m_model_type( model_type ){};

  /*void QStandardItem_Custom::keyPressEvent( QKeyEvent* e )
{
    if( e->type() == QEvent::KeyPress && e->key() == Qt::Key_F1 )
    {
      AfxGetApp()->GetMainWnd()->PostMessage( WM_COMMAND, ID_HELP_FINDER, 0 );
    }
    //else
    //{
    //	QStandardItem::keyPressEvent( e );
    //}
  }*/

  CTetraMesh* m_mesh;
  eModelType m_model_type;
};

namespace
{
  enum eMesherType{	Old, New, Unknown	};
  enum eImgs{			Tetra, Hexa, Old_, New_, Unknown_ };

  const char* g_mesher_type_s[] = {"Old","New","Unknown"};

  eMesherType mesher_type( CModelBase* model )
  {
    eMesherType type_ = Unknown;

    if( model )
    {
      CTetraMesh* mesh = dynamic_cast< CTetraMesh*>( &model->Mesh() );

      type_ = mesh ? 
            mesh->CurrentMesherType() == 0 ? Old : New 
            : Unknown;
    }

    return type_;
  }

  eMesherType mesher_type( QStandardItem_Custom* item )
  {

    CTetraMesh* mesh = item->m_mesh;
    eMesherType type_ = mesh ? 
            mesh->CurrentMesherType() == 0 ? Old : New 
            : Unknown;
    return type_;
  }

  // If mesh is not tetra then mesh is NULL
  eMesherType mesher_type( CTetraMesh* mesh )
  {
    eMesherType type_ = mesh ? 
            mesh->CurrentMesherType() == 0 ? Old : New 
            : Unknown;
    return type_;
  }

  bool is_mesher_type( CTetraMesh* mesh, eMesherType _type )
  {
    return mesher_type( mesh ) == _type;
  }

  bool switch_mesher( QStandardItem_Custom* item )
  {
    eMesherType type_ = mesher_type( item );
    if ( type_ != Unknown )
    {
      item->m_mesh->SwitchMesher((CTetraMesh::eMesherType)( type_ == Old ? New : Old ));
      return true;
    }
    return false;
  }

  bool any_mesher( std::vector< QStandardItem_Custom* >& items, eMesherType _type )
  {
    bool res = false;

    for( int i=0; i< items.size(); i++ )
    {
      QStandardItem_Custom* item = items[ i ];
      CTetraMesh* mesh = item->m_mesh;
      if( mesh ) // -> Tetra
      {
        eMesherType type_ = mesher_type( mesh );

        if( type_ == _type )
        {
          res = true;
          break;
        }
      }
    }
    return res;
  }

  // TODO: '.jpg': doesn't work / '.png': yes, it works 	

  CFemAppModel::ModelType modelType( CModelBase* model )
  {
    C3DModel* _model = static_cast< C3DModel* >( model );
    return _model->modelType();
  }

  eModelType model_type( CModelBase* model )
  {
    return dynamic_cast< CTetraModel* >( model ) == nullptr ? Hexa_M : Tetra_M;

    /*CTetraModel* m_t = dynamic_cast< CTetraModel* >( model );
    if( m_t )
      return Tetra_M;
    else
    {
      CHexaModel* m_h = dynamic_cast< CHexaModel* >( model );
      if( m_h )
        return Hexa_M;
      else
        return Unknown_M;
    }*/
  }

}

// Attach custom object to QStandardItem in Qt
// https://stackoverflow.com/questions/23619124/attach-custom-object-to-qstandarditem-in-qt


MeshConverterTreeView::MeshConverterTreeView(QWidget *parent)	: QTreeView(parent)
{
  init();
  update();
}

void MeshConverterTreeView::init()
{
  setEditTriggers( QAbstractItemView::NoEditTriggers );

  setContextMenuPolicy(Qt::CustomContextMenu); // Just for given items
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  setSizeAdjustPolicy( QAbstractScrollArea::AdjustToContents ); // MCR: This works but it only resizes vertically
  setHeaderHidden( true );
  setWindowTitle( QObject::tr("Mesh converter") );

  resize( 250, 250 /*500*/ );

  connect(	this, SIGNAL( customContextMenuRequested( const QPoint & ) ), this, SLOT( on_custom_context_menu( const QPoint &) ) );

  setModel( &m_model );

  // Icons
  /*bool res = QFile::exists( ICON_TETRA );
  QString s = QString::asprintf("%s/%s", util::GetAppPath().c_str(), ICON_TETRA.toStdString().c_str() );
  bool res2 = QFile::exists( s );
  Printer::instance()->debug("ICON_TETRA: '%s' exist: %d / app_path: %s res: %d", ICON_TETRA.toStdString().c_str(), res, s.toStdString().c_str(), res2 );*/
  
  const QString IMG_BASE_PATH	= QString::fromStdString( util::GetAppPath() ) + QString("/img/");
  const QString ICON_HEXA		= IMG_BASE_PATH + QString("hexa.png");
  const QString ICON_TETRA	= IMG_BASE_PATH + QString("tetra.png");
  const QString ICON_OLD		= IMG_BASE_PATH + QString("old.png");
  const QString ICON_NEW		= IMG_BASE_PATH + QString("new.png");
  const QString ICON_UNKNOWN	= IMG_BASE_PATH + QString("undefined.png");

  /*bool res = QFile::exists( ICON_TETRA );
  Printer::instance()->debug("ICON_TETRA: '%s' exist: %d", ICON_TETRA.toStdString().c_str(), res );*/

  m_icons[ Tetra ]	= QIcon( ICON_TETRA );
  m_icons[ Hexa ]		= QIcon( ICON_HEXA );
  m_icons[ Old_ ]		= QIcon( ICON_OLD );
  m_icons[ New_ ]		= QIcon( ICON_NEW );
  m_icons[ Unknown_ ] = QIcon( ICON_UNKNOWN );

  m_actions[ Swap ]		= new QAction("Swap mesher", this);
  m_actions[ To_Tetra ]	= new QAction("Convert to tetra", this);
  m_actions[ All_To_New ] = new QAction("Swap all meshers to NEW", this);
  m_actions[ All_To_Old ] = new QAction("Swap all meshers to OLD", this);

  m_menus[ Root ]		= new QMenu(); 
  m_menus[ Mesher ]	= new QMenu(); 
  m_menus[ Model ]	= new QMenu(); 

  m_menus[ Mesher ]->addAction(	m_actions[ Swap ] );
  m_menus[ Root ]->addAction(		m_actions[ All_To_New ] );
  m_menus[ Root ]->addAction(		m_actions[ All_To_Old ] );
  m_menus[ Model ]->addAction(	m_actions[ To_Tetra ] );

  //MCR: C++11, Linux?
  for( int i=0; i < Num_Actions; i++ )
    connect( m_actions[ (eActions)i ], &QAction::triggered,	this, [ this, i ]{ action( (eActions)i ); });
}
/*void MeshConverterTreeView::keyPressEvent( QKeyEvent* e )
{
  if( e->type() == QEvent::KeyPress && e->key() == Qt::Key_F1 )
  {
    AfxGetApp()->GetMainWnd()->PostMessage( WM_COMMAND, ID_HELP_FINDER, 0 );
  }
  else
  {
    QTreeView::keyPressEvent( e );
  }
}*/


void MeshConverterTreeView::clean()
{
  m_model.clear();
  m_items.clear();
}

void MeshConverterTreeView::update()
{
  clean();

  CModelBase* model_b = dynamic_cast< CModelBase* >( GetGeomecDoc()->Model() );
  if( model_b )
  {
    QStandardItem* parent_m = m_model.invisibleRootItem();

    add_models( model_b, parent_m );
  }

  expandAll();

  QApplication::processEvents();
  adjustSize();
}

void MeshConverterTreeView::on_custom_context_menu( const QPoint &point )
{
  eMenus menu_ = menu( point );
  if( menu_ != None )
    m_menus[ menu_ ]->exec( mapToGlobal( point ) );
}

MeshConverterTreeView::eMenus MeshConverterTreeView::menu( const QPoint &point )
{
  QModelIndex index = indexAt(point);
  if( index.isValid() ) 
  {
    QStandardItem* item_ = ( (QStandardItemModel*)model() )->itemFromIndex( index );
    QStandardItem_Custom* item = dynamic_cast<QStandardItem_Custom* >( item_ );

    if( item ) //QStandardItem_Custom
    {
      if( item->m_mesh )
        return Mesher;
      else
      {
        int row = item->row();

        bool is_root_model = !item->parent() && row == 0;
        if( is_root_model )
          return Root;
        else if( item->m_model_type != CFemAppModel::eUndefinedModel )
        {
          CHexaGUI* pHexaGUI = dynamic_cast< CHexaGUI*>( GetGeomecDoc()->GUI() );

          if( pHexaGUI && pHexaGUI->CanConvertToTetra() )
            return Model;
        }
      }
    }
  }
  return None;
}



// NOT USED
void MeshConverterTreeView::resize_wnd()
{
  QApplication::processEvents();
  adjustSize();
}

MeshConverterTreeView::~MeshConverterTreeView()
{
  std::map< int, QAction* >::iterator it;
  for ( it = m_actions.begin(); it != m_actions.end(); it++ )
    if( it->second )
      delete it->second;

  std::map< int, QMenu* >::iterator it2;
  for ( it2 = m_menus.begin(); it2 != m_menus.end(); it2++ )
    if( it2->second )
      delete it2->second;

  // delete all the tree items ??? -> clear does this for me ? 
}

void MeshConverterTreeView::action( eActions action_ )
{
  const QString TEXT_CONFIRM_TITLE	= QString("Critical action"); 

  const QString TEXT_CONFIRM_TEXT		= QString(	"<p> \
                            By performing this action, the current mesh and results <br> \
                            will be <font color='red'><b>deleted</b></font> \
                          </p> \
                          <p style='text-align:center'> \
                            Perform action? \
                          </p>"); 

  QMessageBox::StandardButton res = QMessageBox::critical( nullptr, TEXT_CONFIRM_TITLE, TEXT_CONFIRM_TEXT,  QMessageBox::Yes | QMessageBox::No );
  if( res == QMessageBox::Yes )
  {
    switch( action_ )
    {
      case Swap:			swap_mesher();		break;
      case All_To_New:	all_to_new();		break;
      case All_To_Old:	all_to_old();		break;
      case To_Tetra:		convert_to_tetra(); break;

      default:								break;
    }
  }
}

void MeshConverterTreeView::swap_mesher()
{
  QModelIndex idx = currentIndex();

  int row = idx.row();
  int col = idx.column();
  
  //https://stackoverflow.com/questions/26052818/get-qstandarditem-for-the-custom-context-menu-of-a-qtreeview

  QStandardItem* item_ = ( (QStandardItemModel*)model() )->itemFromIndex( idx );

  QStandardItem_Custom* item = dynamic_cast<QStandardItem_Custom* >( item_ );


  QString name = item->text();

  std::string name_s = name.toStdString();

  //QVariant data = item->data();

  //itemFromIndex( idx );


  eMesherType type_ = mesher_type( item );

  bool res = switch_mesher( item );
  if( res )
  {
    /*QMessageBox::information(this, "Info", 
      QString::asprintf("Mesher swapwed from '%s' to '%s'", 
        type_ == New ? g_mesher_type_s[ New ] : g_mesher_type_s[ Old ], 
        type_ == New ? g_mesher_type_s[ Old ] : g_mesher_type_s[ New ] ) );*/

    type_ = mesher_type( item );

    QString mesher_name = QString("Mesher: ") + QString( g_mesher_type_s[ type_ ] );
    item->setText( mesher_name );

    item->setIcon( m_icons[ type_ == New ? New_ : Old_ ] ); //FIXME
  }
  else
    QMessageBox::information(this, "Info", "Mesher type not swapped" );

  m_actions[ All_To_New ]->setEnabled( any_mesher( m_items, Old ) );
  m_actions[ All_To_Old ]->setEnabled( any_mesher( m_items, New ) );

}

void MeshConverterTreeView::convert_to_tetra()
{
  CHexaGUI* pHexaGUI = dynamic_cast <CHexaGUI*>( GetGeomecDoc()->GUI() );
  pHexaGUI->ConvertToTetraModel();
  update();
}

void MeshConverterTreeView::all_to( int type_ )
{
  for( int i=0; i< m_items.size(); i++ )
  {
    QStandardItem_Custom* item = m_items[ i ];
    CTetraMesh* mesh = item->m_mesh;
    if( mesh ) // -> Tetra
    {
      eMesherType _type = (eMesherType)type_;

      if( is_mesher_type( mesh, _type ) )
      {
      }
      else
      {
        mesh->SwitchMesher( (CTetraMesh::eMesherType)_type );
        item->setIcon( m_icons[ _type == New ? New_ : Old_ ] );
        QString mesher_name = QString("Mesher: ") + QString( g_mesher_type_s[ type_ ] );
        item->setText( mesher_name );
      }
    }
  }

  m_actions[ All_To_New ]->setEnabled( any_mesher( m_items, Old ) );
  m_actions[ All_To_Old ]->setEnabled( any_mesher( m_items, New ) );

  //QMessageBox::information(this, "Swap all meshers", "All meshers swapped to NEW");
}

void MeshConverterTreeView::all_to_new(){ all_to( New ); }
void MeshConverterTreeView::all_to_old(){ all_to( Old ); }

QStandardItem* MeshConverterTreeView::add_model( CModelBase* model_b, QStandardItem* parent, const QString& name )
{
  QStandardItem_Custom *item = new QStandardItem_Custom( name );
  parent->appendRow( item );

  eModelType mt = model_type( model_b );
  QIcon& model_icon = m_icons[ mt == Tetra_M ? Tetra : Hexa ];

  QStandardItem_Custom *item2 = new QStandardItem_Custom( model_icon, QString("Name: ") + model_b->Name(), !item->parent() ? mt : Unknown_M );
  item->appendRow( item2 );

  eMesherType type_ = mesher_type( model_b );
  QIcon& icon =  m_icons[ type_ == Old ? Old_ :  type_ == New ? New_ : Unknown_ ];
  QString mesher_name = QString("Mesher: %1").arg( type_ == Old ? "Old" : type_ == New ? "New" : "Unknown" );
  CTetraMesh* mesh = model_b == nullptr ? nullptr : dynamic_cast< CTetraMesh*>( &model_b->Mesh() );
  QStandardItem_Custom *item3 = new QStandardItem_Custom( icon, mesher_name, mesh );
  item->appendRow( item3 );
  m_items.push_back( item3 );

  //Printer::instance()->debug("%s - %s - %s", name.toStdString().c_str(), model_b->Name().toStdString().c_str(), mesher_name.toStdString().c_str() );

  return item;
}

void MeshConverterTreeView::add_models( CModelBase* model_b, QStandardItem* parent )
{
  QStandardItemModel* model = parent->model();

  QStandardItem* root_parent = parent;

  add_model( model_b, parent, QString("Model") );

  CNewWellPathEntry* wpEntry = dynamic_cast< CNewWellPathEntry* >( model_b->GraphEntry( MD_NEW_WELLPATH ) );
  if( wpEntry != 0 )
  {
    QStandardItem_Custom *item = new QStandardItem_Custom( QString("Well-paths") );
    parent->appendRow( item );
    parent = item;
    //QStandardItem* previous_parent = parent;

    CNewWellPathEntry::TNodeSet nodes = wpEntry->EntryNodes();
  
    for (CNewWellPathEntry::TNodeSet::iterator it = nodes.begin(); it != nodes.end(); ++it)
    {
      if ((*it)->hasZoomInModel())
      {
        CModelBase* model_ = &(*it)->ZoomInModel();
        parent = add_model( model_, parent, QString("Zoom-in model") );

        model_ = &(*it)->ZoomInModel().CasingModel();
        add_model( model_, parent, QString("Casing model") );

        //parent = previous_parent;
      }
    }
  }

  if( !model_b->parentModel() )
  {
    CZoominModelEntry* ziEntry = dynamic_cast<CZoominModelEntry *>(model_b->GraphEntry(MD_BASE_ZOOMIN_MODEL));
    if (ziEntry)
    {
      parent = root_parent;
      QStandardItem_Custom *item = new QStandardItem_Custom( QString("Zoom-in models") );
      parent->appendRow( item );
      parent = item;

      const CZoominModelEntry::TNodeSet& zoominModels = ziEntry->EntryNodes();

      for (CZoominModelEntry::TNodeSet::iterator it = zoominModels.begin(); it != zoominModels.end(); ++it)
        add_models( &(*it)->ChildModel(), parent );
    }
  }
}

bool MeshConverterTreeView::any_old_mesher()
{
  return any_mesher( m_items, Old );
}

bool MeshConverterTreeView::any_old_mesher( CModelBase* model_b )
{
  eMesherType type_ = mesher_type( model_b );
  if( type_ == Old )
    return true;

  CNewWellPathEntry* wpEntry = dynamic_cast <CNewWellPathEntry*>(model_b->GraphEntry(MD_NEW_WELLPATH));
  if (wpEntry != 0)
  {
  CNewWellPathEntry::TNodeSet nodes = wpEntry->EntryNodes();
  for (CNewWellPathEntry::TNodeSet::iterator it = nodes.begin(); it != nodes.end(); ++it)
  {
      if ((*it)->hasZoomInModel())
      {
    // Zoom-in
    CModelBase* model_ = &(*it)->ZoomInModel();
    type_ = mesher_type( model_ );
    if( type_ == Old )
      return true;

    // Casing
    model_ = &(*it)->ZoomInModel().CasingModel();
    type_ = mesher_type( model_ );
    if( type_ == Old )
      return true;
      }
  }
  }

  if (!model_b->parentModel())
  {
  CZoominModelEntry* ziEntry = dynamic_cast<CZoominModelEntry *>(model_b->GraphEntry(MD_BASE_ZOOMIN_MODEL));
  if (ziEntry)
  {
      const CZoominModelEntry::TNodeSet& zoominModels = ziEntry->EntryNodes();
      for (CZoominModelEntry::TNodeSet::iterator it = zoominModels.begin(); it != zoominModels.end(); ++it)
    {
      bool res = any_old_mesher( &(*it)->ChildModel() );
      if( res )
        return true;
    }
  }
  }
  return false;
}