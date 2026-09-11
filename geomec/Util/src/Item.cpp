#include "stdafx.h"

#include "Item.h"

#include "Printer.h"

const std::string CItem::SEPARATOR = "SEPARATOR";

std::map< CItem::eEvtType, int> CItem::m_fixed_items_map = CItem::fix_items_init();

bool CItem::remove(){ return parent->RemoveMenu( pos, MF_BYPOSITION ); };

CItem::CItem( TPathV items_path_v, std::string item_name ) : CItem()
{
  parent = sub_menu( items_path_v );
  if( parent )
    pos = FindMenuItem( parent, item_name.c_str() );
};

int CItem::FindMenuItem(CMenu* Menu, std::string name)
{
   ASSERT(Menu);
   ASSERT(::IsMenu(Menu->GetSafeHmenu()));

   int count = Menu->GetMenuItemCount();
   for (int i = 0; i < count; i++)
   {
      CString str;
      if( Menu->GetMenuString(i, str, MF_BYPOSITION) )
      if( strcmp( str, name.c_str() ) == 0 )
      return i;
   }

   return -1;
}

int CItem::num_items( CMenu* Menu ){ return Menu->GetMenuItemCount(); }

CItem::TItemsV CItem::items_DEPRECATED( CMenu* Menu )
{
  TItemsV items_v;

  int count = Menu->GetMenuItemCount();
  for (int i = 0; i < count; i++)
  {
    CMenu* item = Menu->GetSubMenu( i );
    items_v.push_back( item );
  }

  return items_v;
}

//--------------------------------------------- remove all -----------------------------------------

bool CItem::remove_all( CMenu* sm, int excluded )
{
  int nItems = num_items( sm );
  for( int i = nItems-1; i > excluded-1 ; i-- ) //dont delete items: 'Add', 'Remove' -> only plugins
    if( !sm->RemoveMenu( i, MF_BYPOSITION ) )
      return false;
  return true;
}

bool CItem::remove_all( TPathV items_path_v, int excluded ) 
{
  CMenu* menu = sub_menu( items_path_v );
  return remove_all( menu, excluded );
};

bool CItem::remove( TPathV items_path_v, std::string item_name )
{
  CMenu* menu = sub_menu( items_path_v );
  int pos = FindMenuItem( menu, item_name.c_str() );
  if( pos > -1 )
    return menu->RemoveMenu( pos, MF_BYPOSITION );
  return false;
};


//--------------------------------------------- sub menu -----------------------------------------
bool CItem::disable_MENU_NOT_ITEM() // doesn't work : several tries
{
  //CString str = "Foo";

  //MENUITEMINFO info;
  //info.cbSize = sizeof(MENUITEMINFO);
  //info.fMask = MIIM_ID | MIIM_SUBMENU | MIIM_STATE | MIIM_DATA | MIIM_FTYPE | MIIM_STATE | MIIM_STRING;
  //info.fMask = MIIM_STATE;

  //VERIFY(pMenu->GetMenuItemInfo(1, &info, TRUE));
  //parent->GetMenuItemInfo(pos, &info, TRUE);

  //int res_i = parent->EnableMenuItem( info.wID, MF_GRAYED ); //MF_ENABLED
  
  //res = EnableMenuItem( parent->GetSafeHmenu(), info.wID,  MF_DISABLED | MF_GRAYED  ); //MF_ENABLED, MF_DISABLED
  //int res_i = EnableMenuItem( parent->GetSafeHmenu(), pos, MF_DISABLED | MF_GRAYED | MF_BYPOSITION ); //MF_ENABLED, MF_DISABLED
  
  //int res_i = EnableMenuItem( parent->GetSafeHmenu(), pos, MF_DISABLED | MF_BYPOSITION ); //MF_ENABLED, MF_DISABLED

  //HMENU hdl_menu = parent->GetSafeHmenu();
  //int res_i = EnableMenuItem( info.hSubMenu, pos, MF_DISABLED | MF_BYPOSITION ); //MF_ENABLED, MF_DISABLED
  

  MENUITEMINFO info2;
  info2.cbSize = sizeof(MENUITEMINFO);
  info2.fMask = MIIM_STATE;
  info2.fState = MFS_DISABLED;
  bool res = SetMenuItemInfo( parent->GetSafeHmenu(), pos, true, &info2 ); // true -> by position

  //DrawMenuBar( AfxGetApp()->m_pMainWnd->m_hWnd );

  return res;
}

bool CItem::rename( std::string name ) 
{
  MENUITEMINFO info;
  info.cbSize = sizeof(MENUITEMINFO);
  info.fMask = MIIM_ID;
  parent->GetMenuItemInfo(pos, &info, TRUE);
  bool res = ModifyMenu( parent->GetSafeHmenu(), pos, MF_BYPOSITION | MF_STRING, info.wID, name.c_str() );
  return res;
}

