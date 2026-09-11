#ifndef MYTREEVIEW_H
#define MYTREEVIEW_H

#include <QtWidgets\QTreeView>
#include <QtWidgets\QMenu>

#include <QtGui\QStandardItemModel>

#include <vector>
#include <map>

class QStandardItem;
class CModelBase;
class QStandardItem_Custom;
class QKeyEvent;

class MeshConverterTreeView : public QTreeView
{
  Q_OBJECT

  enum eMenus{	Root, Mesher, Model, None };
  enum eActions{	Swap, To_Tetra, All_To_New, All_To_Old, Num_Actions };

  QStandardItemModel m_model;

  std::map< int, QIcon > m_icons;
  std::map< int, QAction* > m_actions;
  std::map< int, QMenu* > m_menus;

  std::vector< QStandardItem_Custom* > m_items;

private:

  
  ~MeshConverterTreeView();
  void init();
  void clean();
  void add_models( CModelBase* model_b, QStandardItem* parent );
  QStandardItem* add_model( CModelBase* model_b, QStandardItem* parent, const QString& name );

  eMenus menu( const QPoint &point );
  void action( eActions action_ );

public:
  
  MeshConverterTreeView(QWidget *parent = 0);
  bool any_old_mesher();
  static bool any_old_mesher( CModelBase* model_b );

  void update();

private slots:
  void swap_mesher();

  void convert_to_tetra();

  void all_to_new();
  void all_to_old();
  void all_to( int type_ );

  void resize_wnd();
  void on_custom_context_menu( const QPoint &point );
};

#endif // MYTREEVIEW_H
