#pragma once

class IMaterial;
class CMaterialEntry;
class CModelBase;
class CDepletionStage;
class CFormationBase;

class CGraphEntry;
class CLibraryMaterial;

#include <vector>

class MaterialManager
{
  CModelBase& m_model;

private:
  bool load();
  //IMaterial* material( const CGraphEntry::TEntryNodeSet& nodes, int mat_model );
  //IMaterial* material( CModelBase& model, int mat_model );
  void print_all_values();
  CGraphEntry::TEntryNodeSet nodes();
  bool value(	CLibraryMaterial *mat_lib, int type_, double val );
  double value( CLibraryMaterial *mat_lib, int type_);
  void connect_materials_to_formations( const std::vector <IMaterial*> materials );
  bool dep_has_mat( CDepletionStage& stage, CFormationBase* formation );
  bool mat_and_dep_same_model( CDepletionStage& stage, CFormationBase* formation, IMaterial* material );
  void copy_diff_type_mat_lib( IMaterial* src, IMaterial* dst );
  bool connect_mat_to_form_dep( IMaterial* material, CFormationBase* formation, CDepletionStage& stage );
  bool copy_material_library_value(	CLibraryMaterial *old_mat_lib, 
                    CLibraryMaterial *new_mat_lib, 
                    int from_type, 
                    int to_type_1, 
                    int to_type_2 = -1);
public:
  MaterialManager( CModelBase& model );
  CModelBase& model(){ return m_model; };
  CMaterialEntry* create(const QString& mat_name, int mat_model);
  bool remove( int idx );
  bool connect_material_to_formation( IMaterial* material, CFormationBase* formation );
  bool unlink_mat_from_form_dep( IMaterial* material, CFormationBase* formation, CDepletionStage& stage );
  std::vector< double > values( int idx );
  bool value( int idx, int type_, double val );
  IMaterial* material( int idx );
  IMaterial* material( std::string name );
  int size();
  std::vector< std::string > list();
};