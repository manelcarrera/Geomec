#include "stdafx.h"

#include "MaterialManager.h"

#include "Material.h"
#include "FormationBase.h"
#include "MaterialEntry.h"
#include "LibraryMaterial.h"
#include "MaterialHelperFactory.h"
#include "Printer.h"

MaterialManager::MaterialManager( CModelBase& model ) : m_model( model ) //, m_nodes( nullptr )
{
  //CMaterialEntry* entry = dynamic_cast <CMaterialEntry*> ( model.GraphEntry( MD_ROCK_MATERIAL ) );
  //m_nodes = entry->GraphEntryNodes();
}

/*
1. Mat Entry from model
2. Mat Lib from Mat Entry
3. Mat Creator from Mat Factory creates Mat of a type (they call 'model' to the 'type')
4. Add Mat Creator (with Mat) to Map Lib
5. Returns Mat Entry (but not needed as it is easy to get it)
*/
CMaterialEntry* MaterialManager::create( const QString& mat_name, int mat_model)
{
  CMaterialEntry* mat_entry = dynamic_cast <CMaterialEntry*> ( m_model.GraphEntry( MD_ROCK_MATERIAL ) );
  
  ml::CMaterialLibrary& mat_lib = mat_entry->MaterialLibrary();

  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();

  ml::TMaterialCreatorRefPtr mat_creator = f->getMatCreator( mat_model );

  QString name(!mat_name.isEmpty() ? mat_name : mat_creator->MaterialModelName() + " (inversion)"); 

  ml::CMaterial& mat = mat_lib.AddMaterial(name, *mat_creator);

  // search materialEntry for material that has &IMaterial::LibraryMaterial() == &mat

  // materialEntry->GraphEntryNodes() contains IMaterial which is wrapper around the created material library's material

  //  USE THIS ONE: static IMaterial* Create(CMaterialEntry& entry, CLibraryMaterial& libmat)
  //CLibraryMaterial& lib_mat=;
  //IMaterial* imat =  IMaterial::Create( *mat_entry, lib_mat);
  //IMaterial* imat =  CMaterialFactory::getInstance()->Create( *mat_entry, lib_mat );

  return mat_entry;
}
/*
  1. Create material : name and type
  2. From Entry get nodes
  3. From nodes get a ptr to 1st Mat
  4. TODO: I DON'T UNDERSTAND why Mat number should be the same as formations number
      I think it's not needed, just dynamic_cast de node to have the Mat (as in find_material) 
*/

bool MaterialManager::load()
{
  CMaterialEntry* mat_entry =	create( "linear rgi", MM_LINEAR ); //FIXME

  CGraphEntry::TEntryNodeSet mat_nodes = mat_entry->GraphEntryNodes();

  IMaterial* material = dynamic_cast <IMaterial*> (*(mat_nodes.begin()));

  TFormationBaseEntry* form_entry = dynamic_cast <TFormationBaseEntry*> ( m_model.GraphEntry( MD_BASE_FORMATION ) );

  std::vector <IMaterial*> materials( form_entry->EntryNodes().size(), material );

  //connectMaterials2Formations(m_model, materials); // in CRockMechProcessor::LoadGrid()

  return true; //FIXME
}

std::vector< std::string > MaterialManager::list()
{
  /*CGraphEntry::TEntryNodeSet mat_nodes = nodes();
  IMaterial* material = dynamic_cast <IMaterial*> (*(mat_nodes.begin()));
  TFormationBaseEntry* form_entry = dynamic_cast <TFormationBaseEntry*> ( m_model.GraphEntry( MD_BASE_FORMATION ) );
  std::vector <IMaterial*> materials( form_entry->EntryNodes().size(), material );

  for( int i=0; i< materials.size(); i++ )
    Printer::instance()->debug("mat mgr : materials_list : [%d] [%s]", i, materials[ i ]->Name().toStdString().c_str() );*/

  std::vector< std::string > materials_v;
  CMaterialEntry* mat_entry = dynamic_cast <CMaterialEntry*> ( m_model.GraphEntry( MD_ROCK_MATERIAL ) );
  for( int i=0; i < size(); i++ )
    materials_v.push_back( mat_entry->Material( i ).Name().toStdString() );
    //Printer::instance()->debug("mat mgr : materials_list : [%d] [%s]", i, mat_entry->Material( i ).Name().toStdString().c_str() );
  return materials_v;
  //return materials;
}