bool CItem::is_name( std::string name, CItem item ) 
{
  std::string item_name = item.name();
  item_name.erase(std::remove(item_name.begin(), item_name.end(), '&'), item_name.end());

  return ( item_name.find( name ) != std::string::npos );
}

std::string CItem::name() 
{
  CString str;
  if( parent && pos != -1 )
    parent->GetMenuString( pos, str, MF_BYPOSITION);
  return std::string( (LPCTSTR)str );
}

CItem CItem::item( std::string name, CMenu* menu ) 
{
  CItem item_;

  if( menu == nullptr )
    menu = AfxGetApp()->m_pMainWnd->GetMenu();

  int count = menu->GetMenuItemCount();
  for( int i=0; i < count; i++ )
  {
    CItem _item( menu, i );
    Printer::instance()->debug("%d: %s", i, _item.name().c_str() );
    //if( _item.exist() )
    if( is_name( name, _item ) )
    {
      item_ = _item;
      break;
    }
    else
    {
      //Printer::instance()->debug("%d: %s", i, _item.name().c_str() );

      CMenu* sub_menu = menu->GetSubMenu( i );
      if( sub_menu )
      {
        item_ = item( name, sub_menu );
        if( item_.exist() )
          break;
      }
      else
      {
        CItem _item2( menu, i );
        if( is_name( name, _item2 ) )
        {
          item_ = _item2;
          break;
        }
        else
        {
          //Printer::instance()->debug("%d: %s", i, _item.name().c_str() );
        }
      }
    }
  }
  return item_;
}


CMenu* CItem::sub_menu( std::string name ) 
{
  CMenu* menu = AfxGetApp()->m_pMainWnd->GetMenu();
  return sub_menu( menu, name );
}

CMenu* CItem::sub_menu( CMenu* menu, std::string name ) 
{
  if( menu )
  {
    int pos = FindMenuItem( menu, name.c_str() );
    if( pos > -1 )
    {
      CMenu* menu_ = menu->GetSubMenu( pos ); 
      return menu_;
    }
  }
  return nullptr;
}

CMenu* CItem::sub_menu( TItemsNameV items_v ) 
{
  int nE = items_v.size();
  
  CMenu* menu = AfxGetApp()->m_pMainWnd->GetMenu();

  int nLast = nE;//item_ == Item ? nE : nE-1;

  for( int i=0; i < nLast && menu; i++ )
    menu = sub_menu( menu, items_v[ i ] );

  return menu;
};

//--------------------------------------------- add -----------------------------------------

bool CItem::add( TPathV items_path_v, TItemsNameV items_v, eEvtType type_ ) 
{
  CMenu* menu = sub_menu( items_path_v );
  if( menu )
  {
    if( type_ == Manual ) //FIXME
      menu->AppendMenu( MF_BYPOSITION | MF_SEPARATOR, WM_USER + 1000, "" ); //FIXME
     
    for( int i=0; i < items_v.size(); i++ )
    {
      std::string name = items_v[ i ];
      bool res = menu->AppendMenu( MF_BYPOSITION | MF_ENABLED | MF_STRING, WM_USER + type_ + i, name.c_str() ); //FIXME: 'WM_USER' HC
      if( res )
      {
        //Printer::instance()->debug("CItem::add : %s : %s : evt: %d", type_ == Launch ? "Launch" : type_ == Manual ? "Manual" : "Remove" , name.c_str(), type_ + i );
      }
      else
        return false;
    }
    return true;
  }
  return false;
};

bool CItem::add( TPathV items_path_v, std::string name, int evt_id ) 
{
  CMenu* menu = sub_menu( items_path_v );
  if( menu )
  {
    bool res = menu->AppendMenu( MF_BYPOSITION | MF_ENABLED | MF_STRING, evt_id, name.c_str() );
    return res;
  }
  return false;
};

std::map< CItem::eEvtType, int> CItem::fix_items_init()
{
  std::map< eEvtType, int> fixed_items_map;

  fixed_items_map[ Launch ] = 2;
  fixed_items_map[ Manual ] = 7;
  fixed_items_map[ Remove ] = 0;

  return fixed_items_map;
}

int CItem::fix_items( eEvtType type_ ){ return m_fixed_items_map[ type_ ]; }

bool CItem::refresh( TPathV items_path_v, TItemsNameV items_v,  eEvtType type_ )
{
  bool res = remove_all( items_path_v, fix_items( type_ ) );
  if( res )
    return add( items_path_v, items_v, type_ );
  return false;
}
