#pragma once

#include <vector>
#include <string>

class CMenu;

class CItem
{
  typedef std::vector<CMenu*> TItemsV; 

  CMenu* parent;
  int pos;

public:

  static const std::string SEPARATOR;

  // WM_USER: 0x0400 -> FIXME: HC
  enum eEvtType
  {
    Launch	= 100,
    Manual	= Launch+10,
    Remove	= Manual+10,
    Max		= Remove+10
  };

  typedef std::vector< std::string > TItemsNameV;
  typedef std::vector< std::string > TPathV;

private:

  static std::map< eEvtType, int> m_fixed_items_map;

private:

  //static
  static CMenu* sub_menu( std::string name );						//sub-menu from main menu
  static CMenu* sub_menu( CMenu* menu, std::string name );		//sub-menu (name) from a sub-menu (CMenu*)
  static CMenu* sub_menu( TItemsNameV items_v );					//sub-menu giving the names path
  
  static TItemsV items_DEPRECATED( CMenu* Menu );

  static int fix_items( eEvtType type_ ); //FIXME: HC

  static std::map< eEvtType, int> fix_items_init();

public:

  //non-static
  CItem() : parent(nullptr),pos(-1){}
  CItem( const CItem& item ) : parent( item.parent ),pos( item.pos ){}
  CItem( CMenu* parent_, int pos_ ) : parent( parent_ ), pos( pos_ ){}
  CItem( TPathV items_path_v, std::string item_name );

  bool remove();
  bool exist(){ return ( parent && (pos > -1) ); };
  bool valid(){ return exist(); };

  bool rename( std::string name );
  bool disable_MENU_NOT_ITEM(); //doesn't work
  std::string name();

  //static
  static int FindMenuItem(CMenu* Menu, std::string name);
  static int num_items( CMenu* Menu );

  static bool remove_all( CMenu* sm, int excluded = 0 );						 //remove all elems from a sub-menu (CMenu*)
  static bool remove_all( TPathV items_path_v, int excluded = 0 );			//remove all items from a sub-menu giving the names path
  static bool remove( TPathV items_path_v, std::string item_name );
  
  static bool add( TPathV items_path_v, TItemsNameV items_v, eEvtType type_ );	// add an item giving the names path
  static bool add( TPathV items_path_v, std::string name, int evt_id );

  static bool refresh( TPathV items_path_v, TItemsNameV items_v, eEvtType type_ );

  static CItem item( std::string name, CMenu* menu = nullptr );
  static bool is_name( std::string name, CItem item );
};