bool MaterialManager::remove( int idx )
{
  idx = idx == -1 ? size()-1 : idx; 
  bool res = false;
  if( idx < size() )
  {
    CMaterialEntry* mat_entry = dynamic_cast <CMaterialEntry*> ( m_model.GraphEntry( MD_ROCK_MATERIAL ) );
    res = mat_entry->Material( idx ).Destroy();
  }
  return res;
}


/*IMaterial* MaterialManager::material( const CGraphEntry::TEntryNodeSet& nodes, int mat_model )
{
  IMaterial* material = 0;
  for( CGraphEntry::TEntryNodeSet::const_iterator node = nodes.begin(); node != nodes.end(); ++node )
  {
    material = dynamic_cast <IMaterial*> (*node);
    if( material->LibraryMaterial().MaterialModel() == mat_model )
      break;

    material = 0;	// ???? MCR
  }
  return material;
}*/

/*IMaterial* MaterialManager::material( CModelBase& model, int mat_model )
{
  CMaterialEntry* mat_entry = dynamic_cast <CMaterialEntry*> ( model.GraphEntry( MD_ROCK_MATERIAL ) );
  CGraphEntry::TEntryNodeSet nodes = mat_entry->GraphEntryNodes();

  IMaterial* material = 0;
  for( CGraphEntry::TEntryNodeSet::const_iterator node = nodes.begin(); node != nodes.end(); ++node )
  {
    material = dynamic_cast <IMaterial*> (*node);
    if( material->LibraryMaterial().MaterialModel() == mat_model )
      break;

    material = 0;	// ???? MCR
  }
  return material;
}*/

CGraphEntry::TEntryNodeSet MaterialManager::nodes()
{
  CMaterialEntry* mat_entry = dynamic_cast <CMaterialEntry*> ( m_model.GraphEntry( MD_ROCK_MATERIAL ) );
  return mat_entry->GraphEntryNodes();
}

IMaterial* MaterialManager::material( std::string name )
{
  CGraphEntry::TEntryNodeSet nodes_ = nodes();

  IMaterial* material = nullptr;
  for( CGraphEntry::TEntryNodeSet::const_iterator node = nodes_.begin(); node != nodes_.end(); ++node )
  {
    material = dynamic_cast <IMaterial*>( *node );
    if( material->Name().toStdString() == name )
      break;
    material = nullptr;
  }
  return material;
}

IMaterial* MaterialManager::material( int idx )
{
  /*CMaterialEntry* mat_entry = dynamic_cast <CMaterialEntry*> ( m_model.GraphEntry( MD_ROCK_MATERIAL ) );
  CGraphEntry::TEntryNodeSet nodes = mat_entry->GraphEntryNodes();

  idx = idx == -1 ? nodes.size()-1 : idx; 

  IMaterial* material = nullptr;
  if( idx < nodes.size() )
  {
    CGraphEntry::TEntryNodeSet::const_iterator it = nodes.begin();
    std::advance( it, idx );

    material = dynamic_cast <IMaterial*>( *it );

  }*/
  idx = idx == -1 ? size()-1 : idx; 

  IMaterial* material = nullptr;
  if( idx < size() )
  {
    CMaterialEntry* mat_entry = dynamic_cast <CMaterialEntry*> ( m_model.GraphEntry( MD_ROCK_MATERIAL ) );
    if( mat_entry )
      material = &mat_entry->Material( idx );
  }
  return material;
}

int MaterialManager::size()
{
  int size_=0;
  CMaterialEntry* mat_entry = dynamic_cast <CMaterialEntry*> ( m_model.GraphEntry( MD_ROCK_MATERIAL ) );
  if( mat_entry )
  {
    CGraphEntry::TEntryNodeSet nodes = mat_entry->GraphEntryNodes();
    size_ = nodes.size();
  }
  return size_;
}

void MaterialManager::connect_materials_to_formations(const std::vector <IMaterial*> materials )
{
  TFormationBaseEntry* formationBaseEntry = dynamic_cast <TFormationBaseEntry*> ( m_model.GraphEntry( MD_BASE_FORMATION ) );
  
  size_t material_idx = 0;

  for(TFormationBaseEntry::TNodeSet::const_iterator formation = formationBaseEntry->EntryNodes().begin();
    formation != formationBaseEntry->EntryNodes().end(); 
    ++formation, ++material_idx )
  {
    connect_material_to_formation( materials[ material_idx ], *formation);
  }
}
bool MaterialManager::dep_has_mat( CDepletionStage& stage, CFormationBase* formation )
{
  return formation->ConnectedMaterial( stage ) != 0;
}
bool MaterialManager::mat_and_dep_same_model( CDepletionStage& stage, CFormationBase* formation, IMaterial* material )
{
  CMaterialServer* mat_server = formation->ConnectedMaterial( stage );
  return mat_server->MaterialModel() == material->MaterialModel();
}

void MaterialManager::copy_diff_type_mat_lib( IMaterial* src, IMaterial* dst )
{
  QString name = dst->Name();

  dst->CloneValues( *src ); // copy from server to material ????

  dst->Name(name);

  CLibraryMaterial *src_mat_lib = &src->LibraryMaterial(); // IMaterial / CLibraryMatreial
  CLibraryMaterial *dst_mat_lib = &dst->LibraryMaterial();

  copy_material_library_value(	src_mat_lib, dst_mat_lib, 
                  IDT_VALUETYPE_YOUNGS_MODULUS, 
                  IDT_VALUETYPE_YOUNGMODULUS_NORM, IDT_VALUETYPE_YOUNGMODULUS_TRANS);

  bool res = copy_material_library_value( src_mat_lib, dst_mat_lib, 
                      IDT_VALUETYPE_YOUNG_DECOMP, 
                      IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP, IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP);
  if( !res )
    copy_material_library_value(	src_mat_lib, dst_mat_lib, 
                    IDT_VALUETYPE_YOUNGS_MODULUS, 
                    IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP, IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP);
          
  copy_material_library_value(	src_mat_lib, dst_mat_lib, 
                  IDT_VALUETYPE_POISSONS_RATIO, 
                  IDT_VALUETYPE_POISSONRATIO_NORM, IDT_VALUETYPE_POISSONRATIO_TRANS);
          
  res = copy_material_library_value(	src_mat_lib, dst_mat_lib, 
                    IDT_VALUETYPE_POISSON_DECOMP, 
                    IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP, IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP);
  if (!res)
    copy_material_library_value(	src_mat_lib, dst_mat_lib, 
                    IDT_VALUETYPE_POISSONS_RATIO, 
                    IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP, IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP);
          
  copy_material_library_value(	src_mat_lib, dst_mat_lib, 
                  IDT_VALUETYPE_SHEARMODULUS, 
                  IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS, IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP);
}

bool MaterialManager::connect_mat_to_form_dep( IMaterial* material, CFormationBase* formation, CDepletionStage& stage )
{
  bool res=false;
  if( formation ) // formation may be 0 when we were handling interface elements
  {
    CMaterialServer* dep_mat_server = formation->ConnectedMaterial( stage );
    if( dep_mat_server )
    {
      IMaterial* mat = dep_mat_server->LibraryMaterial();
      if( mat )
      {
        if( dep_mat_server->MaterialModel() != material->MaterialModel() )
          copy_diff_type_mat_lib( mat, material );

        dep_mat_server->UnLink( *mat ); // ???? MCR
      }
      dep_mat_server->LinkTo( *material );
      //assert( dep_mat_server->IsLinkedTo(*material) );
      res = dep_mat_server->IsLinkedTo(*material);
    }
  }
  return res;
}

bool MaterialManager::unlink_mat_from_form_dep( IMaterial* material, CFormationBase* formation, CDepletionStage& stage )
{
  bool res=false;
  if( formation ) // formation may be 0 when we were handling interface elements
  {
    CMaterialServer* dep_mat_server = formation->ConnectedMaterial( stage );
    if( dep_mat_server )
    {
      IMaterial* mat = dep_mat_server->LibraryMaterial();
      if( mat )
        dep_mat_server->UnLink( *mat ); // ???? MCR
      res = !dep_mat_server->IsLinkedTo(*material);
    }
  }
  return res;
}

bool MaterialManager::connect_material_to_formation( IMaterial* material, CFormationBase* formation )
{
  bool res=false;
  if( formation ) // formation may be 0 when we were handling interface elements
  {
    CDepletionStageEntry* dep_entry = dynamic_cast < CDepletionStageEntry* > ( m_model.GraphEntry( MD_BASE_DEPLETION_STAGE ) );

    int dep_idx=0;
    for( CDepletionStageEntry::iterator stage = dep_entry->begin(); stage != dep_entry->end(); ++stage )
    {
      res = connect_mat_to_form_dep( material, formation, *stage );
      //Printer::instance()->debug("connect_material_to_formation : dep:[%d] : res:[%d]", dep_idx, res);
      break; //FIXME: do it to all the depletion stages or just to initial one? 
      dep_idx++;
      if( !res )
        break;
    }
      
  }
  return res;
}

bool MaterialManager::copy_material_library_value(	CLibraryMaterial *old_mat_lib, 
                          CLibraryMaterial *new_mat_lib, 

                          int from_type, 
  
                          int to_type_1, 
                          int to_type_2)
{
  CLibraryMaterialParameter *old_lib_param, *new_lib_param;
          
  if( ( old_lib_param = old_mat_lib->ParameterByValueTypeID( from_type ) ) )
  {
    if( !new_mat_lib->ParameterByValueTypeID( from_type ) )
    {
      double old_lib_param_value = old_lib_param->Value();

      if( ( new_lib_param = new_mat_lib->ParameterByValueTypeID( to_type_1 ) ) )
        new_lib_param->Value( old_lib_param_value );

      if( to_type_2 > -1 && ( new_lib_param = new_mat_lib->ParameterByValueTypeID( IDT_VALUETYPE_YOUNGMODULUS_TRANS ) ) )
        new_lib_param->Value( old_lib_param_value );
    }
    return true;
  }
  return false;
}
bool MaterialManager::value( int idx, int type_, double val )
{
  bool res = false;

  IMaterial* mat = material( idx );
  CLibraryMaterialParameter *param = mat->LibraryMaterial().ParameterByValueTypeID( type_ );
  if( param )
    res=param->Value( val );
  return res;
}


bool MaterialManager::value( CLibraryMaterial *lib_mat, int type_, double val )
{
  bool res = false;
  CLibraryMaterialParameter *param = lib_mat->ParameterByValueTypeID( type_ );
  if( param )
    res=param->Value( val );
  return res;
}

double MaterialManager::value( CLibraryMaterial *mat_lib, int type_)
{
  double val = 9999.99; // FIXME
  CLibraryMaterialParameter *param = mat_lib->ParameterByValueTypeID( type_ );
  if( param )
    val=param->Value();
  return val;
}

void MaterialManager::print_all_values()
{
  QString dump;

  struct Param{ int id; std::string desc; };
  //std::set< Param > ids_s;

  std::map<int,std::string> ids_m;

  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();

  for( int i = mlMatModel::MM_LINEAR; i<= mlMatModel::MM_CAMCLAY_CREEP; i++ )
  {
    ml::TMaterialCreatorRefPtr creator = f->getMatCreator( i );
    ml::CMaterial* mat = creator->OnCreateMaterial();
    creator->Create( QString::asprintf("model_%02d",i), *mat);

    /*IMaterial* mat = material( i );
    if( !mat )
      continue;*/
    //CLibraryMaterial& lib_mat = mat->LibraryMaterial();

    //std::string dump = std::string();

    CLibraryMaterial* lib_mat = dynamic_cast<CLibraryMaterial*>(mat);
    int size = lib_mat->ParameterSize();

    

    if( lib_mat )
    {
      std::vector<int> ids_v;
      std::vector<std::string> vals_v;

      dump += QString::asprintf("[\"%s\", [\n", lib_mat->MaterialModelName().toStdString().c_str() );

      //int size= mat->ParameterSize();
      for( size_t j=0; j < size; j++ )
      {
        //ml::CMatParam& param = lib_mat.MatParameter( j );
        CLibraryMaterialParameter& param2 = lib_mat->Parameter( j );
        //ml::CMatParam& param2 = mat->MatParameter(j);

        //Printer::instance()->debug("{ %d, %d, %d, %s, %f }", i, j, param2.ValueTypeID(), param.Name().toStdString().c_str(), param.Value() );
        //Printer::instance()->debug("{ %d, %d, %d, %s }\n", i, j, param2.ValueTypeID(), param.Name().toStdString().c_str() );

        //Param param={ param2.ValueTypeID(), param2.Name().toStdString() };
        //std::pair< std::set< Param >::iterator, bool > res = ids_s.insert( param2.ValueTypeID() );

        ids_m[ param2.ValueTypeID() ] = param2.Name().toStdString();

        ids_v.push_back( param2.ValueTypeID() );
        
        //std::fmod( param2.Value(), 1.0 );

        if( int( param2.Value() ) != param2.Value() ) //is decimal
        {
          std::string str = std::to_string( param2.Value() );
          str.erase ( str.find_last_not_of('0') + 1, std::string::npos );

          dump += QString::asprintf("\t[%d, %d, \"%s\", %s],\n", j, param2.ValueTypeID(), param2.Name().toStdString().c_str(), str.c_str() );
          vals_v.push_back( str );
        }
        else //is int
        {
          dump += QString::asprintf("\t[%d, %d, \"%s\", %d],\n", j, param2.ValueTypeID(), param2.Name().toStdString().c_str(), (int)param2.Value() );
          vals_v.push_back( std::to_string( (int)param2.Value() ) );
        }
        //dump += QString::asprintf("\t[%d, %d, \"%s\", %d],\n", j, param2.ValueTypeID(), param2.Name().toStdString().c_str(), param2.Value() );
      }
      dump.chop(2); // remove "\n,"
      Printer::instance()->debug("\n%s ]\n],\n", dump.toStdString().c_str() );

      // params list per model
      dump = "";
      for( int k=0; k < size; k++ )
        dump += QString::asprintf("%d, ", ids_v[ k ] );
      dump.chop(2);
      Printer::instance()->debug("\n[ %s ]", dump.toStdString().c_str() );

      // default values list per model
      dump = "";
      for( int k=0; k < size; k++ )
        dump += QString::asprintf("%s, ", vals_v[ k ] );
      dump.chop(2);
      Printer::instance()->debug("\n( %s )", dump.toStdString().c_str() );

      dump = "";
    }
  }

  std::map<int,std::string>::iterator it;
  
  dump = "\n[\n";
  for( it = ids_m.begin(); it != ids_m.end(); it++ )
    dump += QString::asprintf("\t[ %d, \"%s\" ],\n", it->first, it->second.c_str() );
  Printer::instance()->debug( "%s ]", dump.toStdString().c_str() );

  dump = "\n";
  int count=0;
  for( it = ids_m.begin(); it != ids_m.end(); it++ )
    dump += QString::asprintf("%s = %d\n", QString::fromStdString( it->second ).replace("'","").replace(" ","_").replace(".","").replace(",","").replace("/"," ").toStdString().c_str(), count++ );
  Printer::instance()->debug( "%s", dump.toStdString().c_str() );


  //Printer::instance()->debug("params size: [%d]", ids_s.size() );
}

std::vector< double > MaterialManager::values( int idx )
{
  std::vector<double> vals;

  IMaterial* mat = material( idx );
  CLibraryMaterial& lib_mat = mat->LibraryMaterial();
  int size = lib_mat.ParameterSize();

  for( size_t i=0; i < size; i++ )
  {
    ml::CMatParam& param = lib_mat.MatParameter( i );
    CLibraryMaterialParameter& param2 = lib_mat.Parameter( i );
    vals.push_back( param.Value() );

    Printer::instance()->debug("mat mgr : values : ids:[%d] : name:[%s] : id:[%d] : val:[%f]", idx, param.Name().toStdString().c_str(), param2.ValueTypeID(), param.Value() );
  }
  return vals;
}